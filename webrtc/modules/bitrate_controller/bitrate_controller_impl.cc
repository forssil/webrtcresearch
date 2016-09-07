/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#include "webrtc/modules/bitrate_controller/bitrate_controller_impl.h"

#include <algorithm>
#include <map>
#include <utility>

#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/modules/rtp_rtcp/include/rtp_rtcp_defines.h"

namespace webrtc {

class BitrateControllerImpl::RtcpBandwidthObserverImpl
    : public RtcpBandwidthObserver {
 public:
  explicit RtcpBandwidthObserverImpl(BitrateControllerImpl* owner)
      : owner_(owner) {
  }
  virtual ~RtcpBandwidthObserverImpl() {
  }
  // Received RTCP REMB or TMMBR.
  void OnReceivedEstimatedBitrate(uint32_t bitrate) override {
    owner_->OnReceivedEstimatedBitrate(bitrate);
  }
  // Received RTCP receiver block.
  void OnReceivedRtcpReceiverReport(const ReportBlockList& report_blocks,
                                    int64_t rtt,
                                    int64_t now_ms) override {
    if (report_blocks.empty())
      return;

    int fraction_lost_aggregate = 0;
    int total_number_of_packets = 0;

    // Compute the a weighted average of the fraction loss from all report
    // blocks.
    for (const RTCPReportBlock& report_block : report_blocks) {
      std::map<uint32_t, uint32_t>::iterator seq_num_it =
          ssrc_to_last_received_extended_high_seq_num_.find(
              report_block.sourceSSRC);

      int number_of_packets = 0;
      if (seq_num_it != ssrc_to_last_received_extended_high_seq_num_.end()) {
        number_of_packets =
            report_block.extendedHighSeqNum - seq_num_it->second;
      }

      fraction_lost_aggregate += number_of_packets * report_block.fractionLost;
      total_number_of_packets += number_of_packets;

      // Update last received for this SSRC.
      ssrc_to_last_received_extended_high_seq_num_[report_block.sourceSSRC] =
          report_block.extendedHighSeqNum;
    }
    if (total_number_of_packets < 0) {
      LOG(LS_WARNING) << "Received report block where extended high sequence "
                         "number goes backwards, ignoring.";
      return;
    }
    if (total_number_of_packets == 0)
      fraction_lost_aggregate = 0;
    else
      fraction_lost_aggregate  = (fraction_lost_aggregate +
          total_number_of_packets / 2) / total_number_of_packets;
    if (fraction_lost_aggregate > 255)
      return;

    RTC_DCHECK_GE(total_number_of_packets, 0);

    owner_->OnReceivedRtcpReceiverReport(fraction_lost_aggregate, rtt,
                                         total_number_of_packets, now_ms);
  }

 private:
  std::map<uint32_t, uint32_t> ssrc_to_last_received_extended_high_seq_num_;
  BitrateControllerImpl* owner_;
};

BitrateController* BitrateController::CreateBitrateController(
    Clock* clock,
    BitrateObserver* observer,
    RtcEventLog* event_log) {
  return new BitrateControllerImpl(clock, observer, event_log);
}

BitrateController* BitrateController::CreateBitrateController(
    Clock* clock,
    RtcEventLog* event_log) {
  return CreateBitrateController(clock, nullptr, event_log);
}

BitrateControllerImpl::BitrateControllerImpl(Clock* clock,
                                             BitrateObserver* observer,
                                             RtcEventLog* event_log)
    : clock_(clock),
      observer_(observer),
      last_bitrate_update_ms_(clock_->TimeInMilliseconds()),
      event_log_(event_log),
      bandwidth_estimation_(event_log),
      reserved_bitrate_bps_(0),
      last_bitrate_bps_(0),
      last_fraction_loss_(0),
      last_rtt_ms_(0),
      last_reserved_bitrate_bps_(0) {
  // This calls the observer_ if set, which means that the observer provided by
  // the user must be ready to accept a bitrate update when it constructs the
  // controller. We do this to avoid having to keep synchronized initial values
  // in both the controller and the allocator.
  MaybeTriggerOnNetworkChanged();
}

RtcpBandwidthObserver* BitrateControllerImpl::CreateRtcpBandwidthObserver() {
  return new RtcpBandwidthObserverImpl(this);
}

void BitrateControllerImpl::SetStartBitrate(int start_bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.SetSendBitrate(start_bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::SetMinMaxBitrate(int min_bitrate_bps,
                                             int max_bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.SetMinMaxBitrate(min_bitrate_bps, max_bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::SetBitrates(int start_bitrate_bps,
                                        int min_bitrate_bps,
                                        int max_bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.SetBitrates(start_bitrate_bps,
                                      min_bitrate_bps,
                                      max_bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::ResetBitrates(int bitrate_bps,
                                          int min_bitrate_bps,
                                          int max_bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_ = SendSideBandwidthEstimation(event_log_);
    bandwidth_estimation_.SetBitrates(bitrate_bps, min_bitrate_bps,
                                      max_bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::SetReservedBitrate(uint32_t reserved_bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    reserved_bitrate_bps_ = reserved_bitrate_bps;
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::OnReceivedEstimatedBitrate(uint32_t bitrate) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.UpdateReceiverEstimate(clock_->TimeInMilliseconds(),
                                                 bitrate);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::UpdateProbeBitrate(uint32_t bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.SetSendBitrate(bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::UpdateDelayBasedEstimate(uint32_t bitrate_bps) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.UpdateDelayBasedEstimate(clock_->TimeInMilliseconds(),
                                                   bitrate_bps);
  }
  MaybeTriggerOnNetworkChanged();
}

int64_t BitrateControllerImpl::TimeUntilNextProcess() {
  const int64_t kBitrateControllerUpdateIntervalMs = 25;
  rtc::CritScope cs(&critsect_);
  int64_t time_since_update_ms =
      clock_->TimeInMilliseconds() - last_bitrate_update_ms_;
  return std::max<int64_t>(
      kBitrateControllerUpdateIntervalMs - time_since_update_ms, 0);
}

void BitrateControllerImpl::Process() {
  if (TimeUntilNextProcess() > 0)
    return;
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.UpdateEstimate(clock_->TimeInMilliseconds());
  }
  MaybeTriggerOnNetworkChanged();
  last_bitrate_update_ms_ = clock_->TimeInMilliseconds();
}

void BitrateControllerImpl::OnReceivedRtcpReceiverReport(
    uint8_t fraction_loss,
    int64_t rtt,
    int number_of_packets,
    int64_t now_ms) {
  {
    rtc::CritScope cs(&critsect_);
    bandwidth_estimation_.UpdateReceiverBlock(fraction_loss, rtt,
                                              number_of_packets, now_ms);
  }
  MaybeTriggerOnNetworkChanged();
}

void BitrateControllerImpl::MaybeTriggerOnNetworkChanged() {
  if (!observer_)
    return;

  uint32_t bitrate_bps;
  uint8_t fraction_loss;
  int64_t rtt;

  if (GetNetworkParameters(&bitrate_bps, &fraction_loss, &rtt))
    observer_->OnNetworkChanged(bitrate_bps, fraction_loss, rtt);
}

bool BitrateControllerImpl::GetNetworkParameters(uint32_t* bitrate,
                                                 uint8_t* fraction_loss,
                                                 int64_t* rtt) {
  rtc::CritScope cs(&critsect_);
  int current_bitrate;
  bandwidth_estimation_.CurrentEstimate(&current_bitrate, fraction_loss, rtt);
  *bitrate = current_bitrate;
  *bitrate -= std::min(*bitrate, reserved_bitrate_bps_);
  *bitrate =
      std::max<uint32_t>(*bitrate, bandwidth_estimation_.GetMinBitrate());

  bool new_bitrate = false;
  if (*bitrate != last_bitrate_bps_ || *fraction_loss != last_fraction_loss_ ||
      *rtt != last_rtt_ms_ ||
      last_reserved_bitrate_bps_ != reserved_bitrate_bps_) {
    last_bitrate_bps_ = *bitrate;
    last_fraction_loss_ = *fraction_loss;
    last_rtt_ms_ = *rtt;
    last_reserved_bitrate_bps_ = reserved_bitrate_bps_;
    new_bitrate = true;
  }
  return new_bitrate;
}

bool BitrateControllerImpl::AvailableBandwidth(uint32_t* bandwidth) const {
  rtc::CritScope cs(&critsect_);
  int bitrate;
  uint8_t fraction_loss;
  int64_t rtt;
  bandwidth_estimation_.CurrentEstimate(&bitrate, &fraction_loss, &rtt);
  if (bitrate > 0) {
    bitrate = bitrate - std::min<int>(bitrate, reserved_bitrate_bps_);
    bitrate = std::max(bitrate, bandwidth_estimation_.GetMinBitrate());
    *bandwidth = bitrate;
    return true;
  }
  return false;
}
}  // namespace webrtc
