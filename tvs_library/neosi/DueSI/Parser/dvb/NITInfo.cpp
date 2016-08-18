/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-05-30 18:31:16 +0900 (금, 30 5월 2014) $
 * $LastChangedRevision: 821 $
 * Description:
 * Note:
 *****************************************************************************/

#include <sstream>
#include <stdio.h>
#include "NITInfo.h"
#include <stdlib.h>

NITInfo::NITInfo() : network_id(0), transport_stream_id(0), network_name(""), service_id(0), linkage_type(0), service_type(0), channel_number(0)
{
    original_network_id = 0;
}

NITInfo::~NITInfo() {
	// TODO Auto-generated destructor stub
	network_name = ""; network_name.clear();
}

string NITInfo::toXMLString() {
	char chr[256];
	ostringstream stringStream;

	stringStream << "<NITInfo ";
	sprintf(chr, "network_id=\"%u\" ", network_id); stringStream << chr;
	sprintf(chr, "transport_stream_id=\"%u\" ", transport_stream_id); stringStream << chr;
	sprintf(chr, "network_name=\"%s\" ", network_name.c_str()); stringStream << chr;
	sprintf(chr, "original_network_id=\"%u\" ", original_network_id); stringStream << chr;
	sprintf(chr, "service_id=\"%u\" ", service_id); stringStream << chr;
	sprintf(chr, "linkage_type=\"%u\" ", linkage_type); stringStream << chr;
	sprintf(chr, "service_type=\"%u\" />", service_type); stringStream << chr;
	return stringStream.str();
}

string NITInfo::toString() {
	char chr[256];
	ostringstream stringStream;

	sprintf(chr, "network_id<%u>", network_id); stringStream << chr;
	sprintf(chr, "transport_stream_id<%u>", transport_stream_id); stringStream << chr;
	sprintf(chr, "network_name<%s>", network_name.c_str()); stringStream << chr;
	sprintf(chr, "original_network_id<%u>", original_network_id); stringStream << chr;
	sprintf(chr, "service_id<%u>", service_id); stringStream << chr;
	sprintf(chr, "linkage_type<%u>", linkage_type); stringStream << chr;
	sprintf(chr, "service_type<%u>", service_type); stringStream << chr;

	return stringStream.str();
}
