#include "stdafx.h"
#include "AESWin.h"
#include "RawSignalBuf.h"
#include "LogMgn.h"

#define MEM_BUF_SIZE  16*1024*1024
#define MAX_MEM_DOTS   2*1024*1024

CRawSignalBuf::CRawSignalBuf()
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		m_dwBufUsedLen[i] = 0;
		mMutex[i] = CreateMutex(NULL, FALSE, NULL);
		m_GlobalMem[i] = NULL;
	}
}


CRawSignalBuf::~CRawSignalBuf()
{
	int i;
	for (i = 0; i < mUeseChannelNums; i++)
	{
		GlobalUnlock(m_GlobalMem[i]);
		GlobalFree(m_GlobalMem[i]);
	}
}

int CRawSignalBuf::AddLineDot(int ChNo, int bufLen, double * inBufP)
{
	int i;
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	CString strTemp;
	if (bufLen + m_dwBufUsedLen[ChNo] >= MAX_MEM_DOTS)
	{
		strTemp.Format("\r ÄÚ´æ¶ÓÁÐÂú: m_dwBufUsedLen=%u,bufLen=%d\r", m_dwBufUsedLen[ChNo], bufLen);
		pApp->m_LogMgnP->Addlog(strTemp);
		return 0;
	}
//	strTemp.Format("AddLine: ChNo=%d,bufLen=%d\r", ChNo, bufLen);
//	pApp->m_LogMgnP->Addlog(strTemp);
	for (i = 0; i < bufLen; i++)
	{
		*(m_pdwBuf[ChNo] + m_dwBufUsedLen[ChNo]) = *(inBufP + i);
		m_dwBufUsedLen[ChNo]++;
	}
	return 1;
}


void CRawSignalBuf::GetLineDot(int ChNo, int dotsum, double * pDwBuf)
{
	memcpy(pDwBuf, m_pdwBuf[ChNo], dotsum*sizeof(double));
	m_dwBufUsedLen[ChNo] -= dotsum;
	memmove(m_pdwBuf[ChNo], m_pdwBuf[ChNo] + dotsum, m_dwBufUsedLen[ChNo] * sizeof(double));
}


void  CRawSignalBuf::LockBuf(int ChNo)
{
	WaitForSingleObject(mMutex[ChNo], INFINITE);
}
void CRawSignalBuf::UnlockBuf(int ChNo)
{
	ReleaseMutex(mMutex[ChNo]);
}


void CRawSignalBuf::Clear_Buf()
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		m_dwBufUsedLen[i] = 0;
	}
}

int  CRawSignalBuf::GetIfHaveData(int ChNo, int needDots)
{
	if (m_dwBufUsedLen[ChNo] >= needDots)
		return 1;
	return 0;
}

void  CRawSignalBuf::UpdateArtPara()
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	mUeseChannelNums = 0;
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (pApp->m_arrayChannel_Item[i].iMonitor_Flag == 1)
		{
			mUeseChannelNums++;
		}
	}
	CString strTemp;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (m_GlobalMem[i] == NULL)
		{
			m_GlobalMem[i] = GlobalAlloc(GHND, MEM_BUF_SIZE);
			if (m_GlobalMem[i] == NULL)
			{
				strTemp.Format(" CRawSignalBuf GlobalAlloc failed i=%d,bufLen=%d\r", i,MEM_BUF_SIZE);
				pApp->m_LogMgnP->Addlog(strTemp);

			}
			m_pdwBuf[i] = (double *)GlobalLock(m_GlobalMem[i]);
		}
	}

}




