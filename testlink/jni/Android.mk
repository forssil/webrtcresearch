LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := metrics_default
LOCAL_SRC_FILES := ../package/Debug/libmetrics_default.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_coding_module
LOCAL_SRC_FILES := ../package/Debug/libaudio_coding_module.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := neteq
LOCAL_SRC_FILES := ../package/Debug/libneteq.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_conference_mixer
LOCAL_SRC_FILES := ../package/Debug/libaudio_conference_mixer.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := openmax_dl
LOCAL_SRC_FILES := ../package/Debug/libopenmax_dl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_decoder_interface
LOCAL_SRC_FILES := ../package/Debug/libaudio_decoder_interface.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := openmax_dl_armv7
LOCAL_SRC_FILES := ../package/Debug/libopenmax_dl_armv7.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_device
LOCAL_SRC_FILES := ../package/Debug/libaudio_device.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := openmax_dl_neon
LOCAL_SRC_FILES := ../package/Debug/libopenmax_dl_neon.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_encoder_interface
LOCAL_SRC_FILES := ../package/Debug/libaudio_encoder_interface.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := opus
LOCAL_SRC_FILES := ../package/Debug/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audioproc_debug_proto
LOCAL_SRC_FILES := ../package/Debug/libaudioproc_debug_proto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := paced_sender
LOCAL_SRC_FILES := ../package/Debug/libpaced_sender.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_processing
LOCAL_SRC_FILES := ../package/Debug/libaudio_processing.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := pcm16b
LOCAL_SRC_FILES := ../package/Debug/libpcm16b.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_processing_neon
LOCAL_SRC_FILES := ../package/Debug/libaudio_processing_neon.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := protobuf_lite
LOCAL_SRC_FILES := ../package/Debug/libprotobuf_lite.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := bitrate_controller
LOCAL_SRC_FILES := ../package/Debug/libbitrate_controller.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := red
LOCAL_SRC_FILES := ../package/Debug/libred.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := cng
LOCAL_SRC_FILES := ../package/Debug/libcng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := remote_bitrate_estimator
LOCAL_SRC_FILES := ../package/Debug/libremote_bitrate_estimator.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := common_audio
LOCAL_SRC_FILES := ../package/Debug/libcommon_audio.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rent_a_codec
LOCAL_SRC_FILES := ../package/Debug/librent_a_codec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := common_audio_neon
LOCAL_SRC_FILES := ../package/Debug/libcommon_audio_neon.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtc_base
LOCAL_SRC_FILES := ../package/Debug/librtc_base.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := cpu_features
LOCAL_SRC_FILES := ../package/Debug/libcpu_features.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtc_base_approved
LOCAL_SRC_FILES := ../package/Debug/librtc_base_approved.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libcpu_features_android
LOCAL_SRC_FILES := ../package/Debug/libcpu_features_android.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtc_event_log
LOCAL_SRC_FILES := ../package/Debug/librtc_event_log.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := g711
LOCAL_SRC_FILES := ../package/Debug/libg711.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtc_event_log_proto
LOCAL_SRC_FILES := ../package/Debug/librtc_event_log_proto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := g722
LOCAL_SRC_FILES := ../package/Debug/libg722.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtp_rtcp
LOCAL_SRC_FILES := ../package/Debug/librtp_rtcp.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := gflags
LOCAL_SRC_FILES := ../package/Debug/libgflags.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := ilbc
LOCAL_SRC_FILES := ../package/Debug/libilbc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := system_wrappers
LOCAL_SRC_FILES := ../package/Debug/libsystem_wrappers.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := isac
LOCAL_SRC_FILES := ../package/Debug/libisac.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := voice_engine
LOCAL_SRC_FILES := ../package/Debug/libvoice_engine.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := isac_neon
LOCAL_SRC_FILES := ../package/Debug/libisac_neon.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := webrtc_common
LOCAL_SRC_FILES := ../package/Debug/libwebrtc_common.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := level_indicator
LOCAL_SRC_FILES := ../package/Debug/liblevel_indicator.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := webrtc_opus
LOCAL_SRC_FILES := ../package/Debug/libwebrtc_opus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := media_file
LOCAL_SRC_FILES := ../package/Debug/libmedia_file.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := webrtc_utility
LOCAL_SRC_FILES := ../package/Debug/libwebrtc_utility.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rtc_task_queue
LOCAL_SRC_FILES := ../package/Debug/librtc_task_queue.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := audio_decoder_factory_interface
LOCAL_SRC_FILES := ../package/Debug/libaudio_decoder_factory_interface.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := static_add
LOCAL_SRC_FILES := ../package/Debug/libstatic_add.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -DV8_DEPRECATION_WARNINGS -DCLD_VERSION=2 -D_FILE_OFFSET_BITS=64
LOCAL_CFLAGS += -DNO_TCMALLOC -DDISABLE_NACL -DCHROMIUM_BUILD
LOCAL_CFLAGS += -DCR_CLANG_REVISION=247874-1 -DUSE_LIBJPEG_TURBO=1 -DENABLE_WEBRTC=1
LOCAL_CFLAGS += -DENABLE_MEDIA_ROUTER=1 -DUSE_PROPRIETARY_CODECS -DENABLE_BROWSER_CDMS
LOCAL_CFLAGS += -DENABLE_CONFIGURATION_POLICY -DENABLE_NOTIFICATIONS
LOCAL_CFLAGS += -DDONT_EMBED_BUILD_METADATA -DFIELDTRIAL_TESTING_ENABLED
LOCAL_CFLAGS += -DENABLE_AUTOFILL_DIALOG=1 -DENABLE_PRINTING=1
LOCAL_CFLAGS += -DENABLE_BASIC_PRINTING=1 -DENABLE_SPELLCHECK=1 
LOCAL_CFLAGS += -DUSE_BROWSER_SPELLCHECKER=1 -DENABLE_SUPERVISED_USERS=1 -DVIDEO_HOLE=1
LOCAL_CFLAGS += -DV8_USE_EXTERNAL_STARTUP_DATA -DENABLE_WEBVR -DSAFE_BROWSING_DB_REMOTE
LOCAL_CFLAGS += -DEXPAT_RELATIVE_PATH -DWEBRTC_ARCH_ARM -DWEBRTC_ARCH_ARM_V7
LOCAL_CFLAGS += -DWEBRTC_DETECT_NEON -DWEBRTC_POSIX -DWEBRTC_LINUX -DWEBRTC_ANDROI
LOCAL_CFLAGS += -DWEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE -DOPUS_FIXED_POINT
LOCAL_CFLAGS += -DUSE_LIBPCI=1 -DUSE_OPENSSL=1 -DUSE_OPENSSL_CERTS=1
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DANDROID
LOCAL_CFLAGS += -D__GNU_SOURCE=1 '-DCHROME_BUILD_ID=""' -DHAVE_SYS_UIO_H
LOCAL_CFLAGS += -DDYNAMIC_ANNOTATIONS_ENABLED=1 -DWTF_USE_DYNAMIC_ANNOTATIONS=1 -D_DEBUG

LOCAL_C_INCLUDES := ../package/include/
LOCAL_MODULE    := webrtc_voe_wrapper
LOCAL_STATIC_LIBRARIES += static_add libstlport


LOCAL_SRC_FILES := voewrapper.cpp
include $(BUILD_SHARED_LIBRARY)
