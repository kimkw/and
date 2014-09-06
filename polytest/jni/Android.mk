LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_LIB_TYPE        := STATIC
OPENCV_INSTALL_MODULES := on
OPENCV_CAMERA_MODULES  := off

include C:\Users\user\Desktop\final\android-ndk-r9d-windows-x86_64\OpenCV-2.4.9-android-sdk\OpenCV-2.4.9-android-sdk\sdk\native\jni/OpenCV.mk

LOCAL_MODULE    := test
LOCAL_SRC_FILES := test.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
