DATAHOME=/data/lkn_home/config
SISCHEMA=/si/schema

adb push parser.xml $DATAHOME/$SISCHEMA/
#adb push cables.xml $DATAHOME/$SISCHEMA/
#adb push satellites.xml $DATAHOME/$SISCHEMA/
#adb push terrestrial.xml $DATAHOME/$SISCHEMA/
adb push scanconfig.xml $DATAHOME/$SISCHEMA/
#adb push DVB/BAT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/EIT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/NIT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/RST.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/SDT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/TDT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/TOT.xml $DATAHOME/$SISCHEMA/DVB/
#adb push DVB/descriptor.xml $DATAHOME/$SISCHEMA/DVB/
#adb push PSIP/EIT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/ETT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/MGT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/RTT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/STT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/VCT.xml $DATAHOME/$SISCHEMA/PSIP/
#adb push PSIP/descriptor.xml $DATAHOME/$SISCHEMA/PSIP/
adb push skb/descriptor.xml $DATAHOME/$SISCHEMA/skb/
#adb push lgu/descriptor.xml $DATAHOME/$SISCHEMA/lgu/
adb push lkn/descriptor.xml $DATAHOME/$SISCHEMA/lkn/
adb shell sync
