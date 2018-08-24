#include "MemoryCheck.h"
#include "Utility.h"

txSet<void*> MemoryCheck::mUsedPtrs;
ThreadLock MemoryCheck::mLock;

void MemoryCheck::usePtr(void* ptr)
{
	LOCK(mLock);
	if (!mUsedPtrs.insert(ptr).second)
	{
		LOG_ERROR("error : ptr is in use!");
	}
	UNLOCK(mLock);
}

void MemoryCheck::unusePtr(void* ptr)
{
	LOCK(mLock);
	if (!mUsedPtrs.tryErase(ptr))
	{
		LOG_ERROR("error : not find ptr! can not unuse it!");
	}
	UNLOCK(mLock);
}

bool MemoryCheck::canAccess(void* ptr)
{
	bool ret = false;
	LOCK(mLock);
	ret = mUsedPtrs.contains(ptr);
	UNLOCK(mLock);
	return ret;
}