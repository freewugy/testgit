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

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include <iostream>
#include <string>
#include <list>
#include "Element.h"

typedef enum {
	eLength = 0,
	eCount,
} LoopType;

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "tinyxml/tinyxml.h"
#include <stdlib.h>
#include <map>

#include "BitStream.h"
#include "StringUtil.h"

class BitStream;

using namespace std;

/**
 * @class XMLParser
 * @brief parse BitStream By XML-Schema
 */
class XMLParser {
public:
	/**
	 * @brief 생성자
	 */
	XMLParser();

	/**
	 * @brief 소멸자
	 */
	virtual ~XMLParser();

	/**
	 * @brief schema XML string을 Parsing해서 TiXmlDocument 객체 생성
	 * @param strXML string of XML (저장된 XML파일을 읽어서 string으로 변환한 것임)
	 */
	void init(string strXML);

	/**
	 * @brief BitStream을 파싱한다.
	 * @param bs BitStream Data
	 * @param name table name (ex, SDT, EITpf, etc)
	 * @return XmlNode 파싱한 결과를 XML instance
	 */
	TiXmlNode* parseXML(BitStream *bs, string name);

	/**
	 * @brief Descriptor Bitstream을 파싱한다.
	 * @param bs Bitstream Data
	 * @param descriptor_length Bitstream Length
	 * @return XmlNode
	 */
	TiXmlNode* parseDescriptor(BitStream *bs, int descriptor_length);
private:
	/**
	 * @brief BitStream에서 XML schema에 맞추어서 parsing
	 * @param [IN] bs
	 * @param [IN] n
	 * @param [OUT] section
	 * @param length
	 * @return int parsing한 길이
	 */
	int parsingBSByXML(BitStream *bs, TiXmlNode *n, TiXmlElement* section, int length);

	/**
	 * @brief return length which saved
	 * @param name name of length
	 */
	uint32_t getPredefinedLength(string name);

	/**
	 * @brief Node에서 length를 구해온다
	 * @param [IN] node TiXmlNode
	 * @param [OUT] length
	 * @return 0이면 length, 1이면 count
	 */
	LoopType getLengthFromNode(TiXmlNode* node, int* length);

	TiXmlElement* parseElement(BitStream* bs, TiXmlNode* node, int* parseLength);

	int parseDescriptor(BitStream *bs, int descriptor_length, TiXmlElement *xmlElm);

	bool ifConditioned(string strName, string strCondition);

	bool elseConditioned(string strName);
	string compoundEntry(uint8_t* input_src, int length);
	string compoundEntryMSS(uint8_t* input_src, int length);

	int mLengthOfDescriptor;
	string mDescriptorName;
	/**
	 * @brief root DOM Document
	 */
    TiXmlDocument *mSchemaXML;

    /**
     * @brief map < key : name of length, value : length >
     */
    map<string, uint32_t> mMapNameValue;

    map<string, int> mMapIfCondition;

    string mTableName;
};
#endif /* XMLPARSER_H_ */
