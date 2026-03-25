// CLocSignalWnd.cpp: 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "CLocSignalWnd.h"
#include "HitManage.h"
#include "LogMgn.h"


// CLocSignalWnd

IMPLEMENT_DYNAMIC(CLocSignalWnd, CWnd)

CLocSignalWnd::CLocSignalWnd()
{
	mEvent_Arrive_Count = 0;
	mStartIndex = 0;
}

CLocSignalWnd::~CLocSignalWnd()
{
}


BEGIN_MESSAGE_MAP(CLocSignalWnd, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CLocSignalWnd 消息处理程序




void CLocSignalWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	CString theStr;
	theStr.Format("CLocSignalWnd:OnSize cx:%d,cy:%d",cx,cy);
	pApp->m_LogMgnP->Addlog(theStr);

	if (cx == 0 || cy == 0)
		return;
	mWndClientWidth = cx;
	mWndClientHeight = cy;
	SetTeeChartCoordParamter();
}


void CLocSignalWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

#if 0
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	theStr.Format("CLocSignalWnd:OnPaint");
	pApp->m_LogMgnP->Addlog(theStr);
#endif
	DrawChno(&dc);
	int i;
	for (i = 0; i < MAX_TEECHART_GRA; i++)
		mTeeChart[i].OnDraw(&dc);

}

void  CLocSignalWnd::SetTeeChartCoordParamter()
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();

	CRect theRect;
	int i, yyy = 50;
	int height = (mWndClientHeight - 200) / MAX_TEECHART_GRA;

	for (i = 0; i < MAX_TEECHART_GRA; i++)
	{
		theRect.left = 60;
		theRect.top = yyy;
		theRect.right = (mWndClientWidth - 120)  + theRect.left;
		theRect.bottom = theRect.top + height;

		mTeeChart[i].SetCoordinate_System(theRect, 0, pApp->mSampleTime, pApp->mCoordMinVolt, pApp->mCoordMaxVolt);
		mTeeChart[i].SetLineColor(RGB(0,0,255));
		mTeeChart[i].SetGraphState(0);
		yyy += height + 35;
		mTeeChart[i].SetAxis_ShowFlag(0);
	}
	mTeeChart[MAX_TEECHART_GRA-1].SetAxis_ShowFlag(1);
}

void CLocSignalWnd::AddEventArrive(int Chno, int HitIndex, int ArriveTime)
{
	if (mEvent_Arrive_Count < MAX_EVENT_ARRIVE)
	{
		mArrEvent_Arrive[mEvent_Arrive_Count].iChNo = Chno;
		mArrEvent_Arrive[mEvent_Arrive_Count].iHitIndex = HitIndex;
		mArrEvent_Arrive[mEvent_Arrive_Count].iArriveTime = ArriveTime;
		mEvent_Arrive_Count++;
	}
}

void CLocSignalWnd::ShowEventData()
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	HIT_ITEM_HEAD theHitHead;
	double* mAmpBuf;
	double x, y;
	mAmpBuf = new double[pApp->mSampleRate * pApp->mSampleTime];
	mStartIndex = 0;
	int i,j,index = mStartIndex;
	j = 0;
	while (index < mEvent_Arrive_Count)
	{
		pApp->mHitManageP->LockBuf();
		theHitHead = pApp->mHitManageP->GetHitItem(mArrEvent_Arrive[index].iChNo, mArrEvent_Arrive[index].iHitIndex, mAmpBuf);
		pApp->mHitManageP->UnlockBuf();
		if (theHitHead.iChannelNo == -1)
			continue;
		mTeeChart[index].ClearData();
		mTeeChart[index].audio_start_pos = (double)mArrEvent_Arrive[index].iArriveTime / pApp->mSampleRate;
		for (i = 0; i < theHitHead.iHitLength; i++)
		{
			x = (double)i / pApp->mSampleRate;
			y = *(mAmpBuf + i) * 1000;
			mTeeChart[index].OnlyAddData(x, y);
		}
		index++;
		j++;
		if (MAX_TEECHART_GRA == j)
			break;
	}
	delete[]mAmpBuf;
}

void CLocSignalWnd::DrawChno(CDC* pDC)
{
	CString theStr;
	mStartIndex = 0;
	int j,index = mStartIndex;
	int xxx, yyy;
	j = 0;
	xxx = mWndClientWidth / 2;
	int height = (mWndClientHeight - 200) / MAX_TEECHART_GRA;
	yyy = 10;
	while (index < mEvent_Arrive_Count)
	{
		theStr.Format("第%d通道", mArrEvent_Arrive[index].iChNo+1);
		pDC->TextOut(xxx, yyy, theStr);
		index++;
		j++;
		yyy += height + 35;
		if (MAX_TEECHART_GRA == j)
			break;
	}

}




