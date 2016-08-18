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

#include "DescriptorFactory.h"
#include <stdio.h>
#include "Logger.h"
#include "StringUtil.h"

static const char* TAG = "DescriptorFactory";

DescriptorFactory::~DescriptorFactory(){
}

string DescriptorFactory::getDescriptorName(int tag) {
	string descriptor_name = "NOT FOUND";
	map< int, string >::iterator iterPos;

	iterPos = this->mMapDescriptorInfo.find(tag);
	if (iterPos != mMapDescriptorInfo.end()) {
		descriptor_name = iterPos->second;
	}

	return descriptor_name;
}

TiXmlNode* DescriptorFactory::getDescriptor(int tag) {
	map< int, TiXmlNode*>::iterator iterDescriptor;

	iterDescriptor = this->mMapDescriptor.find(tag);

	return iterDescriptor->second;
}

void DescriptorFactory::setXMLFile(list<string> xml_file) {
	for(list<string>::iterator itr=xml_file.begin(); itr != xml_file.end(); itr++) {
		this->addDescriptorFile(*itr);
	}
}

void DescriptorFactory::updateDescriptorInfo(map<int, string> mapDescriptorInfo) {
	for(map<int, string>::iterator itr=mapDescriptorInfo.begin(); itr != mapDescriptorInfo.end(); itr++) {
		mMapDescriptorInfo.insert(make_pair(itr->first, itr->second));
	}
}

void DescriptorFactory::updateDescriptor(map<int, TiXmlNode*> mapDescriptor) {
	for(map<int, TiXmlNode*>::iterator itr=mapDescriptor.begin(); itr != mapDescriptor.end(); itr++) {
		mMapDescriptor.insert(make_pair(itr->first, itr->second));
	}
}

void DescriptorFactory::addDescriptorFile(string fileName) {
#ifdef ANDROID
#ifdef __FOR_LINKNET__
	fileName = "/data/lkn_home/config/si/" + fileName;
#endif
#ifdef __COMMON_MODULE__
    fileName = "/data/tvs/si/config/" + fileName;
#endif
#endif
	L_INFO(TAG, ": %s\n", fileName.c_str());

	string strDescriptor;
	try {
		strDescriptor = get_file_contents(fileName.c_str());
	} catch(int err) {
		L_ERROR(TAG, "=== OOPS : %s : errno : %d\n", fileName.c_str(), err);
		return;
	}
	TiXmlDocument* tinyXmlDoc = new TiXmlDocument();

	tinyXmlDoc->Parse(strDescriptor.c_str());

	TiXmlNode* node = tinyXmlDoc->FirstChild("descriptors");

	map<int, string> mapDescriptorInfo;
	mapDescriptorInfo = getMapDescriptorInfo(node);

	updateDescriptorInfo(mapDescriptorInfo);

	map<int, TiXmlNode*> mapDescriptor;
	mapDescriptor = getMapDescriptor(node);
	updateDescriptor(mapDescriptor);
}

void DescriptorFactory::printDescriptorInfo() {
	for(map< int, string >::iterator iter = mMapDescriptorInfo.begin(); iter != mMapDescriptorInfo.end(); iter++) {
		L_INFO(TAG, "0x%02x : %s\n", iter->first, iter->second.c_str());
	}
}

map<int, string> DescriptorFactory::getMapDescriptorInfo(TiXmlNode *node) {
	map<int, string> mapDescriptorInfo;

	for(TiXmlNode* n = node->FirstChild("descriptor"); n; n = n->NextSibling("descriptor")) {
		string descriptor_name(n->ToElement()->Attribute("name"));
		const char* strId = n->ToElement()->Attribute("value");
		mapDescriptorInfo.insert(make_pair(hexCharToInt(strId), descriptor_name));
	}
	return mapDescriptorInfo;
}

map<int, TiXmlNode*> DescriptorFactory::getMapDescriptor(TiXmlNode *node) {
	map<int, TiXmlNode*> mapDescriptor;

	for(TiXmlNode* n = node->FirstChild("descriptor"); n; n = n->NextSibling("descriptor")) {
		const char* strId = n->ToElement()->Attribute("value");
		mapDescriptor.insert(make_pair(hexCharToInt(strId), n));
	}
	return mapDescriptor;
}
