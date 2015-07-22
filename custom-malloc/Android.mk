LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := utm
LOCAL_SRC_FILES := malloc.c
# LOCAL_CFLAGS := -I$(LOCAL_PATH)/../libc/
# LOCAL_STATIC_LIBRARIES := libc_nomalloc

include $(BUILD_SHARED_LIBRARY)
