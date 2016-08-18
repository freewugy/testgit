/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-21 17:32:52 +0900 (월, 21 4월 2014) $
 * $LastChangedRevision: 665 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef SERVICEINFO_H_
#define SERVICEINFO_H_

#include <string>
#include <list>
#include <map>
#include "Element.h"

using namespace std;

typedef enum {
    NOT_DEFINED = 0, QAM16, QAM32, QAM64, QAM128, QAM256,
} MODULATION_SCHEME;

/**
 * @brief
 */
typedef struct _CAInfo {
    /**
     * @brief CAS system ID
     */
    uint16_t CA_system_ID;

    /**
     * @brief ECM PID
     */
    uint16_t CA_PID;
} CAInfo;

/**
 * @brief
 */
typedef struct _Local_area_code_info {
    /**
     * @brief 1st local area code
     */
    uint8_t local_area_code_1;

    /**
     * @brief 2nd local area code
     */
    uint8_t local_area_code_2;

    /**
     * @brief 3rd local area code
     */
    uint8_t local_area_code_3;
} Local_area_code_info;

typedef struct _Product_info {
    uint8_t product_type;
    string id_product;
    string nm_product;
    string desc_product;
    uint32_t amt_price;
    uint32_t amt_dc_incr;
    uint8_t FG_term;
    uint8_t FG_value;

    uint32_t dt_product_start;
    uint32_t dt_product_end;

} ProductInfo;

class Element;
/**
 * @class ServiceInfo
 * @brief 채널 정보를 가지고 있는 class
 */
class ServiceInfo {
public:
    /**
     * @brief 생성자
     */
    ServiceInfo();

    ServiceInfo(const ServiceInfo& serviceInfo);

    /**
     * @brief 소멸자
     */
    virtual ~ServiceInfo();

//    ServiceInfo operator=(const ServiceInfo& serviceInfo);

    /**
     * @brief return String in XML
     * @return string XML String
     *
     */
    string toXMLString();

    /**
     * @brief channel unique id
     *        ex) 1.100.902 ( original_network_id.trasport_stream_id.service_id )
     */
    string service_uid;

    /**
     * @brief transport stream id
     */
    uint32_t transport_stream_id;

    /**
     * @brief original network id
     */
    uint32_t original_network_id;

    /**
     * @brief channel id
     */
    uint16_t service_id;

    /**
     * @brief channel type
     */
    uint8_t service_type;

    /**
     * @brief name of channel
     */
    string service_name;

    /**
     * @brief channel number
     */
    uint16_t channel_number;

    /**
     * @brief IP address
     *        default : "0.0.0.0"
     */
    uint32_t IP_address;

    /**
     * @brief Transport Stream port number
     *        default : 0
     */
    uint16_t TS_port_number;

    /**
     * @brief channel frequency ( unit : KHz )
     */
    uint32_t frequency;

    /**
     * @brief modulation ( 8vsb, 64qam, 256qam )
     */
    uint8_t modulation;

    /**
     * @brief tunerID ( tuner0, tuner1 )
     */
    uint8_t tunerID;

    /**
     * @brief symbol rate
     */
    uint32_t symbol_rate;

    /**
     * @brief FEC inner ( ex : 7/8 ... )
     */
    uint8_t FEC_inner;

    /**
     * @brief genre code
     */
    uint8_t genre_code;

    /**
     * @brief category code
     */
    uint8_t category_code;

    /**
     * @brief local area code ( Private Spec )
     *        for SKB only
     */
    uint8_t local_area_code;

    /**
     * @brief running status
     *        4 : available in IPTV
     */
    uint8_t running_status;

    /**
     * @brief
     */
    uint8_t free_CA_mode;

    /**
     * @brief channel rating
     */
    uint8_t channel_rating;

    /**
     * @brief pay channel
     */
    uint8_t pay_channel;

    /**
     * @brief preview time in pay channel
     */
    uint32_t pay_ch_sample_sec;

    /**
     * @brief resolution
     */
    uint8_t resolution;

    /**
     * @brief channel image url
     */
    string channel_image_url;

    /**
     * @brief PCR PID
     */
    uint32_t pcr_pid;

    uint16_t program_number;

    /**
     * @brief list of product id
     */
    list<string> list_product_id;

    /**
     * @brief list of Elementary Stream Information
     */
    list<elementary_stream> list_elementary_stream;

    /**
     * @brief list of CAS Information
     */
    list<CAInfo> listCAInfo;

    /**
     * @brief save Element
     * @param elm Element instance
     */
    void saveData(Element elm);
    void setTunerID(int tunerid);
    /**
     * @brief make string about this class
     * @return string
     */
    string toString();

    /**
     * @brief get value from list of Element
     */
    string getValue(string name);

    /**
     * @brief service information initialization ( make service unique ID )
     */
    void init();

    string getUri();

private:
    list<Element> m_listElm;

    bool bCompound;

    /**
     * @brief make Unique ID
     * @return void
     */
    void makeUid();

    string convertIPAddressFromUint32(uint32_t ip);
};

#endif
