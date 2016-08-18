/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-06-05 19:40:19 +0900 (목, 05 6월 2014) $
 * $LastChangedRevision: 855 $
 * Description:
 * Note:
 *****************************************************************************/

#include "Parser.h"
#include "BitStream.h"
#include "StringUtil.h"
#include "DescriptorFactory.h"
#include "Logger.h"
#include "dvb/SDTParser.h"
#include "dvb/EITParser.h"
#include "dvb/NITParser.h"
#include "dvb/TDTParser.h"
#include "mpeg/PMTInfo.h"
#include "mpeg/PMTParser.h"
#include "database/SIDataManager.h"
#include "Timer.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <cerrno>
#include <algorithm>
#include <string>

SIDataManager& dataManager = SIDataManager::getInstance();
static const char* TAG = "Parser";

list<PMTData> gQueuePMTData;

#ifdef __FOR_FUNCTION_TEST__
#define PERIOD_WEB_EPG_UPDATE   1800000 /* 30 min */
#else
//#define PERIOD_WEB_EPG_UPDATE   3600000 /* 1 hour */
#define PERIOD_WEB_EPG_UPDATE	1000 * 60 * 60    /* 1 hour */

//#define PERIOD_WEB_EPG_UPDATE   1800000 /* 30 min */
//#define PERIOD_WEB_EPG_UPDATE   600000 /* 10 min */
#endif
//#define CALCULATOR_ONE_MINUTE   60000 /* 1 min */
#define CALCULATOR_ONE_MINUTE   1000 * 60 * 1 /* 1 min */
void checkWEPGUpdate(int id);
void timerSet(bool timerflag);

WepgDataHandler* pWepgDataHandler;
static bool bWepgDataHandlerInit = false;
static bool bWepgDataCompleted = false;
static bool bTimerStart = false;
static int period = 0;

void WepgDataHandlerCallbackFn(int id, string channel_uid, vector<EventInfo *> listProgramInfo)
{
    SIChangeEvent event;

    if(id > 0) {
        if(pWepgDataHandler->isRunning()) {
            event.setEventType(SIChangeEvent::PROGRAM_COMPLETED);
            dataManager.saveProgramInfo(channel_uid, listProgramInfo, event, false, true);
        }
        else {
			vector<EventInfo*>::iterator itr;
			for (itr = listProgramInfo.begin(); itr != listProgramInfo.end(); itr++) {
				if((*itr) != NULL)
					delete (*itr);
			}
			listProgramInfo.clear();
        }

    } else if(id == WEB_EPG_GET_CHANNEL_MAP_FAILURE) {
        event.setEventType(SIChangeEvent::ERROR_WEPG_CONNECTION);
        dataManager.notifySIEvent(event);
        timerSet(false);
        // fail : Wait for IPConnected
    } else if(id == WEB_EPG_COMPLETED) { // Completed
        L_DEBUG(TAG, "WEB EPG Completed\n");
        event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
        dataManager.saveProgramInfo(channel_uid, listProgramInfo, event, false, true);
        timerSet(true);

    } else if(id == WEB_EPG_UPDATED) { // Completed
        L_DEBUG(TAG, "WEB EPG Updated\n");
        event.setEventType(SIChangeEvent::PROGRAM_MAP_COMPLETED);
        dataManager.saveProgramInfo(channel_uid, listProgramInfo, event, false, true);
        timerSet(true);

    } else if(id == WEB_EPG_NOTCHANGE){
    	L_DEBUG(TAG, "WEB EPG Updated - NOT CHANGE (VERSION SAME) \n");
    	timerSet(true);

    } else {
       if(listProgramInfo.size() > 0) {
           //L_DEBUG(TAG, ">> tracking for memory leak(2)");
           vector<EventInfo*>::iterator itr;
           for (itr = listProgramInfo.begin(); itr != listProgramInfo.end(); itr++) {
               if((*itr) != NULL)
                   delete (*itr);
           }
       }
       listProgramInfo.clear();
   }

}

void timerSet(bool timerflag){

	if (timerflag == true && period == PERIOD_WEB_EPG_UPDATE )
		return;
	else if(timerflag == false && period == CALCULATOR_ONE_MINUTE )
		return;

	if (period != 0) {
		if (delete_timer(ID_TIMER_CHECK_WEPG_UPDATE) < 0) {
			L_DEBUG(TAG, "### Not exist timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
		} else {
			L_DEBUG(TAG, "### success delete timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
			period = 0;
		}
	}

	if (timerflag == true){
		period = PERIOD_WEB_EPG_UPDATE; // 60 min
	}else{
		period = CALCULATOR_ONE_MINUTE; // 1 min
	}

	sleep(2);
	if (set_timer(ID_TIMER_CHECK_WEPG_UPDATE, period, (TimerHandler *) checkWEPGUpdate) < 0)
		L_ERROR(TAG, "### failed!! timer [%d] ###\n\n",	ID_TIMER_CHECK_WEPG_UPDATE);
	else
		L_DEBUG(TAG, "### success set timer [%d] ###\n\n",	ID_TIMER_CHECK_WEPG_UPDATE);

	L_INFO(TAG, "=== WEB EPG Update Timer START : Period [%d] seconds ===\n",period / 1000);
	bTimerStart = true;
	bWepgDataCompleted = true;
}

void checkWEPGUpdate(int id)
{
    L_INFO(TAG, "=== [%d]Called ===\n", id);
#ifdef __FOR_FUNCTION_TEST__
    pWepgDataHandler->start();
#else
    pWepgDataHandler->update();
#endif
    return;
}

Parser::~Parser()
{
    L_INFO(TAG, "Called\n");
}

bool Parser::readSchemaXML(string xmlFileName)
{
    string strXML = "";
    try {
        strXML = get_file_contents(xmlFileName.c_str());
    } catch (int err) {
        L_ERROR(TAG, "=== OOPS : %s : errno : %d\n", xmlFileName.c_str(), err);
        return false;
    }

    TiXmlDocument doc;
    doc.Parse(strXML.c_str());
    TiXmlNode* root = doc.FirstChild("SITypes");

    for (TiXmlNode* node = root->FirstChild("SIType"); node; node = node->NextSibling("SIType")) {
        list < string > listFile;
        list < string > listTable;

        const char* siType = node->ToElement()->Attribute("name");

        if(mSIType.compare(siType) == 0) {
            for (TiXmlNode* n = node->FirstChild("file"); n; n = n->NextSibling("file")) {
                const char* strName = n->ToElement()->Attribute("name");

                listFile.push_back(strName);
            }

            for (TiXmlNode* n = node->FirstChild("mso"); n; n = n->NextSibling("mso")) {
                string strName(n->ToElement()->Attribute("name"));

                if(strName.compare(mMSOType) == 0) {
                    for (TiXmlNode* child = n->FirstChild("setting"); child; child = child->NextSibling("setting")) {
                        mBXmlUse = (1 == atoi(child->ToElement()->Attribute("xml_use")));
                        mUTCOffSet = atoi(child->ToElement()->Attribute("utc_offset"));

                        if(mBXmlUse == false)
                            listFile.clear();
                    }

                    for (TiXmlNode* child = n->FirstChild("file"); child; child = child->NextSibling("file")) {
                        const char* strName = child->ToElement()->Attribute("name");

                        listFile.push_back(strName);
                    }

                    for (TiXmlNode* child = n->FirstChild("Table"); child; child = child->NextSibling("Table")) {
                        const char* strName = child->ToElement()->Attribute("name");

                        listTable.push_back(strName);
                    }
                }
            }

            DescriptorFactory::getInstance().setXMLFile(listFile);
        }
    }

    return true;
}

void Parser::init(string siType, string msoType)
{
    mSIType = siType;
    mMSOType = msoType;

    std::transform(mMSOType.begin(), mMSOType.end(), mMSOType.begin(), ::tolower);
    std::transform(mSIType.begin(), mSIType.end(), mSIType.begin(), ::tolower);

    string xmlFileName = "";
#ifdef ANDROID
    xmlFileName = "/data/lkn_home/config/si/schema/parser.xml";
#else
    xmlFileName = "schema/parser.xml";
#endif

    if(readSchemaXML(xmlFileName)) {
        bInit = true;
        L_INFO(TAG, "=== Parser INIT : %s : %s ===\n", mSIType.c_str(), mMSOType.c_str());
    } else {
        L_ERROR(TAG, " === OOPS : Parser INIT [%s] FAILED ===\n\n", xmlFileName.c_str());
    }

    if(bWepgDataHandlerInit == false && mChannelMapUrl.length() > 10) {
        L_INFO(TAG, "=== WEB EPG INITIALIZE SUCCESS : %s ===\n", mChannelMapUrl.c_str());

		if (init_timer() < 0) {
			// error init
			L_ERROR(TAG, "## Error!!! init\n");
		}
        pWepgDataHandler = new WepgDataHandler(mChannelMapUrl, (WepgDataHandlerCallback *) WepgDataHandlerCallbackFn);

        bWepgDataHandlerInit = true;

    }

    TDTParser::getInstance().setUTCOffset(mUTCOffSet);
}

void Parser::setWepgChannelMapUrl(string channelMapUrl)
{
    if(channelMapUrl.length() < 10) {
        L_WARN(TAG, "ChannelMap Url is INVALID === [%s] ===\n", channelMapUrl.c_str());
        return;
    }
    mChannelMapUrl = channelMapUrl;

    if(bWepgDataHandlerInit == false) {
        L_INFO(TAG, "=== WEB EPG INITIALIZE SUCCESS : %s ===\n", mChannelMapUrl.c_str());

		if (init_timer() < 0) {
			// error init
			L_ERROR(TAG, "## Error!!! init\n");
		}
        pWepgDataHandler = new WepgDataHandler(mChannelMapUrl, (WepgDataHandlerCallback *) WepgDataHandlerCallbackFn);

        bWepgDataHandlerInit = true;


    } else if(bWepgDataHandlerInit == true) {
        L_INFO(TAG, "=== WEB EPG Handler INITIALIZE ===\n");
        pWepgDataHandler->stop(); // scan start tl epg stop
        if(bTimerStart) {
            if(delete_timer(ID_TIMER_CHECK_WEPG_UPDATE) < 0)
                L_ERROR(TAG, "### failed!! timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
            else{
    			L_DEBUG(TAG, "### success delete timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);

    		}
            period = 0;
            bTimerStart = false;

            bWepgDataCompleted = false;
        }
        bWepgDataHandlerInit = false;
    }
}

/*void Parser::setNetworkConnectionInfo(bool isConnected)
{
    bIPConnected = isConnected;
    if(bIPConnected && bWepgDataCompleted == false) {
        if(bWepgDataHandlerInit && mListNITInfo.size() > 0) {
            pWepgDataHandler->stop();
            pWepgDataHandler->setListNITInfo(mListNITInfo);
            pWepgDataHandler->start();
        } else {
            L_INFO(TAG, "=== NOT READY TO START : After Channel Map completed, will be started ===\n");
        }
    } else {
        L_INFO(TAG, "=== DISCARD : Because Network connection is false, OR Already Web EPG Completed ===\n");
    }
}*/

void Parser::setNetworkConnectionInfo(bool isConnected)
{
    bIPConnected = isConnected;
    if(bIPConnected && bWepgDataCompleted == false) {
        if(bWepgDataHandlerInit && mListNITInfo.size() > 0) {
            L_INFO(TAG, "=== WEB EPG Handler STOP : NET CONN ===\n");
            pWepgDataHandler->stop();
            bWepgDataCompleted = false;

            L_INFO(TAG, "=== WEB EPG Handler START : NET CONN ===\n");
            pWepgDataHandler->setListNITInfo(mListNITInfo);
            pWepgDataHandler->start();
        } else {
            L_INFO(TAG, "=== NOT READY TO START : After Channel Map completed, will be started ===\n");
        }
    } else {
    	pWepgDataHandler->stop();
    	timerSet(false);
    	bWepgDataCompleted = false;
        L_INFO(TAG, "=== DISCARD : Because Network connection is false, OR Already Web EPG Completed ===\n");
    }
}

void Parser::process(string tableName, void* sectionData, SIChangeEvent event)
{
    if((tableName.compare("SCAN_TEST") == 0)) {
		if (event.getEventType() == SIChangeEvent::SCAN_COMPLETED 	|| event.getEventType() == SIChangeEvent::SCAN_STOP) {
			if (event.getFoundChannel() == -1 && bWepgDataHandlerInit) {
				pWepgDataHandler->stop();
				if (bTimerStart) {
					if (delete_timer(ID_TIMER_CHECK_WEPG_UPDATE) < 0)L_ERROR(TAG, "### failed!! timer [%d] ###\n\n",ID_TIMER_CHECK_WEPG_UPDATE);
					else {
						L_DEBUG(TAG, "### success delete timer [%d]  ###\n\n",	ID_TIMER_CHECK_WEPG_UPDATE);
					}
					period = 0;
					bTimerStart = false;

					bWepgDataCompleted = false;
				}
			}
			if (bWepgDataHandlerInit == false) {
				pWepgDataHandler->setListNITInfo(mListNITInfo);
				pWepgDataHandler->start();
			}
			bWepgDataHandlerInit = true;
			SIDataManager& sidm = SIDataManager::getInstance();
			sidm.setScanMode(0);
		}

        dataManager.notifyScanProcessTest(event);
        return;
    } else if((tableName.compare("SCAN") == 0)) {
        if(event.getEventType() == SIChangeEvent::SCAN_COMPLETED || event.getEventType() == SIChangeEvent::SCAN_STOP) {
            if(event.getFoundChannel() == -1 && bWepgDataHandlerInit) {
                pWepgDataHandler->stop();
                if(bTimerStart) {
                    if(delete_timer(ID_TIMER_CHECK_WEPG_UPDATE) < 0)
                        L_ERROR(TAG, "### failed!! timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
                    else{

            			L_DEBUG(TAG, "### success delete timer [%d]  ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);

            		}
                    period = 0;
                    bTimerStart = false;

                    bWepgDataCompleted = false;
                }
            }
			if (bWepgDataHandlerInit == false) {
				pWepgDataHandler->setListNITInfo(mListNITInfo);
				pWepgDataHandler->start();
			}
			bWepgDataHandlerInit = true;
			SIDataManager& sidm = SIDataManager::getInstance();
			sidm.setScanMode(0);
        }

        dataManager.notifySIEvent(event);
        return;
    } else if((tableName.compare("TIMEOUT_READY") == 0)) {
        dataManager.notifySIEvent(event);
        SIChangeEvent event_ch_complte;
        event_ch_complte.setEventType(SIChangeEvent::CHANNEL_MAP_COMPLETED);
        event_ch_complte.setChannelVersion(0);
        dataManager.notifySIEvent(event_ch_complte);
        return;
    }  else if((tableName.compare("NIT_CHMAP_READY") == 0)) {
        dataManager.notifySIEvent(event);
        return;
    }  else if ((tableName.compare("CHMAP_COMPLETE") == 0)) {
    	dataManager.notifySIEvent(event);
    	return;
    }


    if(!sectionData) {
        L_ERROR(TAG, "=== OOPS : Table[%s] sectionData is EMPTY ===\n", tableName.c_str());
        return;
    }

    if(tableName.compare("SDT") == 0) {
        SDTParser sdtParser;
        sdtParser.setXMLUse(mBXmlUse);

        sdtParser.process(sectionData);
        list<ServiceInfo *> listServiceInfo = sdtParser.getServiceList();
        uint8_t version = sdtParser.getVersion();

        // eschoi_20140419 | couldn't compare to version for several sdt
        L_INFO(TAG, "SDT : Version[0x%02x] Channel List size[%u]\n", version, listServiceInfo.size());
        mVersionSDT = version;
        if(listServiceInfo.size() > 0) {
            list<ServiceInfo *> listNITServiceInfo;
            for (list<ServiceInfo *>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
                ServiceInfo* serviceInfo = *itr;
                if(updateServiceInfoCable(serviceInfo))
                    listNITServiceInfo.push_back(serviceInfo);
            }
            if(!listNITServiceInfo.empty()) {
            	dataManager.saveChannelInfo(listNITServiceInfo, event,mVersionSDT, false);
            }
            L_DEBUG(TAG,"SDT update listNITServiceInfo size[%d]\n",listNITServiceInfo.size());
        }
        else {
			L_DEBUG(TAG,"[DEBUG]no update data!!!\n");
        	//return;
        }
        if(event.getEventType() == SIChangeEvent::CHANNEL_MAP_COMPLETED) {
        	event.setChannelVersion(mVersionSDT);
            dataManager.setListNITInfo(mListNITInfo, event);
        }
    } else if(tableName.compare("NIT") == 0) {
        if(mListNITInfo.size() > 0) {
            for (list<NITInfo *>::iterator itr = this->mListNITInfo.begin(); itr != this->mListNITInfo.end(); itr++) {
                delete *itr;
            }
            mListNITInfo.clear();
        }

        NITParser nitParser;
        nitParser.setUTCOffset(mUTCOffSet);
        nitParser.process(sectionData);
        list<NITInfo*> listNITInfo = nitParser.getNITList();
        uint8_t version = nitParser.getVersion();

        for (list<NITInfo*>::iterator itr = listNITInfo.begin(); itr != listNITInfo.end(); itr++) {
            mListNITInfo.push_back(*itr);
        }
        L_INFO(TAG, "NIT : Changed Version[0x%02x] --> [0x%02x] Channel List size[%u]\n", mVersionNIT, version, mListNITInfo.size());
        mVersionNIT = version;
		string defaultChannelUid = nitParser.getDefaultChannelUid();
		L_INFO(TAG,"defaultChannelUid [%s]\n",defaultChannelUid.c_str());
        dataManager.setDefaultChannelUid(defaultChannelUid);
        dataManager.notifySIEvent(event);
        // loneowl_20140530 | use macro. why? skb buile error
        if(bWepgDataHandlerInit) {
            pWepgDataHandler->stop();
            if(bTimerStart) {
                if(delete_timer(ID_TIMER_CHECK_WEPG_UPDATE) < 0)
                L_ERROR(TAG, "### failed!! timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
                else{
        			L_DEBUG(TAG, "### success delete timer [%d] ###\n\n", ID_TIMER_CHECK_WEPG_UPDATE);
                }
                period = 0;
                bTimerStart = false;

                bWepgDataCompleted = false;
            }
            pWepgDataHandler->setListNITInfo(mListNITInfo);
            pWepgDataHandler->start();
        }
    } else if(tableName.compare("PMT") == 0) {
    	L_DEBUG(TAG, " \n");
        PMTParser pmtParser;
        pmtParser.process(sectionData);
        list<PMTInfo*> listPMTInfo = pmtParser.getPMTList();

        if(listPMTInfo.size() > 0) {
            dataManager.savePMTInfo(listPMTInfo, event);
        }
    } else if(tableName.compare("TDT") == 0) {
        TDTParser::getInstance().process(sectionData);

        uint32_t tdt_date = TDTParser::getInstance().getCurrentSeconds();

        SIChangeEvent tdt_event;
        tdt_event.setEventType(SIChangeEvent::TDT_DATE);
        tdt_event.setTDTDate(tdt_date);

        dataManager.notifySIEvent(tdt_event);
    } else {
        L_ERROR(TAG, "=== %s : CANNOT SUPPORT === \n", tableName.c_str());
    }
}

void Parser::receivedTable(string tableName, void* sectionData, SIChangeEvent event)
{
    if(bInit == false) {
        L_ERROR(TAG, "\n\n === OOPS : Parser NOT INIT === \n");
        L_INFO(TAG, "called Parser::init(string siType, string msoType)\n");
        return;
    }

    if(mMSOType.compare("lkn") == 0) {
        process(tableName, sectionData, event);
    } else {
        L_ERROR(TAG, "=== %s : CANNOT SUPPORT === \n", mSIType.c_str());
    }
}

bool Parser::updateServiceInfoCable(ServiceInfo* serviceInfo)
{
    bool isInNIT = false;
//    L_DEBUG(TAG,"nitInfo size [%d] \n",mListNITInfo.size());

    for (list<NITInfo*>::iterator itr = mListNITInfo.begin(); itr != mListNITInfo.end(); itr++) {
        NITInfo* nitInfo = (NITInfo *) (*itr);
//        L_DEBUG(TAG,"nitInfo->service_id [%d] serviceInfo->service_id[%d]\n",nitInfo->service_id, serviceInfo->service_id);
        if(nitInfo->service_id == serviceInfo->service_id) {
            serviceInfo->frequency = nitInfo->mTunerDeliveryParam.cable.frequency;
            serviceInfo->modulation = nitInfo->mTunerDeliveryParam.cable.modulation;
            serviceInfo->symbol_rate = nitInfo->mTunerDeliveryParam.cable.symbolRate;
            serviceInfo->FEC_inner = nitInfo->mTunerDeliveryParam.cable.fecInner;
            if(nitInfo->channel_number > 0)
                serviceInfo->channel_number = nitInfo->channel_number;

            if(serviceInfo->channel_number == 0)
                serviceInfo->channel_number = serviceInfo->service_id;

            return true;
        }
    }

    return isInNIT;
}
