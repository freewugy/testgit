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

#include <string.h>
#include <sstream>
#include "include/ServiceInfo.h"
#include "Logger.h"
#include "StreamTypeUtil.h"

static const char* TAG = "ServiceInfo";

ServiceInfo::ServiceInfo()
        : service_uid(""), service_id(0), service_type(0), service_name(""), channel_number(0), IP_address(0), TS_port_number(0), frequency(0), modulation(
                0), symbol_rate(0), FEC_inner(0), genre_code(0), category_code(0), local_area_code(0), running_status(0), free_CA_mode(0), channel_rating(
                0), pay_channel(0), pay_ch_sample_sec(0), resolution(0), channel_image_url(""), pcr_pid(0x1FFF), bCompound(false), transport_stream_id(
                0), original_network_id(0), tunerID(0)
{
    program_number = 0;
}

ServiceInfo::ServiceInfo(const ServiceInfo& sInfo)
{
    service_uid = sInfo.service_uid;
    service_id = sInfo.service_id;
    service_type = sInfo.service_type;
    service_name = sInfo.service_name;
    channel_number = sInfo.channel_number;
    IP_address = sInfo.IP_address;
    TS_port_number = sInfo.TS_port_number;
    frequency = sInfo.frequency;
    modulation = sInfo.modulation;
    tunerID = sInfo.tunerID;
    symbol_rate = sInfo.symbol_rate;
    FEC_inner = sInfo.FEC_inner;
    genre_code = sInfo.genre_code;
    category_code = sInfo.category_code;
    running_status = sInfo.running_status;
    free_CA_mode = sInfo.free_CA_mode;
    channel_rating = sInfo.channel_rating;
    local_area_code = sInfo.local_area_code;
    pay_channel = sInfo.pay_channel;
    pay_ch_sample_sec = sInfo.pay_ch_sample_sec;
    resolution = sInfo.resolution;
    channel_image_url = sInfo.channel_image_url;
    pcr_pid = sInfo.pcr_pid;
    transport_stream_id = sInfo.transport_stream_id;
    original_network_id = sInfo.original_network_id;
    bCompound = sInfo.bCompound;
    program_number = sInfo.program_number;

    if(sInfo.list_product_id.size() > 0) {
        for (list<string>::const_iterator p_itr = sInfo.list_product_id.begin(); p_itr != sInfo.list_product_id.end(); p_itr++) {
            list_product_id.push_back(*p_itr);
        }
    }
    if(sInfo.list_elementary_stream.size() > 0) {

        for (list<elementary_stream>::const_iterator e_itr = sInfo.list_elementary_stream.begin(); e_itr != sInfo.list_elementary_stream.end();
                e_itr++) {

            elementary_stream elstreamInfo = *e_itr;
            list_elementary_stream.push_back(elstreamInfo);
        }
    }
    if(sInfo.listCAInfo.size() > 0) {
        for (list<CAInfo>::const_iterator c_itr = sInfo.listCAInfo.begin(); c_itr != sInfo.listCAInfo.end(); c_itr++) {
            CAInfo caInfo = *c_itr;
            listCAInfo.push_back(caInfo);
        }
    }
}

ServiceInfo::~ServiceInfo()
{
    m_listElm.clear();
    list_elementary_stream.clear();
    list_product_id.clear();
    listCAInfo.clear();
}

void ServiceInfo::saveData(Element elm)
{
    string name = elm.name;
    string value = elm.value;

    if(name.compare("transport_stream_id") == 0) {
        transport_stream_id = (uint32_t) atoi(value.c_str()); // TUint32 transport_stream_id;
        return;
    } else if(name.compare("original_network_id") == 0) {
        original_network_id = (uint8_t) atoi(value.c_str()); // TUint32 original_network_id;
        return;
    } else if(name.compare("service_id") == 0) {
        service_id = (uint32_t) atoi(value.c_str()); // TUint32 service_id
        return;
    } else if(name.compare("service_type") == 0) {
        service_type = (uint8_t) atoi(value.c_str()); // TUint8 service_type;
        return;
    } else if(name.compare("service_name") == 0) {
        service_name = value;
        return;
    } else if(name.compare("channel_number") == 0) {
        channel_number = (uint16_t) atoi(value.c_str()); // TUint16 channel_number;
        return;
    } else if(name.compare("IP_address") == 0) {
        IP_address = (uint32_t) atoi(value.c_str());
        return;
    } else if(name.compare("TS_port_number") == 0) {
        TS_port_number = (uint16_t) atoi(value.c_str()); // TUint16 TS_port_number;
        return;
    } else if(name.compare("running_status") == 0) {
        running_status = (uint8_t) atoi(value.c_str()); // TUint8 running_status;
        return;
    } else if(name.compare("free_CA_mode") == 0) {
        free_CA_mode = (uint8_t) atoi(value.c_str()); // TUint8 free_CA_mode;
        return;
    } else if(name.compare("channel_rating") == 0) {
        channel_rating = (uint8_t) atoi(value.c_str()); // TUint8 channel_rating;
        return;
    } else if(name.compare("genre_code") == 0) {
        genre_code = (uint8_t) atoi(value.c_str()); // TUint8 channel_genre;
        return;
    } else if(name.compare("category_code") == 0) {
        category_code = (uint8_t) atoi(value.c_str()); // TUint8 channel_category;
        return;
    } else if(name.compare("local_area_code") == 0) {
        local_area_code = (uint8_t) atoi(value.c_str()); // TUint8 local_area_code;
        return;
    } else if(name.compare("resolution") == 0) {
        resolution = (uint8_t) atoi(value.c_str()); // TUint8 resolution;
        return;
    } else if(name.compare("channel_image_url") == 0) {
        channel_image_url = value; // string channel_image_url;
        return;
    } else if(name.compare("pcr_pid") == 0 || name.compare("PCR_PID") == 0) {
        pcr_pid = (uint32_t) atoi(value.c_str()); // TUint32 pcr_pid;
        return;
    } else if(name.compare("id_product") == 0) {
        list_product_id.push_back(value);
        return;
    }

    m_listElm.push_back(elm);
}

string ServiceInfo::toXMLString()
{
    char chr[128];
    ostringstream stringStream;

    list<Element>::iterator itr;
    sprintf(chr, "<ChannelInfo id=\"%s\" >\n", service_uid.c_str());
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"service_id\" value=\"%u\" />\n", service_id);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"service_type\" value=\"%u\" />\n", service_type);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"service_name\" value=\"%s\" />\n", service_name.c_str());
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"channel_number\" value=\"%u\" />\n", channel_number);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"IP_address\" value=\"%s\" />\n", convertIPAddressFromUint32(IP_address).c_str());
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"TS_port_number\" value=\"%u\" />\n", TS_port_number);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"frequency\" value=\"%u\" />\n", frequency);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"modulation\" value=\"%u\" />\n", modulation);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"symbol_rate\" value=\"%u\" />\n", symbol_rate);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"FEC_inner\" value=\"%u\" />\n", FEC_inner);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"genre_code\" value=\"%u\" />\n", genre_code);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"category_code\" value=\"%u\" />\n", category_code);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"running_status\" value=\"%u\" />\n", running_status);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"free_CA_mode\" value=\"%u\" />\n", free_CA_mode);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"channel_rating\" value=\"%u\" />\n", channel_rating);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"local_area_code\" value=\"%u\" />\n", local_area_code);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"pay_channel\" value=\"%u\" />\n", pay_channel);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"pay_ch_sample_sec\" value=\"%u\" />\n", pay_ch_sample_sec);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"resolution\" value=\"%u\" />\n", resolution);
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"channel_image_url\" value=\"%s\" />\n", channel_image_url.c_str());
    stringStream << chr;
    sprintf(chr, "\t<elm name=\"pcr_pid\" value=\"%u\" />\n", pcr_pid);
    stringStream << chr;

    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;

        sprintf(chr, "\t<elm name=\"%s\" value=\"%s\" />\n", data.name.c_str(), data.value.c_str());
        stringStream << chr;
    }

    if(list_product_id.size() > 0) {
        stringStream << "<\nentry name=\"id_product_list >\n";
        for (list<string>::iterator itr = list_product_id.begin(); itr != list_product_id.end(); itr++) {
            stringStream << "\t\t<elm name=\"id_product\" value=\"" << *itr << "\" />\n";
        }
        stringStream << "\t</ entry>\n";
    }

    if(list_elementary_stream.size() > 0) {

        for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
            elementary_stream esInfo = *itr;
            stringStream << "<\nentry name=\"esInfo\" >\n";
            stringStream << "\t\t<elm name=\"stream_type\" value=\"" << esInfo.stream_type << "\" />\n";
            stringStream << "\t\t<elm name=\"elementary_PID\" value=\"" << esInfo.stream_pid << "\" />\n";
            stringStream << "\t</ entry>\n";
        }
    }

    if(listCAInfo.size() > 0) {
        for (list<CAInfo>::iterator itr = listCAInfo.begin(); itr != listCAInfo.end(); itr++) {
            CAInfo caInfo = *itr;
            stringStream << "<\nentry name=\"ca_system_info\" >\n";
            stringStream << "\t\t<elm name=\"CA_system_ID\" value=\"" << caInfo.CA_system_ID << "\" />\n";
            stringStream << "\t\t<elm name=\"CA_PID\" value=\"" << caInfo.CA_PID << "\" />\n";
            stringStream << "\t</ entry>\n";
        }
    }

    stringStream << "</ChannelInfo>\n";
    return stringStream.str();
}

void ServiceInfo::makeUid()
{
    char chr[32];
    memset(chr, '\0', sizeof(chr));
    sprintf(chr, "%u.%u.%u", original_network_id, transport_stream_id, service_id);

    service_uid = string(chr);
}

void ServiceInfo::setTunerID(int tunerid)
{
	tunerID = tunerid;
}

string ServiceInfo::toString()
{
    char chr[1280];
    memset(chr, '\0', sizeof(chr));
    ostringstream stringStream;

    sprintf(chr, "%s>", service_uid.c_str());
    stringStream << chr;

    sprintf(chr, "service_id<%u>", service_id);
    stringStream << chr;
    sprintf(chr, "service_type<%u>", service_type);
    stringStream << chr;
    sprintf(chr, "service_name<%s>", service_name.c_str());
    stringStream << chr;
    sprintf(chr, "channel_number<%u>", channel_number);
    stringStream << chr;
    sprintf(chr, "IP_address<%s>", convertIPAddressFromUint32(IP_address).c_str());
    stringStream << chr;
    sprintf(chr, "TS_port_number<%u>", TS_port_number);
    stringStream << chr;
    sprintf(chr, "frequency<%u>", frequency);
    stringStream << chr;
    sprintf(chr, "modulation<%u>", modulation);
    stringStream << chr;
    sprintf(chr, "tunerID<%u>", tunerID);
	stringStream << chr;
    sprintf(chr, "symbol_rate<%u>", symbol_rate);
    stringStream << chr;
    sprintf(chr, "FEC_inner<%u>", FEC_inner);
    stringStream << chr;
    sprintf(chr, "genre_code<%u>", genre_code);
    stringStream << chr;
    sprintf(chr, "category_code<%u>", category_code);
    stringStream << chr;
    sprintf(chr, "running_status<%u>", running_status);
    stringStream << chr;
    sprintf(chr, "free_CA_mode<%u>", free_CA_mode);
    stringStream << chr;
    sprintf(chr, "channel_rating<%u>", channel_rating);
    stringStream << chr;
    sprintf(chr, "local_area_code<%u>", local_area_code);
    stringStream << chr;
    sprintf(chr, "pay_channel<%u>", pay_channel);
    stringStream << chr;
    sprintf(chr, "pay_ch_sample_sec<%u>", pay_ch_sample_sec);
    stringStream << chr;
    sprintf(chr, "resolution<%u>", resolution);
    stringStream << chr;
    sprintf(chr, "channel_image_url<%s>", channel_image_url.c_str());
    stringStream << chr;
    sprintf(chr, "pcr_pid<%u>", pcr_pid);
    stringStream << chr;

    list<Element>::iterator itr;
    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;
        sprintf(chr, "%s<%s>", data.name.c_str(), data.value.c_str());
        stringStream << chr;
    }

    stringStream << "\n" << "list_elementary_stream : " << list_elementary_stream.size() << ": ";
    if(list_elementary_stream.size() > 0) {
        for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
            elementary_stream esInfo = (elementary_stream) *itr;
            sprintf(chr, "stream_pid<%d>", esInfo.stream_pid);
            stringStream << chr;
            sprintf(chr, "stream_type<%d>", esInfo.stream_type);
            stringStream << chr;
            stringStream << " : ";
        }
    }

    stringStream << "\n" << "listCAInfo : " << listCAInfo.size() << ": ";
    if(listCAInfo.size() > 0) {
        for (list<CAInfo>::iterator itr = listCAInfo.begin(); itr != listCAInfo.end(); itr++) {
            CAInfo caInfo = (CAInfo) *itr;
            sprintf(chr, "CA_system_ID<%d>", caInfo.CA_system_ID);
            stringStream << chr;
            sprintf(chr, "CA_PID<%d>", caInfo.CA_PID);
            stringStream << chr;
            stringStream << " : ";
        }
    }

    stringStream << "\n" << "id_product : " << list_product_id.size() << ": ";
    if(list_product_id.size() > 0) {
        for (list<string>::iterator itr = list_product_id.begin(); itr != list_product_id.end(); itr++) {
            string id_product = (string)(*itr);
            stringStream << id_product.c_str() << ", ";
        }
    }

    return stringStream.str();
}

void ServiceInfo::init()
{
    if(bCompound == false) {
        makeUid();

        bCompound = true;
    }
}

string ServiceInfo::getValue(string name)
{
    list<Element>::iterator itr;
    for (itr = m_listElm.begin(); itr != m_listElm.end(); itr++) {
        Element data = *itr;

        if(data.name.compare(name) == 0)
            return data.value;
    }

    return "";
}

string ServiceInfo::convertIPAddressFromUint32(uint32_t ip)
{
    char ip_chr[32];
    string ip_str = "";
    sprintf(ip_chr, "%03u.%03u.%03u.%03u", ip >> 24 & 0xff, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);

    return ip_str.assign(ip_chr, 15);
}

#if 1

string ServiceInfo::getUri()
{
	ostringstream stringStream;
	char chr[128];
	char tmpEs[20];
	memset(chr, 0, 128);
	// mChannelUri
	// 1. Scheme+Host+ChNum
	switch (modulation) {
		case 0: // not defined
		sprintf(chr, "tuner://qam64:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		case 1:// qam16
		sprintf(chr, "tuner://qam16:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		case 2:// qam32
		sprintf(chr, "tuner://qam32:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		case 3:// qam64
		sprintf(chr, "tuner://qam64:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		case 4:// qam128
		sprintf(chr, "tuner://qam128:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		case 5:// qam256
		sprintf(chr, "tuner://qam256:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
		default:
		sprintf(chr, "tuner://qam64:%u?ch=%d&pn=%d",  frequency, channel_number,service_id);
		break;
	}
	stringStream << chr;

//	memset(chr, 0, 128);
//	if(program_number > 0) {
//		sprintf(chr, "&pn=%d", program_number);
//		stringStream << chr;
//	}

	memset(chr, 0, 128);
	// 2. CA_Info
	if(listCAInfo.empty()) {
		sprintf(chr, "&ci=0&cp=0");
		stringStream << chr;
	} else {
		for (list<CAInfo>::iterator itr = listCAInfo.begin(); itr != listCAInfo.end(); itr++) {
			CAInfo caInfo = *itr;
			sprintf(chr, "&ci=%d&cp=%d", caInfo.CA_system_ID, caInfo.CA_PID);
		}
		stringStream << chr;
	}

	// 3. ES_Info {PcrPid}
	memset(tmpEs, 0, 40);
	if (pcr_pid != 8191)
	{
		sprintf(tmpEs, "&pp=%d", pcr_pid);
		stringStream << tmpEs;
	}

	// 4. ES_Info {Video-Audio}
	if(list_elementary_stream.empty() == false) {
		int vs_count = 0;
		for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
			elementary_stream esInfo = *itr;
			if(&esInfo == NULL) {
				continue;
			}
			if(StreamTypeUtil::getInstance().isVideoStream(esInfo.stream_type)) {
				vs_count++;
				memset(tmpEs, 0, 40);
				sprintf(tmpEs, "&vp=%d&vc=%d", esInfo.stream_pid, esInfo.stream_type);
			}
		}
		if(vs_count > 0) {
			stringStream << tmpEs;
		}
		int as_count = 0;
		for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
			elementary_stream esInfo = *itr;
			if(&esInfo == NULL) {
				continue;
			}
			if(StreamTypeUtil::getInstance().isAudioStream(esInfo.stream_type)) {
				as_count++;
				memset(tmpEs, 0, 40);
				if(as_count > 1) { // second
					sprintf(tmpEs, "&ap%d=%d&ac%d=%d", as_count, esInfo.stream_pid, as_count, esInfo.stream_type);
					stringStream << tmpEs;
				} else if(as_count == 1)	{ // first
					sprintf(tmpEs, "&ap=%d&ac=%d", esInfo.stream_pid, esInfo.stream_type);
					stringStream << tmpEs;
				}
			}
			L_DEBUG(TAG,"as_count[%d],[%s]\n",as_count, stringStream.str().c_str());
		}
	}
	memset(tmpEs, 0, 40);
	sprintf(tmpEs, "&ct=%d&sn=%d", service_type,tunerID);
	stringStream << tmpEs;

//	L_DEBUG(TAG,"[%s]\n",stringStream.str().c_str());
	return stringStream.str();
}

#else

string ServiceInfo::getUri()
{
    ostringstream stringStream;
    char chr[128];
    char tmpEs[20];

    memset(chr, 0, 128);

    // mChannelUri
    // 1. Scheme+Host+ChNum
    sprintf(chr, "tuner://%d.%d.%d.%d:%d?ch=%d", (IP_address & 0xFF000000) >> 24, (IP_address & 0x00FF0000) >> 16, (IP_address & 0x0000FF00) >> 8,
            (IP_address & 0x000000FF), TS_port_number, channel_number);

    stringStream << chr;

    memset(chr, 0, 128);

    if(program_number > 0) {
        sprintf(chr, "&pn=%d", program_number);
        stringStream << chr;
    }

    memset(chr, 0, 128);

    // 2. CA_Info
    if(listCAInfo.empty()) {
        sprintf(chr, "&ci=0&cp=0");
        stringStream << chr;
    } else {
        for (list<CAInfo>::iterator itr = listCAInfo.begin(); itr != listCAInfo.end(); itr++) {
            CAInfo caInfo = *itr;
            sprintf(chr, "&ci=%d&cp=%d", caInfo.CA_system_ID, caInfo.CA_PID);
        }
        stringStream << chr;
    }

    // 3. ES_Info {PcrPid}
    memset(tmpEs, 0, 20);
    sprintf(tmpEs, "&pp=%d", pcr_pid);
    stringStream << tmpEs;

    // 4. ES_Info {Video-Audio}
    if(list_elementary_stream.empty() == false) {
        int vs_count = 0;
        for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
            elementary_stream esInfo = *itr;
            if(&esInfo == NULL) {
                continue;
            }

            if(StreamTypeUtil::getInstance().isVideoStream(esInfo.stream_type)) {
                vs_count++;
                memset(tmpEs, 0, 20);
                sprintf(tmpEs, "&vp=%d&vc=%d", esInfo.stream_pid, esInfo.stream_type);
            }
        }
        if(vs_count > 0) {
            stringStream << tmpEs;
        }
        int as_count = 0;
        for (list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
            elementary_stream esInfo = *itr;

            if(&esInfo == NULL) {
                continue;
            }
            if(StreamTypeUtil::getInstance().isAudioStream(esInfo.stream_type)) {
                as_count++;
                memset(tmpEs, 0, 20);
                if(as_count > 1) {
                    sprintf(tmpEs, "&ap%d=%d&ac%d=%d", as_count, esInfo.stream_pid, as_count, esInfo.stream_type);
#if 0
                    stringStream << tmpEs;
#endif
                } else {
                    sprintf(tmpEs, "&ap=%d&ac=%d", esInfo.stream_pid, esInfo.stream_type);
                    stringStream << tmpEs;
                }

            }
        }

        sprintf(tmpEs, "&ct=%d", service_type);
        stringStream << tmpEs;
    }

    L_DEBUG("ServiceInfo", "[%s]\n", stringStream.str().c_str());
    return stringStream.str();
}
#endif
