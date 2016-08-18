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

#if !defined(EA_6B2E4923_20BE_424c_A05C_7919499082B7__INCLUDED_)
#define EA_6B2E4923_20BE_424c_A05C_7919499082B7__INCLUDED_

#include <string>
#include <set>
#include <vector>

#include "SIChangeEvent.h"
#include "XML/XMLParser.h"
#include "Table.h"
#include "ServiceInfo.h"
#include "EventInfo.h"
#include "dvb/NITInfo.h"
#include "WEPG/WepgDataHandler.h"
using namespace std;

class NITInfo;
class ServiceInfo;
class EventInfo;

class WepgDataHandler;

typedef struct PMTData_t {
    uint8_t *sectionData;
    uint32_t length;
} PMTData;

/**
* @class Parser
* @brief table filter를 생성하고 수신한 BitStream을 parsing 하는 역할
*/

class Parser
{

public:
	///소멸자
	virtual ~Parser();

	/**
	 * @brief instance를 생성하여서 반환
	 */
	static Parser& getInstance() {
		static Parser instance;

		return instance;
	}

	/**
	 * @brief siType, msoType에 따라서 parser 초기화 수행
	 * @param siType siType ( DVB, PSIP, etc )
	 * @param msoType string ( SKB, LGU, ... )
	 */
	void init(string siType, string msoType);

	/**
	 * @brief ScanManager로부터 section data를 받는다.
	 * @param tableName
	 * @param section
	 * @param event
	 */
	void receivedTable(string tableName, void* sectionData, SIChangeEvent event);

    /**
     * @brief PSIMonitor로부터 section data를 받는다.
     * @param tableName
     * @param section
     * @param event
     */
    void receivedPMTData(const unsigned char *sectionData, const unsigned int length);
    void processPMT(PMTData data);

	void setWepgChannelMapUrl(string channelMapUrl);

	void setNetworkConnectionInfo(bool isConnected=true);

    static pthread_mutex_t m_mutex;

private:
	Parser() {
		bInit = false;
		mBXmlUse = false;
		mUTCOffSet = 0;
		mReceivedEITSch = 0;
		mReceivedEITPf = 0;
		mVersionNIT = 0xff;
		mVersionSDT = 0xff;
		mChannelMapUrl = "";
		bIPConnected = true;
	};                   // Constructor? (the {} brackets) are needed here.
	// Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
	Parser(Parser const&);            // Don't Implement
    void operator=(Parser const&); 				// Don't implement

	list<NITInfo*> mListNITInfo;

	string mChannelMapUrl;
	bool bIPConnected;

	bool bInit;
	bool mBXmlUse;
	uint8_t mUTCOffSet;

	/**
	 * @brief SIType Name \n
	 * For instance, DVB, PSIP, etc
	 */
	string mSIType;
	string mMSOType;
	map<uint32_t, uint32_t> mMapEITSch;
	map<uint32_t, uint32_t> mMapEITPf;
	uint8_t mReceivedEITSch;
	uint8_t mReceivedEITPf;

	uint8_t mVersionNIT;
	uint8_t mVersionSDT;
	bool readSchemaXML(string xmlFileName);

	bool updateServiceInfoCable(ServiceInfo* serviceInfo);
	bool updateServiceInfoTerrestrial(ServiceInfo* serviceInfo);
	bool updateServiceInfoSatellite(ServiceInfo* serviceInfo);
	bool updateServiceInfoS2Satellite(ServiceInfo* serviceInfo);
	void process(string tableName, void* sectionData, SIChangeEvent event);

	static void* parserDataThread(void* param);

    static void ParserLock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    static void ParserUnlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

};

#endif // !defined(EA_6B2E4923_20BE_424c_A05C_7919499082B7__INCLUDED_)
