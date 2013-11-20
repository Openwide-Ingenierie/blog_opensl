LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Blog_opensl
LOCAL_SRC_FILES := Blog_opensl.c audioio.c
LOCAL_LDLIBS += -lOpenSLES -llog -landroid -lm

include $(BUILD_SHARED_LIBRARY)
