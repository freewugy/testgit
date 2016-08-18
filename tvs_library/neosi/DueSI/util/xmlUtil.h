/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * xmlUtil.h
 *
 * LastChangedBy: owl
 * LastChangedDate: 2014. 6. 25.
 * Description:
 * Note:
 *****************************************************************************/
#ifndef XMLUTIL_H_
#define XMLUTIL_H_

#include <tinyxml/tinyxml.h>
#include <string>
#include <stdint.h>

using namespace std;

string getStringFromXMLNode(TiXmlNode* node, string name);
int getIntFromXMLNode(TiXmlNode* node, string name);
uint32_t getLongIntFromXMLNode(TiXmlNode* node, string name);


string getStringFromXMLElement(TiXmlElement* elm, string name);
int getIntFromXMLElement(TiXmlElement* elm, string name);
uint32_t getLongIntFromXMLElement(TiXmlElement* elm, string name);

string getStringFromXMLElementText(TiXmlElement* elm);
int getIntFromXMLElementText(TiXmlElement* elm);
uint32_t getLongIntFromXMLElementText(TiXmlElement* elm);

int getIPAddressFromXMLElement(TiXmlElement* elm, string name);

#endif /* XMLUTIL_H_ */
