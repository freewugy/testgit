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

#include "DVB_Parental_Rating_Descriptor.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ParentalRating::ParentalRating(const uint8_t * const buffer)
{
    m_country_code.assign((char *)&buffer[0], 3);
    m_rating = buffer[3];
}

const std::string &ParentalRating::getCountryCode(void) const
{
    return m_country_code;
}

uint8_t ParentalRating::getRating(void) const
{
    return m_rating;
}

DVB_Parental_Rating_Descriptor::DVB_Parental_Rating_Descriptor(const uint8_t * const buffer) : Descriptor(buffer)
{
    for (size_t i = 0; i < mLength; i += 4) {
        ASSERT_MIN_DLEN(i + 4);
        m_parental_ratings.push_back(new ParentalRating(&buffer[i + 2]));
    }
}

DVB_Parental_Rating_Descriptor::~DVB_Parental_Rating_Descriptor(void)
{
    for (ParentalRatingIterator i = m_parental_ratings.begin(); i != m_parental_ratings.end(); ++i)
        delete *i;
}

const ParentalRatingList *DVB_Parental_Rating_Descriptor::getParentalRatings(void) const
{
    return &m_parental_ratings;
}

////////////////////////////////////////////////////////////////////////////////

