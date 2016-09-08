/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_processing/aecm/aecm_core.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

extern "C" {
#include "webrtc/common_audio/ring_buffer.h"
#include "webrtc/common_audio/signal_processing/include/real_fft.h"
}
#include "webrtc/modules/audio_processing/aecm/echo_control_mobile.h"
#include "webrtc/modules/audio_processing/utility/delay_estimator_wrapper.h"
extern "C" {
#include "webrtc/system_wrappers/include/cpu_features_wrapper.h"
}
#include "webrtc/typedefs.h"



static const ALIGN8_BEG int16_t WebRtcAecm_kSqrtHanning[] ALIGN8_END =
    {
        
        0,100,201,301,401,502,602,702,802,903,1003,1103,1203,1303,1403,1503,1603,
        1703,1802,1902,2002,2101,2201,2300,2399,2499,2598,2697,2796,2894,2993,3092,3190,
        3289,3387,3485,3583,3681,3778,3876,3973,4071,4168,4265,4362,4458,4555,4651,4747,
        4843,4939,5034,5130,5225,5320,5415,5509,5604,5698,5792,5886,5979,6072,6165,6258,
        6351,6443,6535,6627,6719,6810,6901,6992,7083,7173,7263,7353,7443,7532,7621,7710,
        7798,7886,7974,8061,8149,8235,8322,8408,8494,8580,8665,8750,8835,8919,9003,9087,
        9170,9253,9336,9418,9500,9582,9663,9744,9824,9904,9984,10063,10142,10221,10299,10377,
        10454,10531,10608,10684,10760,10836,10911,10985,11060,11133,11207,11280,11352,11425,11496,11567,
        11638,11709,11779,11848,11917,11986,12054,12122,12189,12256,12322,12388,12454,12519,12583,12647,
        12711,12774,12836,12898,12960,13021,13082,13142,13202,13261,13320,13378,13435,13493,13549,13605,
        13661,13716,13771,13825,13878,13932,13984,14036,14088,14139,14189,14239,14288,14337,14385,14433,
        14480,14527,14573,14619,14664,14708,14752,14796,14838,14881,14922,14963,15004,15044,15084,15122,
        15161,15198,15236,15272,15308,15344,15379,15413,15447,15480,15512,15544,15576,15607,15637,15667,
        15696,15724,15752,15779,15806,15832,15858,15883,15907,15931,15954,15976,15998,16020,16040,16061,
        16080,16099,16117,16135,16152,16169,16185,16200,16215,16229,16242,16255,16267,16279,16290,16300,
        16310,16319,16328,16336,16343,16350,16356,16362,16367,16371,16375,16378,16380,16382,16383,16384
    };

static const ALIGN8_BEG int16_t WebRtcAecm_kSqrtHanning128[] ALIGN8_END =
{
    0,200,400,600,800,1000,1200,1400,1599,1798,1997,2196,2394,2592,2790,2987,3184,
    3380,3576,3771,3965,4159,4353,4545,4738,4929,5120,5309,5498,5687,5874,6061,6246,
    6431,6615,6797,6979,7160,7339,7518,7695,7871,8047,8220,8393,8564,8734,8903,9071,
    9237,9402,9565,9727,9887,10046,10204,10359,10514,10667,10818,10967,11115,11262,11406,11549,
    11690,11830,11968,12103,12238,12370,12500,12629,12755,12880,13003,13124,13243,13360,13475,13587,
    13698,13807,13914,14019,14121,14222,14320,14416,14510,14602,14692,14780,14865,14948,15029,15107,
    15184,15258,15330,15399,15466,15531,15594,15654,15712,15768,15821,15872,15920,15966,16010,16051,
    16090,16127,16161,16193,16222,16249,16273,16295,16315,16332,16346,16359,16369,16376,16381,16383
};

static const ALIGN8_BEG int16_t WebRtcAecm_kSqrtHanning64[] ALIGN8_END =
 {
 0, 399, 798, 1196, 1594, 1990, 2386, 2780, 3172,
 3562, 3951, 4337, 4720, 5101, 5478, 5853, 6224,
 6591, 6954, 7313, 7668, 8019, 8364, 8705, 9040,
 9370, 9695, 10013, 10326, 10633, 10933, 11227, 11514,
 11795, 12068, 12335, 12594, 12845, 13089, 13325, 13553,
 13773, 13985, 14189, 14384, 14571, 14749, 14918, 15079,
 15231, 15373, 15506, 15631, 15746, 15851, 15947, 16034,
 16111, 16179, 16237, 16286, 16325, 16354, 16373, 16384
 };


//#endif

#ifdef AECM_WITH_ABS_APPROX
//Q15 alpha = 0.99439986968132  const Factor for magnitude approximation
static const uint16_t kAlpha1 = 32584;
//Q15 beta = 0.12967166976970   const Factor for magnitude approximation
static const uint16_t kBeta1 = 4249;
//Q15 alpha = 0.94234827210087  const Factor for magnitude approximation
static const uint16_t kAlpha2 = 30879;
//Q15 beta = 0.33787806009150   const Factor for magnitude approximation
static const uint16_t kBeta2 = 11072;
//Q15 alpha = 0.82247698684306  const Factor for magnitude approximation
static const uint16_t kAlpha3 = 26951;
//Q15 beta = 0.57762063060713   const Factor for magnitude approximation
static const uint16_t kBeta3 = 18927;
#endif

static const int16_t kNoiseEstQDomain = 15;
static const int16_t kNoiseEstIncCount = 5;

static void ComfortNoise(AecmCore* aecm,
                         const uint16_t* dfa,
                         ComplexInt16* out,
                         const int16_t* lambda);

static void WindowAndFFT(AecmCore* aecm,
                         int16_t* fft,
                         const int16_t* time_signal,
                         ComplexInt16* freq_signal,
                         int time_signal_scaling) {
  int i = 0;

  // FFT of signal
  for (i = 0; i < PART_LEN; i++) {
    // Window time domain signal and insert into real part of
    // transformation array |fft|
    int16_t scaled_time_signal = time_signal[i] << time_signal_scaling;
    fft[i] = (int16_t)((scaled_time_signal * WebRtcAecm_kSqrtHanning[i]) >> 14);
    scaled_time_signal = time_signal[i + PART_LEN] << time_signal_scaling;
    fft[PART_LEN + i] = (int16_t)((
        scaled_time_signal * WebRtcAecm_kSqrtHanning[PART_LEN - i]) >> 14);
  }

  // Do forward FFT, then take only the first PART_LEN complex samples,
  // and change signs of the imaginary parts.
  WebRtcSpl_RealForwardFFT(aecm->real_fft, fft, (int16_t*)freq_signal);
  for (i = 0; i < PART_LEN; i++) {
    freq_signal[i].imag = -freq_signal[i].imag;
  }
}

static void InverseFFTAndWindow(AecmCore* aecm,
                                int16_t* fft,
                                ComplexInt16* efw,
                                int16_t* output,
                                const int16_t* nearendClean) {
  int i, j, outCFFT;
  int32_t tmp32no1;
  // Reuse |efw| for the inverse FFT output after transferring
  // the contents to |fft|.
  int16_t* ifft_out = (int16_t*)efw;

  // Synthesis
  for (i = 1, j = 2; i < PART_LEN; i += 1, j += 2) {
    fft[j] = efw[i].real;
    fft[j + 1] = -efw[i].imag;
  }
  fft[0] = efw[0].real;
  fft[1] = -efw[0].imag;

  fft[PART_LEN2] = efw[PART_LEN].real;
  fft[PART_LEN2 + 1] = -efw[PART_LEN].imag;

  // Inverse FFT. Keep outCFFT to scale the samples in the next block.
  outCFFT = WebRtcSpl_RealInverseFFT(aecm->real_fft, fft, ifft_out);
  for (i = 0; i < PART_LEN; i++) {
    ifft_out[i] = (int16_t)WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(
                    ifft_out[i], WebRtcAecm_kSqrtHanning[i], 14);
    tmp32no1 = WEBRTC_SPL_SHIFT_W32((int32_t)ifft_out[i],
                                     outCFFT - aecm->dfaCleanQDomain);
    output[i] = (int16_t)WEBRTC_SPL_SAT(WEBRTC_SPL_WORD16_MAX,
                                        tmp32no1 + aecm->outBuf[i],
                                        WEBRTC_SPL_WORD16_MIN);

    tmp32no1 = (ifft_out[PART_LEN + i] *
        WebRtcAecm_kSqrtHanning[PART_LEN - i]) >> 14;
    tmp32no1 = WEBRTC_SPL_SHIFT_W32(tmp32no1,
                                    outCFFT - aecm->dfaCleanQDomain);
    aecm->outBuf[i] = (int16_t)WEBRTC_SPL_SAT(WEBRTC_SPL_WORD16_MAX,
                                                tmp32no1,
                                                WEBRTC_SPL_WORD16_MIN);
  }

  // Copy the current block to the old position
  // (aecm->outBuf is shifted elsewhere)
  memcpy(aecm->xBuf, aecm->xBuf + PART_LEN, sizeof(int16_t) * PART_LEN);
  memcpy(aecm->dBufNoisy,
         aecm->dBufNoisy + PART_LEN,
         sizeof(int16_t) * PART_LEN);
  if (nearendClean != NULL)
  {
    memcpy(aecm->dBufClean,
           aecm->dBufClean + PART_LEN,
           sizeof(int16_t) * PART_LEN);
  }
}

// Transforms a time domain signal into the frequency domain, outputting the
// complex valued signal, absolute value and sum of absolute values.
//
// time_signal          [in]    Pointer to time domain signal
// freq_signal_real     [out]   Pointer to real part of frequency domain array
// freq_signal_imag     [out]   Pointer to imaginary part of frequency domain
//                              array
// freq_signal_abs      [out]   Pointer to absolute value of frequency domain
//                              array
// freq_signal_sum_abs  [out]   Pointer to the sum of all absolute values in
//                              the frequency domain array
// return value                 The Q-domain of current frequency values
//
static int TimeToFrequencyDomain(AecmCore* aecm,
                                 const int16_t* time_signal,
                                 ComplexInt16* freq_signal,
                                 uint16_t* freq_signal_abs,
                                 uint32_t* freq_signal_sum_abs) {
  int i = 0;
  int time_signal_scaling = 0;

  int32_t tmp32no1 = 0;
  int32_t tmp32no2 = 0;

  // In fft_buf, +16 for 32-byte alignment.
  int16_t fft_buf[PART_LEN4 + 16];
  int16_t *fft = (int16_t *) (((uintptr_t) fft_buf + 31) & ~31);

  int16_t tmp16no1;
#ifndef WEBRTC_ARCH_ARM_V7
  int16_t tmp16no2;
#endif
#ifdef AECM_WITH_ABS_APPROX
  int16_t max_value = 0;
  int16_t min_value = 0;
  uint16_t alpha = 0;
  uint16_t beta = 0;
#endif

#ifdef AECM_DYNAMIC_Q
  tmp16no1 = WebRtcSpl_MaxAbsValueW16(time_signal, PART_LEN2);
  time_signal_scaling = WebRtcSpl_NormW16(tmp16no1);
#endif

  WindowAndFFT(aecm, fft, time_signal, freq_signal, time_signal_scaling);

  // Extract imaginary and real part, calculate the magnitude for
  // all frequency bins
  freq_signal[0].imag = 0;
  freq_signal[PART_LEN].imag = 0;
  freq_signal_abs[0] = (uint16_t)WEBRTC_SPL_ABS_W16(freq_signal[0].real);
  freq_signal_abs[PART_LEN] = (uint16_t)WEBRTC_SPL_ABS_W16(
                                freq_signal[PART_LEN].real);
  (*freq_signal_sum_abs) = (uint32_t)(freq_signal_abs[0]) +
                           (uint32_t)(freq_signal_abs[PART_LEN]);

  for (i = 1; i < PART_LEN; i++)
  {
    if (freq_signal[i].real == 0)
    {
      freq_signal_abs[i] = (uint16_t)WEBRTC_SPL_ABS_W16(freq_signal[i].imag);
    }
    else if (freq_signal[i].imag == 0)
    {
      freq_signal_abs[i] = (uint16_t)WEBRTC_SPL_ABS_W16(freq_signal[i].real);
    }
    else
    {
      // Approximation for magnitude of complex fft output
      // magn = sqrt(real^2 + imag^2)
      // magn ~= alpha * max(|imag|,|real|) + beta * min(|imag|,|real|)
      //
      // The parameters alpha and beta are stored in Q15

#ifdef AECM_WITH_ABS_APPROX
      tmp16no1 = WEBRTC_SPL_ABS_W16(freq_signal[i].real);
      tmp16no2 = WEBRTC_SPL_ABS_W16(freq_signal[i].imag);

      if(tmp16no1 > tmp16no2)
      {
        max_value = tmp16no1;
        min_value = tmp16no2;
      } else
      {
        max_value = tmp16no2;
        min_value = tmp16no1;
      }

      // Magnitude in Q(-6)
      if ((max_value >> 2) > min_value)
      {
        alpha = kAlpha1;
        beta = kBeta1;
      } else if ((max_value >> 1) > min_value)
      {
        alpha = kAlpha2;
        beta = kBeta2;
      } else
      {
        alpha = kAlpha3;
        beta = kBeta3;
      }
      tmp16no1 = (int16_t)((max_value * alpha) >> 15);
      tmp16no2 = (int16_t)((min_value * beta) >> 15);
      freq_signal_abs[i] = (uint16_t)tmp16no1 + (uint16_t)tmp16no2;
#else
#ifdef WEBRTC_ARCH_ARM_V7
      __asm __volatile(
        "smulbb %[tmp32no1], %[real], %[real]\n\t"
        "smlabb %[tmp32no2], %[imag], %[imag], %[tmp32no1]\n\t"
        :[tmp32no1]"+&r"(tmp32no1),
         [tmp32no2]"=r"(tmp32no2)
        :[real]"r"(freq_signal[i].real),
         [imag]"r"(freq_signal[i].imag)
      );
#else
      tmp16no1 = WEBRTC_SPL_ABS_W16(freq_signal[i].real);
      tmp16no2 = WEBRTC_SPL_ABS_W16(freq_signal[i].imag);
      tmp32no1 = tmp16no1 * tmp16no1;
      tmp32no2 = tmp16no2 * tmp16no2;
      tmp32no2 = WebRtcSpl_AddSatW32(tmp32no1, tmp32no2);
#endif // WEBRTC_ARCH_ARM_V7
      tmp32no1 = WebRtcSpl_SqrtFloor(tmp32no2);

      freq_signal_abs[i] = (uint16_t)tmp32no1;
#endif // AECM_WITH_ABS_APPROX
    }
    (*freq_signal_sum_abs) += (uint32_t)freq_signal_abs[i];
  }

  return time_signal_scaling;
}

int WebRtcAecm_ProcessBlock(AecmCore* aecm,
                            const int16_t* farend,
                            const int16_t* nearendNoisy,
                            const int16_t* nearendClean,
                            int16_t* output) {
  int i;

  uint32_t xfaSum;
  uint32_t dfaNoisySum;
  uint32_t dfaCleanSum;
  uint32_t echoEst32Gained;
  uint32_t tmpU32;

  int32_t tmp32no1;

  uint16_t xfa[PART_LEN1];
  uint16_t dfaNoisy[PART_LEN1];
  uint16_t dfaClean[PART_LEN1];
  uint16_t* ptrDfaClean = dfaClean;
  const uint16_t* far_spectrum_ptr = NULL;

  // 32 byte aligned buffers (with +8 or +16).
  // TODO(kma): define fft with ComplexInt16.
  int16_t fft_buf[PART_LEN4 + 2 + 16]; // +2 to make a loop safe.
  int32_t echoEst32_buf[PART_LEN1 + 8];
  int32_t dfw_buf[PART_LEN2 + 8];
  int32_t efw_buf[PART_LEN2 + 8];

  int16_t* fft = (int16_t*) (((uintptr_t) fft_buf + 31) & ~ 31);
  int32_t* echoEst32 = (int32_t*) (((uintptr_t) echoEst32_buf + 31) & ~ 31);
  ComplexInt16* dfw = (ComplexInt16*)(((uintptr_t)dfw_buf + 31) & ~31);
  ComplexInt16* efw = (ComplexInt16*)(((uintptr_t)efw_buf + 31) & ~31);

  int16_t hnl[PART_LEN1];
  int16_t numPosCoef = 0;
  int16_t nlpGain = ONE_Q14;
  int delay;
  int16_t tmp16no1;
  int16_t tmp16no2;
  int16_t mu;
  int16_t supGain;
  int16_t zeros32, zeros16;
  int16_t zerosDBufNoisy, zerosDBufClean, zerosXBuf;
  int far_q;
  int16_t resolutionDiff, qDomainDiff, dfa_clean_q_domain_diff;

  const int kMinPrefBand = 4;
  const int kMaxPrefBand = 24;
  int32_t avgHnl32 = 0;
    
    uint32_t eb =0;
    uint32_t ea =0;
    int16_t temp =0;

  // Determine startup state. There are three states:
  // (0) the first CONV_LEN blocks
  // (1) another CONV_LEN blocks
  // (2) the rest

  if (aecm->startupState < 2)
  {
    aecm->startupState = (aecm->totCount >= CONV_LEN) +
                         (aecm->totCount >= CONV_LEN2);
  }
  // END: Determine startup state

  // Buffer near and far end signals
  memcpy(aecm->xBuf + PART_LEN, farend, sizeof(int16_t) * PART_LEN);
  memcpy(aecm->dBufNoisy + PART_LEN, nearendNoisy, sizeof(int16_t) * PART_LEN);
  if (nearendClean != NULL)
  {
    memcpy(aecm->dBufClean + PART_LEN,
           nearendClean,
           sizeof(int16_t) * PART_LEN);
  }

  // Transform far end signal from time domain to frequency domain.
  far_q = TimeToFrequencyDomain(aecm,
                                aecm->xBuf,
                                dfw,
                                xfa,
                                &xfaSum);

  // Transform noisy near end signal from time domain to frequency domain.
  zerosDBufNoisy = TimeToFrequencyDomain(aecm,
                                         aecm->dBufNoisy,
                                         dfw,
                                         dfaNoisy,
                                         &dfaNoisySum);
  aecm->dfaNoisyQDomainOld = aecm->dfaNoisyQDomain;
  aecm->dfaNoisyQDomain = (int16_t)zerosDBufNoisy;
  if (nearendClean == NULL)
  {
    ptrDfaClean = dfaNoisy;
    aecm->dfaCleanQDomainOld = aecm->dfaNoisyQDomainOld;
    aecm->dfaCleanQDomain = aecm->dfaNoisyQDomain;
    dfaCleanSum = dfaNoisySum;
  } else
  {
    // Transform clean near end signal from time domain to frequency domain.
    zerosDBufClean = TimeToFrequencyDomain(aecm,
                                           aecm->dBufClean,
                                           dfw,
                                           dfaClean,
                                           &dfaCleanSum);
    aecm->dfaCleanQDomainOld = aecm->dfaCleanQDomain;
    aecm->dfaCleanQDomain = (int16_t)zerosDBufClean;
  }


  if (WebRtc_AddFarSpectrumFix(aecm->delay_estimator_farend,
                               xfa,
                               PART_LEN1,
                               far_q) == -1) {
    return -1;
  }
  delay = WebRtc_DelayEstimatorProcessFix(aecm->delay_estimator,
                                          dfaNoisy,
                                          PART_LEN1,
                                          zerosDBufNoisy);
  if (delay == -1)
  {
    return -1;
  }
  else if (delay == -2)
  {
    // If the delay is unknown, we assume zero.
    // NOTE: this will have to be adjusted if we ever add lookahead.
    delay = 0;
  }


  if (aecm->fixedDelay >= 0)
  {
    // Use fixed delay
    delay = aecm->fixedDelay;
  }
    aecm->currentDelay = delay +1 ;

    // Get the delay
    // Save far-end history and estimate delay
    WebRtcAecm_UpdateFarHistory(aecm, xfa,dfw, far_q);
  // Get aligned far end spectrum
  far_spectrum_ptr = WebRtcAecm_AlignedFarend(aecm, &far_q, delay);
  zerosXBuf = (int16_t) far_q;
  if (far_spectrum_ptr == NULL)
  {
    return -1;
  }

  // Calculate log(energy) and update energy threshold levels
  WebRtcAecm_CalcEnergies(aecm,
                          far_spectrum_ptr,
                          zerosXBuf,
                          dfaNoisySum,
                          echoEst32);

  // Calculate stepsize
  mu = WebRtcAecm_CalcStepSize(aecm);

  // Update counters
  aecm->totCount++;

  // This is the channel estimation algorithm.
  // It is base on NLMS but has a variable step length,
  // which was calculated above.
  WebRtcAecm_UpdateChannel(aecm,
                           far_spectrum_ptr,
                           zerosXBuf,
                           dfaNoisy,
                           mu,
                           echoEst32);
  supGain = WebRtcAecm_CalcSuppressionGain(aecm);


  // Calculate Wiener filter hnl[]
  for (i = 0; i < PART_LEN1; i++)
  {
    // Far end signal through channel estimate in Q8
    // How much can we shift right to preserve resolution
    tmp32no1 = WEBRTC_SPL_SHIFT_W32(echoEst32[i], -zerosXBuf) - aecm->echoFilt[i];
    aecm->echoFilt[i] += (tmp32no1 * 50) >> 8;

    zeros32 = WebRtcSpl_NormW32(aecm->echoFilt[i]) + 1;
    zeros16 = WebRtcSpl_NormW16(supGain) + 1;
    if (zeros32 + zeros16 > 16)
    {
      // Multiplication is safe
      // Result in
      // Q(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN+
      //   aecm->xfaQDomainBuf[diff])
      echoEst32Gained = WEBRTC_SPL_UMUL_32_16((uint32_t)aecm->echoFilt[i],
                                              (uint16_t)supGain);
      resolutionDiff = 14 - RESOLUTION_CHANNEL16 - RESOLUTION_SUPGAIN;
      resolutionDiff += (aecm->dfaCleanQDomain );
    } else
    {
      tmp16no1 = 17 - zeros32 - zeros16;
      resolutionDiff = 14 + tmp16no1 - RESOLUTION_CHANNEL16 -
                       RESOLUTION_SUPGAIN;
      resolutionDiff += (aecm->dfaCleanQDomain );
      if (zeros32 > tmp16no1)
      {
        echoEst32Gained = WEBRTC_SPL_UMUL_32_16((uint32_t)aecm->echoFilt[i],
                                                supGain >> tmp16no1);
      } else
      {
        // Result in Q-(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN-16)
        echoEst32Gained = (aecm->echoFilt[i] >> tmp16no1) * supGain;
      }
    }

    zeros16 = WebRtcSpl_NormW16(aecm->nearFilt[i]);
    assert(zeros16 >= 0);  // |zeros16| is a norm, hence non-negative.
    dfa_clean_q_domain_diff = aecm->dfaCleanQDomain - aecm->dfaCleanQDomainOld;
    if (zeros16 < dfa_clean_q_domain_diff && aecm->nearFilt[i]) {
      tmp16no1 = aecm->nearFilt[i] << zeros16;
      qDomainDiff = zeros16 - dfa_clean_q_domain_diff;
      tmp16no2 = ptrDfaClean[i] >> -qDomainDiff;
    } else {
      tmp16no1 = dfa_clean_q_domain_diff < 0
          ? aecm->nearFilt[i] >> -dfa_clean_q_domain_diff
          : aecm->nearFilt[i] << dfa_clean_q_domain_diff;
      qDomainDiff = 0;
      tmp16no2 = ptrDfaClean[i];
    }
    tmp32no1 = (int32_t)(tmp16no2 - tmp16no1);
    tmp16no2 = (int16_t)(tmp32no1 >> 4);
    tmp16no2 += tmp16no1;
    zeros16 = WebRtcSpl_NormW16(tmp16no2);
    if ((tmp16no2) & (-qDomainDiff > zeros16)) {
      aecm->nearFilt[i] = WEBRTC_SPL_WORD16_MAX;
    } else {
      aecm->nearFilt[i] = qDomainDiff < 0 ? tmp16no2 << -qDomainDiff
                                          : tmp16no2 >> qDomainDiff;
    }

    // Wiener filter coefficients, resulting hnl in Q14
    if (echoEst32Gained == 0)
    {
      hnl[i] = ONE_Q14;
    } else if (aecm->nearFilt[i] == 0)
    {
      hnl[i] = 0;
    } else
    {
      // Multiply the suppression gain
      // Rounding
      echoEst32Gained += (uint32_t)(aecm->nearFilt[i] >> 1);
      tmpU32 = WebRtcSpl_DivU32U16(echoEst32Gained,
                                   (uint16_t)aecm->nearFilt[i]);

      // Current resolution is
      // Q-(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN- max(0,17-zeros16- zeros32))
      // Make sure we are in Q14
      tmp32no1 = (int32_t)WEBRTC_SPL_SHIFT_W32(tmpU32, resolutionDiff);
      if (tmp32no1 > ONE_Q14)
      {
        hnl[i] = 0;
      } else if (tmp32no1 < 0)
      {
        hnl[i] = ONE_Q14;
      } else
      {
        // 1-echoEst/dfa
        hnl[i] = ONE_Q14 - (int16_t)tmp32no1;
        if (hnl[i] < 0)
        {
          hnl[i] = 0;
        }
      }
    }
    if (hnl[i])
    {
      numPosCoef++;
    }
  }
  // Only in wideband. Prevent the gain in upper band from being larger than
  // in lower band.
  if (aecm->mult == 2)
  {
    // TODO(bjornv): Investigate if the scaling of hnl[i] below can cause
    //               speech distortion in double-talk.
    for (i = 0; i < PART_LEN1; i++)
    {
      hnl[i] = (int16_t)((hnl[i] * hnl[i]) >> 14);
    }

    for (i = kMinPrefBand; i <= kMaxPrefBand; i++)
    {
      avgHnl32 += (int32_t)hnl[i];
    }
    assert(kMaxPrefBand - kMinPrefBand + 1 > 0);
    avgHnl32 /= (kMaxPrefBand - kMinPrefBand + 1);

    for (i = kMaxPrefBand; i < PART_LEN1; i++)
    {
      if (hnl[i] > (int16_t)avgHnl32)
      {
        hnl[i] = (int16_t)avgHnl32;
      }
    }
  }

  // Calculate NLP gain, result is in Q14
  if (aecm->nlpFlag)
  {
      eb =0;
      ea =0;
      for (i = 5; i < PART_LEN1; i++)
      {
           temp =0;
          temp = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                       hnl[i], 14));
          eb+= (uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
          ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].real, dfw[i].real)>>8;
          temp= (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                                       hnl[i], 14));
          eb+=(uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
          ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].imag, dfw[i].imag)>>8;
          
      }

      tmpU32=  WebRtcSpl_DivU32U16(eb, ea>>16);
      if (tmpU32 > 6231) {
          nlpGain = tmpU32 >> 2;
          if (nlpGain>ONE_Q14) {
              nlpGain = ONE_Q14;
          }
      }
      else
      {
          nlpGain =0;
      }
      eb =0;
      ea =0;
      for (i = 1; i < 15; i++)
      {
          temp =0;
          temp = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                hnl[i], 14));
          eb+= (uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
          ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].real, dfw[i].real)>>8;
          temp= (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                               hnl[i], 14));
          eb+=(uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
          ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].imag, dfw[i].imag)>>8;
          
      }
      tmpU32=  WebRtcSpl_DivU32U16(eb, ea>>16);
      tmp16no2 = ONE_Q14;
      if (nlpGain < (ONE_Q14>>7)) {
          if (tmpU32 < ONE_Q14 ) {
              tmp16no2 = eb >>22;
              if (tmp16no2>ONE_Q14) {
                  tmp16no2 = ONE_Q14;
              }
          }
      }
      


    for (i = 0; i < PART_LEN1; i++)
    {
      // Truncate values close to zero and one.
        if (i>15) {
            if (hnl[i] > NLP_COMP_HIGH)
            {
                hnl[i] = ONE_Q14;
            } else if (hnl[i] < NLP_COMP_LOW)
            {
                hnl[i] = 0;
            }
            
            
            // NLP
            if ((hnl[i] == ONE_Q14) && (nlpGain == ONE_Q14))
            {
                hnl[i] = ONE_Q14;
            } else
            {
                hnl[i] = (int16_t)((hnl[i] * nlpGain) >> 14);
            }

        }
        else
        {
            hnl[i] = (int16_t)((hnl[i] * tmp16no2) >> 14);
        }
      
      // multiply with Wiener coefficients

         efw[i].real = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
         hnl[i], 14));
         efw[i].imag = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
         hnl[i], 14));

    }
  }
  else
  {
    // multiply with Wiener coefficients
    for (i = 0; i < PART_LEN1; i++)
    {

     efw[i].real = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                   hnl[i], 14));
      efw[i].imag = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                              hnl[i], 14));
    }
  }


  if (aecm->cngMode == AecmTrue)
  {
    ComfortNoise(aecm, ptrDfaClean, efw, hnl);
  }

  InverseFFTAndWindow(aecm, fft, efw, output, nearendClean);

  return 0;
}

static void ComfortNoise(AecmCore* aecm,
                         const uint16_t* dfa,
                         ComplexInt16* out,
                         const int16_t* lambda) {
  int16_t i;
  int16_t tmp16;
  int32_t tmp32;

  int16_t randW16[PART_LEN];
  int16_t uReal[PART_LEN1];
  int16_t uImag[PART_LEN1];
  int32_t outLShift32;
  int16_t noiseRShift16[PART_LEN1];

  int16_t shiftFromNearToNoise = kNoiseEstQDomain - aecm->dfaCleanQDomain;
  int16_t minTrackShift;

  assert(shiftFromNearToNoise >= 0);
  assert(shiftFromNearToNoise < 16);

  if (aecm->noiseEstCtr < 100)
  {
    // Track the minimum more quickly initially.
    aecm->noiseEstCtr++;
    minTrackShift = 6;
  } else
  {
    minTrackShift = 9;
  }

  // Estimate noise power.
  for (i = 0; i < PART_LEN1; i++)
  {
    // Shift to the noise domain.
    tmp32 = (int32_t)dfa[i];
    outLShift32 = tmp32 << shiftFromNearToNoise;

    if (outLShift32 < aecm->noiseEst[i])
    {
      // Reset "too low" counter
      aecm->noiseEstTooLowCtr[i] = 0;
      // Track the minimum.
      if (aecm->noiseEst[i] < (1 << minTrackShift))
      {
        // For small values, decrease noiseEst[i] every
        // |kNoiseEstIncCount| block. The regular approach below can not
        // go further down due to truncation.
        aecm->noiseEstTooHighCtr[i]++;
        if (aecm->noiseEstTooHighCtr[i] >= kNoiseEstIncCount)
        {
          aecm->noiseEst[i]--;
          aecm->noiseEstTooHighCtr[i] = 0; // Reset the counter
        }
      }
      else
      {
        aecm->noiseEst[i] -= ((aecm->noiseEst[i] - outLShift32)
                              >> minTrackShift);
      }
    } else
    {
      // Reset "too high" counter
      aecm->noiseEstTooHighCtr[i] = 0;
      // Ramp slowly upwards until we hit the minimum again.
      if ((aecm->noiseEst[i] >> 19) > 0)
      {
        // Avoid overflow.
        // Multiplication with 2049 will cause wrap around. Scale
        // down first and then multiply
        aecm->noiseEst[i] >>= 11;
        aecm->noiseEst[i] *= 2049;
      }
      else if ((aecm->noiseEst[i] >> 11) > 0)
      {
        // Large enough for relative increase
        aecm->noiseEst[i] *= 2049;
        aecm->noiseEst[i] >>= 11;
      }
      else
      {
        // Make incremental increases based on size every
        // |kNoiseEstIncCount| block
        aecm->noiseEstTooLowCtr[i]++;
        if (aecm->noiseEstTooLowCtr[i] >= kNoiseEstIncCount)
        {
          aecm->noiseEst[i] += (aecm->noiseEst[i] >> 9) + 1;
          aecm->noiseEstTooLowCtr[i] = 0; // Reset counter
        }
      }
    }
  }

  for (i = 0; i < PART_LEN1; i++)
  {
    tmp32 = aecm->noiseEst[i] >> shiftFromNearToNoise;
    if (tmp32 > 32767)
    {
      tmp32 = 32767;
      aecm->noiseEst[i] = tmp32 << shiftFromNearToNoise;
    }
    noiseRShift16[i] = (int16_t)tmp32;

    tmp16 = ONE_Q14 - lambda[i];
    noiseRShift16[i] = (int16_t)((tmp16 * noiseRShift16[i]) >> 14);
  }

  // Generate a uniform random array on [0 2^15-1].
  WebRtcSpl_RandUArray(randW16, PART_LEN, &aecm->seed);

  // Generate noise according to estimated energy.
  uReal[0] = 0; // Reject LF noise.
  uImag[0] = 0;
  for (i = 1; i < PART_LEN1; i++)
  {
    // Get a random index for the cos and sin tables over [0 359].
    tmp16 = (int16_t)((359 * randW16[i - 1]) >> 15);

    // Tables are in Q13.
    uReal[i] = (int16_t)((noiseRShift16[i] * WebRtcAecm_kCosTable[tmp16]) >>
        13);
    uImag[i] = (int16_t)((-noiseRShift16[i] * WebRtcAecm_kSinTable[tmp16]) >>
        13);
  }
  uImag[PART_LEN] = 0;

  for (i = 0; i < PART_LEN1; i++)
  {
    out[i].real = WebRtcSpl_AddSatW16(out[i].real, uReal[i]);
    out[i].imag = WebRtcSpl_AddSatW16(out[i].imag, uImag[i]);
  }
}
#ifdef FLOATPRO
void CopyFixFFT2Float(ComplexInt16* freq_signal,int16_t zeros,float* fp){
    int i;
    float normalize =(zeros>0)?(1.f/(float)(1<<zeros)):((1<<(-zeros)));
   
    for (i=0; i< PART_LEN; i++) {
        fp[2*i] = (float)freq_signal[i].real * normalize;
        fp[2*i+1] =(float)freq_signal[i].imag * normalize;
    }
}
void PowerEstimation(float *fpin,float* fpout) {
    int i;
    float temp[PART_LEN] ={0};
    for (i=0 ; i< PART_LEN; i++) {
        temp[i]=fpin[2*i]*fpin[2*i]+fpin[2*i+1]*fpin[2*i+1];
    }
    
    fpout[0] *= 0.3f; //250ms
    fpout[0] += 0.7f*(temp[0]*0.9f+temp[1]*0.1f);
    for (i=1 ;i<48; i++) {
        fpout[i] *= 0.3f;
        fpout[i] += 0.7f*(temp[i-1]*0.1f+temp[i]*0.8f+temp[i+1]*0.1f);
    }
    for (i=48 ;i<PART_LEN-2; i++) {
        fpout[i] *= 0.3f;
        fpout[i] += 0.7f*(temp[i-1]*0.1f+temp[i]*0.7f+temp[i+1]*0.1f+temp[i+2]*0.05f+temp[i-2]*0.05f);
    }

    fpout[PART_LEN-2] *= 0.3f;
    fpout[PART_LEN-2] += 0.7f*(temp[PART_LEN-2]*0.7f+temp[PART_LEN-3]*0.1f+temp[PART_LEN-4]*0.05f+temp[PART_LEN-1]*0.15f);
    fpout[PART_LEN-1] *= 0.3f;
    fpout[PART_LEN-1] += 0.7f*(temp[PART_LEN-1]*0.85f+temp[PART_LEN-2]*0.1f+temp[PART_LEN-3]*0.05f);
}
void NoiseTrack(AecmCore* aecm)
{
    int i;
    //float temp[PART_LEN] ={0};
    float smooth_alpah=0.;
    float freqgain=1.f;
    float minnoise =0.f;
    for(i=0;i<PART_LEN;i++) {
        if (i>32) {
            freqgain = (16.f)/(i-16.f);
        }
        minnoise=freqgain*NOISEGATE;
        //farnoise
        if (aecm->aecmfloat.FarNoise[i]>=aecm->aecmfloat.FarPower[i]) {
            aecm->aecmfloat.FarNoise[i]=aecm->aecmfloat.FarPower[i]>minnoise?aecm->aecmfloat.FarPower[i]:minnoise;
        }else {
            smooth_alpah = aecm->aecmfloat.FarNoise[i]/10.f/aecm->aecmfloat.FarPower[i];
            smooth_alpah = smooth_alpah>0.01f?smooth_alpah:0.01f;
            
            aecm->aecmfloat.FarNoise[i] += smooth_alpah*(aecm->aecmfloat.FarPower[i]-aecm->aecmfloat.FarNoise[i]);
        }
        //near noise
        if (aecm->aecmfloat.NearNoise[i]>=aecm->aecmfloat.NearPower[i]) {
            aecm->aecmfloat.NearNoise[i]=aecm->aecmfloat.NearPower[i]>minnoise?aecm->aecmfloat.NearPower[i]:minnoise;
        }else {
            smooth_alpah = aecm->aecmfloat.NearNoise[i]/10.f/aecm->aecmfloat.NearPower[i];
            smooth_alpah = smooth_alpah>0.01f?smooth_alpah:0.01f;
            aecm->aecmfloat.NearNoise[i] += smooth_alpah*(aecm->aecmfloat.NearPower[i]-aecm->aecmfloat.NearNoise[i]);
        }
        
    }
    
    
}
void UpdateH(AecmCore* aecm) {
    int i,delay;
    float real,imag;
    delay =(aecm->aecmfloat.FarWritePosition - aecm->currentDelay + MAX_DELAY)%MAX_DELAY * PART_LEN2;
    for (i=0; i<PART_LEN; i++) {
        if (aecm->aecmfloat.FarPower[i]>1.4*aecm->aecmfloat.FarNoise[i]) {
            real = aecm->aecmfloat.FarSpectrumHistory[delay + 2*i] *aecm->aecmfloat.NearSpectrum[2*i] +
                   aecm->aecmfloat.FarSpectrumHistory[delay + 2*i+1]*aecm->aecmfloat.NearSpectrum[2*i+1];
            imag = aecm->aecmfloat.FarSpectrumHistory[delay + 2*i] *aecm->aecmfloat.NearSpectrum[2*i+1] -
                   aecm->aecmfloat.FarSpectrumHistory[delay + 2*i+1]*aecm->aecmfloat.NearSpectrum[2*i];
            aecm->aecmfloat.CrossRelation[2*i] += aecm->aecmfloat.SmoothFactor*(real -aecm->aecmfloat.CrossRelation[2*i]);
            aecm->aecmfloat.CrossRelation[2*i+1] += aecm->aecmfloat.SmoothFactor*(imag -aecm->aecmfloat.CrossRelation[2*i+1]);
            
            real = aecm->aecmfloat.FarSpectrumHistory[delay + 2*i] *aecm->aecmfloat.FarSpectrumHistory[delay +2*i] +
                   aecm->aecmfloat.FarSpectrumHistory[delay + 2*i+1]*aecm->aecmfloat.FarSpectrumHistory[delay +2*i+1];
            
            aecm->aecmfloat.AutoRelation[2*i] += aecm->aecmfloat.SmoothFactor*(real -aecm->aecmfloat.AutoRelation[2*i]);
            
            aecm->aecmfloat.AutoRelation[2*i+1] = 0.f;
            real = aecm->aecmfloat.CrossRelation[2*i] / (aecm->aecmfloat.AutoRelation[2*i]+ .1f*aecm->aecmfloat.FarNoise[i]);
            imag = aecm->aecmfloat.CrossRelation[2*i+1] / (aecm->aecmfloat.AutoRelation[2*i]+ .1f*aecm->aecmfloat.FarNoise[i]);
            aecm->aecmfloat.H[2*i] = (real);
            aecm->aecmfloat.H[2*i+1] = (imag);
        }
        
        aecm->aecmfloat.EchoEstSpectrum[2*i] = aecm->aecmfloat.H[2*i] * aecm->aecmfloat.FarSpectrumHistory[delay + 2*i]
                                            - aecm->aecmfloat.H[2*i+1] * aecm->aecmfloat.FarSpectrumHistory[delay + 2*i+1];
        aecm->aecmfloat.EchoEstSpectrum[2*i+1] = aecm->aecmfloat.H[2*i+1] * aecm->aecmfloat.FarSpectrumHistory[delay + 2*i]
                                            + aecm->aecmfloat.H[2*i] * aecm->aecmfloat.FarSpectrumHistory[delay + 2*i+1];
        aecm->aecmfloat.ErrorSpectrum[2*i] = aecm->aecmfloat.NearSpectrum[2*i] - aecm->aecmfloat.EchoEstSpectrum[2*i];
        aecm->aecmfloat.ErrorSpectrum[2*i+1] = aecm->aecmfloat.NearSpectrum[2*i+1] - aecm->aecmfloat.EchoEstSpectrum[2*i+1];
        real = (aecm->aecmfloat.ErrorSpectrum[2*i]*aecm->aecmfloat.ErrorSpectrum[2*i])
               +(aecm->aecmfloat.ErrorSpectrum[2*i+1]*aecm->aecmfloat.ErrorSpectrum[2*i+1]);
        imag = (aecm->aecmfloat.NearSpectrum[2*i]*aecm->aecmfloat.NearSpectrum[2*i])
               +(aecm->aecmfloat.NearSpectrum[2*i+1]*aecm->aecmfloat.NearSpectrum[2*i+1]);
        if (real>imag) {
            aecm->aecmfloat.ErrorSpectrum[2*i] = aecm->aecmfloat.NearSpectrum[2*i];
            aecm->aecmfloat.ErrorSpectrum[2*i+1] = aecm->aecmfloat.NearSpectrum[2*i+1];
        }
    }
}
void CopyFloatFFT2Fix(ComplexInt16* freq_signal,int16_t zeros,float* fp) {
    int i,tempvalue;
    float normalize =(zeros>0)?((float)(1<<zeros)):(1.f/(float)(1<<(-zeros)));
    
    for (i=0; i< PART_LEN; i++) {
        tempvalue = (int) (fp[2*i] * normalize);
        if(tempvalue>32767) {
            freq_signal[i].real = 32767;
        }
        else if (tempvalue<-32768) {
            freq_signal[i].real = -32768;
        }
        else {
            freq_signal[i].real = (int16_t) tempvalue;
        }
        
        tempvalue = (int)( fp[2*i+1] * normalize);
        if(tempvalue>32767) {
            freq_signal[i].imag = 32767;
        }
        else if (tempvalue<-32768) {
            freq_signal[i].imag = -32768;
        }
        else {
            freq_signal[i].imag = (int16_t) tempvalue;
        }
        
    }

}
void NLP(AecmCore* aecm) {
    int i;
    float ratio,gain,gainarray[PART_LEN];
    float allband1,allband2,allband3;
    allband1 =0.f;
    allband2 =0.f;
    allband3 =0.f;
    gainarray[0] =0.f;
    gain = 1.f;
    for (i=1; i<PART_LEN; i++) {
        gain = 1.f;
        if (aecm->aecmfloat.NearPower[i]>aecm->aecmfloat.NearNoise[i] ) {
            ratio = aecm->aecmfloat.ErrorPower[i]/aecm->aecmfloat.NearPower[i];
            if (i>32) {
                gain = (ratio-0.25f)/(0.65f);
            }
            else {
                gain = (ratio-0.2f)/0.3f;
                
            }
            gain =gain>0.00000001?gain:0.f;
            gain *=gain;
            gain = gain<0.001f?0.001f:gain;
            gain = gain >1.f?1.f:gain;
        }
        gainarray[i] = gain;
        allband1 += aecm->aecmfloat.ErrorPower[i]*gain*gain;
        allband2 += aecm->aecmfloat.EchoPower[i];
        allband3 += aecm->aecmfloat.NearPower[i];
        
    }
    if (allband2>0.00001f) {
        gain = (allband1/allband2);
        gain *= 0.707f;
        gain =gain <1.f?gain:1.f;
        
    }
    if (allband3>0.001f) {
        ratio = (allband1-0.001*allband3)/(0.499f*allband3);
        ratio = ratio>1.f?1.f:ratio;
        allband2=(32768*32768.f*0.05f-allband3)/(32768*32768.f*0.0499999f);
        allband2= allband2>1.f?1.f:allband2;
        allband2= allband2<0.001f?0.001f:allband2;
        ratio = ratio<0.1f*allband2?0.1f*allband2:ratio;
        ratio *= gain;
    }
    else
    {
        ratio = 1.f;
    }
    
    gain = gainarray[0]*0.8f+0.2f*gainarray[1];
    aecm->aecmfloat.gain[0] += 0.7f*(gain-aecm->aecmfloat.gain[0]);
    aecm->aecmfloat.ErrorSpectrum[0]*= aecm->aecmfloat.gain[0];
    aecm->aecmfloat.ErrorSpectrum[1]*= aecm->aecmfloat.gain[0];
    for (i=1; i<32; i++) {
        gain = gainarray[i]*0.7f+0.15f*gainarray[i+1]+0.15f*gainarray[i-1];
        aecm->aecmfloat.gain[i] += 0.7f*(gain-aecm->aecmfloat.gain[i]);
        if (allband2>0.8f) {
            gain = ratio >0.315f?ratio:0.315f;
        }
        else{
            gain = ratio >0.315f*(allband2/0.8f)?ratio:0.315f*(allband2/0.8f);
        }
        
        
        aecm->aecmfloat.ErrorSpectrum[2*i]*=aecm->aecmfloat.gain[i]*gain;
        aecm->aecmfloat.ErrorSpectrum[2*i+1]*=aecm->aecmfloat.gain[i]*gain;
    }
    for (i=32; i<PART_LEN-2; i++) {
        gain = gainarray[i]*0.5f+0.15f*gainarray[i+1]+0.15f*gainarray[i-1]+0.1f*gainarray[i+2]+0.1f*gainarray[i-2];
        aecm->aecmfloat.gain[i] += 0.7f*(gain-aecm->aecmfloat.gain[i]);
        aecm->aecmfloat.ErrorSpectrum[2*i]*=aecm->aecmfloat.gain[i]*ratio;
        aecm->aecmfloat.ErrorSpectrum[2*i+1]*=aecm->aecmfloat.gain[i]*ratio;
    }
    gain = gainarray[PART_LEN-1]*0.8f+0.2f*gainarray[PART_LEN-2];
    aecm->aecmfloat.gain[PART_LEN-1] += 0.8f*(gain-aecm->aecmfloat.gain[PART_LEN-1]);
    aecm->aecmfloat.ErrorSpectrum[2*PART_LEN-2]*=aecm->aecmfloat.gain[PART_LEN-1]*ratio;
    aecm->aecmfloat.ErrorSpectrum[2*PART_LEN-1]*=aecm->aecmfloat.gain[PART_LEN-1]*ratio;
    
}
int WebRtcAecm_ProcessBlockwithFloat(AecmCore* aecm,
                                     const int16_t* farend,
                                     const int16_t* nearendNoisy,
                                     const int16_t* nearendClean,
                                     int16_t* output){
    int i;
    
    uint32_t xfaSum;
    uint32_t dfaNoisySum;
    uint32_t dfaCleanSum;
    uint32_t echoEst32Gained;
    uint32_t tmpU32;
    
    int32_t tmp32no1;
    
    uint16_t xfa[PART_LEN1];
    uint16_t dfaNoisy[PART_LEN1];
    uint16_t dfaClean[PART_LEN1];
    uint16_t* ptrDfaClean = dfaClean;
    const uint16_t* far_spectrum_ptr = NULL;
    
    // 32 byte aligned buffers (with +8 or +16).
    // TODO(kma): define fft with ComplexInt16.
    int16_t fft_buf[PART_LEN4 + 2 + 16]; // +2 to make a loop safe.
    int32_t echoEst32_buf[PART_LEN1 + 8];
    int32_t dfw_buf[PART_LEN2 + 8];
    int32_t efw_buf[PART_LEN2 + 8];
    
    int16_t* fft = (int16_t*) (((uintptr_t) fft_buf + 31) & ~ 31);
    int32_t* echoEst32 = (int32_t*) (((uintptr_t) echoEst32_buf + 31) & ~ 31);
    ComplexInt16* dfw = (ComplexInt16*)(((uintptr_t)dfw_buf + 31) & ~31);
    ComplexInt16* efw = (ComplexInt16*)(((uintptr_t)efw_buf + 31) & ~31);
    
    int16_t hnl[PART_LEN1];
    int16_t numPosCoef = 0;
    int16_t nlpGain = ONE_Q14;
    int delay;
    int16_t tmp16no1;
    int16_t tmp16no2;
    int16_t mu;
    int16_t supGain;
    int16_t zeros32, zeros16;
    int16_t zerosDBufNoisy, zerosDBufClean, zerosXBuf;
    int far_q;
    int16_t resolutionDiff, qDomainDiff, dfa_clean_q_domain_diff;
    
    const int kMinPrefBand = 4;
    const int kMaxPrefBand = 24;
    int32_t avgHnl32 = 0;
    
    uint32_t eb =0;
    uint32_t ea =0;
    int16_t temp =0;
    
    // Determine startup state. There are three states:
    // (0) the first CONV_LEN blocks
    // (1) another CONV_LEN blocks
    // (2) the rest
    
    if (aecm->startupState < 2)
    {
        aecm->startupState = (aecm->totCount >= CONV_LEN) +
        (aecm->totCount >= CONV_LEN2);
    }
    // END: Determine startup state
    
    // Buffer near and far end signals
    memcpy(aecm->xBuf + PART_LEN, farend, sizeof(int16_t) * PART_LEN);
    memcpy(aecm->dBufNoisy + PART_LEN, nearendNoisy, sizeof(int16_t) * PART_LEN);
    if (nearendClean != NULL)
    {
        memcpy(aecm->dBufClean + PART_LEN,
               nearendClean,
               sizeof(int16_t) * PART_LEN);
    }
    
    // Transform far end signal from time domain to frequency domain.
    far_q = TimeToFrequencyDomain(aecm,
                                  aecm->xBuf,
                                  dfw,
                                  xfa,
                                  &xfaSum);
#ifdef FLOATPRO
    if(1==aecm->enablefloatpro) {
        CopyFixFFT2Float(dfw,far_q,aecm->aecmfloat.FarSpectrumHistory+aecm->aecmfloat.FarWritePosition*PART_LEN2);
        (++aecm->aecmfloat.FarWritePosition);
        aecm->aecmfloat.FarWritePosition %= MAX_DELAY;
    }
#endif
    // Transform noisy near end signal from time domain to frequency domain.
    zerosDBufNoisy = TimeToFrequencyDomain(aecm,
                                           aecm->dBufNoisy,
                                           dfw,
                                           dfaNoisy,
                                           &dfaNoisySum);
    aecm->dfaNoisyQDomainOld = aecm->dfaNoisyQDomain;
    aecm->dfaNoisyQDomain = (int16_t)zerosDBufNoisy;
#ifdef FLOATPRO
    if(1==aecm->enablefloatpro) {
        CopyFixFFT2Float(dfw,aecm->dfaNoisyQDomain,aecm->aecmfloat.NearSpectrum);
    }
#endif
    if (nearendClean == NULL)
    {
        ptrDfaClean = dfaNoisy;
        aecm->dfaCleanQDomainOld = aecm->dfaNoisyQDomainOld;
        aecm->dfaCleanQDomain = aecm->dfaNoisyQDomain;
        dfaCleanSum = dfaNoisySum;
    } else
    {
        // Transform clean near end signal from time domain to frequency domain.
        zerosDBufClean = TimeToFrequencyDomain(aecm,
                                               aecm->dBufClean,
                                               dfw,
                                               dfaClean,
                                               &dfaCleanSum);
        aecm->dfaCleanQDomainOld = aecm->dfaCleanQDomain;
        aecm->dfaCleanQDomain = (int16_t)zerosDBufClean;
    }
    
    
    if (WebRtc_AddFarSpectrumFix(aecm->delay_estimator_farend,
                                 xfa,
                                 PART_LEN1,
                                 far_q) == -1) {
        return -1;
    }
    delay = WebRtc_DelayEstimatorProcessFix(aecm->delay_estimator,
                                            dfaNoisy,
                                            PART_LEN1,
                                            zerosDBufNoisy);
    if (delay == -1)
    {
        return -1;
    }
    else if (delay == -2)
    {
        // If the delay is unknown, we assume zero.
        // NOTE: this will have to be adjusted if we ever add lookahead.
        delay = 0;
    }
    
    
    if (aecm->fixedDelay >= 0)
    {
        // Use fixed delay
        delay = aecm->fixedDelay;
    }
    aecm->currentDelay = delay +1 ;
#ifdef FLOATPRO
    if (1==aecm->enablefloatpro) {
        PowerEstimation(aecm->aecmfloat.NearSpectrum,aecm->aecmfloat.NearPower);
        delay = (aecm->aecmfloat.FarWritePosition - aecm->currentDelay +MAX_DELAY)%MAX_DELAY;
        PowerEstimation(aecm->aecmfloat.FarSpectrumHistory+delay*PART_LEN2,aecm->aecmfloat.FarPower);
        NoiseTrack(aecm);
        UpdateH(aecm);
        PowerEstimation(aecm->aecmfloat.ErrorSpectrum, aecm->aecmfloat.ErrorPower);
        PowerEstimation(aecm->aecmfloat.EchoEstSpectrum, aecm->aecmfloat.EchoPower);
        NLP(aecm);
    }
    delay = aecm->currentDelay -1;
#endif
    // Get the delay
    // Save far-end history and estimate delay
    WebRtcAecm_UpdateFarHistory(aecm, xfa,dfw, far_q);
    // Get aligned far end spectrum
    far_spectrum_ptr = WebRtcAecm_AlignedFarend(aecm, &far_q, delay);
    zerosXBuf = (int16_t) far_q;
    if (far_spectrum_ptr == NULL)
    {
        return -1;
    }
    
    // Calculate log(energy) and update energy threshold levels
    WebRtcAecm_CalcEnergies(aecm,
                            far_spectrum_ptr,
                            zerosXBuf,
                            dfaNoisySum,
                            echoEst32);
    
    // Calculate stepsize
    mu = WebRtcAecm_CalcStepSize(aecm);
    
    // Update counters
    aecm->totCount++;
    
    // This is the channel estimation algorithm.
    // It is base on NLMS but has a variable step length,
    // which was calculated above.
    WebRtcAecm_UpdateChannel(aecm,
                             far_spectrum_ptr,
                             zerosXBuf,
                             dfaNoisy,
                             mu,
                             echoEst32);
    supGain = WebRtcAecm_CalcSuppressionGain(aecm);
    
    
    // Calculate Wiener filter hnl[]
    for (i = 0; i < PART_LEN1; i++)
    {
        // Far end signal through channel estimate in Q8
        // How much can we shift right to preserve resolution
        tmp32no1 = WEBRTC_SPL_SHIFT_W32(echoEst32[i], -zerosXBuf) - aecm->echoFilt[i];
        aecm->echoFilt[i] += (tmp32no1 * 50) >> 8;
        
        zeros32 = WebRtcSpl_NormW32(aecm->echoFilt[i]) + 1;
        zeros16 = WebRtcSpl_NormW16(supGain) + 1;
        if (zeros32 + zeros16 > 16)
        {
            // Multiplication is safe
            // Result in
            // Q(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN+
            //   aecm->xfaQDomainBuf[diff])
            echoEst32Gained = WEBRTC_SPL_UMUL_32_16((uint32_t)aecm->echoFilt[i],
                                                    (uint16_t)supGain);
            resolutionDiff = 14 - RESOLUTION_CHANNEL16 - RESOLUTION_SUPGAIN;
            resolutionDiff += (aecm->dfaCleanQDomain );
        } else
        {
            tmp16no1 = 17 - zeros32 - zeros16;
            resolutionDiff = 14 + tmp16no1 - RESOLUTION_CHANNEL16 -
            RESOLUTION_SUPGAIN;
            resolutionDiff += (aecm->dfaCleanQDomain );
            if (zeros32 > tmp16no1)
            {
                echoEst32Gained = WEBRTC_SPL_UMUL_32_16((uint32_t)aecm->echoFilt[i],
                                                        supGain >> tmp16no1);
            } else
            {
                // Result in Q-(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN-16)
                echoEst32Gained = (aecm->echoFilt[i] >> tmp16no1) * supGain;
            }
        }
        
        zeros16 = WebRtcSpl_NormW16(aecm->nearFilt[i]);
        assert(zeros16 >= 0);  // |zeros16| is a norm, hence non-negative.
        dfa_clean_q_domain_diff = aecm->dfaCleanQDomain - aecm->dfaCleanQDomainOld;
        if (zeros16 < dfa_clean_q_domain_diff && aecm->nearFilt[i]) {
            tmp16no1 = aecm->nearFilt[i] << zeros16;
            qDomainDiff = zeros16 - dfa_clean_q_domain_diff;
            tmp16no2 = ptrDfaClean[i] >> -qDomainDiff;
        } else {
            tmp16no1 = dfa_clean_q_domain_diff < 0
            ? aecm->nearFilt[i] >> -dfa_clean_q_domain_diff
            : aecm->nearFilt[i] << dfa_clean_q_domain_diff;
            qDomainDiff = 0;
            tmp16no2 = ptrDfaClean[i];
        }
        tmp32no1 = (int32_t)(tmp16no2 - tmp16no1);
        tmp16no2 = (int16_t)(tmp32no1 >> 4);
        tmp16no2 += tmp16no1;
        zeros16 = WebRtcSpl_NormW16(tmp16no2);
        if ((tmp16no2) & (-qDomainDiff > zeros16)) {
            aecm->nearFilt[i] = WEBRTC_SPL_WORD16_MAX;
        } else {
            aecm->nearFilt[i] = qDomainDiff < 0 ? tmp16no2 << -qDomainDiff
            : tmp16no2 >> qDomainDiff;
        }
        
        // Wiener filter coefficients, resulting hnl in Q14
        if (echoEst32Gained == 0)
        {
            hnl[i] = ONE_Q14;
        } else if (aecm->nearFilt[i] == 0)
        {
            hnl[i] = 0;
        } else
        {
            // Multiply the suppression gain
            // Rounding
            echoEst32Gained += (uint32_t)(aecm->nearFilt[i] >> 1);
            tmpU32 = WebRtcSpl_DivU32U16(echoEst32Gained,
                                         (uint16_t)aecm->nearFilt[i]);
            
            // Current resolution is
            // Q-(RESOLUTION_CHANNEL+RESOLUTION_SUPGAIN- max(0,17-zeros16- zeros32))
            // Make sure we are in Q14
            tmp32no1 = (int32_t)WEBRTC_SPL_SHIFT_W32(tmpU32, resolutionDiff);
            if (tmp32no1 > ONE_Q14)
            {
                hnl[i] = 0;
            } else if (tmp32no1 < 0)
            {
                hnl[i] = ONE_Q14;
            } else
            {
                // 1-echoEst/dfa
                hnl[i] = ONE_Q14 - (int16_t)tmp32no1;
                if (hnl[i] < 0)
                {
                    hnl[i] = 0;
                }
            }
        }
        if (hnl[i])
        {
            numPosCoef++;
        }
    }
    // Only in wideband. Prevent the gain in upper band from being larger than
    // in lower band.
    if (aecm->mult == 2)
    {
        // TODO(bjornv): Investigate if the scaling of hnl[i] below can cause
        //               speech distortion in double-talk.
        for (i = 0; i < PART_LEN1; i++)
        {
            hnl[i] = (int16_t)((hnl[i] * hnl[i]) >> 14);
        }
        
        for (i = kMinPrefBand; i <= kMaxPrefBand; i++)
        {
            avgHnl32 += (int32_t)hnl[i];
        }
        assert(kMaxPrefBand - kMinPrefBand + 1 > 0);
        avgHnl32 /= (kMaxPrefBand - kMinPrefBand + 1);
        
        for (i = kMaxPrefBand; i < PART_LEN1; i++)
        {
            if (hnl[i] > (int16_t)avgHnl32)
            {
                hnl[i] = (int16_t)avgHnl32;
            }
        }
    }
    
    // Calculate NLP gain, result is in Q14
    if (aecm->nlpFlag)
    {
        eb =0;
        ea =0;
        for (i = 5; i < PART_LEN1; i++)
        {
            temp =0;
            temp = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                  hnl[i], 14));
            eb+= (uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
            ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].real, dfw[i].real)>>8;
            temp= (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                                 hnl[i], 14));
            eb+=(uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
            ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].imag, dfw[i].imag)>>8;
            
        }
        
        tmpU32=  WebRtcSpl_DivU32U16(eb, ea>>16);
        if (tmpU32 > 6231) {
            nlpGain = tmpU32 >> 2;
            if (nlpGain>ONE_Q14) {
                nlpGain = ONE_Q14;
            }
        }
        else
        {
            nlpGain =0;
        }
        eb =0;
        ea =0;
        for (i = 1; i < 15; i++)
        {
            temp =0;
            temp = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                  hnl[i], 14));
            eb+= (uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
            ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].real, dfw[i].real)>>8;
            temp= (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                                 hnl[i], 14));
            eb+=(uint32_t)WEBRTC_SPL_MUL_16_U16(temp, temp)>>8;
            ea+= (uint32_t)WEBRTC_SPL_MUL_16_U16(dfw[i].imag, dfw[i].imag)>>8;
            
        }
        tmpU32=  WebRtcSpl_DivU32U16(eb, ea>>16);
        tmp16no2 = ONE_Q14;
        if (nlpGain < (ONE_Q14>>7)) {
            if (tmpU32 < ONE_Q14 ) {
                tmp16no2 = eb >>22;
                if (tmp16no2>ONE_Q14) {
                    tmp16no2 = ONE_Q14;
                }
            }
        }
        
        
        
        for (i = 0; i < PART_LEN1; i++)
        {
            // Truncate values close to zero and one.
            if (i>15) {
                if (hnl[i] > NLP_COMP_HIGH)
                {
                    hnl[i] = ONE_Q14;
                } else if (hnl[i] < NLP_COMP_LOW)
                {
                    hnl[i] = 0;
                }
                
                
                // NLP
                if ((hnl[i] == ONE_Q14) && (nlpGain == ONE_Q14))
                {
                    hnl[i] = ONE_Q14;
                } else
                {
                    hnl[i] = (int16_t)((hnl[i] * nlpGain) >> 14);
                }
                
            }
            else
            {
                hnl[i] = (int16_t)((hnl[i] * tmp16no2) >> 14);
            }
            
            // multiply with Wiener coefficients

            efw[i].real = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                         hnl[i], 14));
            efw[i].imag = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                                         hnl[i], 14));

        }
    }
    else
    {
        // multiply with Wiener coefficients
        for (i = 0; i < PART_LEN1; i++)
        {

            efw[i].real = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].real,
                                                                         hnl[i], 14));
            efw[i].imag = (int16_t)(WEBRTC_SPL_MUL_16_16_RSFT_WITH_ROUND(dfw[i].imag,
                                                                         hnl[i], 14));

        }
    }
#ifdef FLOATPRO
    if (1 == aecm->enablefloatpro && aecm->currentDelay>1) {
        //delay =(aecm->aecmfloat.FarWritePosition - aecm->currentDelay + MAX_DELAY)%MAX_DELAY * PART_LEN2;
        CopyFloatFFT2Fix(efw, aecm->dfaNoisyQDomain,aecm->aecmfloat.ErrorSpectrum);
    }
    
#endif
    
    if (aecm->cngMode == AecmTrue)
    {
        //ComfortNoise(aecm, ptrDfaClean, efw, hnl);
    }
    
    InverseFFTAndWindow(aecm, fft, efw, output, nearendClean);
    
    return 0;
}
#endif
