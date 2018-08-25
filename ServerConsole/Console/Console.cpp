#include "Console.h"
#include "CustomThread.h"
#include "txMemoryTrace.h"
#include "ShareMemoryClient.h"
#include "StreamBuffer.h"
#include "Utility.h"

ShareMemoryClient* Console::mShareMemoryClient = NULL;
std::string Console::mOutputPath = "Output/";

Console::Console(const std::string& name)
	:FrameComponent(name)
{
	mConsoleThread = TRACE_NEW(CustomThread, mConsoleThread, "ConsoleThread");
	mReceiveThread = TRACE_NEW(CustomThread, mReceiveThread, "ReceiveThread");
	mShareMemoryClient = TRACE_NEW(ShareMemoryClient, mShareMemoryClient);
	mBufferSize = 1024 * 1024 * 2;
	mBuffer = TRACE_NEW_ARRAY(char, mBufferSize, mBuffer);
}

Console::~Console()
{
	TRACE_DELETE(mConsoleThread);
	TRACE_DELETE(mReceiveThread);
	TRACE_DELETE(mShareMemoryClient);
}

void Console::init()
{
	mShareMemoryClient->Open("DebugSystem", mBufferSize + sizeof(DATA_HEADER));
	mConsoleThread->start(consoleThread, this);
	mReceiveThread->start(receiveThread, this);
}

bool Console::consoleThread(void* args)
{
	std::cout << "输入命令:";
	std::string cmd;
	std::cin >> cmd;
	std::cout << "输入的命令为 : " << cmd << ",正在执行命令,稍后查看输出文件..." << std::endl;
	DATA_HEADER writeHeader;
	writeHeader.mCmd = DEBUG_SYSTEM_CMD;
	writeHeader.mDataSize = cmd.length() + 1;
	mShareMemoryClient->WriteCmdData(writeHeader, cmd.c_str());
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