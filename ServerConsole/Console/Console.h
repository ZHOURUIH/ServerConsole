#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "FrameComponent.h"
#include "ThreadLock.h"

const int DEBUG_SYSTEM_CMD = 2;
const std::string ROOM_LIST = "RoomList";
const std::string PLAYER_LIST = "PlayerList";

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
protected:
	CustomThread* mConsoleThread;
	CustomThread* mReceiveThread;
	static ShareMemoryClient* mShareMemoryClient;
	char* mBuffer;
	int mBufferSize;
	static std::string mOutputPath;
};

#endif