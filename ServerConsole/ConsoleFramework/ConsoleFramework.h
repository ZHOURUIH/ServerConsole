#ifndef _CONSOLE_FRAMEWORK_H_
#define _CONSOLE_FRAMEWORK_H_

#include "txSingleton.h"
#include "ServerConfig.h"
#include "txMemoryTrace.h"
#include "GameLog.h"
#include "txCommandSystem.h"
#include "Console.h"

#define GET_SYSTEM(type) getSystem<type>(TOSTRING(type))

#define REGISTE_FRAME_COMPONENT(type)								\
	{type* component = TRACE_NEW(type, component, TOSTRING(type));	\
	mFrameComponentVector.push_back(component);						\
	mFrameComponentMap.insert(TOSTRING(type), component);}

class FrameComponent;
class ConsoleFramework : public txSingleton<ConsoleFramework>
{
public:
	ConsoleFramework();
	virtual ~ConsoleFramework();
	bool init();
	void update(float elapsedTime);
	void destroy();
	bool isStop() { return mStop; }
	void stop() { mStop = true; }
	// 获得成员变量
#if RUN_PLATFORM == PLATFORM_LINUX
	const unsigned long& getStartMiliTime() { return mStartMiliTime; }
#endif
	template<typename T>
	T* getSystem(const std::string& name)
	{
		return static_cast<T*>(mFrameComponentMap.tryGet(name, NULL));
	}
protected:
#if RUN_PLATFORM == PLATFORM_LINUX
	unsigned long mStartMiliTime;
#endif
	std::atomic<bool> mStop;
	ThreadLock mLock;
	txVector<FrameComponent*> mFrameComponentVector;
	txMap<std::string, FrameComponent*> mFrameComponentMap;
};

#endif
