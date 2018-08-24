/******************************************************************
++  File Name :   FFMClass.h
++  Description: ����ȴ��
---------------------------------------------------------------
++  Author:  Fei ZhaoDong
++  Create time: 2004/3/25 ���� 10:00:00
++  Version:     1.1
++  Modifier:
++  Activities:
++  Update List: 2004/3/30 ���� 02:59:45
*******************************************************************/
// FFMClass.h: interface for the CFFMClass.
//
//////////////////////////////////////////////////////////////////////
#ifndef _SHARE_MEMORY_CLIENT_H_
#define _SHARE_MEMORY_CLIENT_H_
//#include <Winbase.h>
#include <aclapi.h>

//////////////////////////////////////////////////////////////////////
// Macro definition
// ���鹲��ȴ涨�x
#define DEFAULT_FILENAME NULL    // Ĭ�J���ļ���
#define DEFAULT_MAPNAME  "Local//_MAP_"   // Ĭ�J�Ĺ���ȴ���
#define DEFAULT_MAPSIZE  (0xFFFF + 1) * 100  // Ĭ�J�Ĺ���ȴ��С
const DWORD NETRGUSER_CFM_CODE = 0x1211DBFF; // У�a, ��������
const DWORD NETRGUSER_CMD_NONE = 0;   // ��ʼ��ָ��a, �oָ��
// �����e�`�a���x
#define ERROR_LEN    256    // �e�`�����L��
#define ERROR_INVALID_CMDCODE 0xE00001FF  // �ѽ�������ȫһ�ӵĹ���ȴ�
#define ERROR_NO_MAPFILE 0xE00002FF  // δ���乲��ȴ��ļ�
#define ERROR_INVALID_CFMCODE 0xE00003FF  // У�a��ƥ��
//////////////////////////////////////////////////////////////////////
// �ȴ��ļ���ʽ���x
#pragma pack(1)
// ��춴惦������ăȴ��ļ���ʽ
typedef struct _tagDATA_HEADER
{
	DWORD dwConfirmCode; // У�a
	DWORD nCommandCode;  // ָ���R�e�a
	DWORD dwDataSize;  // �����Ĵ�С
	BYTE  dwReserved[19]; // ����
	BYTE  bInfo[1];   // ������ʼ��ַ
	_tagDATA_HEADER()
	{
		dwConfirmCode = NETRGUSER_CFM_CODE;
		nCommandCode = NETRGUSER_CMD_NONE;
		dwDataSize = 0;
		ZeroMemory(dwReserved, 19);
		ZeroMemory(bInfo, 1);
	}
}DATA_HEADER, *LPDATA_HEADER;
typedef DWORD(WINAPI *PSetEntriesInAcl)(ULONG, PEXPLICIT_ACCESS, PACL, PACL*);
// ��춴惦���𔵓��Ĺ���ȴ��ļ���ʽ (������)
typedef struct _tagANS_HEADER
{
}ANS_HEADER, *LPANS_HEADER;

#pragma pack()

//////////////////////////////////////////////////////////////////////
// ��x������ȴ�͑���
class ShareMemoryClient
{
public:
	ShareMemoryClient();
	virtual ~ShareMemoryClient();
	ShareMemoryClient(DWORD dwAccess, char *szMapName, DWORD dwSize);
	BOOL Open(DWORD dwAccess = FILE_MAP_READ | FILE_MAP_WRITE, char *szMapName = DEFAULT_MAPNAME, DWORD dwSize = 0);      // ���_һ���ȴ��ļ�
	LPVOID GetBuffer();       // �@ȡ��ǰ�ȴ��ļ���ָ�
	void Destory();        // �P�]��ǰ���ȴ��ļ����L��
	DWORD GetSize();      // �@ȡ�ȴ��ļ���С
	BOOL GetCmdDataSize(DWORD *pDataSize);  // �xȡ�������С
	BOOL ReadCmdData(DWORD dwCommandCode, DWORD dwBufSize, LPVOID pOutBuf); // �xȡ�����
	BOOL WriteCmdData(DWORD memSize, DWORD nCommandCode, DWORD dwDataSize, const LPVOID pBuf); // ���������
protected:
	void _Init();    // ��ʼ������
protected:
	HANDLE m_hFileMap;   // �ȴ��ļ����
	LPVOID m_lpFileMapBuffer; // �ȴ��ļ�ָ�
	char *m_pMapName;  // �ȴ��ļ���
	DWORD m_dwSize;   // ���n�^��С
	BOOL m_bOpenFlag;  // �Ƿ��ѽ����_��һ���ȴ��ļ�
	DWORD m_dwLastError;  // �e�`���a
};

#endif