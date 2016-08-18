/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-15 11:11:57 +0900 (화, 15 4월 2014) $
 * $LastChangedRevision: 610 $
 * Description:
 * Note:
 *****************************************************************************/

#include "PMTParser.h"
#include "Logger.h"

static const char* TAG = "PMTParser";
static int pmt_count =0;

PMTParser::PMTParser()
{
	pmt_count++;
	L_DEBUG("PMTParser", ">> constructor [%d]",pmt_count);
	init();
}

PMTParser::~PMTParser()
{
    L_DEBUG(TAG, "<< destructor");
    for (list<PMTInfo *>::iterator itr = listTotalPMTInfo.begin(); itr != listTotalPMTInfo.end(); itr++) {
        if((*itr) != NULL) {
        	PMTInfo* info = *itr;
        	info->list_elementary_stream.clear();
        	delete *itr;
      //  	L_DEBUG(TAG, "<< delete pmt info");
        }
    }
    listTotalPMTInfo.clear();
}

bool PMTParser::process(void* sectionData)
{
    Section* pSec = (Section*)sectionData;

    process(pSec->m_pSection, pSec->m_pSectionLen);
}


bool PMTParser::process(uint8_t* sectionData, int length)
{
	uint32_t value;
    PMTInfo* info = new PMTInfo();
    BitStream* bs =new BitStream(sectionData, length, false);
    if(false == bs->skipByte(3)
            || false == bs->readBits(16, &(info->program_number))
            || false == bs->skipByte(3)
            || false == bs->skipBits(3)
            || false == bs->readBits(13, &(info->pcr_pid))
            || false == bs->skipBits(4)
            || false == bs->readBits(12, &value)) {
        L_ERROR(TAG, "parse() failed\n");
        delete bs;
        delete info;
        return false;
    }

    if(value>0)
    {
    	if(false == bs->skipByte(value)) {
            L_ERROR(TAG, "parse() skipByte failed\n");
            delete bs;
            delete info;
            return false;
    	}
    }

    while(bs->getRemainBytes() > 4)
    {
    	uint16_t descleng;
    	elementary_stream es;
    	es.language_code = 0;
    	uint8_t audio_type = 0;
    	if(false == bs->readBits(8, &es.stream_type)
    	        || false ==	bs->skipBits(3)
    	        || false == bs->readBits(13, &es.stream_pid)
    	        || false == bs->skipBits(4)
    	        || false == bs->readBits(12, &descleng)) {
    	    L_ERROR(TAG, "parse() failed\n");
    	    delete bs;
            delete info;
    	    return false;
    	}

    	if(descleng > 0)
    	{
    		uint8_t* desc_ptr = bs->getPtr();
			BitStream* desc_bs =new BitStream(desc_ptr, descleng, false);

			while(desc_bs->getRemainBytes() > 4)
			{
				uint8_t tag,length;
				desc_bs->readBits(8,&tag);
				desc_bs->readBits(8,&length);
				if(length > 0)
				{
//					L_INFO(TAG, "desc tag: 0x%02X length=%d\n",tag,length);
					if(tag == 0x0a) { // ISO 639 language
						for(int i = 0; i < length; i += 4) {
							desc_bs->readBits(24, &es.language_code);
							desc_bs->readBits(8, &audio_type);
						}
					} else {
						desc_bs->skipByte(length);
					}
				}
			}

			delete desc_bs;

			if(false == bs->skipByte(descleng)) {
	            L_ERROR(TAG, "skipByte()[%u] failed\n", descleng);
	            delete bs;
            	delete info;
	            return false;
			}
    	}
    	info->list_elementary_stream.push_back(es);
    }
	listTotalPMTInfo.push_back(info);
	delete bs;
   	L_DEBUG(TAG, "Received PMTInfo[%u] : ES_Info[%u]\n", listTotalPMTInfo.size(), info->list_elementary_stream.size());

    return true;
}

bool PMTParser::parse(BitStream* bs)
{
    return true;
}

bool PMTParser::init() {
    return true;
}
