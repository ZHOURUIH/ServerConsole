#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "FrameComponent.h"
#include "ThreadLock.h"

const int CMD_CODE = 2;

class CustomThread;
class ShareMemoryClient;
class StreamBuffer;
class Console : public FrameComponent
{
public:
	Console(const std::string& name);
	virtual ~Console();
	virtual void init();
	virtual void update() {}
protected:
	static bool consoleThread(void* args);
	static bool receiveThread(void* args);
	static void parseCmd(const std::string& cmdStr);
protected:
	CustomThread* mConsoleThread;
	CustomThread* mReceiveThread;
	//StreamBuffer* mReceiveBuffer;
	//ThreadLock mReceiveLock;
	static ShareMemoryClient* mShareMemoryClient;
};

#endif