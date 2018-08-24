#include "ServerDefine.h"
#include "ConsoleFramework.h"
#include "Utility.h"
#include "TimeLock.h"

#if RUN_PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#endif

int main()
{
	// 设置随机数种子
	srand((unsigned int)time(0));

	TimeLock timeLock(30);
	ConsoleFramework* mConsoleFramework = TRACE_NEW(ConsoleFramework, mConsoleFramework);
	mConsoleFramework->init();
	while (!mConsoleFramework->isStop())
	{
		long frameTime = timeLock.update();
		mConsoleFramework->update(frameTime / 1000.0f);
	}
	TRACE_DELETE(mConsoleFramework);
	return 0;
}
