/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-03 13:58:35 +0900 (Tue, 03 Jun 2014) $
 * $LastChangedRevision: 837 $
 * Description:
 * Note:
 *****************************************************************************/

#if !defined(_SI_VECTOR_)
#define _SI_VECTOR_

#include <vector>
#include <pthread.h>

template <class OBJ> class SIVector
{

public:
	
	SIVector() {
		mItemCount=0;
		pthread_mutexattr_init(&mAttr);
		pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mSIVectorMutex, &mAttr);
	}

	virtual ~SIVector(){
	    clear();
		pthread_mutex_destroy(&mSIVectorMutex);
		};
	
	void push_back(OBJ msg)
	{
		lock();
		mSIVector.push_back(msg);
		mItemCount = mSIVector.size();
		unlock();
	};

	OBJ At(int idx, int& result)
	{
		lock();
		OBJ retval;
		if(mSIVector.size()<=idx || idx<0)
		{
			result = 0;
		}
		else
		{
			result = 1;
			retval = mSIVector.at(idx);
		}				
		unlock();
		return retval;
	};
	
	OBJ AtNoLock(int idx, int& result)
	{
		OBJ retval;
		if(mSIVector.size()<=idx || idx<0)
		{
			result = 0;
		}
		else
		{
			result = 1;
			retval = mSIVector.at(idx);
		}			
		return retval;
	};
	
	int size()
	{
		int ret=0;
		lock();
		ret = mItemCount;
		unlock();
		return ret;
	};
	
	int sizeNoLock()
	{
		return mItemCount;
	};
	
	bool erase(OBJ item)
	{
		lock();
		bool bFind = false;
		int findidx;
		int sizeN = mItemCount;
		for(int i=0;i<sizeN; i++)
		{
			OBJ itm = mSIVector.at(i);
			if(itm == item)
			{
				bFind = true;
				findidx = i;
				break;
			}
		}
		if(bFind)
		{
			mSIVector.erase((mSIVector.begin()+findidx));
		}
		mItemCount = mSIVector.size();
		unlock();
		return bFind;
	};
	
	void eraseNoLock(OBJ item)
	{
		bool bFind = false;
		int findidx;
		int sizeN = mItemCount;
		for(int i=0;i<sizeN; i++)
		{
			OBJ itm = mSIVector.at(i);
			if(itm == item)
			{
				bFind = true;
				findidx = i;
				break;
			}
		}
		if(bFind)
		{
			mSIVector.erase((mSIVector.begin()+findidx));
		}
		mItemCount = mSIVector.size();
	};
	
	void clear()
	{
		lock();
		mSIVector.clear();
		mItemCount=0;		
		unlock();
	};
	
	void clearNoLock()
	{
		mSIVector.clear();
		mItemCount=0;		
	};
	
	void lock()
	{
		pthread_mutex_lock(&mSIVectorMutex);
	};
	void unlock()
	{
		pthread_mutex_unlock(&mSIVectorMutex);
	};
	
private:
	int mItemCount;
	std::vector<OBJ> mSIVector;
	pthread_mutex_t mSIVectorMutex;
	pthread_mutexattr_t mAttr;
	
};
#endif // !defined(_SI_VECTOR_)
