/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 * xmlUtil.cpp
 *
 * LastChangedBy: owl
 * LastChangedDate: 2014. 6. 25.
 * Description:
 * Note:
 *****************************************************************************/

#include "xmlUtil.h"
#include "StringUtil.h"
#include <vector>

string getStringFromXMLNode(TiXmlNode* node, string name)
{
    TiXmlElement* elm = node->ToElement();
    return getStringFromXMLElement(elm, name);
}

int getIntFromXMLNode(TiXmlNode* node, string name)
{
    TiXmlElement* elm = node->ToElement();
    return getIntFromXMLElement(elm, name);
}

uint32_t getLongIntFromXMLNode(TiXmlNode* node, string name)
{
    TiXmlElement* elm = node->ToElement();
    return getLongIntFromXMLElement(elm, name);
}

string getStringFromXMLElement(TiXmlElement* elm, string name)
{
    if(elm->Attribute(name.c_str()) == NULL) {
        return "";
    } else {
        return elm->Attribute(name.c_str());
    }
}

int getIntFromXMLElement(TiXmlElement* elm, string name)
{
    if(elm->Attribute(name.c_str()) == NULL) {
        return 0;
    } else {
        return atoi(elm->Attribute(name.c_str()));
    }
}

uint32_t getLongIntFromXMLElement(TiXmlElement* elm, string name)
{
    if(elm->Attribute(name.c_str()) == NULL) {
        return 0;
    } else {
        return atoll(elm->Attribute(name.c_str()));
    }
}

string getStringFromXMLElementText(TiXmlElement* elm)
{
    if(elm->GetText() == NULL) {
        return "";
    } else {
        return elm->GetText();
    }
}

int getIntFromXMLElementText(TiXmlElement* elm)
{
    if(elm->GetText() == NULL) {
        return 0;
    } else {
        return atoi(elm->GetText());
    }
}

uint32_t getLongIntFromXMLElementText(TiXmlElement* elm)
{
    if(elm->GetText() == NULL) {
        return 0;
    } else {
        return atol(elm->GetText());
    }
}

int getIPAddressFromXMLElement(TiXmlElement* elm, string name)
{
    if(elm->Attribute(name.c_str()) == NULL) {
        return 0;
    } else {
        string ip_addr = elm->Attribute(name.c_str());
        int ipAddr = 0;
        vector < string > listStr;
        int count = splitStringByDelimiter(ip_addr, ".", listStr);
        if(count == 3) {
            ipAddr = atoi(listStr[0].c_str()) << 24 | atoi(listStr[1].c_str()) << 16 | atoi(listStr[2].c_str()) << 8 | atoi(listStr[3].c_str());
            return ipAddr;
        }
    }

    return 0;
}
