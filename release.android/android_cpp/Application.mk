APP_STL := gnustl_static
APP_OPTIM := release
APP_CPPFLAGS += -frtti
APP_ABI := armeabi
#APP_ABI := armeabi armeabi-v7a
APP_PLATFORM := android-9
#APP_PLATFORM := android-14

# not compile corefundation library for android.
# corefoundation
APP_MODULES := game 
