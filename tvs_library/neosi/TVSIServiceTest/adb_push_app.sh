SYSDIR=/system
LIBDIR=lib
APPDIR=app

adb push ./bin/TVSIServiceTest.apk $SYSDIR/$APPDIR
adb shell sync
