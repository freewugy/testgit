/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-21 14:23:57 +0900 (월, 21 4월 2014) $
 * $LastChangedRevision: 661 $
 * Description:
 * Note:
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "PMTInfo.h"

PMTInfo::PMTInfo() {
	program_number = 0;
	pcr_pid = 0x1fff;
}

PMTInfo::~PMTInfo() {
	// TODO Auto-generated destructor stub
}

string PMTInfo::toXMLString() {
	string xml="";
	char chr[4096];
	ostringstream stringStream;

	list<Element>::iterator itr;
	stringStream << "<PMT ";
	sprintf(chr, "program_number=\"%u\" ", program_number); stringStream << chr;
	sprintf(chr, "pcr_pid=\"0x%04x\" ", pcr_pid); stringStream << chr;

	stringStream << " >\n";
	if(list_elementary_stream.size() > 0) {
		for(list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
			elementary_stream esInfo = *itr;
			stringStream << "\t<esInfo ";
			sprintf(chr, "stream_type=\"0x%04x\" ", esInfo.stream_type); stringStream << chr;
			sprintf(chr, "elementary_PID=\"0x%04x\" ", esInfo.stream_pid); stringStream << chr;
			sprintf(chr, "language_code=\"0x%x\" ", esInfo.language_code); stringStream << chr;
			stringStream << " />\n";
		}
	}

	stringStream << "</PMT>";
	return stringStream.str();
}

string PMTInfo::toString() {
	string xml="";
	char chr[4096];
	ostringstream stringStream;

	sprintf(chr, "program_number<%u>", program_number); stringStream << chr;
	sprintf(chr, "pcr_pid<0x%04x>", pcr_pid); stringStream << chr;

	if(list_elementary_stream.size() > 0) {
		for(list<elementary_stream>::iterator itr = list_elementary_stream.begin(); itr != list_elementary_stream.end(); itr++) {
			elementary_stream esInfo = *itr;
			sprintf(chr, "stream_pid<0x%04x>", esInfo.stream_pid); stringStream << chr;
			sprintf(chr, "stream_type<0x%x>", esInfo.stream_type); stringStream << chr;
			sprintf(chr, "language_code<0x%x>", esInfo.language_code); stringStream << chr;
		}
	}

	return stringStream.str();
}
