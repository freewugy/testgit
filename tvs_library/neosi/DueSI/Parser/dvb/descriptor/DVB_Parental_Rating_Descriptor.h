/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: loneowl $
 * $LastChangedDate: 2014-04-10 15:55:50 +0900 (목, 10 4월 2014) $
 * $LastChangedRevision: 580 $
 * Description:
 * Note:
 *****************************************************************************/

#ifndef DVB_PARENTAL_RATING_DESCRIPTOR_H_
#define DVB_PARENTAL_RATING_DESCRIPTOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "Descriptor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class ParentalRating
{
    protected:
        std::string m_country_code;
        unsigned m_rating                 : 8;

    public:
        ParentalRating(const uint8_t * const buffer);

        const std::string &getCountryCode(void) const;
        uint8_t getRating(void) const;
};

typedef std::list<ParentalRating *> ParentalRatingList;
typedef ParentalRatingList::iterator ParentalRatingIterator;
typedef ParentalRatingList::const_iterator ParentalRatingConstIterator;

class DVB_Parental_Rating_Descriptor : public Descriptor
{
    protected:
        ParentalRatingList m_parental_ratings;

    public:
        DVB_Parental_Rating_Descriptor(const uint8_t * const buffer);
        ~DVB_Parental_Rating_Descriptor(void);

        const ParentalRatingList *getParentalRatings(void) const;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* DVB_PARENTAL_RATING_DESCRIPTOR_H_ */



