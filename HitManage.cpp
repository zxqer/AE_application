#include "stdafx.h"
#include "AESWin.h"
#include "MainFrm.h"
#include "HitManage.h"
#include "LogMgn.h"

#define MEM_BUF_SIZE  16*1024*1024
#define MEM_BUF_DOTS   2*1024*1024

CHitManage::CHitManage()
{
	//m_HitArray.SetSize(100);
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		mIndexArray[i] = 0;
	}
	mPlay_Flag = 0;

	m_GlobalMem = GlobalAlloc(GHND, MEM_BUF_SIZE);
	m_pduBuf = (double *)GlobalLock(m_GlobalMem);
	mSaveFileLen = 0;
	mUsedBufLen = 0;
	mStartBufPos = 0;
	mMutex = CreateMutex(NULL, FALSE, NULL);

}


CHitManage::~CHitManage()
{
	GlobalUnlock(m_GlobalMem);
	GlobalFree(m_GlobalMem);
}

void  CHitManage::StartPlay()
{
	if (mPlay_Flag == 1)
		return;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	if (mSaveFile.m_hFile != CFile::hFileNull)
	{
		mSaveFile.Close();

	}

	CString msLine;
	strcpy_s(datFileName, pApp->m_szSaveFilePath);
	strcat_s(datFileName, "data\\");

	strcpy_s(dexFileName, datFileName);
	if (pApp->mSaveDataFlag)
	{
		CTime tt = CTime::GetCurrentTime();
		msLine = tt.Format("Wavelength_%Y年%m月%d日%H时%M分%S秒") + ".axi";
		strcat_s(datFileName, msLine);

		msLine = tt.Format("Wavelength_%Y年%m月%d日%H时%M分%S秒") + ".ind";
		strcat_s(dexFileName, msLine);

		mSaveFile.Open(datFileName, CFile::modeCreate | CFile::modeReadWrite);

		mIndexFile.Open(dexFileName, CFile::modeCreate | CFile::modeReadWrite);
	}
	else
	{
		datFileName[0] = 0;
		dexFileName[0] = 0;
	}
	mPlay_Flag = 1;
}

void CHitManage::StopPlay()
{
	HIT_ITEM_HEAD theHitHead;
	int i,count;

	if (mPlay_Flag == 0)
		return;
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	if (pApp->mSaveDataFlag)
	{
		mSaveFile.Write(m_pduBuf, sizeof(double) * mUsedBufLen);
		mSaveFile.Close();
		count = m_HitArray.GetSize();
		for (i = 0; i < count; i++)
		{
			theHitHead = m_HitArray.GetAt(i);
			mIndexFile.Write(&theHitHead, sizeof(HIT_ITEM_HEAD));
		}
		mIndexFile.Close();
	}
	mPlay_Flag = 0;
}

void  CHitManage::AddHitItem(int ChanNo, double inTot, int inHitLength, double * inAmpBuf)
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	HIT_ITEM_HEAD theHitItemHead;
	theHitItemHead.iChannelNo = ChanNo;
	theHitItemHead.iIndexNo = mIndexArray[ChanNo];
	theHitItemHead.douTot = inTot;
	theHitItemHead.iHitLength = inHitLength;
	if (mUsedBufLen + inHitLength > MEM_BUF_DOTS)
	{
		if (pApp->mSaveDataFlag)
			mSaveFile.Write(m_pduBuf,sizeof(double)*mUsedBufLen);
		mSaveFileLen += mUsedBufLen;
		mStartBufPos = mSaveFileLen;
		mUsedBufLen = 0;
	}
	theHitItemHead.dwFileOffset = mUsedBufLen + mSaveFileLen;
	for (int i = 0; i < inHitLength; i++)
	{
		*(m_pduBuf+i+ mUsedBufLen) = inAmpBuf[i];
	}
	mUsedBufLen += inHitLength;
	m_HitArray.Add(theHitItemHead);
//	CString theStr;
//	theStr.Format("AddHitItem chno:%d index: %d tot:%f", ChanNo, mIndexArray[ChanNo],inTot);
//	pApp->m_LogMgnP->Addlog(theStr);

	pApp->m_pMainWnd->PostMessage(WM_AES_UPDATE_VIEW,ChanNo, mIndexArray[ChanNo]);
	mIndexArray[ChanNo]++;
}

void  CHitManage::LockBuf()
{
	WaitForSingleObject(mMutex, INFINITE);
}
void CHitManage::UnlockBuf()
{
	ReleaseMutex(mMutex);
}

HIT_ITEM_HEAD CHitManage::GetHitItem(int ChanNo, int HitIndex, double * outAmpBuf)
{
	HIT_ITEM_HEAD theHitHead;
	int i, count,retchano;
	long long  offset;

	count = m_HitArray.GetSize();
	retchano = -1;
	for (i = 0; i < count; i++)
	{
		theHitHead = m_HitArray.GetAt(i);
		if (theHitHead.iChannelNo == ChanNo && theHitHead.iIndexNo == HitIndex)
		{
			retchano = ChanNo;
			break;
		}
	}
	if (retchano != -1)
	{
		if (theHitHead.dwFileOffset >= mStartBufPos && mUsedBufLen !=0)
		{
			memcpy(outAmpBuf, m_pduBuf + theHitHead.dwFileOffset - mStartBufPos, theHitHead.iHitLength*sizeof(double));
		}
		else
		{
			if (mPlay_Flag != 1)
			{
				offset = (long long)theHitHead.dwFileOffset * sizeof(double);
				mSaveFile.Seek(offset, CFile::begin);
				mSaveFile.Read(outAmpBuf, theHitHead.iHitLength * sizeof(double));
				mSaveFile.SeekToEnd();
			}
			else
			{
				retchano = -1;
			}
		}
	}
	theHitHead.iChannelNo = retchano;
	return theHitHead;
}

int  CHitManage::FindHitIndexByTot(int ChanNo, double inTot)
{
	HIT_ITEM_HEAD theHitHead;
	int i, count;
	count = m_HitArray.GetSize();
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	for (i = 0; i < count; i++)
	{
		theHitHead = m_HitArray.GetAt(i);
		if (theHitHead.iChannelNo == ChanNo)
		{
			if (theHitHead.douTot >= inTot)
			{
				theStr.Format("FindHitIndexByTot:%f,channo:%d", theHitHead.douTot, ChanNo);
				pApp->m_LogMgnP->Addlog(theStr);
				return theHitHead.iIndexNo;
			}
		}
	}
	return -1;
}

void CHitManage::ClearPlayBuf()
{
	m_HitArray.RemoveAll();
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		mIndexArray[i] = 0;
	}
	mSaveFileLen = 0;
	mUsedBufLen = 0;
	mStartBufPos = 0;

}

void CHitManage::OpenHistoryName(CString strFileName)
{
	ClearPlayBuf();

	strcpy_s(datFileName, strFileName);
	CString theTempStr = strFileName.TrimRight(".axi");
	theTempStr += ".ind";
	strcpy_s(dexFileName, theTempStr);
	if (mIndexFile.Open(dexFileName, CFile::modeRead) != 0)
	{
		HIT_ITEM_HEAD theHitHead;
		int ReadLen;
		do
		{
			ReadLen = mIndexFile.Read(&theHitHead, sizeof(HIT_ITEM_HEAD));
			if (ReadLen == sizeof(HIT_ITEM_HEAD))
				m_HitArray.Add(theHitHead);
			else
				break;

		} while (1);
		mIndexFile.Close();
		if (m_HitArray.GetCount() > 0)
		{
			if (mSaveFile.m_hFile == CFile::hFileNull)
				mSaveFile.Open(datFileName, CFile::modeRead);
			CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
			pApp->m_pMainWnd->PostMessage(WM_AES_DRAW_HISTORY,0,0);
		}
	}

}

void  CHitManage::CloseOpenDataFile()
{
	if (mSaveFile.m_hFile != CFile::hFileNull)
	{
		mSaveFile.Close();

	}
}

void CHitManage::SaveDataToTxt(CStdioFile* pTxtOutFile)
{
	HIT_ITEM_HEAD theHitHead;
	CString theStr;
	int i,j,Count;
	Count = m_HitArray.GetCount();

	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	double *mAmpBuf = new double[pApp->mSampleRate * pApp->mSampleTime];
	double dtemp;
	for (i = 0; i < Count; i++)
	{
		theHitHead = m_HitArray.GetAt(i);
		theStr.Format("声发射事件序号：%d\t通道号：%d \t起始时间：%f\n", theHitHead.iIndexNo, theHitHead.iChannelNo, theHitHead.douTot);
		pTxtOutFile->WriteString(theStr);
		theHitHead = pApp->mHitManageP->GetHitItem(theHitHead.iChannelNo, theHitHead.iIndexNo, mAmpBuf);
		if (theHitHead.iChannelNo == -1)
			continue;
		theStr.Format("时间戳(秒)\t\t幅值\n");
		pTxtOutFile->WriteString(theStr);
		for (j = 0; j < theHitHead.iHitLength; j++)
		{
			//if ((j % 10) == 0)
			{
				dtemp = (double)j / pApp->mSampleRate / 1000 + theHitHead.douTot;
				theStr.Format("%f\t\t%f\n", dtemp, *(mAmpBuf + j));
				pTxtOutFile->WriteString(theStr);

			}
		}
	}
	delete[]mAmpBuf;
}









