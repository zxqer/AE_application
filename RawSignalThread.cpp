#include "stdafx.h"
#include "AESWin.h"
#include "LogMgn.h"
#include "RawSignalThread.h"
#include "RawSignalBuf.h"
#include "HitManage.h"



DWORD WINAPI RawSignalProcessThreadFun(LPVOID lpParam)
{
	CRawSignalThread * pThread = (CRawSignalThread *)lpParam;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString strTemp;
	int ProcessFlag;
	int i,ret,chNo;
	DWORD AlreadyRead[8];

	for (i = 0; i < 8; i++)
	{
		AlreadyRead[i] = 0;
	}
	while (1)
	{
		WaitForSingleObject(pThread->hRawSignalEvent, INFINITE);
		for (chNo = 0; chNo < MAX_CHANNELS; chNo++)
		{
			if (pApp->m_arrayChannel_Item[chNo].iMonitor_Flag == 0)
				continue;
			pApp->mRawSignalBufP->LockBuf(chNo);
			if (pApp->mRawSignalBufP->GetIfHaveData(chNo, pThread->mReadDots))
			{
				pApp->mRawSignalBufP->GetLineDot(chNo, pThread->mReadDots, pThread->pArrayAmpBuf);
				ret = 1;
			}
			else
				ret = 0;
			pApp->mRawSignalBufP->UnlockBuf(chNo);
			if (ret == 1)
			{
				if (pApp->m_arrayChannel_Item[chNo].mUperLimit != 0 || pApp->m_arrayChannel_Item[chNo].mLowerLimit != 0)
				{
					pThread->FFT_LvBoProcess(pApp->m_arrayChannel_Item[chNo].mLowerLimit, pApp->m_arrayChannel_Item[chNo].mUperLimit, pApp->m_arrayChannel_Item[chNo].mSoftAmpGain, pThread->pArrayAmpBuf, pThread->mReadDots, pApp->mSampleTime);
				}
				ProcessFlag = 0;
				for (i = 0; i < pThread->mReadDots; i++)
				{

					if (*(pThread->pArrayAmpBuf + i) >  pThread->mArrayYuVaule[chNo] && *(pThread->pArrayAmpBuf + i) < 10.0)
					{
					//	strTemp.Format(" ProcessFlag chno =%d *pArrayAmpBuf: %f, mArrayYuVaule=%f\r", chNo, *(pThread->pArrayAmpBuf + i), pThread->mArrayYuVaule[chNo]);
					//	pApp->m_LogMgnP->Addlog(strTemp);
						ProcessFlag = 1;
						break;
					}
#if 0
					if (fabs(*(pThread->pArrayAmpBuf + i)) >= 0.05)
					{
						strTemp.Format(" chNo =%d  pArrayAmpBuf[%d]: %f", chNo, i, *(pThread->pArrayAmpBuf + i));
						pApp->m_LogMgnP->Addlog(strTemp);

					}
#endif
				}

				if (ProcessFlag == 1)
				{
				//	strTemp.Format(" AddHitItem chNo =%d", chNo);
				//	pApp->m_LogMgnP->Addlog(strTemp);
					pApp->mHitManageP->LockBuf();
					pApp->mHitManageP->AddHitItem(chNo, pThread->mArrayTot[chNo], pThread->mReadDots, pThread->pArrayAmpBuf);
				//	pApp->mHitManageP->AddHitItem(7-chNo, pThread->mArrayTot[7-chNo], pThread->mReadDots, pThread->pArrayAmpBuf);
					pApp->mHitManageP->UnlockBuf();
				}
				pThread->mArrayTot[chNo] += (double)pApp->mSampleTime / 1000;
#if 0
				AlreadyRead[chNo] += pThread->mReadDots;
				strTemp.Format(" chNo =%d,tot:%f,Reads:%u", chNo, pThread->mArrayTot[chNo], AlreadyRead[chNo]);
				pApp->m_LogMgnP->Addlog(strTemp);
#endif
			}
			else
			{
				Sleep(10);
			}

		}
	}
	return 0;
}


CRawSignalThread::CRawSignalThread()
{
	DWORD  threadId;

	hRawSignalThread = CreateThread(NULL, 0, RawSignalProcessThreadFun, this, 0, &threadId);

	hRawSignalEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("WriteEvent")  // object name
		);
	Play_Flag = 0;
	m_GlobalMem = NULL;
	m_outFFtBuf = NULL;

}


CRawSignalThread::~CRawSignalThread()
{
}

void CRawSignalThread::StartPlay()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	if (Play_Flag == 1)
		return;
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		mArrayTot[i] = 0.0;
#if 0
		gain = (double)pApp->m_arrayChannel_Item[i].iAmpGain / 20;
		gain = pow(10, gain);

		softgain = (double)pApp->m_arrayChannel_Item[i].mSoftAmpGain / 20;
		softgain = pow(10, softgain);

		mArrayYuVaule[i] = (double)pApp->m_arrayChannel_Item[i].iTiggerYuValue / 20;
		mArrayYuVaule[i] = pow(10, mArrayYuVaule[i])/1000000*gain*softgain;
#endif
		mArrayYuVaule[i] = (double)pApp->m_arrayChannel_Item[i].iTiggerYuValue / 1000;

	}
	CString theStr;
	theStr.Format("menkandianya:%f", mArrayYuVaule[0]);
	pApp->m_LogMgnP->Addlog(theStr);

	mReadDots = pApp->mSampleRate*pApp->mSampleTime;
	if (m_GlobalMem == NULL)
	{
		m_GlobalMem = GlobalAlloc(GHND, mReadDots*sizeof(double));
		pArrayAmpBuf = (double *)GlobalLock(m_GlobalMem);
	}
	else
	{
		m_GlobalMem = GlobalReAlloc(m_GlobalMem, mReadDots*sizeof(double),GHND);
		pArrayAmpBuf = (double *)GlobalLock(m_GlobalMem);
	}
	if (m_outFFtBuf == NULL)
	{
		m_outFFtBuf = new fftw_complex[mReadDots];
	}
	else
	{
		delete[]m_outFFtBuf;
		m_outFFtBuf = new fftw_complex[mReadDots];
	}

	SetEvent(hRawSignalEvent);
#if 0
	CTime starttime = CTime::GetCurrentTime();

	CString msLine;
	msLine = starttime.Format("starttime : %Y%m%d_%H%M%S");
	pApp->m_LogMgnP->Addlog(msLine);
#endif
	Play_Flag = 1;
}

void CRawSignalThread::StopPlay()
{
	if (Play_Flag == 0)
		return;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	ResetEvent(hRawSignalEvent);
#if 0
	CTime starttime = CTime::GetCurrentTime();

	CString msLine;
	msLine = starttime.Format("endtime : %Y%m%d_%H%M%S");
	pApp->m_LogMgnP->Addlog(msLine);
#endif
	Play_Flag = 0;

}

void CRawSignalThread::FFT_LvBoProcess(int LowLimit,int UperLimit,int SoftGain,double in_outBuf[], int Len,int SampleTime)
{
	int i, j, k;

	fftw_plan plan = fftw_plan_dft_r2c_1d(Len, in_outBuf, m_outFFtBuf, FFTW_ESTIMATE);
	fftw_execute(plan);

	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	if (LowLimit > 0)
	{
		k = LowLimit *  SampleTime;
		for (i = 0; i < k; i++)
		{
			m_outFFtBuf[i][0] = 0;
			m_outFFtBuf[i][1] = 0;
		}
	}
	if (UperLimit > 0)
	{
		k = UperLimit * SampleTime;
		j = Len / 2;
		if (k > j)
		{
			k = j;
		}
		for (i = k; i < j ; i++)
		{
			m_outFFtBuf[i][0] = 0;
			m_outFFtBuf[i][1] = 0;
		}
	}

	fftw_plan backword_plan = fftw_plan_dft_c2r_1d(Len, m_outFFtBuf, in_outBuf, FFTW_ESTIMATE);
	fftw_execute(backword_plan);

	double gain = (double)SoftGain / 20;
	gain = pow(10, gain);


	CString theStr;

	for (i = 0; i < Len; i++)
	{
		in_outBuf[i] = in_outBuf[i] / Len * gain;
#if 0
		if ((i % 100) == 0)
		{
			theStr.Format("in_outBuf[%d]:%f", i, in_outBuf[i]);
			pApp->m_LogMgnP->Addlog(theStr);
		}
#endif

	}
	in_outBuf[0] = 0.0;
	fftw_destroy_plan(plan);
	fftw_destroy_plan(backword_plan);
}



