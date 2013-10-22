LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libv8_base
LOCAL_SRC_FILES := ./libs_r8d.release/libv8_base.arm.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libv8_nosnapshot
LOCAL_SRC_FILES := ./libs_r8d.release/libv8_nosnapshot.arm.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := v8
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/
LOCAL_LDLIBS    := -llog
#LOCAL_CFLAGS    := -g
LOCAL_STATIC_LIBRARIES := libv8_base libv8_nosnapshot
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_SHARED_LIBRARY)
