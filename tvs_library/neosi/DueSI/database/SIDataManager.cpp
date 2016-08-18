/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-10-20 18:00:23 +0900 (월, 20 10월 2014) $
 * $LastChangedRevision: 1032 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SIDataManager.h"
#include "Logger.h"
#include "util/TimeConvertor.h"
#include "StringUtil.h"
#include "StreamTypeUtil.h"
#include "Timer.h"

#ifdef ANDROID
pthread_mutex_t SIDataManager::mSqliteMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
pthread_mutex_t SIDataManager::mSqliteMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#define __DEBUG_LOG_DM__

static const char* TAG = "SIDataManager";

struct comp_serviceinfo_by_serviceid {
    bool operator()(ServiceInfo & n1, ServiceInfo & n2)
    {
        return n1.service_id < n2.service_id;
    }
};

struct comp_serviceinfo_by_channelnumber {
    bool operator()(ServiceInfo & n1, ServiceInfo & n2)
    {
        return n1.channel_number < n2.channel_number;
    }
};

struct comp_eventinfo_by_starttime {
    bool operator()(EventInfo * n1, EventInfo * n2)
    {
        return n1->start_time < n2->start_time;
    }
};

string SIDataManager::getDBFileName()
{
#ifdef ANDROID
    return "/data/lkn_home/run/si/si_info.db";
#else
    return "/data/lkn_home/run/si/si_info.db";
#endif
}

bool SIDataManager::verifyDBFile()
{

	if(mDatabase->verify() == false) {
		string dbFile = getDBFileName();
		L_ERROR(TAG, "=== OOPS : SQL INIT ERROR, RECOVERY DB File ===\n");
		deleteDBFile();
		if(0 == access("/data/lkn_home/run/si/si_info_bak.db", F_OK)) // backup file exist
		{
			recoverDB();
			mDatabase = new SqliteDB(dbFile.c_str());
			L_DEBUG(TAG, " === RECOVERY DB SUCCESS === \n" );
			return !mDatabase->isError();

		}else {
			 L_INFO(TAG, "SQL INIT ERROR Repaired : CREATE TABLE\n");
		}

		mDatabase = new SqliteDB(dbFile.c_str());
		clearTable();
		createTable();

    }

    return !mDatabase->isError();
}

void SIDataManager::deleteDBFile()
{
    if(mEventStatus == PROGRAM_DB_COMPLETE) {
        L_INFO(TAG, "OOPS : DB READ Error, but EPG already completed made, so will skip\n");
        return;
    }

    string dbFile = getDBFileName();
    char cmd_buffer[64];

    delete mDatabase;
    mDatabase = NULL;

    sprintf(cmd_buffer, "rm -rf %s", dbFile.c_str());
    int ret = system(cmd_buffer);

    system("sync");

    siMiliSleep(10);

}

void SIDataManager::initDB()
{
    string dbFile = getDBFileName();

    if(0 == access(dbFile.c_str(), F_OK)) { // if db file exist
        if(mDatabase == NULL)
            mDatabase = new SqliteDB(dbFile.c_str());

        if (this->m_scanMode){
        	L_INFO(TAG, "ScanMode DB CLEAR OK \n");
        	clearTable();
        	createTable();
        }
    } else {
        if(mDatabase == NULL)
            mDatabase = new SqliteDB(dbFile.c_str());

        clearTable();
        createTable();
    }

    if(verifyDBFile()) {
        L_INFO(TAG, "DB INIT OK\n");
    } else {
        L_ERROR(TAG, "\n\n=== OOPS : DB[%s] INIT ERROR ===\n\n", dbFile.c_str());
    }

    mDefaultChannelUid = "0.0.0";
    mPMTCount = 0;
    mServiceStatus = UNKNOWN;
    mEventStatus = UNKNOWN;
}

void SIDataManager::resetDB()
{
    string dbFile = getDBFileName();
    if(mDatabase != NULL) {
        delete mDatabase;
        mDatabase = NULL;
    }

    mDatabase = new SqliteDB(dbFile.c_str());

    if(verifyDBFile()) {
        L_INFO(TAG, "DB INIT OK\n");
    } else {
        L_ERROR(TAG, "\n\n=== OOPS : DB[%s] INIT ERROR ===\n\n", dbFile.c_str());
    }
}

int SIDataManager::copyFile(const char* src, const char* des)
{
    FILE *fsrc, *fdes;
    int a;
    if((fsrc = fopen(src, "rb")) == NULL)
        return -1;
    if((fdes = fopen(des, "wb")) == NULL) {
        fclose(fsrc);
        return -1;
    }

    while (1) {
        a = fgetc(fsrc);
        if(!feof(fsrc))
            fputc(a, fdes);
        else
            break;
    }

    fclose(fsrc);
    fclose(fdes);

    return 0;
}

int SIDataManager::backupDB()
{
#ifdef ANDROID
    string dbFile = "/data/lkn_home/run/si/si_info.db";
    string backup_dbFile = "/data/lkn_home/run/si/si_info_bak.db";
#else
    string dbFile = "si_info.db";
    string backup_dbFile = "si_info_bak.db";
#endif
    copyFile(dbFile.c_str(), backup_dbFile.c_str());

}

int SIDataManager::recoverDB()
{
#ifdef ANDROID
    string dbFile = "/data/lkn_home/run/si/si_info.db";
    string backup_dbFile = "/data/lkn_home/run/si/si_info_bak.db";
#else
    string dbFile = "si_info.db";
    string backup_dbFile = "si_info_bak.db";
#endif
    copyFile(backup_dbFile.c_str(), dbFile.c_str());
}

int SIDataManager::countCHDB()
{
    int result = -1;
    char *strQuery;
    list < vector<string> > rResults;
    strQuery = sqlite3_mprintf("SELECT COUNT(service_uid) FROM service_info;");
    if(strQuery != NULL) {
        result = mDatabase->Select(strQuery, rResults);
    }

    list<vector<string> >::iterator itr;
    for (itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        result = atoi(iinfo[0].c_str());
        break;
    }
    sqlite3_free(strQuery);

    for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        vector < string > temp;
		iinfo.swap(temp);
		temp.clear();
        iinfo.clear();
    }
    rResults.clear();

    return result;
}

int SIDataManager::createTable()
{
    if(mDatabase->createTable() < 0) {
        L_ERROR(TAG, "Create Table Failed\n");
        deleteDBFile();
        return -1;
    }

    SIChangeEvent event;
    event.setEventType(SIChangeEvent::DATA_EMPTY);
    notifyEvent((void*) &event);

    return 0;
}

int SIDataManager::clearTable()
{
    if(mDatabase->clearTable() < 0) {
        L_ERROR(TAG, "Clear Table Failed\n");
        deleteDBFile();
        return -1;
    }

    // elementary stream info map
    mMapESInfo.clear();
    return 0;
}

int SIDataManager::getChannelsInfo(list<ServiceInfo*> &listServiceInfo)
{
    char* strQuery;
    list < vector<string> > rResults;

    strQuery = sqlite3_mprintf("SELECT * FROM service_info ORDER BY channel_number;");
    if(strQuery != NULL)
    	mDatabase->Select(strQuery, rResults);
    sqlite3_free(strQuery);

    if(rResults.size() == 0)
        return -1;

    list<vector<string> >::iterator itr;
    for (itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);

        ServiceInfo* service = new ServiceInfo;
        makeServiceInfoFromDBData(iinfo, service);
        listServiceInfo.push_back(service);
    }

    return 0;
}

int SIDataManager::getChannelList(CHANNEL_TYPE channel_type, list<ServiceInfo*> &listServiceInfo)
{
    SqliteLock(__FUNCTION__);

    char* strQuery = NULL;
    list < vector<string> > rResults;

    switch (channel_type) {
        case ALL:
        strQuery = sqlite3_mprintf("SELECT * FROM service_info ORDER BY channel_number;");
            break;
        case AUDIO_VIDEO:
        strQuery = sqlite3_mprintf("SELECT * FROM service_info WHERE service_type != 2 ORDER BY channel_number;");
            break;
        case AUDIO_ONLY:
        strQuery = sqlite3_mprintf("SELECT * FROM service_info WHERE service_type = 2 ORDER BY channel_number;");
            break;
        default:
            break;
    }

    if(strQuery != NULL) {
        int result = mDatabase->Select(strQuery, rResults);
        L_DEBUG(TAG, "=== result_SIZE [%d] ===\n", result);
        if(result < 0) {
            L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
            deleteDBFile();
            sqlite3_free(strQuery);
            SqliteUnlock(__FUNCTION__);
            return -1;
        }

        for (list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {

        	vector < string > iinfo = (*itr);
            ServiceInfo* service = new ServiceInfo;
            makeServiceInfoFromDBData(iinfo, service);
            listServiceInfo.push_back(service);
        }

        sqlite3_free(strQuery);

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }
   SqliteUnlock(__FUNCTION__);

    return 0;
}

int SIDataManager::isVideoStream(int streamType)
{
    switch (streamType) {
        /* Video */
        case 0x01: //ESStreamType_MPEG1Video:
        case 0x02: //ESStreamType_MPEG2Video:
        case 0x10: //ESStreamType_MPEG4Video:
        case 0x1B: //ESStreamType_H264Video:
        return 1;
        default:
        return 0;
    }
}

int SIDataManager::isAudioStream(int streamType)
{
    switch (streamType) {
        /* Audio */
        case 0x03: //ESStreamType_MPEG1Audio:
        case 0x04: //ESStreamType_MPEG2Audio:
        case 0x06: //ESStreamType_DVBAC3Audio // MPEG2PESPrivateData
        case 0x0F: //ESStreamType_AACAudio:
        case 0x81: //ESStreamType_AC3Audio:
        return 1;
        default:
        return 0;
    }
}

int SIDataManager::isDTVChannel(int serviceType)
{
    if(serviceType == 0x01 || serviceType == 0x011 || serviceType == 0x16 || serviceType == 0x19) {
        return 1;
    }

    return 0;
}

int SIDataManager::isDRadioChannel(int serviceType)
{
    if(serviceType == 0x02) {
        return 1;
    }

    return 0;
}

// next modify.....
int SIDataManager::isLocalChannel(int localareacode)
{
    const int CONST_AREA_COMMON = 0;

    if((localareacode == CONST_AREA_COMMON) || (localareacode == 1) || (localareacode == 41) || (localareacode == 61)) {
        return 1;
    }

    return 0;
}

int SIDataManager::ischangedPMTinfo(list<PMTInfo*> listPMTInfo, string service_uid)
{
    // return data : 0(data changed) , 1(new pmt) , 2(data same), 3(wrong data)
    list<PMTInfo*>::iterator pmt_itr;
    list<elementary_stream>::iterator es_itr;
    map<string, list<elementary_stream> >::iterator es_mitr;
    int status = -1;
    int nEsCount = 0, nCount = 0, cCount = 0;

    pmt_itr = listPMTInfo.begin();
    es_mitr = mMapESInfo.find(service_uid.c_str());

    if(es_mitr != mMapESInfo.end()) {
        list<elementary_stream> elemList = (*es_mitr).second;

        for (list<elementary_stream>::iterator itr = (*pmt_itr)->list_elementary_stream.begin(); itr != (*pmt_itr)->list_elementary_stream.end();
                itr++) {
            elementary_stream esInfo = *itr;
            //L_INFO(TAG, "elementary_PID [%d], stream_type [%d] ,language[%u] \n",esInfo.stream_pid,esInfo.stream_type,esInfo.language_code);
            if(isVideoStream(esInfo.stream_type) == 1 || isAudioStream(esInfo.stream_type) == 1) {
                nEsCount++;
            }
        }

        if(nEsCount == elemList.size()) {
            for (list<elementary_stream>::iterator itr = elemList.begin(); itr != elemList.end(); itr++) {
                nCount++;
                elementary_stream esInfo1 = *itr;
                for (list<elementary_stream>::iterator initr = (*pmt_itr)->list_elementary_stream.begin();
                        initr != (*pmt_itr)->list_elementary_stream.end(); initr++) {
                    elementary_stream esInfo2 = *initr;

                    //L_INFO(TAG, "pid[%d] pid2[%d], type[%d] type2[%d], lang[%u], lang2[%u] \n",esInfo1.stream_pid, esInfo2.stream_pid , esInfo1.stream_type, esInfo2.stream_type, esInfo1.language_code ,esInfo2.language_code);
                    if(esInfo1.stream_pid == esInfo2.stream_pid && esInfo1.stream_type == esInfo2.stream_type
                            && esInfo1.language_code == esInfo2.language_code) {
                        status = 2;
                        cCount++;
                    }
                }

            }
            if(nCount == cCount && status == 2) {
                L_INFO(TAG, "PMT esinfo is same!! \n");
            } else {
                status = 0;
                L_INFO(TAG, "PMT esinfo is changed!! \n");
            }
        } else {
            status = 0;
            L_INFO(TAG, "PMT esinfo is changed!! \n");
        }
    } else {
        L_INFO(TAG, "not found esinfo in PMTmap, added!! \n");
        status = 1;
    }
    return status;
}

int SIDataManager::insertPMTList(list<PMTInfo*> listPMTInfo, SIChangeEvent event)
{
    char *strQuery;

    string tmpStr = "", service_uid = "";
    list < string > querylist;
    list<PMTInfo*>::iterator itr;
    list<elementary_stream> elemList;
    map<string, list<elementary_stream> >::iterator es_mitr;
    int n_CHCount = 0;
    int result =0;
    itr = listPMTInfo.begin();

    if(getServiceUid((*itr)->program_number, &service_uid) < 0) {
        L_DEBUG(TAG, "CANNOT GET Service UID : program_number[%u]\n", (*itr)->program_number);
        return 0;
    }

    L_INFO(TAG, "Insert PMT service_uid[%s], program_number[%u]\n", service_uid.c_str(), (*itr)->program_number);

    strQuery = sqlite3_mprintf("UPDATE service_info SET pcr_pid = %u WHERE service_uid = '%q';", (*itr)->pcr_pid, service_uid.c_str());
    tmpStr = strQuery;
    sqlite3_free(strQuery);
    if(!tmpStr.empty())
        querylist.push_back(tmpStr);

    strQuery = sqlite3_mprintf("DELETE FROM elementary_stream_info WHERE service_uid = '%q';", service_uid.c_str());
    tmpStr = strQuery;
    sqlite3_free(strQuery);
    if(!tmpStr.empty())
        querylist.push_back(tmpStr);

    int esCount = 0;
    list<elementary_stream>::iterator es_itr;
    for (es_itr = (*itr)->list_elementary_stream.begin(); es_itr != (*itr)->list_elementary_stream.end(); es_itr++) {
        if(isVideoStream(es_itr->stream_type) == 1 || isAudioStream(es_itr->stream_type) == 1) {
            esCount++;
#ifdef __DEBUG_LOG_DM__
            L_INFO(TAG, "service_uid.c_str() :[%s]\n",service_uid.c_str());
#endif
            strQuery = sqlite3_mprintf(
                    "INSERT INTO elementary_stream_info SELECT DISTINCT '%q', %d, %d, %u FROM service_info WHERE '%q'=service_info.service_uid;",
                    service_uid.c_str(), es_itr->stream_pid, es_itr->stream_type, es_itr->language_code, service_uid.c_str());
#ifdef __DEBUG_LOG_DM__
            L_INFO(TAG, "elementary_PID [%d], stream_type [%d] ,language[%u] \n",
                    es_itr->stream_pid, es_itr->stream_type, es_itr->language_code);
#endif
            tmpStr = strQuery;
            sqlite3_free(strQuery);
            if(!tmpStr.empty())
                querylist.push_back(tmpStr);
            elemList.push_back(*es_itr);

        }
    }

    list < vector<string> > rResults;
    strQuery = sqlite3_mprintf("SELECT * FROM service_info WHERE service_uid = '%q';", service_uid.c_str());
    if(strQuery != NULL)
    	result = mDatabase->Select(strQuery, rResults);
    sqlite3_free(strQuery);
    if(result > 0) { // valid channel
        n_CHCount++; //n_PMTCount++;
    } else {
        L_WARN(TAG, "PMT Received : not found ch on db, servcie_uid = %s prog_num = 0x%X\n", service_uid.c_str(), (*itr)->program_number);
    }

    strQuery = sqlite3_mprintf("UPDATE service_info SET les_count = %d WHERE service_uid = '%q';", esCount, service_uid.c_str());
    tmpStr = strQuery;
    sqlite3_free(strQuery);
    if(!tmpStr.empty())
        querylist.push_back(tmpStr);

    if(result > 0) { // valid channel
        int res = ischangedPMTinfo(listPMTInfo, service_uid);
        if(res == 0) {
            es_mitr = mMapESInfo.find(service_uid.c_str());
            if(es_mitr != mMapESInfo.end()) {
                // data found
                mMapESInfo.erase(es_mitr);
                //L_INFO(TAG, "erase program number [%d]\n",(*itr)->program_number);
            }
            mMapESInfo.insert(make_pair(service_uid.c_str(), elemList));
            notifyEvent((void*) &event);
        } else if(res == 1) {
            mMapESInfo.insert(make_pair(service_uid.c_str(), elemList));
            notifyEvent((void*) &event);
        }
    }

    if(mDatabase->ExecuteMulti(querylist) < 0) {
        deleteDBFile();
    }
    querylist.clear();
    for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        vector < string > temp;
        iinfo.swap(temp);
        temp.clear();
        iinfo.clear();
    }
    rResults.clear();
//    listPMTInfo.clear();
    return n_CHCount;
}

int SIDataManager::deleteEventList(string service_uid)
{
    char *strQuery;

    // delete event by service_uid
    strQuery = sqlite3_mprintf("DELETE FROM event_info WHERE service_uid = '%q';", service_uid.c_str());
    if(strQuery != NULL) {
        if(mDatabase->Execute(strQuery) < 0) {
            L_ERROR(TAG, "strQuery[%s] Failed\n", strQuery);
            sqlite3_free(strQuery);
            return -1;
        }
    }

    sqlite3_free(strQuery);
    siMiliSleep(100);
    return 0;
}

int SIDataManager::removeOldServiceList(list<ServiceInfo*> listNewServiceInfo)
{
    char* strQuery;
    string tmpStr = "";

    list < string > querylist;

    L_DEBUG(TAG, "Remove ServiceInfo List[%u]\n", listNewServiceInfo.size());

    map<string, list<elementary_stream> >::iterator es_mitr;

    for (list<ServiceInfo *>::iterator itr = listNewServiceInfo.begin(); itr != listNewServiceInfo.end(); itr++) {
        ServiceInfo* serviceInfo = (ServiceInfo*) (*itr);
//        L_DEBUG(TAG, "Delete ServiceInfo uid[%s]\n", serviceInfo->service_uid.c_str());

        if(serviceInfo->listCAInfo.empty() == false) {
            strQuery = sqlite3_mprintf("DELETE FROM ca_info WHERE service_uid='%q';", serviceInfo->service_uid.c_str());
            tmpStr = strQuery;
            sqlite3_free(strQuery);
            if(!tmpStr.empty())
                querylist.push_back(tmpStr);
        }
        strQuery = sqlite3_mprintf("DELETE FROM service_info WHERE transport_stream_id = %u;", serviceInfo->transport_stream_id);
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

       strQuery = sqlite3_mprintf("DELETE FROM elementary_stream_info WHERE service_uid LIKE '%%.%d.%%';", serviceInfo->transport_stream_id);
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        es_mitr = mMapESInfo.find(serviceInfo->service_uid.c_str());
        if(es_mitr != mMapESInfo.end()) {
            // data found
            mMapESInfo.erase(es_mitr);
        }
    }

    if(mDatabase->ExecuteMulti(querylist) < 0) {
        deleteDBFile();
    }

    querylist.clear();

    return 0;
}

int SIDataManager::deleteServiceList(list<ServiceInfo*> listServiceInfo, list<string> &querylist)
{
    char* strQuery;
    string tmpStr = "";

    map<string, list<elementary_stream> >::iterator es_mitr;

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* serviceInfo = (ServiceInfo*) (*itr);

        if(serviceInfo->listCAInfo.empty() == false) {
            strQuery = sqlite3_mprintf("DELETE FROM ca_info WHERE service_uid='%q';", serviceInfo->service_uid.c_str());
            tmpStr = strQuery;
            sqlite3_free(strQuery);
            if(!tmpStr.empty())
                querylist.push_back(tmpStr);
        }
        strQuery = sqlite3_mprintf("DELETE FROM service_info WHERE service_uid= '%q';", serviceInfo->service_uid.c_str());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        strQuery = sqlite3_mprintf("DELETE FROM elementary_stream_info WHERE service_uid='%q';", serviceInfo->service_uid.c_str());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        es_mitr = mMapESInfo.find(serviceInfo->service_uid.c_str());
        if(es_mitr != mMapESInfo.end()) {
            // data found
            mMapESInfo.erase(es_mitr);
        }
    }

    return 0;
}

int SIDataManager::updateServiceList(list<ServiceInfo*> listServiceInfo)
{
	if(listServiceInfo.empty()) {
		L_WARN(TAG, "ServiceList is empty !! \n");
		return 0;
	}
    char *strQuery;
    string tmpStr = "";
    list < string > querylist;
    list<ServiceInfo*>::iterator itr;
    int updatecount =0;
//    L_INFO(TAG, "listServiceInfo.size() is %u\n", listServiceInfo.size());

    removeOldServiceList(listServiceInfo);

//    deleteServiceList(listServiceInfo, querylist);

    for (itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* info = (ServiceInfo*) (*itr);
        if(isLocalChannel(info->local_area_code) == 0 || info->running_status != 4) {
            L_INFO(TAG, "Invalid channel [%d] local code[%d] , running status [%d]\n", info->channel_number,info->local_area_code,info->running_status);
            continue;
        }
        if(isDTVChannel(info->service_type) == 0 && isDRadioChannel(info->service_type) == 0) { // not digital tv channel

            L_INFO(TAG, "Invalid channel not digital tv channel [%d] service_type[%d]\n", info->channel_number, info->service_type);
            continue;
        }
        updatecount++;
//        L_INFO(TAG, "insert channel [%d] name[%s] tsid[%d] serviceuid[%s]\n", info->channel_number, info->service_name.c_str(),
//                info->transport_stream_id, info->service_uid.c_str());

        strQuery =
                sqlite3_mprintf(
                        "INSERT INTO service_info VALUES ('%q', %u, %d, %d, '%q', %d, %u, %d, %u, %d, %u, %d, %d, %d, %d, %d, %d, %d, %d, %u, %d, '%q', %u, %d, %d, %d);",
                        info->service_uid.c_str(), info->transport_stream_id, info->service_id, info->service_type, info->service_name.c_str(),
                        info->channel_number, info->IP_address, info->TS_port_number, info->frequency, info->modulation, info->symbol_rate,
                        info->FEC_inner, info->genre_code, info->category_code, info->running_status, info->free_CA_mode, info->channel_rating,
                        info->local_area_code, info->pay_channel, info->pay_ch_sample_sec, info->resolution, info->channel_image_url.c_str(),
                        info->pcr_pid, info->list_product_id.size(), info->list_elementary_stream.size(), info->listCAInfo.size());
        //add string list
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        if(info->list_elementary_stream.empty()) {
            strQuery =
                    sqlite3_mprintf(
                            "UPDATE service_info SET les_count=(SELECT count(*) FROM elementary_stream_info where elementary_stream_info.service_uid = '%q') WHERE service_info.service_uid='%q';",
                            info->service_uid.c_str(), info->service_uid.c_str());
            tmpStr = strQuery;
            sqlite3_free(strQuery);
            if(!tmpStr.empty())
                querylist.push_back(tmpStr);
        }

        if(info->listCAInfo.size() > 0) {
            list<CAInfo>::iterator c_itr;
            for (c_itr = info->listCAInfo.begin(); c_itr != info->listCAInfo.end(); c_itr++) {
                strQuery = sqlite3_mprintf("INSERT INTO ca_info VALUES ('%q', %d, %d);", info->service_uid.c_str(), c_itr->CA_system_ID,
                        c_itr->CA_PID);

                tmpStr = strQuery;
                sqlite3_free(strQuery);
                if(!tmpStr.empty())
                    querylist.push_back(tmpStr);
            }
        }

    }

    if(mDatabase->ExecuteMulti(querylist) < 0) {
        deleteDBFile();
    }
    querylist.clear();
    listServiceInfo.clear();
#ifdef __DEBUG_LOG_DM__
    L_INFO(TAG, "listServiceInfo.size() is %u , update channel count[%d]\n", listServiceInfo.size(),updatecount );
#endif
    return 0;
}

int SIDataManager::updateEventList(string service_uid, vector<EventInfo*> listEventInfo, bool bWebEPG)
{
    char *strQuery;
    char event_uid[40];
    string tmpStr = "";
    list < string > querylist;

    if(listEventInfo.empty()) {
        L_INFO(TAG, "Eventlist is Empty !! \n");
        return 0;
    }

    // delete event by service_uid
    if(deleteEventList(service_uid) < 0) {
        deleteDBFile();
        return -1;
    }

    int count = 0;
    for (vector<EventInfo*>::iterator itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
        memset(event_uid, 0, sizeof(event_uid));
        snprintf(event_uid, sizeof(event_uid), "%s.%d", service_uid.c_str(), (*itr)->event_id);

        strQuery =
                sqlite3_mprintf(
                        "INSERT INTO event_info VALUES ('%q', %d, %u, '%q', '%q', '%q', '%q', %d, %d, %d, %u, %u, %d, '%q', '%q', '%q', '%q', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);",
                        (*itr)->service_uid.c_str(), (*itr)->service_id, (*itr)->event_id, event_uid, (*itr)->event_name.c_str(),
                        (*itr)->event_short_description.c_str(), (*itr)->event_description.c_str(), (*itr)->running_status, (*itr)->free_CA_mode,
                        (*itr)->rating, (*itr)->start_time, (*itr)->end_time, (*itr)->duration, (*itr)->image_url.c_str(),
                        (*itr)->director_name.c_str(), (*itr)->actors_name.c_str(), (*itr)->price_text.c_str(), (*itr)->dolby, (*itr)->audio,
                        (*itr)->resolution, (*itr)->content_nibble_level_1, (*itr)->content_nibble_level_2, (*itr)->user_nibble_1,
                        (*itr)->user_nibble_2, (*itr)->caption, (*itr)->dvs, (*itr)->listProgramLinkedInfo.size());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        if(++count > 50) {
            count = 0;
            if(mDatabase->ExecuteMulti(querylist) < 0) {
                deleteDBFile();
            }

            querylist.clear();
            siMiliSleep(50);
        }
    }

    if(mDatabase->ExecuteMulti(querylist) < 0) {
        deleteDBFile();
    }
    listEventInfo.clear();
    return 0;
}

int SIDataManager::selectDBEventInfo(char* strQuery, list<EventInfo*> &listEventInfo)
{
    if(strQuery == NULL) {
        return -1;
    }

    list < vector<string> > rResults;

    if(mDatabase->Select(strQuery, rResults) < 0) {
        L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
        deleteDBFile();
        return -1;
    }

    for (list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        EventInfo* event = new EventInfo;

        makeEventInfoFromDBData(iinfo, event);
        listEventInfo.push_back(event);
    }

    for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        vector < string > temp;
		iinfo.swap(temp);
		temp.clear();
        iinfo.clear();
    }
    rResults.clear();

    return 0;
}

int SIDataManager::getProgramList(int serviceid, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_id = %d ORDER BY start_time;", serviceid);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Service ID[%d] : Program Info Size[%d]\n", serviceid, listEventInfo.size());
    return 0;
}

int SIDataManager::getProgramList(string service_uid, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_uid = '%q' ORDER BY start_time;", service_uid.c_str());

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Channel UID[%s] : Program Info Size[%d] \n", service_uid.c_str(), listEventInfo.size());
    return 0;
}

int SIDataManager::getProgramListbyIndex(int serviceid, int startIndex, int reqCount, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_id = %d ORDER BY start_time LIMIT %d, %d;", serviceid, startIndex, reqCount);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Service ID[%d] : index [%u] count [%u] : size[%u]\n", serviceid, startIndex, reqCount, listEventInfo.size());
    return 0;
}

int SIDataManager::getProgramListbyIndex(string channelUid, int startIndex, int reqCount, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_uid = '%q' ORDER BY start_time LIMIT %d, %d;", channelUid.c_str(), startIndex,
            reqCount);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "ChannelUid[%s] : index [%u] count [%u] : size[%u]\n", channelUid.c_str(), startIndex, reqCount, listEventInfo.size());
    return 0;
}

int SIDataManager::getProgramListbyTime(int serviceid, int startTime, int endTime, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery =
            sqlite3_mprintf(
                    "SELECT * FROM event_info WHERE service_id = %d AND ((start_time >= %u AND end_time <= %u) OR (start_time <= %u AND end_time >= %u) OR (start_time <= %u AND end_time >= %u)) ORDER BY start_time;",
                    serviceid, startTime, endTime, startTime, startTime, endTime, endTime);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Service ID[%d] : Time [%s ~ %s] : Size[%u]\n", serviceid, TimeConvertor::getInstance().convertSeconds2Date(startTime).c_str(),
            TimeConvertor::getInstance().convertSeconds2Date(endTime).c_str(), listEventInfo.size());

    return 0;
}

int SIDataManager::getProgramListbyTime(string channelUid, int startTime, int endTime, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery =
            sqlite3_mprintf(
                    "SELECT * FROM event_info WHERE service_uid = '%q' AND ((start_time >= %u AND end_time <= %u) OR (start_time <= %u AND end_time >= %u) OR (start_time <= %u AND end_time >= %u)) ORDER BY start_time;",
                    channelUid.c_str(), startTime, endTime, startTime, startTime, endTime, endTime);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Channel UID[%s] : Time [%s ~ %s], size[%u]\n", channelUid.c_str(),
            TimeConvertor::getInstance().convertSeconds2Date(startTime).c_str(), TimeConvertor::getInstance().convertSeconds2Date(endTime).c_str(),
            listEventInfo.size());

    return 0;
}

int SIDataManager::getProgramListbyName(int serviceid, int currentTime, string programName, bool DesFlag, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    if(DesFlag) {
        strQuery =
                sqlite3_mprintf(
                        "SELECT * FROM event_info WHERE service_id = %d AND (event_name LIKE '%%%s%%' ESCAPE '\\' OR event_short_description LIKE '%%%s%%' ESCAPE '\\' OR event_description LIKE '%%%s%%' ESCAPE '\\') AND ((start_time <= %u and end_time >= %u) OR start_time > %u) ORDER BY start_time;",
                        serviceid, programName.c_str(), programName.c_str(), programName.c_str(), currentTime, currentTime, currentTime);
    } else {
        strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_id = %d AND event_name LIKE '%%%s%%' ESCAPE '\\' ORDER BY start_time;",
                serviceid, programName.c_str());
    }

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Service ID[%d] : event_name [%s], size[%u]\n", serviceid, programName.c_str(), listEventInfo.size());

    return 0;
}

int SIDataManager::getProgramListbyName(string channelUid, int currentTime, string programName, bool DesFlag, list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    if(DesFlag) {
        if(strcmp(channelUid.c_str(), "0.0.0") == 0) {
            strQuery =
                    sqlite3_mprintf(
                            "SELECT * FROM event_info WHERE (event_name LIKE '%%%s%%' ESCAPE '\\' OR event_short_description LIKE '%%%s%%' ESCAPE '\\' OR event_description LIKE '%%%s%%' ESCAPE '\\') AND ((start_time <= %u and end_time >= %u) OR start_time > %u) ORDER BY start_time LIMIT 100;",
                            programName.c_str(), programName.c_str(), programName.c_str(), currentTime, currentTime, currentTime);
        } else {
            strQuery =
                    sqlite3_mprintf(
                            "SELECT * FROM event_info WHERE service_uid = '%q' AND (event_name LIKE '%%%s%%' ESCAPE '\\' OR event_short_description LIKE '%%%s%%' ESCAPE '\\' OR event_description LIKE '%%%s%%' ESCAPE '\\') ORDER BY start_time;",
                            channelUid.c_str(), programName.c_str(), programName.c_str(), programName.c_str());
        }
    } else {
        if(strcmp(channelUid.c_str(), "0.0.0") == 0) {
            strQuery =
                    sqlite3_mprintf(
                            "SELECT * FROM event_info WHERE event_name LIKE '%%%s%%' ESCAPE '\\' AND ((start_time <= %u and end_time >= %u) OR start_time > %u) ORDER BY start_time LIMIT 100;",
                            programName.c_str(), currentTime, currentTime, currentTime);
        } else {
            strQuery = sqlite3_mprintf(
                    "SELECT * FROM event_info WHERE service_uid = '%q' AND event_name LIKE '%%%s%%' ESCAPE '\\' ORDER BY start_time;",
                    channelUid.c_str(), programName.c_str());
        }
    }

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Channel UID[%s] : event_name [%s], size[%u]\n", channelUid.c_str(), programName.c_str(), listEventInfo.size());
    return 0;
}

int SIDataManager::getProgramListbySeries(string channelUid, int requestTime, string programName, int seasonNo, int episodeNo,
        list<EventInfo*> &listEventInfo)
{
    char* strQuery;

    strQuery =
            sqlite3_mprintf(
                    "SELECT *, min(start_time) FROM event_info WHERE service_uid = '%q' AND event_name = '%q' AND (user_nibble_1 + 0) = %u AND (user_nibble_2 + 0) > %u AND start_time >= %u GROUP BY user_nibble_2 ORDER BY (user_nibble_2 + 0) LIMIT 100;",
                    channelUid.c_str(), programName.c_str(), seasonNo, episodeNo, requestTime);

    selectDBEventInfo(strQuery, listEventInfo);

    sqlite3_free(strQuery);

    L_DEBUG(TAG, "Channel UID[%s] : event_name [%s] seasonNo [%u] episodeNo [%u] : Size[%u]\n", channelUid.c_str(), programName.c_str(), seasonNo,
            episodeNo, listEventInfo.size());
    return 0;
}

int SIDataManager::getServiceUid(uint16_t program_number, string *service_uid)
{
    char* strQuery;
    list < vector<string> > rResults;

    strQuery = sqlite3_mprintf("SELECT service_uid FROM service_info where service_id = %u;", program_number);
    if(strQuery != NULL) {
        if(mDatabase->Select(strQuery, rResults) < 0) {
            L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
            sqlite3_free(strQuery);
            deleteDBFile();
            return -1;
        }
    }

    if(rResults.empty()) {
    	sqlite3_free(strQuery);
        return -1;
    }
    sqlite3_free(strQuery);

    list<vector<string> >::iterator itr = rResults.begin();
    vector < string > iinfo = (*itr);
    *service_uid = iinfo[0];


    for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        vector < string > temp;
		iinfo.swap(temp);
		temp.clear();
        iinfo.clear();
    }
    rResults.clear();

    return 0;
}

int SIDataManager::getServiceUidList()
{
    char* strQuery;
    list < vector<string> > rResults;

    strQuery = sqlite3_mprintf("SELECT service_uid FROM service_info ORDER BY service_id;");
    if(strQuery != NULL) {
        if(mDatabase->Select(strQuery, rResults) < 0) {
            L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
            sqlite3_free(strQuery);
            deleteDBFile();
            return -1;
        }
    }

    list<vector<string> >::iterator itr;
    for (itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        mSetChannelUid.insert(iinfo[0]);
    }
    sqlite3_free(strQuery);

    for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);
        vector < string > temp;
		iinfo.swap(temp);
		temp.clear();
        iinfo.clear();
    }
    rResults.clear();

    return 0;
}

void SIDataManager::makeEventInfoFromDBData(vector<string> iinfo, EventInfo* eventinfo)
{
    char* strQuery;

    string event_uid = "";
    eventinfo->service_uid = iinfo[EI_SERVICE_UID];
    eventinfo->service_id = atoi(iinfo[EI_SERVICE_ID].c_str());
    eventinfo->event_id = atoi(iinfo[EI_EVENT_ID].c_str());
    event_uid = iinfo[EI_EVENT_UID];
    eventinfo->event_name = iinfo[EI_EVENT_NAME];
    eventinfo->event_short_description = iinfo[EI_EVENT_SHORT_DESCRIPTION];
    eventinfo->event_description = iinfo[EI_EVENT_DESCRIPTION];
    eventinfo->running_status = atoi(iinfo[EI_RUNNIG_STATUS].c_str());
    eventinfo->free_CA_mode = atoi(iinfo[EI_FREE_CA_MODE].c_str());
    eventinfo->rating = atoi(iinfo[EI_RATING].c_str());
    eventinfo->start_time = atoi(iinfo[EI_START_TIME].c_str());
    eventinfo->end_time = atoi(iinfo[EI_END_TIME].c_str());
    eventinfo->duration = atoi(iinfo[EI_DURATION].c_str());
    eventinfo->image_url = iinfo[EI_IMAGE_URL];
    eventinfo->director_name = iinfo[EI_DIRECTOR_NAME];
    eventinfo->actors_name = iinfo[EI_ACTORS_NAME];
    eventinfo->price_text = iinfo[EI_PRICE_TEXT];
    eventinfo->dolby = atoi(iinfo[EI_DOLBY].c_str());
    eventinfo->audio = atoi(iinfo[EI_AUDIO].c_str());
    eventinfo->resolution = atoi(iinfo[EI_RESOLUTION].c_str());
    eventinfo->content_nibble_level_1 = atoi(iinfo[EI_CONTENT_NIBBLE_LEVEL_1].c_str());
    eventinfo->content_nibble_level_2 = atoi(iinfo[EI_CONTENT_NIBBLE_LEVEL_2].c_str());
    eventinfo->user_nibble_1 = atoi(iinfo[EI_USER_NIBBLE_1].c_str());
    eventinfo->user_nibble_2 = atoi(iinfo[EI_USER_NIBBLE_2].c_str());
    eventinfo->caption = atoi(iinfo[EI_CAPTION].c_str());
    eventinfo->dvs = atoi(iinfo[EI_DVS].c_str());
}

void SIDataManager::makeServiceInfoFromDBData(vector<string> iinfo, ServiceInfo* serviceinfo)
{
    char* strQuery;
    int i = 0;

    serviceinfo->service_uid = iinfo[SVI_SERVICE_UID];
    serviceinfo->service_id = atoi(iinfo[SVI_SERVICE_ID].c_str());
    serviceinfo->service_type = atoi(iinfo[SVI_SERVICE_TYPE].c_str());
    serviceinfo->service_name = iinfo[SVI_SERVICE_NAME];
    serviceinfo->channel_number = atoi(iinfo[SVI_CHANNEL_NUMBER].c_str());
    serviceinfo->IP_address = atoll(iinfo[SVI_IP_ADDRESS].c_str());
    serviceinfo->TS_port_number = atoi(iinfo[SVI_TS_PORT_NUMBER].c_str());
    serviceinfo->frequency = atoi(iinfo[SVI_FREQUENCY].c_str());
    serviceinfo->modulation = atoi(iinfo[SVI_MODULATION].c_str());
    serviceinfo->symbol_rate = atoi(iinfo[SVI_SYMBOL_RATE].c_str());
    serviceinfo->FEC_inner = atoi(iinfo[SVI_FEC_INNER].c_str());
    serviceinfo->genre_code = atoi(iinfo[SVI_GENRE_CODE].c_str());
    serviceinfo->category_code = atoi(iinfo[SVI_CATEGORY_CODE].c_str());
    serviceinfo->running_status = atoi(iinfo[SVI_RUNNING_STATUS].c_str());
    serviceinfo->free_CA_mode = atoi(iinfo[SVI_FREE_CA_MODE].c_str());
    serviceinfo->channel_rating = atoi(iinfo[SVI_CHANNEL_RATING].c_str());
    serviceinfo->local_area_code = atoi(iinfo[SVI_LOCAL_AREA_CODE].c_str());
    serviceinfo->pay_channel = atoi(iinfo[SVI_PAY_CHANNEL].c_str());
    serviceinfo->pay_ch_sample_sec = atoi(iinfo[SVI_PAY_CH_SAMPLE_SEC].c_str());
    serviceinfo->resolution = atoi(iinfo[SVI_RESOLUTION].c_str());
    serviceinfo->channel_image_url = iinfo[SVI_CHANNEL_IMAGE_URL];
    serviceinfo->pcr_pid = atoi(iinfo[SVI_PCR_PID].c_str());

    if(atoi(iinfo[SVI_PRODUCTID_INFO_SIZE].c_str()) > 0) {
        list < vector<string> > rResults;

        strQuery = sqlite3_mprintf("SELECT * FROM product_id_info WHERE service_uid = '%q';", serviceinfo->service_uid.c_str());
        if(strQuery != NULL) {
            if(mDatabase->Select(strQuery, rResults) < 0) {
                L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
                sqlite3_free(strQuery);
                deleteDBFile();
                return;
            }
        }

        sqlite3_free(strQuery);
        list<vector<string> >::iterator pitr;
        for (pitr = rResults.begin(); pitr != rResults.end(); pitr++) {
            vector < string > iinfo = (*pitr);
            serviceinfo->list_product_id.push_back(iinfo[1]);
        }

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }

    if(atoi(iinfo[SVI_ES_INFO_SIZE].c_str()) > 0) {
        list < vector<string> > rResults;

        strQuery = sqlite3_mprintf("SELECT * FROM elementary_stream_info WHERE service_uid = '%q';", serviceinfo->service_uid.c_str());
        if(strQuery != NULL) {
            if(mDatabase->Select(strQuery, rResults) < 0) {
                L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
                sqlite3_free(strQuery);
                deleteDBFile();
                return;
            }
        }
        sqlite3_free(strQuery);
        list<vector<string> >::iterator eitr;
        for (eitr = rResults.begin(); eitr != rResults.end(); eitr++) {
            vector < string > iinfo = (*eitr);
            elementary_stream elstreamInfo;

            elstreamInfo.stream_pid = atoi(iinfo[ES_STREAM_PID].c_str());
            elstreamInfo.stream_type = atoi(iinfo[ES_STREAM_TYPE].c_str());
            elstreamInfo.language_code = atoi(iinfo[ES_LANGUAGE_CODE].c_str());
            serviceinfo->list_elementary_stream.push_back(elstreamInfo);
        }

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }

    if(atoi(iinfo[SVI_CA_INFO_SIZE].c_str()) > 0) {
        list < vector<string> > rResults;

        strQuery = sqlite3_mprintf("SELECT * FROM ca_info WHERE service_uid = '%q';", serviceinfo->service_uid.c_str());
        if(strQuery != NULL) {
            if(mDatabase->Select(strQuery, rResults) < 0) {
                L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
                sqlite3_free(strQuery);
                deleteDBFile();
                return;
            }
        }
        sqlite3_free(strQuery);
        list<vector<string> >::iterator citr;
        for (citr = rResults.begin(); citr != rResults.end(); citr++) {
            vector < string > iinfo = (*citr);
            CAInfo cainfo;
            cainfo.CA_system_ID = atoi(iinfo[CA_SYSTEM_ID].c_str());
            cainfo.CA_PID = atoi(iinfo[CA_PID].c_str());
            serviceinfo->listCAInfo.push_back(cainfo);
        }

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }
}

int SIDataManager::getCurrentProgramInfo(int serviceid, EventInfo** eventinfo)
{
    char* strQuery;
    int result = 0;
    list < vector<string> > rResults;
    if(eventinfo == NULL)
        return -1;

    uint32_t nowtime = TimeConvertor::getInstance().getCurrentSecond();

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_id = %d AND start_time <= %u AND end_time >= %u;", serviceid, nowtime,
            nowtime);

    if(strQuery != NULL) {
        result = mDatabase->Select(strQuery, rResults);
        if(result < 0) {
            L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
            deleteDBFile();
            sqlite3_free(strQuery);
            return -1;
        }

        for (list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            makeEventInfoFromDBData(iinfo, *eventinfo);
        }

        sqlite3_free(strQuery);

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }

    return result;
}

int SIDataManager::getCurrentProgramInfo(string service_uid, EventInfo** eventinfo)
{
    char* strQuery;
    int result = 0;
    list < vector<string> > rResults;
    if(eventinfo == NULL)
        return -1;

    uint32_t nowtime = TimeConvertor::getInstance().getCurrentSecond();
    // resolution setting from es info
    strQuery =
            sqlite3_mprintf(
                    "UPDATE event_info set resolution = (CASE WHEN (SELECT stream_type FROM elementary_stream_info WHERE stream_type = 27 AND service_uid ='%q') THEN 2 ELSE 1 END) WHERE service_uid = '%q';",
                    service_uid.c_str(), service_uid.c_str());
    if(strQuery != NULL) {
        mDatabase->Execute(strQuery);
    }
    sqlite3_free(strQuery);

    strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_uid = '%q' AND start_time <= %u AND end_time >= %u;", service_uid.c_str(),
            nowtime, nowtime);

    if(strQuery != NULL) {
        result = mDatabase->Select(strQuery, rResults);
        if(result < 0) {
            L_ERROR(TAG, "=== SQL Error : Select [%s] ===\n", strQuery);
            deleteDBFile();
            sqlite3_free(strQuery);
            return -1;
        }

        for (list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            makeEventInfoFromDBData(iinfo, *eventinfo);
        }

        sqlite3_free(strQuery);

        for(list<vector<string> >::iterator itr = rResults.begin(); itr != rResults.end(); itr++) {
            vector < string > iinfo = (*itr);
            vector < string > temp;
			iinfo.swap(temp);
			temp.clear();
            iinfo.clear();
        }
        rResults.clear();
    }

    return result;
}

int SIDataManager::getProductInfoList(list<ProductInfo> &listProductInfo)
{
    int res = 0;
    list<ProductInfo>::iterator itr;

    for (itr = mProductInfoList.begin(); itr != mProductInfoList.end(); itr++) {
        ProductInfo pInfo = (ProductInfo) (*itr);

        listProductInfo.push_back(pInfo);
    }
    return mProductInfoList.size();
}

void SIDataManager::sendChannelReady(int mVersion)
{
    //SqliteLock(__FUNCTION__);

    SIChangeEvent event_map_ready;
	event_map_ready.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
	event_map_ready.setChannelVersion(mVersion);
	notifyEvent(&event_map_ready);
	setChannelMapReadyFlag(true);
	//SqliteUnlock(__FUNCTION__);
}

int SIDataManager::saveDefaultChannel(string default_ch)
{
	SqliteLock(__FUNCTION__);
	SIChangeEvent event;
	event.setEventType(SIChangeEvent::SET_DEFAULT_CH);
	event.setChannelUID(default_ch);
	L_INFO(TAG,"default ch [%s]\n",default_ch.c_str());
	notifyEvent(&event);
	SqliteUnlock(__FUNCTION__);
}

int SIDataManager::saveChannelInfo(list<ServiceInfo*> ChannelList, SIChangeEvent event, int mVersion, bool bdummyEpgMode)
{
    SqliteLock(__FUNCTION__);

    L_INFO(TAG, "Event[%s] : Channel Size[%u]\n", event.toString().c_str(), ChannelList.size());

    if(event.getEventType() == SIChangeEvent::CHANNEL_MAP_COMPLETED && !getChannelMapReadyFlag()) {
     		SIChangeEvent event_map_ready;
     		event_map_ready.setEventType(SIChangeEvent::CHANNEL_MAP_READY);
     		event_map_ready.setChannelVersion(mVersion);
     		if(!getScanMode())
     			notifyEvent(&event_map_ready);
     		else
     			L_INFO(TAG, "Scan Processing.... Not Send Event [CHANNEL_MAP_READY]");
     		setChannelMapReadyFlag(true);
     }

    updateServiceList(ChannelList);

   if(event.getEventType() == SIChangeEvent::CHANNEL_MAP_COMPLETED && !bdummyEpgMode) {
        mServiceStatus = SERVICE_DB_COMPLETE;
    } else {
    	if(!getScanMode())
    		notifyEvent((void*) &event);
    	else
    		L_INFO(TAG, "Scan Processing.... Not Send Event [CHANNEL_MAP_READY]");
    }
    SqliteUnlock(__FUNCTION__);
    return 0;
}

int SIDataManager::saveProgramInfo(string service_uid, vector<EventInfo*> listEventInfo, SIChangeEvent event, bool bEitPf, bool bWebEPG)
{
    SqliteLock(__FUNCTION__);

    L_INFO(TAG, "Service UID[%s] : Received ProgramInfo[%u]\n", service_uid.c_str(), listEventInfo.size());
    L_DEBUG(TAG, "ProgramInfo RECEIVED SUCCESS \n\n");
    if (getScanMode()){
    	L_INFO(TAG, " ### ScanProcessing ProgramInfo Not SAVE \n");
    	vector<EventInfo*>::iterator itr;
		for (itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
			delete (*itr);
		}
		listEventInfo.clear();

    	return 0;
    } else{
    	updateEventList(service_uid, listEventInfo, bWebEPG);
    }
	vector<EventInfo*>::iterator itr;
	for (itr = listEventInfo.begin(); itr != listEventInfo.end(); itr++) {
		delete (*itr);
	}
	listEventInfo.clear();

    if(event.getEventType() == SIChangeEvent::PROGRAM_MAP_COMPLETED) { // 여기서 백업
        mEventStatus = PROGRAM_DB_COMPLETE;
        // db back-up
        backupDB();
    }

    notifyEvent((void*) &event);
    SqliteUnlock(__FUNCTION__);
    return 0;

}

void SIDataManager::ready()
{
    // 이 함수 호출이 된 후부터 데이타를 저장한다.
}

void SIDataManager::clear()
{
    // 현재까지 작업한 메모리 또는 DB를 지운다.
}

int SIDataManager::savePMTInfo(list<PMTInfo*> listPMTInfo, SIChangeEvent event)
{
    SqliteLock(__FUNCTION__);

    int nCount = insertPMTList(listPMTInfo, event);
    mPMTCount += nCount;

    L_INFO(TAG, "PMT Received : pmt count[%d] found ch[%d] total ch[%d]\n", listPMTInfo.size(), nCount, mPMTCount);

    SqliteUnlock(__FUNCTION__);
    return 0;
}

int SIDataManager::saveProductInfo(list<ProductInfo> listProductInfo)
{
    SqliteLock(__FUNCTION__);

    for (list<ProductInfo>::iterator itr = listProductInfo.begin(); itr != listProductInfo.end(); itr++) {
        ProductInfo pInfo = (ProductInfo) (*itr);

        mProductInfoList.push_back(pInfo);
    }

    L_INFO(TAG, "ProductInfo Received list count[%d] total[%u]\n", listProductInfo.size(), mProductInfoList.size());

    SqliteUnlock(__FUNCTION__);

    return 0;
}

int SIDataManager::notifySIEvent(SIChangeEvent event)
{
    SqliteLock(__FUNCTION__);
    if(event.getEventType() == SIChangeEvent::SCAN_READY) {

        mPMTCount = 0;
        backupDB();
        if(event.getToBeReceived() > 0) { // received NIT
            resetDB();
            L_INFO(TAG, "[SCAN DEBUG] start : found freq = %d\n", event.getToBeReceived());
        } else {
            L_INFO(TAG, "[SCAN FAIL] start : not make freq map [%d]\n",event.getToBeReceived());
        }

    } else if(event.getEventType() == SIChangeEvent::SCAN_PROCESSING) {
        event.setFoundChannel(mPMTCount);
        L_INFO(TAG, "[SCAN DEBUG] processing : progress = %d found ch = %d\n", event.getReceived(), event.getFoundChannel());
    } else if(event.getEventType() == SIChangeEvent::SCAN_COMPLETED) {
        if(event.getFoundChannel() == -1) { // scan complete with problem.
            recoverDB();
            L_INFO(TAG, "[SCAN FAIL] completed : not make ch db\n");
        } else {
            event.setFoundChannel(mPMTCount);
            int nCount = countCHDB();
            L_INFO(TAG, "[SCAN DEBUG] completed : db count = %d found ch = %d\n", nCount, event.getFoundChannel());
        }
        mPMTCount = 0;
    } else if(event.getEventType() == SIChangeEvent::SCAN_STOP) {
        L_INFO(TAG, "[SCAN DEBUG] stop : \n");
        mPMTCount = 0;
    } else if(event.getEventType() == SIChangeEvent::TDT_DATE) {
        //L_INFO(TAG, "TDT time [%ld]\n", event.getTDTDate());
    }

    notifyEvent((void*) &event);
    SqliteUnlock(__FUNCTION__);
    return 0;
}

int SIDataManager::notifyScanProcessTest(SIChangeEvent event)
{
    //ksh 14.03.13 add for linknet factory test
    if(event.getEventType() == SIChangeEvent::SCAN_READY) {
        if(event.getToBeReceived() > 0) {
            L_INFO(TAG, "[SCAN DEBUG] start : found freq = %d\n", event.getToBeReceived());
        } else {
            L_INFO(TAG, "[SCAN FAIL] start : not make freq map\n");
        }
    } else if(event.getEventType() == SIChangeEvent::SCAN_PROCESSING) {
        L_INFO(TAG, "[SCAN DEBUG] processing : progress = %d found ch = %d\n", event.getReceived(), event.getFoundChannel());
    } else if(event.getEventType() == SIChangeEvent::SCAN_COMPLETED) {
        if(event.getFoundChannel() == -1) { // scan complete with problem.
            L_INFO(TAG, "[SCAN FAIL] completed : not make ch db\n");
        } else {
            L_INFO(TAG, "[SCAN DEBUG] completed : found ch = %d\n", event.getFoundChannel());
        }
    } else if(event.getEventType() == SIChangeEvent::SCAN_STOP) {
        L_INFO(TAG, "[SCAN DEBUG] stop : \n");
    }

    notifyEvent((void*) &event); // >> notify
    return 0;
}

void SIDataManager::setListNITInfo(list<NITInfo*> listNITInfo, SIChangeEvent event)
{
    SqliteLock(__FUNCTION__);
    mSetChannelUid.clear();

    set<string>::iterator citr;
    map<string, list<elementary_stream> >::iterator es_mitr;

    char ch_uid[32];
    char *strQuery;
    string tmpStr = "";
    list < string > querylist;
    getServiceUidList();
    for (list<NITInfo*>::iterator itr = listNITInfo.begin(); itr != listNITInfo.end(); itr++) {
        NITInfo* info = (NITInfo *) (*itr);
        sprintf(ch_uid, "%u.%u.%u", info->original_network_id, info->transport_stream_id, info->service_id);
        citr = mSetChannelUid.find(ch_uid);

        if(citr != mSetChannelUid.end()) {
            // data found
            mSetChannelUid.erase(citr);
        }
    }

    for (citr = mSetChannelUid.begin(); citr != mSetChannelUid.end(); citr++) {
        // trash data delete
        L_INFO(TAG, "trash data service_uid [%s]\n", (*citr).c_str());

        strQuery = sqlite3_mprintf("DELETE FROM service_info WHERE service_uid = '%q';", (*citr).c_str());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        strQuery = sqlite3_mprintf("DELETE FROM elementary_stream_info WHERE service_uid = '%q';", (*citr).c_str());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);

        es_mitr = mMapESInfo.find((*citr).c_str());
        if(es_mitr != mMapESInfo.end()) {
            // data found
            mMapESInfo.erase(es_mitr);
        }

        strQuery = sqlite3_mprintf("DELETE FROM event_info WHERE service_uid = '%q';", (*citr).c_str());
        tmpStr = strQuery;
        sqlite3_free(strQuery);
        if(!tmpStr.empty())
            querylist.push_back(tmpStr);
    }

    if(mDatabase->ExecuteMulti(querylist) < 0) {
        deleteDBFile();
    }

	if(event.getEventType() == SIChangeEvent::CHANNEL_MAP_COMPLETED) {
		L_DEBUG(TAG, "NOTIFY : CHANNEL_MAP_COMPLETED\n");
		if (!getScanMode())
			notifyEvent((void*) &event); // >> notify
		else
			L_INFO(TAG, "Scan Processing.... Not Send Event [CHANNEL_MAP_COMPLETED]");
	}
    querylist.clear();
    SqliteUnlock(__FUNCTION__);
}
