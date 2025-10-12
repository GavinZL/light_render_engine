LOCAL_PATH := ${call my-dir}
include $(CLEAR_VARS)

DIR_OTHER := $(LOCAL_PATH)/../
DIR_3RDPARTY := $(LOCAL_PATH)/../../3rdparty
# 赋值给NDK编译系统
LOCAL_C_INCLUDES := ${LOCAL_PATH} \
                    ${LOCAL_PATH}/common/ \
                    ${LOCAL_PATH}/backend/ \
					${DIR_OTHER}/ \
					${DIR_OTHER}/../log/ \
					${DIR_OTHER}/../ \
					${DIR_OTHER}/../common/ \
					${DIR_3RDPARTY} \

LOCAL_SRC_FILES  :=  $(wildcard ${LOCAL_PATH}/*.cpp) \
					 $(wildcard ${LOCAL_PATH}/common/*.cpp) \
					 $(wildcard ${LOCAL_PATH}/backend/*.cpp) \

ifeq ($(WITH_PREPROCESS),true)
    LOCAL_CFLAGS += -DWITH_PREPROCESS
endif

ifeq ($(BUILD_INDEPENDENT),true)
    LOCAL_CFLAGS += -DBUILD_INDEPENDENT
else
    ifeq ($(ANDROID_DEMO),true)
        LOCAL_SHARED_LIBRARIES := yycommonlib2
    else
        LOCAL_SHARED_LIBRARIES := yycommonlib
    endif
endif

LOCAL_CFLAGS += -fPIC

LOCAL_MODULE := libdispatch_queue
include $(BUILD_STATIC_LIBRARY)