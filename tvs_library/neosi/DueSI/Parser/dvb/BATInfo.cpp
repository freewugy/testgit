/*
 * BATInfo.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: wjpark
 */

/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * Description:
 * Note:
 *****************************************************************************/

#include <sstream>
#include <stdio.h>
#include "BATInfo.h"
#include <stdlib.h>
BATInfo::BATInfo() : bouquet_id(0), transport_stream_id(0), bouquet_name(""), original_network_id(0),service_id(0), linkage_type(0), service_type(0), local_channel_number(0)
{
}

BATInfo::~BATInfo() {
	// TODO Auto-generated destructor stub
}

string BATInfo::toXMLString() {
	char chr[256];
	ostringstream stringStream;

	stringStream << "<BATInfo ";
	sprintf(chr, "bouquet_id=\"%u\" ", bouquet_id); stringStream << chr;
	sprintf(chr, "transport_stream_id=\"%u\" ", transport_stream_id); stringStream << chr;
	sprintf(chr, "bouquet_name=\"%s\" ", bouquet_name.c_str()); stringStream << chr;
	sprintf(chr, "original_network_id=\"%u\" ", original_network_id); stringStream << chr;
	sprintf(chr, "service_id=\"%u\" ", service_id); stringStream << chr;
	sprintf(chr, "linkage_type=\"%u\" ", linkage_type); stringStream << chr;
	sprintf(chr, "service_type=\"%u\" ", service_type); stringStream << chr;
	sprintf(chr, "local_channel_number=\"%u\" />", local_channel_number); stringStream << chr;

	return stringStream.str();
}

string BATInfo::toString() {
	char chr[256];
	ostringstream stringStream;

	sprintf(chr, "network_id<%u>", bouquet_id); stringStream << chr;
	sprintf(chr, "transport_stream_id<%u>", transport_stream_id); stringStream << chr;
	sprintf(chr, "network_name<%s>", bouquet_name.c_str()); stringStream << chr;
	sprintf(chr, "original_network_id<%u>", original_network_id); stringStream << chr;
	sprintf(chr, "service_id<%u>", service_id); stringStream << chr;
	sprintf(chr, "linkage_type<%u>", linkage_type); stringStream << chr;
	sprintf(chr, "service_type<%u>", service_type); stringStream << chr;
	sprintf(chr, "local_channel_number<%u>", local_channel_number); stringStream << chr;
	return stringStream.str();
}



