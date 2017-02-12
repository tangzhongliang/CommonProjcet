APP_CPPFLAGS:=-frtti -fexceptions
APP_ABI:=armeabi armeabi-v7a x86
#APP_ABI:=x86
APP_OPTIM := debug

# In NDK r8e, we have to use GCC 4.7 for C++11 features like thread, mutex, etc.
#  The default GCC is 4.6, which does NOT define macro _GLIBCXX_HAS_GTHREADS,
#  so you’ll get errors like "mutex in namespace std does not name a type" etc.
#NDK_TOOLCHAIN_VERSION := 4.7

# GNU STL implements most C++11 features. Use either gnustl_static or gnustl_shared
#  Without this your C++ code will not be able to access headers like <thread>, <mutex>
APP_STL := gnustl_static
APP_PLATFORM := android-17
APP_BUILD_SCRIPT=$(APP_PROJECT_PATH)/cpp/Android.mk
