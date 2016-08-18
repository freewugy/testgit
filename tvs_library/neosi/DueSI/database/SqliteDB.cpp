/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-08-12 17:10:52 +0900 (화, 12 8월 2014) $
 * $LastChangedRevision: 980 $
 * Description:
 * Note:
 *****************************************************************************/

#include <unistd.h>
#include "SqliteDB.h"
#include "Logger.h"
#include "Timer.h"
#include "ServiceInfo.h"
#include "EventInfo.h"

static const char* TAG = "SqliteDB";

SqliteDB::SqliteDB(const char *strFileName)
{
    int rc = 0;
    m_isError = false;

    rc = sqlite3_open_v2(strFileName, &mdb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(rc != SQLITE_OK) {
        m_isError = true;
    }
    sqlite3_busy_timeout(mdb, 500);

    char *zErrMsg = 0;
    rc = sqlite3_exec(mdb, "PRAGMA integrity_check; PRAGMA synchronous=OFF;PRAGMA count_changes=OFF;PRAGMA temp_store=memory;PRAGMA auto_vacuum=1;", 0, 0, &zErrMsg);

    if(rc != SQLITE_OK || zErrMsg != NULL) {
        m_isError = true;
        L_DEBUG(TAG, "PRAGMA synchronous=OFF;PRAGMA count_changes=OFF;PRAGMA temp_store=memory;PRAGMA auto_vacuum=1;\nResult[%s]\n", zErrMsg);
    }
    sqlite3_free(zErrMsg);
}

int SqliteDB::Send(const char *strQuery)
{
    sqlite3_stmt *stmt;
    int result = 0;

    int err = sqlite3_prepare_v2(this->mdb, strQuery, strlen(strQuery), &stmt, NULL);
    if(err != SQLITE_OK) {
        L_ERROR(TAG, "prepare[%s] failed\n", strQuery);
        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);

        return -1;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        L_INFO(TAG, "[%s] (%s)\n", sqlite3_errmsg(this->mdb), strQuery);
        result = -1;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

int SqliteDB::Select(const char *strQuery, list<vector<string> > &rResult)
{
    int err;
    int nColumn = 0;
    char tbuf[64];
    sqlite3_stmt *stmt;
    vector < string > rowList;

    err = sqlite3_prepare_v2(this->mdb, strQuery, -1, &stmt, NULL);

    if(err != SQLITE_OK) {
        L_ERROR(TAG, "prepare[%s] failed\n", strQuery);
        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);

        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nColumn = sqlite3_column_count(stmt);

        rowList.clear();
        for (int i = 0; i < nColumn; i++) {
            switch (sqlite3_column_type(stmt, i)) {
                case SQLITE_TEXT:
                rowList.push_back((const char *) sqlite3_column_text(stmt, i));
                    break;
                case SQLITE_INTEGER:
                sprintf(tbuf, "%d:", sqlite3_column_int(stmt, i));
                rowList.push_back(tbuf);
                    break;
                case SQLITE_FLOAT:
                sprintf(tbuf, "%f:", sqlite3_column_double(stmt, i));
                rowList.push_back(tbuf);
                    break;
                case SQLITE_NULL:
                    break;
                default:
                    break;
            }
        }
        rResult.push_back(rowList);
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    return rResult.size();
}

bool SqliteDB::isError()
{
    return m_isError;
}

int SqliteDB::Execute(const char *strQuery)
{
    int err;
    sqlite3_stmt *stmt;
    int result = 0;

    err = sqlite3_prepare_v2(this->mdb, strQuery, -1, &stmt, NULL);
    if(err) {
        L_ERROR(TAG, "prepare failed\n");
        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);

        return -1;
    }

    // begin
    result = this->Send("BEGIN;");
    if(result < 0) {
        L_ERROR(TAG, "BEGIN; failed\n");
        return -1;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        L_ERROR(TAG, "[%s]  (%s) result[%d]\n", sqlite3_errmsg(this->mdb), strQuery, result);

        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
        return -1;
    }

    // commit
    result = this->Send("COMMIT;");
    if(result < 0) {
        L_ERROR(TAG, "COMMIT; failed\n");
        return -1;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    return result;
}

int SqliteDB::ExecuteMulti(const list<string> queries)
{
    int i = 0;
    int results = 0;
    char* errMsg;

    if(queries.empty()) {
        L_DEBUG(TAG, "query request EMPTY\n");
        return 0;
    }
    // begin
    sqlite3_exec(this->mdb, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    if(results != SQLITE_OK && results != SQLITE_CONSTRAINT) {
        L_ERROR(TAG, "\t ==== sqlite3_exec ERROR : CODE[%d] , errMsg[%s] ====\n", results, errMsg);
        sqlite3_exec(this->mdb, "END TRANSACTION", NULL, NULL, &errMsg);

        return -1;
    }
    list<string>::const_iterator ils = queries.begin();

    while (ils != queries.end()) {

        int results = sqlite3_exec(this->mdb, ils->c_str(), NULL, NULL, &errMsg);
        if(results != SQLITE_OK && results != SQLITE_CONSTRAINT) {
            L_ERROR(TAG, "\t ==== sqlite3_exec ERROR : CODE[%d] , errMsg[%s] ====\n", results, errMsg);
            L_ERROR(TAG, "%s\n", ils->c_str());

            sqlite3_exec(this->mdb, "END TRANSACTION", NULL, NULL, &errMsg);
            return -1;
        }

        ils++;

        if(++i > 10) {
            siMiliSleep(10);
            i = 0;
        }
    }
    // end
    sqlite3_exec(this->mdb, "END TRANSACTION", NULL, NULL, &errMsg);
    if(results != SQLITE_OK && results != SQLITE_CONSTRAINT) {
        L_DEBUG(TAG, "\n\t ==== sqlite3_exec ERROR : CODE[%d] , errMsg[%s] ====\n", results, errMsg);
    }

    results = sqlite3_total_changes(this->mdb);

    return results;
}

int SqliteDB::createTable()
{
    char *strQuery;
    string tmpStr = "";
    list < string > querylist;
    /************************************************/
    /*                   EVENT                      */
    /************************************************/

    // event info table
    strQuery =
            sqlite3_mprintf(
                    "CREATE TABLE IF NOT EXISTS event_info (service_uid TEXT, service_id SHORT INTEGER, event_id UNSIGNED INTEGER, event_uid TEXT PRIMARY KEY,"
                            " event_name TEXT, event_short_description TEXT, event_description TEXT, running_status UNSIGNED CHAR, free_CA_mode UNSIGNED CHAR , rating UNSIGNED CHAR,"
                            " start_time UNSIGNED INTEGER, end_time UNSIGNED INTEGER, duration SHORT INTEGER, image_url TEXT, director_name TEXT, actors_name TEXT, price_text TEXT,"
                            " dolby UNSIGNED CHAR, audio UNSIGNED CHAR, resolution UNSIGNED CHAR, content_nibble_level_1 UNSIGNED CHAR, content_nibble_level_2 UNSIGNED CHAR,"
                            " user_nibble_1 UNSIGNED CHAR, user_nibble_2 UNSIGNED CHAR, caption UNSIGNED CHAR, dvs UNSIGNED CHAR, lpl_count UNSIGNED CHAR);");

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE INDEX IF NOT EXISTS service_id_idx ON event_info (service_id);");

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE INDEX IF NOT EXISTS service_id_idx2 ON event_info (service_uid);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE INDEX IF NOT EXISTS service_id_idx3 ON event_info (start_time);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE INDEX IF NOT EXISTS service_id_idx4 ON event_info (end_time);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    /************************************************/
    /*                   SERVICE                    */
    /************************************************/

    // service info table
    strQuery =
            sqlite3_mprintf(
                    "CREATE TABLE IF NOT EXISTS service_info (service_uid TEXT PRIMARY KEY, transport_stream_id UNSIGNED INTEGER, service_id UNSIGNED INTEGER, service_type UNSIGNED CHAR, service_name TEXT,"
                            " channel_number SHORT INTEGER, IP_address UNSIGNED INTEGER, TS_port_number SHORT INTEGER, frequency UNSIGNED INTEGER, modulation UNSIGNED CHAR, symbol_rate  UNSIGNED INTEGER,"
                            " FEC_inner UNSIGNED CHAR, genre_code UNSIGNED CHAR, category_code UNSIGNED CHAR, running_status UNSIGNED CHAR, free_CA_mode UNSIGNED CHAR, channel_rating UNSIGNED CHAR,"
                            " local_area_code UNSIGNED CHAR, pay_channel UNSIGNED CHAR, pay_ch_sample_sec UNSIGNED INTEGER, resolution UNSIGNED CHAR, channel_image_url TEXT, pcr_pid UNSIGNED INTEGER,"
                            " lpi_count UNSIGNED CHAR, les_count UNSIGNED CHAR, lca_count UNSIGNED CHAR);");

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE INDEX IF NOT EXISTS channel_number_idx ON service_info (channel_number);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // product id info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS product_id_info (service_uid TEXT, id_product TEXT);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // elementary stream info table
    sqlite3_free(strQuery);
    strQuery =
            sqlite3_mprintf(
                    "CREATE TABLE IF NOT EXISTS elementary_stream_info (service_uid TEXT, stream_pid SHORT INTEGER, stream_type UNSIGNED CHAR, language_code UNSIGNED INTEGER);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // ca info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS ca_info (service_uid TEXT, CA_system_ID SHORT INTEGER, CA_PID SHORT INTEGER);");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    return 0;
}

int SqliteDB::clearTable()
{

    char *strQuery;
    string tmpStr = "";
    /************************************************/
    /*                   EVENT                      */
    /************************************************/

    // event info table
    strQuery = sqlite3_mprintf("DROP TABLE if exists event_info;");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    /************************************************/
    /*                   SERVICE                    */
    /************************************************/
    // service info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("DROP TABLE if exists service_info;");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // product id info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("DROP TABLE if exists product_id_info;");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // elementary stream info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("DROP TABLE if exists elementary_stream_info;");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    // ca info table
    sqlite3_free(strQuery);
    strQuery = sqlite3_mprintf("DROP TABLE if exists ca_info;");
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return -1;
    }

    sqlite3_free(strQuery);
    return 0;
}

bool SqliteDB::checkDBSchema()
{
    char* strQuery;
    list < vector<string> > rResults;

    int res = -1;

    strQuery = sqlite3_mprintf("SELECT * FROM service_info ORDER BY channel_number LIMIT 1;");
    if(Select(strQuery, rResults) < 0) {
        L_DEBUG(TAG, "Channel DB Select ERROR\n");
        return false;
    }

    sqlite3_free(strQuery);

    list<vector<string> >::iterator itr;
    string service_uid;
    list < vector<string> > rResultsProgram;
    for (itr = rResults.begin(); itr != rResults.end(); itr++) {
        vector < string > iinfo = (*itr);

        string service_uid = iinfo[SVI_SERVICE_UID];
        strQuery = sqlite3_mprintf("SELECT * FROM event_info WHERE service_uid = '%q' ORDER BY start_time;", service_uid.c_str());

        res = Select(strQuery, rResultsProgram);

        sqlite3_free(strQuery);
        rResultsProgram.clear();
        if(res == -1) {
            L_DEBUG(TAG, "[%s] Program DB Select ERROR\n", service_uid.c_str());
            return false;
        }

        if(atoi(iinfo[SVI_ES_INFO_SIZE].c_str()) > 0) {
            list < vector<string> > rResultsES;

            strQuery = sqlite3_mprintf("SELECT * FROM elementary_stream_info WHERE service_uid = '%q';", service_uid.c_str());
            res = Select(strQuery, rResultsES);
            sqlite3_free(strQuery);
            if(res == -1) {
                L_DEBUG(TAG, "[%s] elementary_stream_info DB Select ERROR\n", service_uid.c_str());
                return false;
            }
        }
    }

    L_DEBUG(TAG, "DB Select Test OK\n");

    return true;
}

bool SqliteDB::updateDummyData()
{
    ServiceInfo* info = new ServiceInfo;
    info->service_uid = "99.99.99";

    // insert Dummy Channel Info
    char* strQuery;
    strQuery =
            sqlite3_mprintf(
                    "INSERT INTO service_info VALUES ('%q', %u, %d, %d, '%q', %d, %u, %d, %u, %d, %u, %d, %d, %d, %d, %d, %d, %d, %d, %u, %d, '%q', %u, %d, %d, %d);",
                    info->service_uid.c_str(), info->transport_stream_id, info->service_id, info->service_type, info->service_name.c_str(),
                    info->channel_number, info->IP_address, info->TS_port_number, info->frequency, info->modulation, info->symbol_rate,
                    info->FEC_inner, info->genre_code, info->category_code, info->running_status, info->free_CA_mode, info->channel_rating,
                    info->local_area_code, info->pay_channel, info->pay_ch_sample_sec, info->resolution, info->channel_image_url.c_str(),
                    info->pcr_pid, info->list_product_id.size(), info->list_elementary_stream.size(), info->listCAInfo.size());

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    // delete Dummy Channel Info
    strQuery = sqlite3_mprintf("DELETE FROM service_info WHERE service_uid = '%q';", info->service_uid.c_str());
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    L_INFO(TAG, "Channel Info DB OK\n");
    EventInfo* einfo = new EventInfo;
    einfo->service_uid = "99.99.99";
    einfo->service_id = 99;
    einfo->event_id = 99;
    einfo->event_short_description = " ===============================";
    einfo->event_description = " ===============================";
    einfo->image_url = "**************";
    einfo->director_name = "1111****";
    einfo->actors_name = "2222***";
    einfo->price_text = "333";

    strQuery =
            sqlite3_mprintf(
                    "INSERT INTO event_info VALUES ('%q', %d, %u, '%q', '%q', '%q', '%q', %d, %d, %d, %u, %u, %d, '%q', '%q', '%q', '%q', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);",
                    einfo->service_uid.c_str(), einfo->service_id, einfo->event_id, "99.99.99.99", einfo->event_name.c_str(),
                    einfo->event_short_description.c_str(), einfo->event_description.c_str(), einfo->running_status, einfo->free_CA_mode,
                    einfo->rating, einfo->start_time, einfo->end_time, einfo->duration, einfo->image_url.c_str(), einfo->director_name.c_str(),
                    einfo->actors_name.c_str(), einfo->price_text.c_str(), einfo->dolby, einfo->audio, einfo->resolution,
                    einfo->content_nibble_level_1, einfo->content_nibble_level_2, einfo->user_nibble_1, einfo->user_nibble_2, einfo->caption,
                    einfo->dvs, einfo->listProgramLinkedInfo.size());

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    strQuery = sqlite3_mprintf("DELETE FROM event_info WHERE service_uid = '%q';", info->service_uid.c_str());
    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);
    L_INFO(TAG, "Program Info DB OK\n");

    elementary_stream es_info;
    es_info.stream_pid = 0x1fff;
    es_info.stream_type = 0x02;
    es_info.language_code = 0xff;
    strQuery = sqlite3_mprintf("INSERT INTO elementary_stream_info VALUES ('%q', %d, %d, %u);", info->service_uid.c_str(), es_info.stream_pid,
            es_info.stream_type, es_info.language_code);

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    strQuery = sqlite3_mprintf("DELETE FROM elementary_stream_info WHERE service_uid = '%q';", info->service_uid.c_str());

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    L_INFO(TAG, "elementary stream Info DB OK\n");

    CAInfo caInfo;
    caInfo.CA_system_ID = 0xff;
    caInfo.CA_PID = 0x1fff;
    strQuery = sqlite3_mprintf("INSERT INTO ca_info VALUES ('%q', %d, %d);", info->service_uid.c_str(), caInfo.CA_system_ID, caInfo.CA_PID);

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    strQuery = sqlite3_mprintf("DELETE FROM ca_info WHERE service_uid = '%q';", info->service_uid.c_str());

    if(Execute(strQuery) < 0) {
        sqlite3_free(strQuery);
        return false;
    }

    sqlite3_free(strQuery);

    L_INFO(TAG, "ca Info DB OK\n");

    delete info;
    delete einfo;

    return true;
}

bool SqliteDB::verify()
{
    if(isError() || checkDBSchema() == false || updateDummyData() == false) {
        return false;
    }

    return true;
}
