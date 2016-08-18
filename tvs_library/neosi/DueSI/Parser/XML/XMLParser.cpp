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

#include <stdio.h>
#include <sstream>

#include "XMLParser.h"
#include "DescriptorFactory.h"
#include "Logger.h"
#include "StringUtil.h"
#include "ByteStream.h"

static const char* TAG = "XMLParser";

XMLParser::XMLParser()
{
    mLengthOfDescriptor = 0;
}

XMLParser::~XMLParser()
{
    mMapNameValue.clear();
    mMapIfCondition.clear();
}

void XMLParser::init(string strXML)
{
    mSchemaXML = new TiXmlDocument();

    mSchemaXML->Parse(strXML.c_str());
    if(mSchemaXML->Error()) {
        L_ERROR(TAG, "error in %s: %s\n", mSchemaXML->Value(), mSchemaXML->ErrorDesc());
        L_DEBUG(TAG, "%s", strXML.c_str());
    }

    mMapIfCondition.clear();
    mMapNameValue.clear();

    mLengthOfDescriptor = 0;
}

TiXmlNode* XMLParser::parseXML(BitStream *bs, string name)
{
    if(bs == NULL)
        return NULL;

    mMapIfCondition.clear();
    mMapNameValue.clear();

    TiXmlElement* section = new TiXmlElement("Section");
    section->SetAttribute("name", name.c_str());
    mTableName = name;

    TiXmlNode* node = mSchemaXML->FirstChild("Table");
    parsingBSByXML(bs, node, section, 0);

    return (TiXmlNode *) section;
}

uint32_t XMLParser::getPredefinedLength(string name)
{
    int length = 0;
    std::map<string, uint32_t>::iterator it;
    it = mMapNameValue.find(name);

    if(it == mMapNameValue.end()) {
        L_INFO(TAG, "could not find %s\n", name.c_str());
    } else {
        length = it->second;
#ifdef __DEBUG_XML_PARSER__
        L_INFO(TAG, "[%s] is [%d]\n", name.c_str(), length);
#endif
    }

    return length;
}

LoopType XMLParser::getLengthFromNode(TiXmlNode* node, int* length)
{
    *length = -1;
    LoopType type = eLength;

    string strLength(node->ToElement()->Attribute("length"));

    if(strLength.compare("-1") != 0) {
        if(is_number(strLength)) {
            *length = atoi(strLength.c_str());
            type = eCount;
        } else {
            *length = getPredefinedLength(strLength);
            if(hasSuffix(strLength, "count") || hasPrefix(strLength, "num") || strLength.compare("tables_defined") == 0)
                type = eCount;
            else
                type = eLength;
        }

    }
    return type;
}

int XMLParser::parseDescriptor(BitStream *bs, int descriptors_length, TiXmlElement* xmlElm)
{
    // descriptors loop length만큼 loop를 돌아야 한다.
    uint32_t id, current_desc_length;
    int lenParse = 0;
#ifdef __DEBUG_XML_PARSER__
    L_DEBUG(TAG, "descriptors_length is %d, BitStream[%u]\n",
            descriptors_length, bs->getRemainBytes());
#endif

    while (descriptors_length > 0 && bs->getRemainBytes() > 4) {
        bs->readBits(8, &id);
        bs->readBits(8, &current_desc_length);
        descriptors_length -= 2;

        TiXmlNode* node = DescriptorFactory::getInstance().getDescriptor(id);

        if(descriptors_length < current_desc_length) {
            L_ERROR(TAG, "=== OOPS[0x%02x] current_desc_length[%d] is BIGGER THAN descriptors_length[%d]\n", id, current_desc_length, descriptors_length);
            bs->skipByte(descriptors_length);

            current_desc_length = descriptors_length;
            break;
        }

        mLengthOfDescriptor = current_desc_length * 8;

        if(node == NULL) {
            L_DEBUG(TAG, "TableName[%s][%u] descriptor ID[0x%02x] length[%d] NOT FOUND\n", mTableName.c_str(), descriptors_length, id,
                    current_desc_length);
            bs->skipByte(current_desc_length);
        } else {
            mDescriptorName = DescriptorFactory::getInstance().getDescriptorName(id);
#ifdef __DEBUG_XML_PARSER__
            L_DEBUG(TAG, "TableName[%s][%u] descriptor[ID:0x%02x, %s] length[%d]\n",
                    mTableName.c_str(), descriptors_length, id, mDescriptorName.c_str(), current_desc_length);
#endif
            lenParse += parsingBSByXML(bs, node, xmlElm, current_desc_length);
        }

        descriptors_length -= current_desc_length;
    }
    return lenParse;
}

TiXmlNode* XMLParser::parseDescriptor(BitStream *bs, int descriptors_length)
{
    // descriptors loop length만큼 loop를 돌아야 한다.
    TiXmlElement* descriptor = new TiXmlElement("descriptor");
    uint32_t id, current_desc_length;
    int lenParse = 0;
#ifdef __DEBUG_XML_PARSER__
    L_DEBUG(TAG, "descriptors_length is %d, BitStream[%u]\n", descriptors_length, bs->getRemainBytes());
#endif

    bs->readBits(8, &id);
    bs->readBits(8, &current_desc_length);

    TiXmlNode* node = DescriptorFactory::getInstance().getDescriptor(id);

    if(descriptors_length < current_desc_length) {
        L_ERROR(TAG, "=== OOPS[0x%02x] current_desc_length[%d] is BIGGER THAN descriptors_length[%d]\n", id, current_desc_length, descriptors_length);
        delete descriptor;
        return 0;
    }

    mLengthOfDescriptor = current_desc_length * 8;

    if(node == NULL) {
        L_DEBUG(TAG, "descriptor ID[0x%02x] length[%d] NOT FOUND\n", id, current_desc_length);
        delete descriptor;
        return 0;
    } else {
        mDescriptorName = DescriptorFactory::getInstance().getDescriptorName(id);
#ifdef __DEBUG_XML_PARSER__
        L_DEBUG(TAG, "descriptor[ID:0x%02x, %s] length[%u/%d]\n", id, mDescriptorName.c_str(), descriptors_length, current_desc_length);
#endif
        descriptor->SetAttribute("name", mDescriptorName.c_str());
        lenParse += parsingBSByXML(bs, node, descriptor, current_desc_length);
    }

    return (TiXmlNode *) descriptor;
}

TiXmlElement* XMLParser::parseElement(BitStream* bs, TiXmlNode* n, int* parseLength)
{
    TiXmlElement* elem = n->ToElement();
    uint32_t v;
    int length = atoi(elem->Attribute("length"));
    *parseLength = length;

    string strName(elem->Attribute("name"));
    bs->readBits(length, &v);

    if(hasPrefix("__", strName)) {
        return NULL;
    }

    TiXmlElement* newElem = n->Clone()->ToElement();

    newElem->SetAttribute("value", v);
    newElem->RemoveAttribute("length");

    // Map에 저장 map<string, int>
    std::map<string, uint32_t>::iterator it;
    it = mMapNameValue.find(strName);

    if(it == mMapNameValue.end()) {
        mMapNameValue.insert(make_pair(strName, v));
    } else {
        it->second = v;
    }

    return newElem;
}

bool XMLParser::ifConditioned(string strName, string strCondition)
{
    // Map에 저장 map<string, int>
    std::map<string, int>::iterator it;
    it = mMapIfCondition.find(strName);

    if(it != mMapIfCondition.end()) {
        if(it->second == 1) {
            return false;
        }
    }

    uint8_t value = getPredefinedLength(strName);

    int condition = 0;
    int op = 0; // 0 : equal, 1 : smaller than, 2 : smaller than & equal, 3 : bigger than, 4 : bigger than & eqaul
    // get operator
    // get condition
    const char* chr = strCondition.c_str();
    if(chr[0] == '>') {
        if(chr[1] == '=') {
            op = 2;
            condition = atoi(&chr[2]);
        } else {
            op = 1;
            condition = atoi(&chr[1]);
        }
    } else if(chr[0] == '<') {
        if(chr[1] == '=') {
            op = 4;
            condition = atoi(&chr[2]);
        } else {
            op = 3;
            condition = atoi(&chr[1]);
        }
    } else {
        op = 0;
        condition = atoi(chr);
    }
#ifdef __DEBUG_XML_PARSER__
    L_INFO(TAG, "if : name[%s], condition[%d], op[%d], savedValue[%u]\n", strName.c_str(), condition, op, value);
#endif
    bool res = false;
    switch (op) {
        case 0:
        if(value == condition)
            res = true;
            break;
        case 1:
        if(value > condition)
            res = true;
            break;
        case 2:
        if(value >= condition)
            res = true;
            break;
        case 3:
        if(value < condition)
            res = true;
            break;
        case 4:
        if(value <= condition)
            res = true;
            break;
        default:
            break;
    }

    return res;
}

bool XMLParser::elseConditioned(string strName)
{
    // Map에 저장 map<string, int>
    std::map<string, int>::iterator it;
    it = mMapIfCondition.find(strName);

    if(it == mMapIfCondition.end()) {
        return true; // if 조건문에 맞지 않았다.
    } else {
        int condition = it->second;
        if(condition == 1) {
            return false; // if 조건문에 맞아서 이미 실행하였슴
        } else {
            return true; // if 조건문에서 맞지 않아서 실행되지 않음
        }
    }
}

string XMLParser::compoundEntry(uint8_t* input_src, int length)
{
    // TODO: utf8인 경우만 처리함, utf16이나 32로 들어오는 경우에는 다르게 처리해야함, MSS 처리 필요
    string convertedValue = "";

    convertedValue.assign((char*) input_src, length);

    return convertedValue;
}

int XMLParser::parsingBSByXML(BitStream *bs, TiXmlNode *n, TiXmlElement* xmlElem, int length)
{
    int lenParse = 0;

    const char *name = n->Value();

    if(strcmp(name, "elm") == 0) {
        int ll = 0;
        TiXmlElement* element = parseElement(bs, n, &ll);
        if(element != NULL)
            xmlElem->LinkEndChild(element);
        lenParse += ll;
        mLengthOfDescriptor -= ll;
    } else if(strcmp(name, "entry") == 0) {
#ifdef __DEBUG_XML_PARSER__
        const char* sName = n->ToElement()->Attribute("name");
#endif
        int entry_length;
        LoopType type = getLengthFromNode(n, &entry_length);
        if(type == eLength) {
            if(entry_length == -1) {
                if(length == 0) {
                    entry_length = (bs->getRemainBytes() - 4);
                } else {
                    entry_length = (mLengthOfDescriptor / 8);
                }
            }

            int bit_entry_length = entry_length * 8;
#ifdef __DEBUG_XML_PARSER__
            L_DEBUG(TAG, "[%s:%s] : Length Entry[%d], Remain Length[%d]\n",
                    mDescriptorName.c_str(), sName, entry_length, mLengthOfDescriptor);
#endif
            int length_parse_entry;

            if(entry_length <= 0 || mLengthOfDescriptor <= 0) {
#ifdef __DEBUG_XML_PARSER__
                L_DEBUG(TAG, "=== [%s:%s] : BitStream has not MORE DATA ===\n", mDescriptorName.c_str(), sName);
#endif
                n = n->NextSibling();
            } else {
                const char* type = n->ToElement()->Attribute("type");
                if(type == 0) {
                    while (bit_entry_length > 0) {
                        TiXmlElement* entry = new TiXmlElement("entry");
                        entry->SetAttribute("name", n->ToElement()->Attribute("name"));
                        for (TiXmlNode *child = n->FirstChild(); child; child = child->NextSibling()) {
                            length_parse_entry = parsingBSByXML(bs, child, entry, length);
                            bit_entry_length -= length_parse_entry;
                            lenParse += length_parse_entry;
                        }

#ifdef __DEBUG_XML_PARSER__
                        L_DEBUG(TAG, "%s : Remain Entry_length is %d\n", sName, bit_entry_length);
#endif
                        xmlElem->LinkEndChild(entry);
                    }
                } else if(strcmp(type, "C") == 0) {
                    TiXmlElement* elm = new TiXmlElement("elm");
                    elm->SetAttribute("name", n->ToElement()->Attribute("name"));
                    int length = atoi(n->FirstChild()->ToElement()->Attribute("length"));
                    uint8_t src[entry_length];
                    memset(src, '\0', entry_length);
                    int i = 0;
                    uint8_t v;
                    while (bit_entry_length > 0) {
                        bs->readBits(length, &v);
                        bit_entry_length -= length;
                        lenParse += length;

                        src[i++] = v;
#ifdef __DEBUG_XML_PARSER__
                        L_DEBUG(TAG, "%s : Remain Entry_length is %d\n", sName, bit_entry_length);
#endif
                    }

                    elm->SetAttribute("value", compoundEntry(src, entry_length).c_str());
                    xmlElem->LinkEndChild(elm);
                } else if(strcmp(type, "MSS") == 0) {
                    TiXmlElement* elm = new TiXmlElement("elm");
                    elm->SetAttribute("name", n->ToElement()->Attribute("name"));
                    int length = atoi(n->FirstChild()->ToElement()->Attribute("length"));
                    uint8_t src[entry_length];
                    memset(src, '\0', entry_length);
                    int i = 0;
                    uint8_t v;
                    while (bit_entry_length > 0) {
                        bs->readBits(length, &v);
                        bit_entry_length -= length;
                        lenParse += length;

                        src[i++] = v;
#ifdef __DEBUG_XML_PARSER__
                        L_DEBUG(TAG, "%s : Remain Entry_length is %d\n", sName, bit_entry_length);
#endif
                    }

                    elm->SetAttribute("value", compoundEntry(src, entry_length).c_str());
                    xmlElem->LinkEndChild(elm);
                }
            }
        } else if(type == eCount) {
            int entry_count = entry_length;

#ifdef __DEBUG_XML_PARSER__
            L_DEBUG(TAG, "%s : Total Entry_count is %d\n", sName, entry_count);
#endif
            int length_parse_entry;
            const char* type = n->ToElement()->Attribute("type");
            if(type == 0) {
                while (entry_count > 0) {
                    TiXmlElement* entry = new TiXmlElement("entry");
                    entry->SetAttribute("name", n->ToElement()->Attribute("name"));

                    for (TiXmlNode *child = n->FirstChild(); child; child = child->NextSibling()) {
                        length_parse_entry = parsingBSByXML(bs, child, entry, length);
                        lenParse += length_parse_entry;
                    }

                    entry_count--;
#ifdef __DEBUG_XML_PARSER__
                    L_DEBUG(TAG, "%s : Remain Entry_count is %d\n", sName, entry_count);
#endif
                    xmlElem->LinkEndChild(entry);
                }
            } else if(strcmp(type, "C") == 0) {
                TiXmlElement* elm = new TiXmlElement("elm");
                elm->SetAttribute("name", n->ToElement()->Attribute("name"));
                int length = atoi(n->FirstChild()->ToElement()->Attribute("length"));
                uint8_t src[entry_count];
                memset(src, '\0', entry_count);
                int i = 0;
                uint8_t v;
                while (entry_count > 0) {
                    bs->readBits(length, &v);
                    lenParse += length;

                    src[i++] = v;
#ifdef __DEBUG_XML_PARSER__
                    L_DEBUG(TAG, "%s : Remain Entry_count is %d\n", sName, entry_count);
#endif
                    entry_count--;
                }

                elm->SetAttribute("value", compoundEntry(src, entry_count).c_str());
                xmlElem->LinkEndChild(elm);
            }
        }

        return lenParse;
    } else if(strcmp(name, "if") == 0) {
        string strName(n->ToElement()->Attribute("name"));
        string strCondition(n->ToElement()->Attribute("condition"));

        if(ifConditioned(strName, strCondition) == true) {
#ifdef __DEBUG_XML_PARSER__
            L_INFO(TAG, "if [%s] statement : true \n", strName.c_str());
#endif
            mMapIfCondition.insert(make_pair(strName, 1));
            for (TiXmlNode *child = n->FirstChild(); child; child = child->NextSibling()) {
                lenParse += parsingBSByXML(bs, child, xmlElem, length);
            }
        } else { // 다음 하위 진행을 넘어가지 않게 수정
#ifdef __DEBUG_XML_PARSER__
        L_INFO(TAG, "if [%s] statement : false \n", strName.c_str());
#endif
            n = n->NextSibling();
        }
        return lenParse;
    } else if(strcmp(name, "else") == 0) {
        string strName(n->ToElement()->Attribute("name"));
        if(strName.length() > 0 && elseConditioned(strName)) {
#ifdef __DEBUG_XML_PARSER__
            L_INFO(TAG, "else [%s] statement : true\n", strName.c_str());
#endif
            for (TiXmlNode *child = n->FirstChild(); child; child = child->NextSibling()) {
                lenParse += parsingBSByXML(bs, child, xmlElem, length);
            }
        } else {
#ifdef __DEBUG_XML_PARSER__
            L_INFO(TAG, "else [%s] statement : false\n", strName.c_str());
#endif
            n = n->NextSibling();
        }
        return lenParse;
    }

    if(n == NULL || n->NoChildren()) {
        return lenParse;
    }

    for (TiXmlNode* child = n->FirstChild(); child; child = child->NextSibling()) {
        lenParse += parsingBSByXML(bs, child, xmlElem, length);
    }
    return lenParse;
}
