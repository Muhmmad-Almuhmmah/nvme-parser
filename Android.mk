LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := nvme

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/src/include

LOCAL_SRC_FILES := \
    src/main.c \
    src/nve.c \
    src/helper.c

LOCAL_CFLAGS += -Wno-int-conversion

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH))
