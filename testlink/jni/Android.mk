LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := static_add
LOCAL_SRC_FILES := ../package/Debug/libstatic_add.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ../package/include/
LOCAL_MODULE    := webrtc_voe_wrapper
LOCAL_STATIC_LIBRARIES := static_add
LOCAL_SRC_FILES := voewrapper.cpp
include $(BUILD_SHARED_LIBRARY)
