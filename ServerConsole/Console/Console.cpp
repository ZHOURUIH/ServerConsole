#include "Console.h"
#include "CustomThread.h"
#include "txMemoryTrace.h"
#include "ShareMemoryClient.h"
#include "StreamBuffer.h"
#include "Utility.h"

ShareMemoryClient* Console::mShareMemoryClient = NULL;

Console::Console(const std::string& name)
	:FrameComponent(name)
{
	mConsoleThread = TRACE_NEW(CustomThread, mConsoleThread, "ConsoleThread");
	mReceiveThread = TRACE_NEW(CustomThread, mReceiveThread, "ReceiveThread");
	//mReceiveBuffer = TRACE_NEW(StreamBuffer, mReceiveBuffer, 1024 * 1024 * 10);
	mShareMemoryClient = TRACE_NEW(ShareMemoryClient, mShareMemoryClient);
}

Console::~Console()
{
	TRACE_DELETE(mConsoleThread);
	TRACE_DELETE(mReceiveThread);
	TRACE_DELETE(mShareMemoryClient);
}

void Console::init()
{
	mShareMemoryClient->Open();
	mConsoleThread->start(consoleThread, this);
}

bool Console::consoleThread(void* args)
{
	std::cout << "输入命令:";
	std::string cmd;
	std::cin >> cmd;
	parseCmd(cmd);
	std::cout << "输入的命令为 : " << cmd << ",正在执行命令,稍后查看输出文件..." << std::endl;
	return true;
}

bool Console::receiveThread(void* args)
{
	Console* console = (Console*)(args);
	DWORD filesize = 0;
	mShareMemoryClient->GetCmdDataSize(&filesize);
	if (filesize > 0)
	{
		char* buffer = TRACE_NEW_ARRAY(char, filesize, buffer);
		mShareMemoryClient->ReadCmdData(CMD_CODE, filesize, buffer);
		// 将数据写入文件
		FileUtility::writeFile("test", buffer, filesize);
		TRACE_DELETE_ARRAY(buffer);
	}
	return true;
}

void Console::parseCmd(const std::string& cmdStr)
{
	if (cmdStr == "show room list")
	{
		char* buffer = TRACE_NEW_ARRAY(char, 256, buffer);
		memcpy(buffer, cmdStr.c_str(), cmdStr.length());
		mShareMemoryClient->WriteCmdData(0, CMD_CODE, 256, buffer);
	}
}