/*
 * msg_encoder.h
 *
 *  Created on: Jan 30, 2015
 *      Author: jhseo
 */

#ifndef MSG_ENCODER_H_
#define MSG_ENCODER_H_

#include "msg_format.h"
#include "operation_mgr.h"

char *makeSusCheckMsg(IMG_FILE_INFO_T* pImgInfo);
char * makeSusReportMsg(REPORT_TYPE_T reportType);

#endif /* MSG_ENCODER_H_ */
