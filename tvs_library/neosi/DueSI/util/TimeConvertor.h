/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (화, 03 6월 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef TIMECONVERTOR_H_
#define TIMECONVERTOR_H_

#include <stdint.h>
#include <string>
#include <stdio.h>

using namespace std;
/*
 *
 */
class TimeConvertor {
public:
	virtual ~TimeConvertor();

    /**
     * @brief instance를 생성하여서 반환
     */
    static TimeConvertor& getInstance() {
        static TimeConvertor instance;

        return instance;
    }

	uint32_t getCurrentSecond();
	string convertSeconds2Date(uint32_t seconds);
	uint32_t convertDate2Seconds(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST);
	uint32_t convertStrToSeconds(string strTime, string strDate);

	uint32_t convertTimeMJDBCD2Seconds(uint32_t MJD_time, uint32_t BCD_time, int nDST);
	uint32_t convertTimeMJDBCD2Seconds(uint32_t MJD_time, uint32_t BCD_time);
	uint32_t convertTime2TodaySeconds(uint32_t MJD_time, uint32_t BCD_time);
	uint32_t convertTime2TodaySeconds(uint32_t seconds);
	uint32_t convertTimeBCD2Seconds(uint32_t BCD_time);

	bool setCurrentTime(uint32_t current_time);
	bool isValidSystemTime();

private:
    /**
     * @brief 생성자
     */
	TimeConvertor() {
    };                   // Constructor? (the {} brackets) are needed here.
    // Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
	TimeConvertor(TimeConvertor const&);            // Don't Implement
    void operator=(TimeConvertor const&);         // Don't implement

	void GetDateFromMJD(int MJD, int& YY, int& MM, int& DD);
	void GetTimeFromBCDTime(int Time,int& Hour, int& Min, int & Sec);
	bool checkCurrentTime(uint32_t current_time);

	uint32_t m_current_seconds;
};

#endif /* TIMECONVERTOR_H_ */
