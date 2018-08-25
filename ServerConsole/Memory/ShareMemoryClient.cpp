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
// ��ʼ������
void ShareMemoryClient::_Init()
{
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_pMapName = NULL;
	m_bOpenFlag = FALSE;
	mMappingSize = 0;
}
// �P�]��ǰ���ȴ��ļ����L��
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
// ���_һ���ȴ��ļ�
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
// �xȡ�������С
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
// �xȡ�����
int ShareMemoryClient::ReadCmdData(DATA_HEADER& header, void* pOutBuf)
{
	DATA_HEADER* pHeader = (DATA_HEADER*)getMappingBuffer();
	if (pHeader == NULL)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(m_dwLastError);
		return -1;
	}
	// �����ǰ״̬���ǿͻ�����д��,���ʾû�����ݿ��Զ�ȡ
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
	// ��ؐ���������n�^
	memcpy(&header, pHeader, sizeof(DATA_HEADER));
	memcpy(pOutBuf, (char*)pHeader + sizeof(DATA_HEADER), pHeader->mDataSize);
	// ����Ϊ�������Ѷ�ȡ
	pHeader->mFlag = MS_CLIENT_READ;
	return pHeader->mDataSize;
}
bool ShareMemoryClient::WriteCmdData(const DATA_HEADER& header, const void* pBuf)
{
	// �z򞔵���ĺ�����
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
	// ������Y��
	ZeroMemory(getMappingBuffer(), getMappingSize());
	memcpy(getMappingBuffer(), &header, sizeof(DATA_HEADER));
	// �����K
	DATA_HEADER* pData = (DATA_HEADER*)getMappingBuffer();
	memcpy((char*)pData + sizeof(DATA_HEADER), pBuf, header.mDataSize);
	// ����Ϊ��������д��
	pData->mFlag = MS_CLIENT_WRITE;
	return true;
}