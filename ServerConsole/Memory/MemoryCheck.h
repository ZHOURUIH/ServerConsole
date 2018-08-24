#ifndef _MEMORY_CHECK_H_
#define _MEMORY_CHECK_H_

#include "ThreadLock.h"

class MemoryCheck
{
public:
	static void usePtr(void* ptr);
	static void unusePtr(void* ptr);
	static bool canAccess(void* ptr);
protected:
	static txSet<void*> mUsedPtrs;
	static ThreadLock mLock;
};

#endif