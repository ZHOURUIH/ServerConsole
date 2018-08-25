#ifndef _SHARE_MEMORY_CLIENT_H_
#define _SHARE_MEMORY_CLIENT_H_

#include <aclapi.h>

// �����e�`�a���x
#define ERROR_INVALID_CMDCODE 0xE00001FF  // �ѽ�������ȫһ�ӵĹ���ȴ�
#define ERROR_NO_MAPFILE 0xE00002FF  // δ���乲��ȴ��ļ�
#define ERROR_INVALID_CFMCODE 0xE00003FF  // У�a��ƥ��

enum MEMORY_STATE
{
	MS_NONE,
	MS_CLIENT_WRITE,
	MS_CLIENT_READ,
	MS_SERVER_WRITE,
	MS_SERVER_READ,
};

static const int mUserDataLength = 64;
#pragma pack(1)
struct DATA_HEADER
{
	int mCmd; // ������
	int mDataSize;	// ����������
	char mFlag;		// ������״̬,0��ʾ��״̬,1��ʾ�ͻ���������д��,2��ʾ�ͻ����Ѷ�ȡ����,3��ʾ��������д��,4��ʾ�������Ѷ�ȡ
	char mUserData[mUserDataLength];	// 64���ֽڵ��Զ�������
	DATA_HEADER()
	{
		mCmd = 0;
		mDataSize = 0;
		mFlag = MS_NONE;
		ZeroMemory(mUserData, mUserDataLength);
	}
};
typedef DWORD(WINAPI *PSetEntriesInAcl)(ULONG, PEXPLICIT_ACCESS, PACL, PACL*);

#pragma pack()

//////////////////////////////////////////////////////////////////////
// ��x������ȴ�͑���
class ShareMemoryClient
{
public:
	ShareMemoryClient();
	virtual ~ShareMemoryClient();
	bool Open(char* szMapName, int dwSize);      // ���_һ���ȴ��ļ�
	void* getMappingBuffer() { return m_lpFileMapBuffer; }       // �@ȡ��ǰ�ȴ��ļ���ָ�
	void Destory();        // �P�]��ǰ���ȴ��ļ����L��
	int getMappingSize() {return mMappingSize;}
	int GetCmdDataSize();  // �xȡ�������С,С��0��ʾ���ִ���,���ڵ���0��ʾ��ȡ�ɹ�
	// ����ֵΪ��ȡ�����ݳ���,С��0��ʾ��ȡ����,�ļ�ͷ��Ҫ��д������,pOutBuf�ĳ���
	int ReadCmdData(DATA_HEADER& header, void* pOutBuf);
	// ��Ҫ��д�ļ�ͷ�е�������,���ݳ���
	bool WriteCmdData(const DATA_HEADER& header, const void* pBuf);
protected:
	void _Init();    // ��ʼ������
protected:
	HANDLE m_hFileMap;   // �ȴ��ļ����
	void* m_lpFileMapBuffer; // �ȴ��ļ�ָ�
	char *m_pMapName;  // �ȴ��ļ���
	bool m_bOpenFlag;  // �Ƿ��ѽ����_��һ���ȴ��ļ�
	int m_dwLastError;  // �e�`���a
	int mMappingSize;	// �����ڴ�����С
};

#endif