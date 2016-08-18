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

#if !defined(EA_5E8946F3_3A63_43ad_A20C_16593CE5140A__INCLUDED_)
#define EA_5E8946F3_3A63_43ad_A20C_16593CE5140A__INCLUDED_

#include <map>
#include <list>
#include <string>
#include <iostream>

#include "util/tinyxml/tinyxml.h"

using namespace std;

/**
 *  @class DescriptorFactory
 *  @brief descriptor 내에서 특정 tag에 해당하는 descriptor를 추출
 */
class DescriptorFactory
{

public:

	virtual ~DescriptorFactory();

	/**
	 * @brief instance를 생성하여서 반환
	 */
	static DescriptorFactory& getInstance() {
		static DescriptorFactory instance;

		return instance;
	}
	
	/**
	 * @brief DescriptorFactory에서 사용할 xml파일을 지정한다.
	 * @
	 */
	void setXMLFile(list<string> xml_file);

	/**
	 * @brief tag에 대한 descriptor 정보를 추출한다.
	 * @param tag Descriptor ID
	 * @return Descriptor 정보 list<Element*>
	 */
	TiXmlNode* getDescriptor(int tag);

	/**
	 * @brief tag에 해당하는 descriptor name을 반환
	 * @param tag Descriptor ID
	 * @return Descriptor Name
	 */
	string getDescriptorName(int tag);

private:

	DescriptorFactory() {
	};                   // Constructor? (the {} brackets) are needed here.
	// Dont forget to declare these two. You want to make sure they
    // are unaccessable otherwise you may accidently get copies of
    // your singleton appearing.
	DescriptorFactory(DescriptorFactory const&);            // Don't Implement
    void operator=(DescriptorFactory const&); 				// Don't implement


	/**
	 * @brief Descriptor Map < key: id, value: Descriptor>
	 */
	map<int, TiXmlNode*> mMapDescriptor;

	/**
	 * @brief Descriptor Map < key: id, value: Descriptor Name>
	 */
	map<int, string> mMapDescriptorInfo;

	/**
	 * @brief descriptor file을 읽어온다.
	 * @param fileName Descriptor File Name
	 * @return void
	 */
	void addDescriptorFile(string fileName);

	/**
	 * @brief
	 * @return void
	 */
	void printDescriptorInfo();

	/**
	 * @brief
	 * @param mapDescriptorInfo
	 * @return void
	 */
	void updateDescriptorInfo(map<int, string> mapDescriptorInfo);

	/**
	 * @brief
	 * @param mapDescriptor
	 * @return void
	 */
	void updateDescriptor(map<int, TiXmlNode*> mapDescriptor);

	map<int, string> getMapDescriptorInfo(TiXmlNode *node);
	map<int, TiXmlNode*> getMapDescriptor(TiXmlNode *node);
};
#endif // !defined(EA_5E8946F3_3A63_43ad_A20C_16593CE5140A__INCLUDED_)
