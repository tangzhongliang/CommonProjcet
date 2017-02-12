LOCAL_PATH := $(call my-dir)  
include $(CLEAR_VARS)  
OPENCV_LIB_TYPE:=STATIC
ifeq ("$(wildcard $(OPENCV_MK_PATH))","")  
#try to load OpenCV.mk from default install location
include C:\work-software\OpenCV-android-sdk\sdk\native\jni\OpenCV.mk
#include /opt/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk
else
include $(OPENCV_MK_PATH)
endif
LOCAL_MODULE    := native-lib
#MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
#LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := ../cpp/native-lib.cpp

FILE_LIST := $(wildcard /opt/OpenCV-android-sdk/sdk/native/lib/arm64-v8a/*)
LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -ljnigraphics
LOCAL_LDLIBS += -lz # Compression library
# C++11 and threading enabling features.
#  Otherwise c++11, pthread, rtti and exceptions are not enabled by default
LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions
include $(BUILD_SHARED_LIBRARY)