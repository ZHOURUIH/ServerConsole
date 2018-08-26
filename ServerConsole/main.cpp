#include "ServerDefine.h"
#include "ConsoleFramework.h"
#include "Utility.h"
#include "TimeLock.h"

#if RUN_PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#endif

BOOL ConsoleEventHandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		ConsoleFramework* mConsoleFramework = ConsoleFramework::getSingletonPtr();
		if (mConsoleFramework != NULL)
		{
			mConsoleFramework->stop();
			mConsoleFramework->destroy();
		}
	}
	return FALSE;
}

int main()
{
	// 设置随机数种子
	srand((unsigned int)time(0));
	// 设置窗口事件回调
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, true);

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
