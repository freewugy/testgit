/*
 * msg_decoder.h
 *
 *  Created on: Jan 30, 2015
 *      Author: jhseo
 */

#ifndef MSG_DECODER_H_
#define MSG_DECODER_H_

#include "msg_format.h"

int parseCheckRespMsg(char * pCheckRespMsg, UPGRADE_INFO_T** ppUpgradeInfo);

#endif /* MSG_DECODER_H_ */
