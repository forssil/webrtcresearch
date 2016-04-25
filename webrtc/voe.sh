# enter uper layer path
cd ../

echo "Start build Debug version"
#build Debug version
ninja -C out/Debug
echo "end of building Debug version"

echo "start build release version"
#build release version
ninja -C out/Release
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

