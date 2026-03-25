#pragma once

#include "AESWin.h"

class CRawSignalBuf
{
public:
	CRawSignalBuf();
	~CRawSignalBuf();

	HANDLE   mMutex[MAX_CHANNELS];
	HANDLE   m_GlobalMem[MAX_CHANNELS];
	double    *m_pdwBuf[MAX_CHANNELS];
	DWORD    m_dwBufUsedLen[MAX_CHANNELS];
	int mUeseChannelNums;

	void   UpdateArtPara();
	int  GetIfHaveData(int ChNo, int needDots);

	int AddLineDot(int ChNo, int bufLen, double * inBufP);
	void GetLineDot(int ChNo, int dotsum, double * pDwBuf);

	void Clear_Buf();
	void  LockBuf(int ChNo);
	void  UnlockBuf(int ChNo);
};

