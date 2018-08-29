#include "Console.h"
#include "CustomThread.h"
#include "txMemoryTrace.h"
#include "ShareMemoryClient.h"
#include "StreamBuffer.h"
#include "Utility.h"
#include "ConsoleFramework.h"

ShareMemoryClient* Console::mShareMemoryClient = NULL;
std::string Console::mOutputPath = "ConsoleOutput/";

Console::Console(const std::string& name)
	:FrameComponent(name)
{
	mConsoleThread = TRACE_NEW(CustomThread, mConsoleThread, "ConsoleThread");
	mReceiveThread = TRACE_NEW(CustomThread, mReceiveThread, "ReceiveThread");
	mShareMemoryClient = TRACE_NEW(ShareMemoryClient, mShareMemoryClient);
	mBufferSize = 1024 * 1024 * 2;
	mBuffer = TRACE_NEW_ARRAY(char, mBufferSize, mBuffer);
	initCmdList();
}

Console::~Console()
{
	TRACE_DELETE(mConsoleThread);
	TRACE_DELETE(mReceiveThread);
	TRACE_DELETE(mShareMemoryClient);
}

void Console::init()
{
	bool ret = mShareMemoryClient->Open("DebugSystem", mBufferSize + sizeof(DATA_HEADER));
	if (!ret)
	{
		mConsoleFramework->stop();
		return;
	}
	mConsoleThread->start(consoleThread, this);
	mReceiveThread->start(receiveThread, this);
}

bool Console::consoleThread(void* args)
{
	Console* console = (Console*)args;
	SystemUtility::print("输入命令:");
	std::string cmd;
	SystemUtility::input(cmd);
	if (cmd == "ls")
	{
		int count = console->mCmdList.size();
		SystemUtility::print("命令数量 : " + StringUtility::intToString(count) + "\n");
		auto iter = console->mCmdList.begin();
		auto iterEnd = console->mCmdList.end();
		FOR_STL(console->mCmdList, ; iter != iterEnd; ++iter)
		{
			SystemUtility::print(*iter + "\n");
		}
		END_FOR_STL(console->mCmdList);
	}
	else if (cmd == "exit" || cmd == "quit")
	{
		mConsoleFramework->stop();
	}
	else
	{
		if (console->mCmdList.contains(cmd))
		{
			SystemUtility::print("输入的命令为 : " + cmd + ",正在执行命令,稍后查看输出文件...\n");
			DATA_HEADER writeHeader;
			writeHeader.mCmd = DEBUG_SYSTEM_CMD;
			writeHeader.mDataSize = cmd.length() + 1;
			mShareMemoryClient->WriteCmdData(writeHeader, cmd.c_str());
		}
		else
		{
			SystemUtility::print("命令不存在!\n");
		}
	}
	return true;
}

bool Console::receiveThread(void* args)
{
	Console* console = (Console*)(args);
	DATA_HEADER header;
	header.mCmd = DEBUG_SYSTEM_CMD;
	header.mDataSize = console->mBufferSize;
	int ret = mShareMemoryClient->ReadCmdData(header, console->mBuffer);
	if (ret < 0)
	{
		LOG_ERROR_DELAY("read share memory error : %d", GetLastError());
	}
	else if (ret > 0)
	{
		std::string filePath = SystemUtility::getAvailableResourcePath(mOutputPath + header.mUserData + ".txt");
		LOG_INFO_DELAY("已生成文件 : %s", filePath.c_str());
		FileUtility::writeFile(filePath, console->mBuffer);
	}
	return true;
}

void Console::initCmdList()
{
	mCmdList.insert(ROOM_LIST);
	mCmdList.insert(PLAYER_LIST);
	mCmdList.insert(MATCH_POOL);
}