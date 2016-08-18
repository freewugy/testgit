SYSDIR=/system
LIBDIR=lib
APPDIR=app

adb push ./libs/armeabi-v7a/libtvs_duesi.so $SYSDIR/$LIBDIR
adb push ./libs/armeabi-v7a/libtvs_jnisi.so $SYSDIR/$LIBDIR
adb push ./libs/armeabi-v7a/libtvs_tdisi.so $SYSDIR/$LIBDIR
adb shell sync
