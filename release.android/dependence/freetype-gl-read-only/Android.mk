LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libfreetype_gl
LOCAL_CFLAGS           := -Werror -Wno-psabi
LOCAL_SHARED_LIBRARIES := libft2
LOCAL_LDLIBS           := -lGLESv2
LOCAL_SRC_FILES        := edtaa3func.c \
	font-manager.c \
	mat4.c \
	platform.c \
	text-buffer.c \
	texture-atlas.c \
	texture-font.c \
	vector.c \
	vertex-attribute.c \
	vertex-buffer.c \
	shader.c 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_SHARED_LIBRARY)