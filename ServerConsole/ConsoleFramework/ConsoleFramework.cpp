#include "Utility.h"
#include "ConsoleFramework.h"
#include "ServerBase.h"

template<>ConsoleFramework *txSingleton<ConsoleFramework>::ms_Singleton = 0;

ConsoleFramework::ConsoleFramework()
	:
	mStop(false)
{
#if RUN_PLATFORM == PLATFORM_LINUX
	struct timeval startTime;
	gettimeofday(&startTime, NULL);
	mStartMiliTime = startTime.tv_sec * 1000 + startTime.tv_usec / 1000;
#endif
	REGISTE_FRAME_COMPONENT(ServerConfig);
	REGISTE_FRAME_COMPONENT(GameLog);
#ifdef TRACE_MEMORY
	REGISTE_FRAME_COMPONENT(txMemoryTrace);
#endif
	REGISTE_FRAME_COMPONENT(txCommandSystem);
	REGISTE_FRAME_COMPONENT(Console);
	ServerBase::notifyConstructDone();
}

ConsoleFramework::~ConsoleFramework()
{
	destroy();
}

bool ConsoleFramework::init()
{
	// 初始化所有组件
	int count = mFrameComponentVector.size();
	FOR_STL(mFrameComponentVector, int i = 0; i < count; ++i)
	{
		mFrameComponentVector[i]->init();
	}
	END_FOR_STL(mFrameComponentVector);
	return true;
}

void ConsoleFramework::update(float elapsedTime)
{
	LOCK(mLock);
	int count = mFrameComponentVector.size();
	FOR_STL(mFrameComponentVector, int i = 0; i < count; ++i)
	{
		mFrameComponentVector[i]->update(elapsedTime);
	}
	END_FOR_STL(mFrameComponentVector);
	UNLOCK(mLock);
}

void ConsoleFramework::destroy()
{
	LOCK(mLock);
	int count = mFrameComponentVector.size();
	FOR_STL(mFrameComponentVector, int i = 0; i < count; ++i)
	{
		// 销毁顺序与初始化顺序相反
		std::string componentName = mFrameComponentVector[count - 1 - i]->getName();
		TRACE_DELETE(mFrameComponentVector[count - 1 - i]);
		mFrameComponentMap[componentName] = NULL;
		ServerBase::notifyComponentDeconstruct();
	}
	END_FOR_STL(mFrameComponentVector);
	mFrameComponentVector.clear();
	mFrameComponentMap.clear();
	UNLOCK(mLock);
}