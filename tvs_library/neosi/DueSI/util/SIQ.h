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

#if !defined(_SI_QUEUE_)
#define _SI_QUEUE_

#include <queue>
#include <pthread.h>

template <class OBJ> class SIQ
{

public:
	
	SIQ() {
		mItemCount=0;
		pthread_mutex_init(&mSIQMutex, NULL);
	}

	///?�멸??
	virtual ~SIQ(){
		pthread_mutex_destroy(&mSIQMutex);
		};
	
	void enqueue(OBJ msg)
	{
		lock();
		mSIQueue.push(msg);
		mItemCount++;
		unlock();
	};

	OBJ dequeue(int& result)
	{
		lock();
		OBJ retval;
		if(mSIQueue.empty())
		{
			result = 0;
			mItemCount = 0;
		}
		else
		{
			result = 1;
			retval = mSIQueue.front();
			mSIQueue.pop();
			
			mItemCount--;
			if(mItemCount<=0)
				mItemCount=0;
		}				
		unlock();
		return retval;
	};
	
	OBJ dequeueNoLock(int& result)
	{
		OBJ retval;
		if(mSIQueue.empty())
		{
			result = 0;
			mItemCount = 0;
		}
		else
		{
			result = 1;
			retval = mSIQueue.front();
			mSIQueue.pop();
			
			mItemCount--;
			if(mItemCount<=0)
				mItemCount=0;
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
		int ret=0;
		ret = mItemCount;
		return ret;
	};
	
	void clear()
	{
		lock();
		while(!mSIQueue.empty())
		{
			mSIQueue.pop();
		}
		mItemCount=0;		
		unlock();
	};
	
	void lock()
	{
		pthread_mutex_lock(&mSIQMutex);
	};
	void unlock()
	{
		pthread_mutex_unlock(&mSIQMutex);
	};
	
private:
	int mItemCount;
	std::queue<OBJ> mSIQueue;
	pthread_mutex_t mSIQMutex;
	
};
#endif // !defined(_SI_QUEUE_)
