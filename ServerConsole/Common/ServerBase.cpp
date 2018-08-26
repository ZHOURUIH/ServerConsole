#include "ServerBase.h"
#include "ConsoleFramework.h"

ConsoleFramework* ServerBase::mConsoleFramework = NULL;
ServerConfig* ServerBase::mServerConfig = NULL;
txCommandSystem* ServerBase::mCommandSystem = NULL;
GameLog* ServerBase::mGameLog = NULL;
Console* ServerBase::mConsole = NULL;

void ServerBase::notifyConstructDone()
{
	mConsoleFramework = ConsoleFramework::getSingletonPtr();
	mServerConfig = mConsoleFramework->GET_SYSTEM(ServerConfig);
	mCommandSystem = mConsoleFramework->GET_SYSTEM(txCommandSystem);
	mGameLog = mConsoleFramework->GET_SYSTEM(GameLog);
	mConsole = mConsoleFramework->GET_SYSTEM(Console);
}

void ServerBase::notifyComponentDeconstruct()
{
	// 重新再获取一下所有组件
	notifyConstructDone();
}
