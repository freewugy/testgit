SYSDIR=/system
LIBDIR=lib
APPDIR=app

adb push ./bin/TVSIService.apk $SYSDIR/$APPDIR
adb shell sync
