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

#ifndef _NITINFO_H_
#define _NITINFO_H_

#include <string>
#include <list>
#include "Element.h"

using namespace std;

class Element;

typedef struct _CableDeliveryParameter {
    uint32_t frequency;
    uint8_t fecOuter;
    uint8_t modulation;
    uint32_t symbolRate;
    uint8_t fecInner;
} CableDeliveryParameter;

typedef struct _TerrestrialDeliveryParameter {
    uint32_t centreFrequency;
    uint8_t bandwidth;
    uint8_t constellation;
    uint8_t hierarchyInformation;
    uint8_t codeRateHpStream;
    uint8_t codeRateLpStream;
    uint8_t guardInterval;
    uint8_t transmissionMode;
    uint8_t otherFrequencyFlag;
} TerrestrialDeliveryParameter;

typedef struct _SatelliteDeliveryParameter {
    uint32_t frequency;
    uint16_t orbitalPosition;
    uint8_t westEastFlag;
    uint8_t polarization;
    uint8_t rollOff;
    uint8_t modulationSystem;
    uint8_t modulationType;
    uint32_t symbolRate;
    uint8_t fecInner;
} SatelliteDeliveryParameter;

typedef struct _S2SatelliteDeliveryParameter {
    uint8_t scramblingSequenceSelector;
    uint8_t multipleInputStreamFlag;
    uint8_t backwardsCompatibilityIndicator;
    uint32_t scramblingSequenceIndex;
    uint8_t inputStreamIdentifier;
} S2SatelliteDeliveryParameter;

typedef union _tuner_delivery_parameter {
    CableDeliveryParameter cable;
    TerrestrialDeliveryParameter terrestrial;
    SatelliteDeliveryParameter satellite;
    S2SatelliteDeliveryParameter s2satellite;
} tuner_delivery_parameter;

/**
 * @brief Program정보를 담고 있는 class
 * @class NITInfo
 */
class NITInfo {
public:
    NITInfo();
    virtual ~NITInfo();

    uint16_t network_id;
    uint16_t transport_stream_id;
    string network_name;
    uint16_t original_network_id;
    uint16_t service_id;
    uint8_t linkage_type;

    uint8_t service_type;
    uint16_t channel_number;

    tuner_delivery_parameter mTunerDeliveryParam;

    /**
     * @brief toString
     * @return string
     */
    string toString();
    string toXMLString();
};

#endif /* EVENTINFO_H_ */
