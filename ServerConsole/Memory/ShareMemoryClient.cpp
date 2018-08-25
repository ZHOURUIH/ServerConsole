#include "ShareMemoryClient.h"

ShareMemoryClient::ShareMemoryClient()
{
	m_dwLastError = 0;
	mMappingSize = 0;
	_Init();
}
ShareMemoryClient::~ShareMemoryClient()
{
	Destory();
}
// 初始化參數
void ShareMemoryClient::_Init()
{
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_pMapName = NULL;
	m_bOpenFlag = FALSE;
	mMappingSize = 0;
}
// 關閉當前對內存文件的訪問
void ShareMemoryClient::Destory()
{
	if (m_lpFileMapBuffer)
	{
		UnmapViewOfFile(m_lpFileMapBuffer);
		m_lpFileMapBuffer = NULL;
	}
	if (m_hFileMap)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
}
// 打開一個內存文件
bool ShareMemoryClient::Open(char *szMapName, int dwSize)
{
	if (m_bOpenFlag)
	{
		Destory();
	}
	m_pMapName = _strdup(szMapName);
	DWORD dwAccess = FILE_MAP_READ | FILE_MAP_WRITE;
	m_hFileMap = OpenFileMappingA(dwAccess, TRUE, m_pMapName);
	if (m_hFileMap == NULL)
	{
		m_dwLastError = GetLastError();
		Destory();
		return false;
	}
	mMappingSize = dwSize;
	m_lpFileMapBuffer = MapViewOfFile(m_hFileMap, dwAccess, 0, 0, mMappingSize);
	if (m_lpFileMapBuffer == NULL)
	{
		m_dwLastError = GetLastError();
		Destory();
		return false;
	}
	m_bOpenFlag = true;
	return true;
}
// 讀取命令數據大小
int ShareMemoryClient::GetCmdDataSize()
{
	DATA_HEADER* pHeader = (DATA_HEADER*)getMappingBuffer();
	if (pHeader == NULL)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(m_dwLastError);
		return -1;
	}
	if (pHeader->mCmd == 0)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(m_dwLastError);
		return -1;
	}
	return pHeader->mDataSize;
}
// 讀取命令數據
int ShareMemoryClient::ReadCmdData(DATA_HEADER& header, void* pOutBuf)
{
	DATA_HEADER* pHeader = (DATA_HEADER*)getMappingBuffer();
	if (pHeader == NULL)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(m_dwLastError);
		return -1;
	}
	// 如果当前状态不是客户端已写入,则表示没有数据可以读取
	if (pHeader->mFlag != MS_SERVER_WRITE)
	{
		return 0;
	}
	if (pHeader->mCmd == 0)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(m_dwLastError);
		return -1;
	}
	if (pHeader->mDataSize > header.mDataSize)
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(m_dwLastError);
		return -1;
	}
	if (pHeader->mCmd != header.mCmd)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(m_dwLastError);
		return -1;
	}
	ZeroMemory(pOutBuf, header.mDataSize);
	// 拷貝數據到緩衝區
	memcpy(&header, pHeader, sizeof(DATA_HEADER));
	memcpy(pOutBuf, (char*)pHeader + sizeof(DATA_HEADER), pHeader->mDataSize);
	// 设置为服务器已读取
	pHeader->mFlag = MS_CLIENT_READ;
	return pHeader->mDataSize;
}
bool ShareMemoryClient::WriteCmdData(const DATA_HEADER& header, const void* pBuf)
{
	// 檢驗數據的合理性
	if (getMappingBuffer() == 0)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(m_dwLastError);
		return false;
	}
	if (header.mCmd == 0)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(m_dwLastError);
		return false;
	}
	if (header.mDataSize > 0 && pBuf == NULL)
	{
		m_dwLastError = ERROR_INVALID_USER_BUFFER;
		SetLastError(m_dwLastError);
		return false;
	}
	if (header.mDataSize + sizeof(DATA_HEADER) > getMappingSize())
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(m_dwLastError);
		return false;
	}
	// 填寫數據結構
	ZeroMemory(getMappingBuffer(), getMappingSize());
	memcpy(getMappingBuffer(), &header, sizeof(DATA_HEADER));
	// 數據塊
	DATA_HEADER* pData = (DATA_HEADER*)getMappingBuffer();
	memcpy((char*)pData + sizeof(DATA_HEADER), pBuf, header.mDataSize);
	// 设置为服务器已写入
	pData->mFlag = MS_CLIENT_WRITE;
	return true;
}