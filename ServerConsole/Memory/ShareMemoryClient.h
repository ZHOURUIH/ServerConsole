#ifndef _SHARE_MEMORY_CLIENT_H_
#define _SHARE_MEMORY_CLIENT_H_

#include <aclapi.h>

// 以下為錯誤碼定義
#define ERROR_INVALID_CMDCODE 0xE00001FF  // 已經存在完全一樣的共享內存
#define ERROR_NO_MAPFILE 0xE00002FF  // 未分配共享內存文件
#define ERROR_INVALID_CFMCODE 0xE00003FF  // 校驗碼不匹配

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
	int mCmd; // 命令码
	int mDataSize;	// 数据区长度
	char mFlag;		// 数据区状态,0表示无状态,1表示客户端数据已写入,2表示客户端已读取数据,3表示服务器已写入,4表示服务器已读取
	char mUserData[mUserDataLength];	// 64个字节的自定义数据
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
// 類定義，共享內存客戶端
class ShareMemoryClient
{
public:
	ShareMemoryClient();
	virtual ~ShareMemoryClient();
	bool Open(char* szMapName, int dwSize);      // 打開一個內存文件
	void* getMappingBuffer() { return m_lpFileMapBuffer; }       // 獲取當前內存文件的指針
	void Destory();        // 關閉當前對內存文件的訪問
	int getMappingSize() {return mMappingSize;}
	int GetCmdDataSize();  // 讀取命令數據大小,小于0表示出现错误,大于等于0表示获取成功
	// 返回值为读取的数据长度,小于0表示读取错误,文件头需要填写命令码,pOutBuf的长度
	int ReadCmdData(DATA_HEADER& header, void* pOutBuf);
	// 需要填写文件头中的命令码,数据长度
	bool WriteCmdData(const DATA_HEADER& header, const void* pBuf);
protected:
	void _Init();    // 初始化參數
protected:
	HANDLE m_hFileMap;   // 內存文件句柄
	void* m_lpFileMapBuffer; // 內存文件指針
	char *m_pMapName;  // 內存文件名
	bool m_bOpenFlag;  // 是否已經打開了一個內存文件
	int m_dwLastError;  // 錯誤代碼
	int mMappingSize;	// 共享内存区大小
};

#endif