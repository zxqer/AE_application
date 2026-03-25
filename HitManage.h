#pragma once

typedef struct tagHIT_ITEM_HEAD
{
	int   iChannelNo;   //撞击通道号
	int     iIndexNo;        //撞击序号
	double  douTot;       //撞击偏移时间
	int    iHitLength;    //撞击数据长度
	DWORD   dwFileOffset;    //文件偏移
}HIT_ITEM_HEAD,*HIT_ITEM_HEADP;


class CHitManage
{
public:
	CHitManage();
	~CHitManage();

	CArray<HIT_ITEM_HEAD, HIT_ITEM_HEAD&> m_HitArray;
	CFile mSaveFile,mIndexFile;
	DWORD  mSaveFileLen;   //保存文件长度
	int  mIndexArray[MAX_CHANNELS];
	int  mPlay_Flag;
	HANDLE   mMutex;
	HANDLE   m_GlobalMem;
	double    *m_pduBuf;
	DWORD    mStartBufPos;  //内存起始文件位置
	DWORD  mUsedBufLen;   //内存使用长度
	char datFileName[256];
	char dexFileName[256];


	void   StartPlay();
	void   StopPlay();
	void   LockBuf();
	void   UnlockBuf();
	void   ClearPlayBuf();
	void   CloseOpenDataFile();
	void   AddHitItem(int ChanNo,double inTot,int inHitLength,double * inAmpBuf);
	int    FindHitIndexByTot(int ChanNo, double inTot);
	void   OpenHistoryName(CString strFileName);
	HIT_ITEM_HEAD  GetHitItem(int ChanNo, int HitIndex,double * outAmpBuf);
	void SaveDataToTxt(CStdioFile *pTxtOutFile);
};

