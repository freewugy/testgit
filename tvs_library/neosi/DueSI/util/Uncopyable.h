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
#ifndef UNCOPYABLE_H_
#define UNCOPYABLE_H_



////////////////////////////////////////////////////////////////////////////////
#ifdef DOXYGEN_LANG_KOREAN
/**
 * @file	Uncopyable.h
 * @brief	Uncopyable header
 */
#else
/**
 * @file	Uncopyable.h
 * @brief	Uncopyable header
 */
#endif // DOXYGEN_LANG_KOREAN



////////////////////////////////////////////////////////////////////////////////
/**
 * usage

	class SomeClass : private Uncopyable
	{
		...
	};

 */



class Uncopyable
{
protected:
	// allow create and destroy to derived object.
	Uncopyable(){};
	~Uncopyable(){};

	/**
	 * @brief disallow copy construction.
	 *
	 * DO NOT IMPLEMENTS.
	 */
	//Uncopyable(const Uncopyable&);

	/**
	 * @brief disallow copy-assignment operator.
	 *
	 * DO NOT IMPLEMENTS.
	 */
	Uncopyable& operator=(const Uncopyable&);

private:

};



////////////////////////////////////////////////////////////////////////////////



#endif /* UNCOPYABLE_H_ */



