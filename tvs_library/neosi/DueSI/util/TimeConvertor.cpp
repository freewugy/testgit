/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-07-25 09:05:59 +0900 (금, 25 7월 2014) $
 * $LastChangedRevision: 961 $
 * Description:
 * Note:
 *****************************************************************************/

#include <time.h>
#include <vector>
#include "TimeConvertor.h"
#include "StringUtil.h"
#include "Logger.h"

static const char* TAG = "TimeConvertor";

TimeConvertor::~TimeConvertor()
{
    // TODO Auto-generated destructor stub
}

void TimeConvertor::GetDateFromMJD(int MJD, int& YY, int& MM, int& DD)
{
    int K, M, Y;
    int tmp;
    Y = (int) ((MJD * 100 - 1507820) / 36525);
    tmp = (int) (Y * 36525 / 100);
    M = (int) ((((MJD * 10000 - 149561000 - (tmp) * 10000)) / 306001));
    DD = MJD - 14956 - ((Y * 36525) / 100) - ((M * 306001) / 10000);
    if(M == 14 || M == 15) {
        K = 1;
    } else {
        K = 0;
    }
    MM = M - 1 - K * 12;
    YY = 1900 + Y + K;
}

void TimeConvertor::GetTimeFromBCDTime(int Time, int& Hour, int& Min, int & Sec)
{
    Sec = (int) ((uint8_t)(Time & 0x0F) + (uint8_t)((Time >> 4) & 0x0F) * 10);
    Min = (int) ((uint8_t)((Time >> 8) & 0x0F) + (uint8_t)((Time >> 12) & 0x0F) * 10);
    Hour = (int) ((uint8_t)((Time >> 16) & 0x0F) + (uint8_t)((Time >> 20) & 0x0F) * 10);
}

uint32_t TimeConvertor::convertTimeBCD2Seconds(uint32_t BCD_time)
{
    uint32_t convertSeconds = 0;
    int nHour, nMin, nSec;

    GetTimeFromBCDTime(BCD_time, nHour, nMin, nSec);

    convertSeconds = nHour * 3600 + nMin * 60 + nSec;

    return convertSeconds;
}

uint32_t TimeConvertor::convertTime2TodaySeconds(uint32_t MJD_time, uint32_t BCD_time)
{
    uint32_t convertSeconds = 0;
    int nYear, nMonth, nDay, nHour, nMin, nSec;

    GetTimeFromBCDTime(BCD_time, nHour, nMin, nSec);

    time_t now_t;
    struct tm now;
    uint32_t nowsecond;
    now_t = time(NULL);
    localtime_r(&now_t, &now);

    struct tm t;
    t.tm_sec = nSec;
    t.tm_min = nMin;
    t.tm_hour = nHour;
    t.tm_mday = now.tm_mday;
    t.tm_mon = now.tm_mon;
    t.tm_year = now.tm_year;
    t.tm_isdst = -1;

    convertSeconds = mktime(&t);

    return convertSeconds;
}

uint32_t TimeConvertor::convertTime2TodaySeconds(uint32_t seconds)
{
    uint32_t convertSeconds = 0;

    time_t atm = static_cast<time_t>(seconds);
    struct tm * tm_time;
    tm_time = localtime(&atm);

    time_t now_t;
    struct tm now;
    uint32_t nowsecond;
    now_t = time(NULL);
    localtime_r(&now_t, &now);

    struct tm t;
    t.tm_sec = tm_time->tm_sec;
    t.tm_min = tm_time->tm_min;
    t.tm_hour = tm_time->tm_hour;
    t.tm_mday = now.tm_mday;
    t.tm_mon = now.tm_mon;
    t.tm_year = now.tm_year;
    t.tm_isdst = -1;

    convertSeconds = mktime(&t);

    return convertSeconds;
}

uint32_t TimeConvertor::convertTimeMJDBCD2Seconds(uint32_t MJD_time, uint32_t BCD_time, int nDST)
{
    uint32_t convertSeconds = 0;
    int nYear, nMonth, nDay, nHour, nMin, nSec;

    GetDateFromMJD(MJD_time, nYear, nMonth, nDay);
    GetTimeFromBCDTime(BCD_time, nHour, nMin, nSec);

    if(nYear >= 2038)
        nYear = 2037;
    else if(nYear < 1970)
        nYear = 1970;

    struct tm t;
    t.tm_sec = nSec;
    t.tm_min = nMin;
    t.tm_hour = nHour;
    t.tm_mday = nDay;
    t.tm_mon = nMonth ? nMonth - 1 : 0;
    t.tm_year = nYear % 1900;
    t.tm_isdst = nDST;

    convertSeconds = mktime(&t);

    return convertSeconds;
}

uint32_t TimeConvertor::convertTimeMJDBCD2Seconds(uint32_t MJD_time, uint32_t BCD_time)
{
    return convertTimeMJDBCD2Seconds(MJD_time, BCD_time, -1);
}

uint32_t TimeConvertor::convertDate2Seconds(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST)
{
    uint32_t convertSeconds = 0;

    struct tm t;
    t.tm_sec = nSec;
    t.tm_min = nMin;
    t.tm_hour = nHour;
    t.tm_mday = nDay;
    t.tm_mon = nMonth ? nMonth - 1 : 0;
    t.tm_year = nYear % 1900;
    t.tm_isdst = nDST;

    convertSeconds = mktime(&t);

    return convertSeconds;
}

uint32_t TimeConvertor::getCurrentSecond()
{
    time_t t;
    struct tm now;
    uint32_t nowsecond;
    t = time(NULL);
    localtime_r(&t, &now);
    nowsecond = mktime(&now);
    return nowsecond;
}

string TimeConvertor::convertSeconds2Date(uint32_t seconds)
{
    string time;
    char temp[128];
    time_t atm = static_cast<time_t>(seconds);
    struct tm * tm_time;
    tm_time = localtime(&atm);

    strftime(temp, 128, "%Y-%m-%dT%H:%M:%S", tm_time);

    time = temp;
    return temp;
}

uint32_t TimeConvertor::convertStrToSeconds(string strTime, string strDate)
{
#if 1
    char *pstr;
    long hour, minite, second, month, day, year;

    month = strtol(strDate.c_str(), &pstr, 10);
    day = strtol(++pstr, &pstr, 10);
    year = strtol(++pstr, &pstr, 10);

    hour = strtol(strTime.c_str(), &pstr, 10);
    while (!isdigit(*pstr))
        ++pstr;
    minite = strtol(pstr, &pstr, 10);
    while (!isdigit(*pstr))
        ++pstr;
    second = strtol(pstr, &pstr, 10);

#else
    int count = 0;
    // ex) 23:53:00
    vector<string> element;
    count = splitStringByDelimiter(strTime, ":", element);
    if(count != 2) {
        L_ERROR(TAG, " === Wrong Time String[%s] ===\n", strTime.c_str());
        element.clear();
        return 0;
    }

    hour = atoi(element[0].c_str());
    minite = atoi(element[1].c_str());
    second = atoi(element[2].c_str());

    element.clear();

    // ex)) 03/26/2014
    count = splitStringByDelimiter(strDate, "/", element);
    if(count != 2) {
        L_ERROR(TAG, " === Wrong Date String[%s] ===\n", strDate.c_str());
        element.clear();
        return 0;
    }

    month = atoi(element[0].c_str());
    day = atoi(element[1].c_str());
    year = atoi(element[2].c_str());

    element.clear();
#endif
    return convertDate2Seconds(year, month, day, hour, minite, second, 0);
}

bool TimeConvertor::setCurrentTime(uint32_t current_time)
{
    if(checkCurrentTime(current_time)) {
        m_current_seconds = current_time;
        return true;
    } else {
        return false;
    }

}

bool TimeConvertor::checkCurrentTime(uint32_t second)
{
    time_t atm = static_cast<time_t>(second);
    struct tm * tm_time;
    tm_time = localtime(&atm);

    if((tm_time->tm_year + 1900) < 2000) {
        return false;
    }

    // 컴파일한 시간보다 이전이면 return false

    return true;
}

bool TimeConvertor::isValidSystemTime()
{
    uint32_t second = getCurrentSecond();
    time_t atm = static_cast<time_t>(second);
    struct tm * tm_time;
    tm_time = localtime(&atm);

    if((tm_time->tm_year + 1900) < 2010) {
        return false;
    }

    return true;
}
