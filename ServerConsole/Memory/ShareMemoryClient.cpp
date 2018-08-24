/******************************************************************
++  File Name : FFMClass.cpp
++  Description: ����ȴ��
---------------------------------------------------------------
++  Author:  Fei ZhaoDong
++  Create time: 2004/3/25 ���� 10:00:00
++  Version:     1.0
++  Modifier:
++   Activities:
++  Update List: 2004/3/29 ���� 02:59:45
*******************************************************************/

#include "ShareMemoryClient.h"

ShareMemoryClient::ShareMemoryClient()
{
	m_dwLastError = 0;
	_Init();
}
ShareMemoryClient::~ShareMemoryClient()
{
	Destory();
}
ShareMemoryClient::ShareMemoryClient(DWORD dwAccess, char *szMapName, DWORD dwSize)
{
	_Init();
	if (!Open(dwAccess, szMapName, dwSize))
	{
		Destory();
	}
}
// ��ʼ������
void ShareMemoryClient::_Init()
{
	m_hFileMap = NULL;
	m_lpFileMapBuffer = NULL;
	m_pMapName = NULL;
	m_bOpenFlag = FALSE;
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
	if (m_pMapName)
	{
		free(m_pMapName);
		m_pMapName = NULL;
	}
	_Init();
}
// ���_һ���ȴ��ļ�
BOOL ShareMemoryClient::Open(DWORD dwAccess, char *szMapName, DWORD dwSize)
{
	if (m_bOpenFlag)
	{
		Destory();
	}
	if (szMapName != NULL)
	{
		m_pMapName = _strdup(szMapName);
	}
	else
	{
		m_pMapName = _strdup(DEFAULT_MAPNAME);
	}
	m_hFileMap = OpenFileMappingA(dwAccess, TRUE, m_pMapName);
	if (NULL == m_hFileMap)
	{
		m_dwLastError = GetLastError();
		Destory();
		return FALSE;
	}
	m_lpFileMapBuffer = MapViewOfFile(m_hFileMap, dwAccess, 0, 0, dwSize);
	if (NULL == m_lpFileMapBuffer)
	{
		m_dwLastError = GetLastError();
		Destory();
		return FALSE;
	}
	m_bOpenFlag = TRUE;
	return TRUE;
}
// �@ȡ��ǰ�ȴ��ļ���ָ�
LPVOID ShareMemoryClient::GetBuffer()
{
	return (m_lpFileMapBuffer) ? (m_lpFileMapBuffer) : (NULL);
}
// �xȡ�������С
BOOL ShareMemoryClient::GetCmdDataSize(DWORD *pDataSize)
{
	*pDataSize = 0;
	LPDATA_HEADER pHeader = (LPDATA_HEADER)GetBuffer();
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	*pDataSize = pHeader->dwDataSize;
	return TRUE;
}
// �xȡ�����
BOOL ShareMemoryClient::ReadCmdData(DWORD dwCommandCode, DWORD dwBufSize, LPVOID pOutBuf)
{
	LPDATA_HEADER pHeader = (LPDATA_HEADER)GetBuffer();
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (pHeader->dwDataSize > dwBufSize)
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	if (pHeader->nCommandCode != dwCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	ZeroMemory(pOutBuf, dwBufSize);
	// ��ؐ���������n�^
	memcpy(pOutBuf, pHeader->bInfo, pHeader->dwDataSize);
	return TRUE;
}
BOOL ShareMemoryClient::WriteCmdData(DWORD memSize, DWORD nCommandCode, DWORD dwDataSize, const LPVOID pBuf)
{
	if (!memSize)
		memSize = DEFAULT_MAPSIZE;
	m_dwSize = memSize;
	// �z򞔵���ĺ�����
	if (NULL == GetBuffer())
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return FALSE;
	}
	if (NETRGUSER_CMD_NONE == nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return FALSE;
	}
	if (dwDataSize > 0 && pBuf == NULL)
	{
		m_dwLastError = ERROR_INVALID_USER_BUFFER;
		SetLastError(ERROR_INVALID_USER_BUFFER);
		return FALSE;
	}
	if (dwDataSize + sizeof(DATA_HEADER) > GetSize())
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}
	// ������Y��
	// �ļ��^
	DATA_HEADER dataHeader;
	dataHeader.nCommandCode = nCommandCode;
	dataHeader.dwDataSize = dwDataSize;
	ZeroMemory(GetBuffer(), GetSize());
	memcpy(GetBuffer(), &dataHeader, sizeof(DATA_HEADER));
	// �����K
	LPDATA_HEADER pData = (LPDATA_HEADER)GetBuffer();
	memcpy(pData->bInfo, pBuf, dwDataSize);
	return TRUE;
}
// �@ȡ�ȴ��ļ���С
DWORD ShareMemoryClient::GetSize()
{
	return m_dwSize;
}
