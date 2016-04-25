# enter uper layer path
cd ../

echo "Start build Debug version"
#build Debug version
ninja -C out/Debug  audio_device_java
ninja -C out/Debug  audio_coding_module
ninja -C out/Debug  audio_conference_mixer
ninja -C out/Debug  audio_decoder_interface
ninja -C out/Debug  audio_encoder_interfac
ninja -C out/Debug  audio_processing
ninja -C out/Debug  audio_processing_neon
ninja -C out/Debug  audioproc_debug_proto
ninja -C out/Debug  bitrate_controller
ninja -C out/Debug  cng
ninja -C out/Debug  common_audio
ninja -C out/Debug  common_audio_neon
ninja -C out/Debug  cpu_features
ninja -C out/Debug  cpu_features_android
ninja -C out/Debug  g711
ninja -C out/Debug  g722
ninja -C out/Debug  gflagsv
ninja -C out/Debug  ilbc
ninja -C out/Debug  isac
ninja -C out/Debug  isac_neon
ninja -C out/Debug  media_file
ninja -C out/Debug  metrics_default
ninja -C out/Debug  neteq
ninja -C out/Debug  openmax_dl
ninja -C out/Debug  openmax_dl_armv7
ninja -C out/Debug  openmax_dl_neon
ninja -C out/Debug  opus
ninja -C out/Debug  paced_sender
ninja -C out/Debug  pcm16b
ninja -C out/Debug  protobuf_lite
ninja -C out/Debug  redv
ninja -C out/Debug  remote_bitrate_estimator
ninja -C out/Debug  rent_a_codec
ninja -C out/Debug  rtc_base
ninja -C out/Debug  rtc_base_approved
ninja -C out/Debug  rtc_event_log
ninja -C out/Debug  rtc_event_log_protov
ninja -C out/Debug  rtp_rtcp
ninja -C out/Debug  system_wrappersv
ninja -C out/Debug  voice_engine
ninja -C out/Debug  webrtc_common
ninja -C out/Debug  webrtc_opus
ninja -C out/Debug  webrtc_utility
echo "end of building Debug version"

echo "start build release version"
#build release version
@ninja -C out/Release
ninja -C out/Release  audio_device_java
ninja -C out/Release  audio_coding_module
ninja -C out/Release  audio_conference_mixer
ninja -C out/Release  audio_decoder_interface
ninja -C out/Release  audio_encoder_interfac
ninja -C out/Release  audio_processing
ninja -C out/Release  audio_processing_neon
ninja -C out/Release  audioproc_debug_proto
ninja -C out/Release  bitrate_controller
ninja -C out/Release  cng
ninja -C out/Release  common_audio
ninja -C out/Release  common_audio_neon
ninja -C out/Release  cpu_features
ninja -C out/Release  cpu_features_android
ninja -C out/Release  g711
ninja -C out/Release  g722
ninja -C out/Release  gflagsv
ninja -C out/Release  ilbc
ninja -C out/Release  isac
ninja -C out/Release  isac_neon
ninja -C out/Release  media_file
ninja -C out/Release  metrics_default
ninja -C out/Release  neteq
ninja -C out/Release  openmax_dl
ninja -C out/Release  openmax_dl_armv7
ninja -C out/Release  openmax_dl_neon
ninja -C out/Release  opus
ninja -C out/Release  paced_sender
ninja -C out/Release  pcm16b
ninja -C out/Release  protobuf_lite
ninja -C out/Release  redv
ninja -C out/Release  remote_bitrate_estimator
ninja -C out/Release  rent_a_codec
ninja -C out/Release  rtc_base
ninja -C out/Release  rtc_base_approved
ninja -C out/Release  rtc_event_log
ninja -C out/Release  rtc_event_log_protov
ninja -C out/Release  rtp_rtcp
ninja -C out/Release  system_wrappersv
ninja -C out/Release  voice_engine
ninja -C out/Release  webrtc_common
ninja -C out/Release  webrtc_opus
ninja -C out/Release  webrtc_utility
echo "end of building release version"

#create package foler
rm -rf package
mkdir package
cd package
mkdir include
mkdir Debug
mkdir Release
cd ../

#copy head files
echo "Start copy header files"
cp ./webrtc/voice_engine/include/* ./package/include
cp ./webrtc/typedefs.h ./package/include
cp ./webrtc/transport.h ./package/include
cp ./webrtc/common_types.h ./package/include
echo "end of copying header files"

#copy Debug version libs
echo "Start to copy Debug libs"
cp ./out/Debug/lib.java/audio_device_java.jar ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_coding_module.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_conference_mixer.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_decoder_interface.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_device.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_encoder_interface.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_processing.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudio_processing_neon.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libaudioproc_debug_proto.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libbitrate_controller.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libcng.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/common_audio/libcommon_audio.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/common_audio/libcommon_audio_neon.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/build/android/libcpu_features.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/system_wrappers/libcpu_features_android.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libg711.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libg722.a ./package/Debug -v
cp ./out/Debug/obj/third_party/gflags/libgflags.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libilbc.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libisac.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libisac_neon.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libmedia_file.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/system_wrappers/libmetrics_default.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libneteq.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl_armv7.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl_neon.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/third_party/opus/libopus.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libpaced_sender.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libpcm16b.a ./package/Debug -v
cp ./out/Debug/obj/chromium/src/third_party/protobuf/libprotobuf_lite.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libred.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libremote_bitrate_estimator.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/librent_a_codec.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/base/librtc_base.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/base/librtc_base_approved.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/librtc_event_log.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/librtc_event_log_proto.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/librtp_rtcp.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/system_wrappers/libsystem_wrappers.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/voice_engine/libvoice_engine.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/libwebrtc_common.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libwebrtc_opus.a ./package/Debug -v
cp ./out/Debug/obj/webrtc/modules/libwebrtc_utility.a ./package/Debug -v

echo "End of copying Debug libs"


#copy release version libs
echo "Start to copy release libs"
cp ./out/Release/lib.java/audio_device_java.jar ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_coding_module.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_conference_mixer.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_decoder_interface.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_device.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_encoder_interface.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_processing.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudio_processing_neon.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libaudioproc_debug_proto.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libbitrate_controller.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libcng.a ./package/Release -v
cp ./out/Release/obj/webrtc/common_audio/libcommon_audio.a ./package/Release -v
cp ./out/Release/obj/webrtc/common_audio/libcommon_audio_neon.a ./package/Release -v
cp ./out/Release/obj/chromium/src/build/android/libcpu_features.a ./package/Release -v
cp ./out/Release/obj/webrtc/system_wrappers/libcpu_features_android.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libg711.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libg722.a ./package/Release -v
cp ./out/Release/obj/third_party/gflags/libgflags.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libilbc.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libisac.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libisac_neon.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libmedia_file.a ./package/Release -v
cp ./out/Release/obj/webrtc/system_wrappers/libmetrics_default.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libneteq.a ./package/Release -v
cp ./out/Release/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl.a ./package/Release -v
cp ./out/Release/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl_armv7.a ./package/Release -v
cp ./out/Release/obj/chromium/src/third_party/openmax_dl/dl/libopenmax_dl_neon.a ./package/Release -v
cp ./out/Release/obj/chromium/src/third_party/opus/libopus.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libpaced_sender.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libpcm16b.a ./package/Release -v
cp ./out/Release/obj/chromium/src/third_party/protobuf/libprotobuf_lite.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libred.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libremote_bitrate_estimator.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/librent_a_codec.a ./package/Release -v
cp ./out/Release/obj/webrtc/base/librtc_base.a ./package/Release -v
cp ./out/Release/obj/webrtc/base/librtc_base_approved.a ./package/Release -v
cp ./out/Release/obj/webrtc/librtc_event_log.a ./package/Release -v
cp ./out/Release/obj/webrtc/librtc_event_log_proto.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/librtp_rtcp.a ./package/Release -v
cp ./out/Release/obj/webrtc/system_wrappers/libsystem_wrappers.a ./package/Release -v
cp ./out/Release/obj/webrtc/voice_engine/libvoice_engine.a ./package/Release -v
cp ./out/Release/obj/webrtc/libwebrtc_common.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libwebrtc_opus.a ./package/Release -v
cp ./out/Release/obj/webrtc/modules/libwebrtc_utility.a ./package/Release -v

echo "End of copying release libs"

tar czvf package.tar.gz package

