#ifndef _SERVER_BASE_H_
#define _SERVER_BASE_H_

class ConsoleFramework;
class txCommandSystem;
class GameLog;
class ServerConfig;
class Console;
class ServerBase
{
public:
	static void notifyConstructDone();
public:
	static ConsoleFramework* mConsoleFramework;
	static ServerConfig* mServerConfig;
	static txCommandSystem* mCommandSystem;
	static GameLog* mGameLog;
	static Console* mConsole;
};

#endif
