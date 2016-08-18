/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-07-24 12:49:17 +0900 (목, 24 7월 2014) $
 * $LastChangedRevision: 957 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <sqlite3.h>

enum program_linked_info_type {
    PLI_LINKED_SERVICE_FLAG = 1,
    PLI_LINKED_SERVICE_TEXT,
    PLI_BUTTON_TYPE,
    PLI_C_MENU,
    PLI_BUTTON_IMAGE_FILENAME,
    PLI_DISPLAY_START_TIME,
    PLI_DISPLAY_END_TIME,
    PLI_VAS_ITEMID,
    PLI_VAS_PATH,
    PLI_VAS_SERVICEID,
};

enum cainfo_type {
    CA_SYSTEM_ID = 1, CA_PID,
};

enum event_info_type {
    EI_SERVICE_UID = 0,
    EI_SERVICE_ID,
    EI_EVENT_ID,
    EI_EVENT_UID,
    EI_EVENT_NAME,
    EI_EVENT_SHORT_DESCRIPTION,
    EI_EVENT_DESCRIPTION,
    EI_RUNNIG_STATUS,
    EI_FREE_CA_MODE,
    EI_RATING,
    EI_START_TIME,
    EI_END_TIME,
    EI_DURATION,
    EI_IMAGE_URL,
    EI_DIRECTOR_NAME,
    EI_ACTORS_NAME,
    EI_PRICE_TEXT,
    EI_DOLBY,
    EI_AUDIO,
    EI_RESOLUTION,
    EI_CONTENT_NIBBLE_LEVEL_1,
    EI_CONTENT_NIBBLE_LEVEL_2,
    EI_USER_NIBBLE_1, // eschoi_20140821 | used to Season for WEPG v2, 0 ~ 999
    EI_USER_NIBBLE_2, // eschoi_20140821 | used to EpisodeNo for WEPG v2
    EI_CAPTION,
    EI_DVS,
    EI_PROGAMLINKED_INFO_SIZE,
};

enum service_info_type {
    SVI_SERVICE_UID = 0,
    SVI_TS_ID,
    SVI_SERVICE_ID,
    SVI_SERVICE_TYPE,
    SVI_SERVICE_NAME,
    SVI_CHANNEL_NUMBER,
    SVI_IP_ADDRESS,
    SVI_TS_PORT_NUMBER,
    SVI_FREQUENCY,
    SVI_MODULATION,
    SVI_SYMBOL_RATE,
    SVI_FEC_INNER,
    SVI_GENRE_CODE,
    SVI_CATEGORY_CODE,
    SVI_RUNNING_STATUS,
    SVI_FREE_CA_MODE,
    SVI_CHANNEL_RATING,
    SVI_LOCAL_AREA_CODE,
    SVI_PAY_CHANNEL,
    SVI_PAY_CH_SAMPLE_SEC,
    SVI_RESOLUTION,
    SVI_CHANNEL_IMAGE_URL,
    SVI_PCR_PID,
    SVI_PRODUCTID_INFO_SIZE,
    SVI_ES_INFO_SIZE,
    SVI_CA_INFO_SIZE,
};

using namespace std;

class SqliteDB {

private:

public:
    SqliteDB(const char *strFileName);

    ~SqliteDB()
    {
        if(mdb)
            sqlite3_close(mdb);
    }

    bool isError();

    int Execute(const char *strQuery);
    int ExecuteMulti(const list<string> queries);

    int Select(const char *strQuery, list<vector<string> > &rResult);

    int createTable();
    int clearTable();

    bool verify();
private:
    bool m_isError;
    sqlite3 *mdb;

    int Send(const char *strQuery);

    bool checkDBSchema();
    bool updateDummyData();
};

#endif /* SQLITEDB_H_ */
