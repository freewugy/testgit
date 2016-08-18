/*
 * NeoSIHandler.cpp
 *
 *  Created on: 2013. 7. 1.
 *      Author: owl
 */

#include "NeoSIHandler.h"
#include <map>
#include <list>
#include <sstream>
#include <sys/time.h>
#include "util/TimeConvertor.h"
#include "util/Logger.h"

static const char* TAG = "NeoSIHandler";

NeoSIHandler::NeoSIHandler()
{
    SIManager& siManager = SIManager::getInstance();
    siManager.addUpdateEventListener(this);
}

NeoSIHandler::~NeoSIHandler()
{
}

void NeoSIHandler::init(string provider)
{
    mProvider = provider;
}

void NeoSIHandler::start(int nProvider)
{
    string epguri;
    SIManager& siManager = SIManager::getInstance();

    siManager.initialize();

    switch (nProvider) {
        case 0: // SKB
            break;
        case 1: // linknet_iptv
        epguri = "i://233.14.173.241:5000?pn=lkn&db=f&wepg=http://139.228.191.244/EPG/channelmap.txt&xml=y&websi=http://139.228.191.244/EPG/channels.xml";

            break;
        case 2: // linknet_iptv
        epguri = "i://233.14.173.241:5000?pn=lkn&db=f&wepg=http://139.228.191.244/EPG/channelmap.txt&xml=n&websi=http://139.228.191.244/EPG/channels.xml";
            break;

        case 3: //lgu+
            break;
        case 4: // satellite
            break;
        default: // skb
            break;
    }

    L_DEBUG(TAG, "SI Start : URI[%s]\n", epguri.c_str());
    siManager.Start(epguri, 1);
}

void NeoSIHandler::stop()
{
    L_DEBUG(TAG, "===  Called ===\n");
    SIManager& siManager = SIManager::getInstance();
    siManager.Stop();
}

void NeoSIHandler::scanStart(int nProvider)
{
    L_DEBUG(TAG, "=== NOT SUPPORTED ===\n");
}

void NeoSIHandler::scanStop()
{
    L_DEBUG(TAG, "=== NOT SUPPORTED ===\n");
}

string NeoSIHandler::getTime()
{
    char timeBuf[256];
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    sprintf(timeBuf, "%02d:%02d:%02d:%03ld", tm->tm_hour, tm->tm_min, tm->tm_sec, (tv.tv_usec / 1000));

    return timeBuf;
}

void NeoSIHandler::receiveSIChangeEvent(SIChangeEvent* e)
{
    L_INFO(TAG, "%s\n", e->toString().c_str());
    switch (e->getEventType()) {
        case SIChangeEvent::CHANNEL_COMPLETED:
            break;
        case SIChangeEvent::CHANNEL_UPDATED:
            break;
        case SIChangeEvent::CHANNEL_ADD_EVENT:
            break;
        case SIChangeEvent::CHANNEL_MODIFY_EVENT:
            break;
        case SIChangeEvent::CHANNEL_REMOVE_EVENT:
            break;
        case SIChangeEvent::CHANNEL_MAP_COMPLETED: {
            mMapChannelNo.clear();
            makeMapChannelNo();
        }
            break;
        case SIChangeEvent::CHANNEL_MAP_READY:
            break;
        case SIChangeEvent::CHANNEL_PROGRAM_MAP_READY:
            break;
        case SIChangeEvent::PROGRAM_COMPLETED:
            break;
        case SIChangeEvent::PROGRAM_UPDATED:
            break;
        case SIChangeEvent::PROGRAM_ADD_EVENT:
            break;
        case SIChangeEvent::PROGRAM_MODIFY_EVENT:
            break;
        case SIChangeEvent::PROGRAM_REMOVE_EVENT:
            break;
        case SIChangeEvent::PROGRAM_MAP_COMPLETED: {
            break;
        }
        case SIChangeEvent::PROGRAM_PROCESSING:
            break;
        case SIChangeEvent::PROGRAM_MAP_READY:
            break;
        case SIChangeEvent::SCAN_READY:
            break;
        case SIChangeEvent::SCAN_PROCESSING:
            break;
        case SIChangeEvent::DATA_READING:
            break;
        case SIChangeEvent::DATA_EMPTY:
            break;
        case SIChangeEvent::ERROR_HAPPENED:
            break;

        case SIChangeEvent::SCAN_COMPLETED: {
            break;
        }
        case SIChangeEvent::SCAN_STOP: {
        }
            break;

        default:
            break;
    }
}

void NeoSIHandler::printchannellist(bool bProgram)
{
    int i = 0;
    SIManager& siManager = SIManager::getInstance();

    printf("DefaultChannelUid [%s]\n", siManager.getDefaultChannel().c_str());

    list<ServiceInfo *> listChannelInfo = siManager.getChannelList(0);
    list<ServiceInfo *>::iterator itr;

    list<EventInfo*> listEventInfo;
    for (itr = listChannelInfo.begin(); itr != listChannelInfo.end(); itr++) {
        ServiceInfo* pChannel = *itr;
        printf("[%03d] : ChannelID : %s:[%03u/%d],[%s],IP_addr : [%u:%d]\n", i++, pChannel->service_uid.c_str(), pChannel->channel_number,
                pChannel->service_id, pChannel->service_name.c_str(), pChannel->IP_address, pChannel->TS_port_number);
        printf("%s\n", pChannel->toString().c_str());
        printf("URI[%s]\n", pChannel->getUri().c_str());
        if(bProgram) {
            listEventInfo.clear();
            string channelUid = pChannel->service_uid;

            listEventInfo = siManager.getProgramList(channelUid);
            printProgramInfo(listEventInfo);
        }

        delete (*itr);
    }
}

void NeoSIHandler::printChannelInfo(list<ServiceInfo *> listChannel, bool bProgram)
{
    list<ServiceInfo *>::iterator itr;
    SIManager& siManager = SIManager::getInstance();
    printf("DefaultChannelUid [%s]\n", siManager.getDefaultChannel().c_str());
    int i = 0;

    list<EventInfo*> listEventInfo;
    for (itr = listChannel.begin(); itr != listChannel.end(); itr++) {
        ServiceInfo* pChannel = *itr;
        printf("[%03d] : ChannelID : %s:[%03u/%d],[%s],IP_addr : [%u:%d]\n", i++, pChannel->service_uid.c_str(), pChannel->channel_number,
                pChannel->service_id, pChannel->service_name.c_str(), pChannel->IP_address, pChannel->TS_port_number);
        if(bProgram) {
            listEventInfo.clear();
            string channelUid = pChannel->service_uid;

            listEventInfo = siManager.getProgramList(channelUid);
            printProgramInfo(listEventInfo);
        }

        delete (*itr);
    }
}

void NeoSIHandler::printProductInfo()
{
    SIManager& siManager = SIManager::getInstance();
    list < ProductInfo > listProductInfo = siManager.getProductInfoList();

    for (list<ProductInfo>::iterator itr = listProductInfo.begin(); itr != listProductInfo.end(); itr++) {
        ProductInfo productInfo = (ProductInfo)(*itr);

        printf("product_type[%u], product[id:%s, nm:%s, desc:%s],"
                " amt[price:%u, dc_incr:%u], FG[term:%u, value:%u], dt_product[%s~%s]\n", productInfo.product_type, productInfo.id_product.c_str(),
                productInfo.nm_product.c_str(), productInfo.desc_product.c_str(), productInfo.amt_price, productInfo.amt_dc_incr, productInfo.FG_term,
                productInfo.FG_value, TimeConvertor::getInstance().convertSeconds2Date(productInfo.dt_product_start).c_str(),
                TimeConvertor::getInstance().convertSeconds2Date(productInfo.dt_product_end).c_str());
    }
}

void NeoSIHandler::printProgramInfo(list<EventInfo *> listProgram)
{
    if(listProgram.size() == 0)
        return;

    list<EventInfo *>::iterator itr;
    int i = 0;
    for (itr = listProgram.begin(); itr != listProgram.end(); itr++) {
        EventInfo* pProgram = (EventInfo*) (*itr);
        printf("[%03d] : id[%u] time[%s ~ %s]\n", i++, pProgram->event_id,
                TimeConvertor::getInstance().convertSeconds2Date(pProgram->start_time).c_str(),
                TimeConvertor::getInstance().convertSeconds2Date(pProgram->end_time).c_str());
        printf("%s\n", pProgram->toString().c_str());
        // memory free
        delete (*itr);
    }
}

TDSResult NeoSIHandler::getChannelList(list<ServiceInfo *> &listServiceInfo)
{
    SIManager& siManager = SIManager::getInstance();
    listServiceInfo = siManager.getChannelList(0);

    if(listServiceInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getDefaultChannel(string *channelUid)
{
    SIManager& siManager = SIManager::getInstance();

    list<ServiceInfo *> listServiceInfo = siManager.getChannelList(0);
    if(listServiceInfo.size() == 0)
        return TDS_INFO_EMPTY;

    list<ServiceInfo *>::iterator itr = listServiceInfo.begin();
    ServiceInfo * serviceInfo = (ServiceInfo *) *itr;
    *channelUid = serviceInfo->service_uid;
    return TDS_OK;
}

TDSResult NeoSIHandler::getProgramList(string channelUid, list<EventInfo*> &listEventInfo)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramList(channelUid);

    if(listEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getMultiChannelProgramList(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramList(channelUid);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getMultiChannelProgramListByTime(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startTime,
        int endTime)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramListByTime(channelUid, startTime, endTime);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getMultiChannelProgramListByIndex(list<string> channelList, map<string, list<EventInfo*> > &mapEventInfo, int startIdx,
        int reqCount)
{
    SIManager& siManager = SIManager::getInstance();
    for (list<string>::iterator itr = channelList.begin(); itr != channelList.end(); itr++) {
        string channelUid = *itr;
        list<EventInfo *> listEventInfo = siManager.getProgramListByIndex(channelUid, startIdx, reqCount);

        mapEventInfo.insert(make_pair(channelUid, listEventInfo));
    }

    if(mapEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getProgramListByIndex(string channelUid, list<EventInfo*> &listEventInfo, int startIdx, int reqCount)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListByIndex(channelUid, startIdx, reqCount);

    if(listEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getProgramListByTime(string channelUid, list<EventInfo*> &listEventInfo, int startTime, int endTime)
{
    SIManager& siManager = SIManager::getInstance();
    listEventInfo = siManager.getProgramListByTime(channelUid, startTime, endTime);

    if(listEventInfo.size() > 0) {
        return TDS_OK;
    } else {
        return TDS_INFO_EMPTY;
    }
}

TDSResult NeoSIHandler::getCurrentProgramInfo(uint16_t channel_number, EventInfo** eventInfo)
{
    uint16_t service_id = 0;
    if(getServiceIDByChannelNumber(channel_number, &service_id) == TDS_OK) {
        SIManager& siManager = SIManager::getInstance();
        *eventInfo = siManager.getCurrentProgram(service_id);
        if((*eventInfo) != NULL) {
            printf("event : [%s]\n", (*eventInfo)->toString().c_str());
            return TDS_OK;
        } else {
            return TDS_INFO_EMPTY;
        }
    }
    return TDS_FAILURE;

}

TDSResult NeoSIHandler::getProgramInfo(uint16_t channel_number, int offset, EventInfo** eventInfo, int* pPrevPresent, int* pNextPresent)
{
    string service_uid;
    list<EventInfo*> listProgramInfo;
    if(getServiceUIDByChannelNumber(channel_number, &service_uid) == TDS_OK) {
        SIManager& siManager = SIManager::getInstance();
        listProgramInfo = siManager.getProgramList(service_uid);

        if(listProgramInfo.size() > 0) {
            uint32_t currentSec = TimeConvertor::getInstance().getCurrentSecond();
            list<EventInfo*>::iterator itr;
            int index = 0;
            for (itr = listProgramInfo.begin(); itr != listProgramInfo.end(); itr++) {
                EventInfo* info = (EventInfo *) (*itr);
                index++;
                if(info->start_time <= currentSec && info->end_time >= currentSec) {
                    break;
                }
            }

            index += offset;
            if(index > (int) listProgramInfo.size()) {
                L_ERROR(TAG, "Fail to get Program : offset[%d] channel number[%u],  current time is %u", offset, channel_number, currentSec);
                return TDS_INFO_EMPTY;
            }

            if(index < 0) {
                L_ERROR(TAG, "Fail to get Program : offset[%d] channel number[%u],  current time is %u", offset, channel_number, currentSec);
                return TDS_INFO_EMPTY;
            }

            int i = 0;
            for (itr = listProgramInfo.begin(); itr != listProgramInfo.end(); itr++) {
                i++;
                if(i == index) {
                    *eventInfo = (EventInfo *) (*itr);
                    break;
                }
            }

            if(index > 1) {
                *pPrevPresent = 1;
            } else {
                *pPrevPresent = 0;
            }

            if(index == (int) listProgramInfo.size()) {
                *pNextPresent = 0;
            } else {
                *pNextPresent = 1;
            }

            return TDS_OK;
        } else {
            return TDS_INFO_EMPTY;
        }
    }

    return TDS_FAILURE;
}

TDSResult NeoSIHandler::getServiceIDByChannelNumber(uint16_t channel_number, uint16_t *service_id)
{
    TDSResult result = TDS_INFO_EMPTY;

    SIManager& siManager = SIManager::getInstance();
    list<ServiceInfo*> listServiceInfo = siManager.getChannelList(0);

    if(listServiceInfo.size() <= 0) {
        return TDS_INFO_EMPTY;
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* sInfo = (ServiceInfo*) (*itr);

        if(sInfo->channel_number == channel_number) {
            *service_id = sInfo->service_id;
            result = TDS_OK;
            break;
        }
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        delete (*itr);
    }
    listServiceInfo.clear();

    return result;
}

TDSResult NeoSIHandler::getChannelNumberByServiceID(uint16_t service_id, uint16_t* channel_number)
{
    TDSResult result = TDS_INFO_EMPTY;

    SIManager& siManager = SIManager::getInstance();
    list<ServiceInfo*> listServiceInfo = siManager.getChannelList(0);

    if(listServiceInfo.size() <= 0) {
        return TDS_INFO_EMPTY;
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* sInfo = (ServiceInfo*) (*itr);

        if(sInfo->service_id == service_id) {
            *channel_number = sInfo->channel_number;
            result = TDS_OK;
            break;
        }
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        delete (*itr);
    }
    listServiceInfo.clear();

    return result;
}

void NeoSIHandler::setNetwork()
{
    SIManager& siManager = SIManager::getInstance();
    siManager.setNetworkConnectionInfo(true);
}

TDSResult NeoSIHandler::getServiceUIDByChannelNumber(uint16_t channel_number, string *service_uid)
{
    TDSResult result = TDS_INFO_EMPTY;

    if(mMapChannelNoServiceUID.size() <= 0) {
        return TDS_INFO_EMPTY;
    }

    map<uint16_t, string>::iterator itr = mMapChannelNoServiceUID.find(channel_number);
    if(itr != mMapChannelNoServiceUID.end()) {
        *service_uid = itr->second;
        result = TDS_OK;
    }

    return result;
}

void NeoSIHandler::makeMapChannelNo()
{
    SIManager& siManager = SIManager::getInstance();
    list<ServiceInfo*> listServiceInfo = siManager.getChannelList(0);

    if(listServiceInfo.size() <= 0) {
        return;
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        ServiceInfo* sInfo = (ServiceInfo*) (*itr);

        mMapChannelNo.insert(make_pair(sInfo->channel_number, sInfo->service_id));
        mMapChannelNoServiceUID.insert(make_pair(sInfo->channel_number, sInfo->service_uid));
    }

    for (list<ServiceInfo*>::iterator itr = listServiceInfo.begin(); itr != listServiceInfo.end(); itr++) {
        delete (*itr);
    }

    listServiceInfo.clear();
}

