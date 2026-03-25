// GraphWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "MainFrm.h"
#include "AESSCrollView.h"
#include "GraphWnd.h"
#include "GrapthSetDlg.h"
#include "LogMgn.h"
#include "HitManage.h"


#define WND_MARGIN          50   //窗口边空  
#define WND_TEE_AXIS_SCREEN_UNITX    80  //坐标轴屏幕单位  
#define WND_TEE_AXIS_SCREEN_UNITY    50  //坐标轴屏幕单位  
#define AXIS_SCREEN_DARW_LEN    10  //坐标轴屏幕线延申长度  

#define SCATTER_DOT_SIZE    8  //二维散点图大小  

// CGraphWnd

#define MIN_SHOW_DOTS_BS   10

IMPLEMENT_DYNAMIC(CGraphWnd, CWnd)

CGraphWnd::CGraphWnd()
{
	mGraphState = 0;
	mYAixState = 1;
	mChannelNo = 0;
	mAmpBuf = NULL;
	mAmpFFTBuf = NULL;
	m_outBuf = NULL;
	mSelectNode = -1;
	UpdateArtPara();
//	m_PointArrayX.SetSize(0, 50000);
//	m_PointArrayY.SetSize(0, 50000);

}

CGraphWnd::~CGraphWnd()
{
	if (mAmpBuf != NULL)
	{
		delete[]mAmpBuf;
		delete[]mAmpFFTBuf;
		delete[]m_outBuf;
	}
//	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
//	CString theStr;
//	theStr.Format("delete GraWnd!!!");
//	pApp->m_LogMgnP->Addlog(theStr);
}

//更新采集参数
void CGraphWnd::UpdateArtPara()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	if (mAmpBuf != NULL)
	{
		delete[]mAmpBuf;
		delete[]mAmpFFTBuf;
		delete[]m_outBuf;
	}
	CString theStr;
	//theStr.Format("UpdateArtPara mSampleRate：%d mSampleTime:%d", pApp->mSampleRate, pApp->mSampleTime);
	//pApp->m_LogMgnP->Addlog(theStr);

	mAmpBuf = new double[pApp->mSampleRate*pApp->mSampleTime];
	if (mAmpBuf == NULL)
	{
		theStr.Format("UpdateArtPara mAmpBuf allocate buf failed");
		pApp->m_LogMgnP->Addlog(theStr);

	}
	mAmpFFTBuf = new double[pApp->mSampleRate*pApp->mSampleTime];
	if (mAmpFFTBuf == NULL)
	{
		theStr.Format("UpdateArtPara mAmpFFTBuf allocate buf failed");
		pApp->m_LogMgnP->Addlog(theStr);

	}
	m_outBuf = new fftw_complex[pApp->mSampleRate*pApp->mSampleTime];
	if (m_outBuf == NULL)
	{
		theStr.Format("UpdateArtPara m_outBuf allocate buf failed");
		pApp->m_LogMgnP->Addlog(theStr);

	}

}

BEGIN_MESSAGE_MAP(CGraphWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_COMMAND(ID_SET_GRAPH, &CGraphWnd::OnSetGraph)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_GRAWND_PROCESS_DATA, GraWnd_Process_Data)
	ON_MESSAGE(WM_GRAWND_DRAW_HISDATA, GraWnd_Draw_HisData)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_WM_WINDOWPOSCHANGED()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CGraphWnd 消息处理程序




void CGraphWnd::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << mGraphState;
		ar << mYAixState;
		ar << mChannelNo;

	}
	else
	{	// loading code
		ar >> mGraphState;
		ar >> mYAixState;
		ar >> mChannelNo;
		SetWndTitle();
	}
}


void CGraphWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
		
					   // TODO: 在此处添加消息处理程序代码
#if 0
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	theStr.Format("OnPaint mGraphState：%d mYAixState:%d", mGraphState,mYAixState);
	pApp->m_LogMgnP->Addlog(theStr);
#endif
	DrawAxisAndText(&dc);
	if(mGraphState == 0)
		DrawWaveform_graph(&dc);
	else 
		Draw2D_Scatter(&dc);

	DrawSelectNode(&dc);
}


BOOL CGraphWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect theRect;
	GetClientRect(theRect);
	pDC->FillSolidRect(theRect, RGB(255, 255, 255));
#if 0
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	theStr.Format("OnEraseBkgnd：%x", mWndMoveFlag);
	pApp->m_LogMgnP->Addlog(theStr);
#endif
	return TRUE;
}


//BOOL CGraphWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
//}


int CGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
//	SetTimer(0, 100, NULL);
	return 0;
}

void  CGraphWnd::SetWndTitle()
{
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	switch (mYAixState)
	{
		case 0 :
			if (mChannelNo == -1)
			{
				theStr = "撞击对时间（秒）<所有通道>";
			}
			else
			{
				theStr.Format("撞击对时间（秒） <通道%d>", mChannelNo + 1);
			}
			mCoordItem.mAxisXMin = 0;
			mCoordItem.mAxisXMax = 50;

			mCoordItem.mAxisYMin = 0;
			mCoordItem.mAxisYMax = 1000;

			break;
		case 1:
			theStr.Format("电压毫伏对时间（毫秒） <通道%d>", mChannelNo + 1);
			mCoordItem.mAxisYMin = pApp->mCoordMinVolt;
			mCoordItem.mAxisYMax = pApp->mCoordMaxVolt;
			mCoordItem.mAxisXMin = 0;
			mCoordItem.mAxisXMax = pApp->mSampleTime;
			break;
		case 2:
			theStr.Format("电源dB对频率（KHz） <通道%d>", mChannelNo + 1);
			mCoordItem.mAxisYMin = 0;
			mCoordItem.mAxisYMax = 120;
			mCoordItem.mAxisXMin = 0;
			mCoordItem.mAxisXMax = 300;
			break;
		case 3:
			theStr.Format("幅值dB对时间（秒） <通道%d>", mChannelNo + 1);
			mCoordItem.mAxisYMin = 0;
			mCoordItem.mAxisYMax = 120;
			mCoordItem.mAxisXMin = 0;
			mCoordItem.mAxisXMax = 50;
			break;
		default:
		break;
	}
	mCoordItem.mInitAxisXMin = mCoordItem.mAxisXMin;
	mCoordItem.mInitAxisXMax = mCoordItem.mAxisXMax;
	mCoordItem.mInitAxisYMin = mCoordItem.mAxisYMin;
	mCoordItem.mInitAxisYMax = mCoordItem.mAxisYMax;

	SetWindowText(theStr);
	SetCoordParamter();
}

void  CGraphWnd::ClearData()
{
	m_PointArrayX.RemoveAll();
	m_PointArrayY.RemoveAll();
}


void CGraphWnd::OnSetGraph()
{
	// TODO: 在此添加命令处理程序代码
	CGrapthSetDlg theDialog;

	theDialog.mComboValue = mYAixState;
	theDialog.m_GraphComBox = mGraphState;
	if(mChannelNo != -1)
		theDialog.mChannelNo = mChannelNo+1;
	if (theDialog.DoModal() == IDOK)
	{
		mYAixState = theDialog.mComboValue;
		if(theDialog.mChannelNo > 0)
			mChannelNo = theDialog.mChannelNo-1;
		else
			mChannelNo = -1;
		mGraphState = theDialog.m_GraphComBox;
		SetWndTitle();
		Invalidate();
	}

}




void CGraphWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	mCoordItem.mWndClientWidth = cx;
	mCoordItem.mWndClientHeight = cy;
	SetWndTitle();
	// TODO: 在此处添加消息处理程序代码
}

void  CGraphWnd::SetCoordParamter()
{
	mCoordItem.mScreen_OriginX = WND_MARGIN;
	mCoordItem.mScreen_OriginY = mCoordItem.mWndClientHeight - WND_MARGIN;
	mCoordItem.mScreenWidth = mCoordItem.mWndClientWidth - WND_MARGIN - WND_MARGIN / 2;
	mCoordItem.mScreenHeight = mCoordItem.mWndClientHeight - WND_MARGIN - WND_MARGIN / 2;


#if 0
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	theStr.Format("mAxisXMax: %d,mAxisXMin:%d", mCoordItem.mAxisXMax, mCoordItem.mAxisXMin);
	pApp->m_LogMgnP->Addlog(theStr);
#endif

	int xxx = WND_TEE_AXIS_SCREEN_UNITX;
	while (1)
	{
		mCoordItem.mAxisXUint = (int)((double)(mCoordItem.mAxisXMax - mCoordItem.mAxisXMin) / mCoordItem.mScreenWidth*xxx);
		if (mCoordItem.mAxisXUint <= 5)
			break;
		if ((mCoordItem.mAxisXUint % 5) == 0)
			break;
		xxx++;
	}
	if (mCoordItem.mAxisXUint == 0)
		mCoordItem.mAxisXUint = 1;
	xxx = WND_TEE_AXIS_SCREEN_UNITY;
	while (1)
	{
		mCoordItem.mAxisYUint = (int)((double)(mCoordItem.mAxisYMax - mCoordItem.mAxisYMin) / mCoordItem.mScreenHeight*xxx);
		if (mCoordItem.mAxisYUint <= 5)
			break;
		if ((mCoordItem.mAxisYUint % 5) == 0)
			break;
		xxx++;
	}
	if (mCoordItem.mAxisYUint == 0)
		mCoordItem.mAxisYUint = 1;
	ReCalculateSelectRect();
}

CPoint  CGraphWnd::ptConvertScreenToLogical(CPoint pt)
{
	pt.x -= mCoordItem.mScreen_OriginX;
	pt.y = mCoordItem.mScreen_OriginY - pt.y;
	double x, y;
	x = (double)pt.x * (mCoordItem.mAxisXMax - mCoordItem.mAxisXMin) / mCoordItem.mScreenWidth;
	y = (double)pt.y * (mCoordItem.mAxisYMax - mCoordItem.mAxisYMin) / mCoordItem.mScreenHeight;
	pt.x = (int)x + mCoordItem.mAxisXMin;
	pt.y = (int)y + mCoordItem.mAxisYMin;
	return pt;
}

CPoint  CGraphWnd::ptConvertLogicalToScreen(CPoint pt)
{
	CPoint rePt;
	double x, y;
	pt.x -= mCoordItem.mAxisXMin;
	pt.y -= mCoordItem.mAxisYMin;
	x = (double)pt.x * mCoordItem.mScreenWidth / (mCoordItem.mAxisXMax - mCoordItem.mAxisXMin);
	y = (double)pt.y *  mCoordItem.mScreenHeight / (mCoordItem.mAxisYMax - mCoordItem.mAxisYMin);
	rePt.x = (int)x + mCoordItem.mScreen_OriginX;
	rePt.y = mCoordItem.mScreen_OriginY-(int)y;
	return rePt;
}

int  CGraphWnd::dfConvertLogicalXToScreen(double x)
{
	int rex;
	x -= (double)mCoordItem.mAxisXMin;
	x = x * mCoordItem.mScreenWidth / (mCoordItem.mAxisXMax - mCoordItem.mAxisXMin);
	rex = (int)x + mCoordItem.mScreen_OriginX;
	return rex;
}
int CGraphWnd::dfConvertLogicalYToScreen(double y)
{
	int rey;
	y -= (double)mCoordItem.mAxisYMin;
	y = y *  mCoordItem.mScreenHeight / (mCoordItem.mAxisYMax - mCoordItem.mAxisYMin);
	rey = mCoordItem.mScreen_OriginY - (int)y;
	return rey;
}


void CGraphWnd::DrawAxisAndText(CDC* pDC)
{
	CRect theRect;
	theRect.left = 0;
	theRect.top = 0;
	theRect.right = mCoordItem.mWndClientWidth;
	theRect.bottom = mCoordItem.mWndClientHeight;
	pDC->FillSolidRect(theRect, RGB(232, 232, 232));

	theRect.left = dfConvertLogicalXToScreen(mCoordItem.mAxisXMin);
	theRect.top = dfConvertLogicalYToScreen(mCoordItem.mAxisYMin);
	theRect.right = dfConvertLogicalXToScreen(mCoordItem.mAxisXMax);
	theRect.bottom = dfConvertLogicalYToScreen(mCoordItem.mAxisYMax);
	theRect.NormalizeRect();
	pDC->FillSolidRect(theRect, RGB(255, 255, 255));

	CPen NewPen1(PS_SOLID, 1, RGB(235, 235, 235));
	CPen *pOldPen = pDC->SelectObject(&NewPen1);
	CPoint ScreenPt, LogicalPt;
	
	int i, start = mCoordItem.mAxisXMin;
	int logicalWidth = mCoordItem.mAxisXMax - mCoordItem.mAxisXMin;
	int logicalHeight = mCoordItem.mAxisYMax - mCoordItem.mAxisYMin;
	for (i = 0; i < (logicalWidth / mCoordItem.mAxisXUint +1); i++)
	{
		LogicalPt.x = start;
		LogicalPt.y = mCoordItem.mAxisYMin;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);

		LogicalPt.y = mCoordItem.mAxisYMax;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mCoordItem.mAxisXUint;
	}
	start = mCoordItem.mAxisYMin;
	while (start < mCoordItem.mAxisYMax)
	{
		LogicalPt.x = mCoordItem.mAxisXMin;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		LogicalPt.x = mCoordItem.mAxisXMin + logicalWidth;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mCoordItem.mAxisYUint;
	}


	pDC->SelectObject(pOldPen);
	CPen NewPen2(PS_SOLID, 1, RGB(0, 0, 0));
	pOldPen = pDC->SelectObject(&NewPen2);

	start = mCoordItem.mAxisXMin;
	for (i = 0; i <= logicalWidth / mCoordItem.mAxisXUint; i++)
	{
			LogicalPt.x = start;
			LogicalPt.y = mCoordItem.mAxisYMin;
			ScreenPt = ptConvertLogicalToScreen(LogicalPt);
			pDC->MoveTo(ScreenPt.x, ScreenPt.y);
			ScreenPt.y += AXIS_SCREEN_DARW_LEN;
			pDC->LineTo(ScreenPt.x, ScreenPt.y);

			start += mCoordItem.mAxisXUint;
	}

	start = mCoordItem.mAxisYMin;
	while (start  < mCoordItem.mAxisYMax)
	{
		LogicalPt.x = mCoordItem.mAxisXMin;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		ScreenPt.x -= AXIS_SCREEN_DARW_LEN;
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mCoordItem.mAxisYUint;
	}

	pDC->SelectObject(pOldPen);

	CString theStr;
	int yValue;
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	start = mCoordItem.mAxisYMin;
	yValue = mCoordItem.mAxisYMin;
	while (start < mCoordItem.mAxisYMax)
	{
		LogicalPt.x = mCoordItem.mAxisXMin;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		ScreenPt.x -= WND_MARGIN;
		ScreenPt.y -= 10;
		theStr.Format("%d", yValue);
		pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
		start += mCoordItem.mAxisYUint;
		yValue += mCoordItem.mAxisYUint;
	}
	pDC->SetTextColor(RGB(0, 0, 0));
	yValue = mCoordItem.mAxisXMin;
	start = mCoordItem.mAxisXMin;
	for (i = 0; i <= logicalWidth / mCoordItem.mAxisXUint; i++)
	{
		LogicalPt.x = start;
		LogicalPt.y = mCoordItem.mAxisYMin;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		ScreenPt.y += AXIS_SCREEN_DARW_LEN;
		ScreenPt.x -= 5;
		theStr.Format("%d", yValue);
		pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
		yValue += mCoordItem.mAxisXUint;
		start += mCoordItem.mAxisXUint;
	}


}

void  CGraphWnd::DrawWaveform_graph(CDC* pDC)
{
	int i,Count = m_PointArrayX.GetSize();
	if (Count == 0)
		return;
	double xxx, yyy;
	int x, y;
	CPen NewPen1(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *pOldPen = pDC->SelectObject(&NewPen1);
	for (i = 0; i < Count; i++)
	{
		xxx = m_PointArrayX.GetAt(i);
		yyy = m_PointArrayY.GetAt(i);
		x = dfConvertLogicalXToScreen(xxx);
		y = dfConvertLogicalYToScreen(yyy);
		if( i == 0)
			pDC->MoveTo(x, y);
		else
		{
			pDC->LineTo(x, y);
		}
	}
	pDC->SelectObject(pOldPen);
}

void  CGraphWnd::Draw2D_Scatter(CDC* pDC)
{
	int i, Count = m_PointArrayX.GetSize();
	double xxx, yyy;
	CRect theRect;
	if (Count == 0)
		return;
	CRect theScreenRect;
	theScreenRect.left = mCoordItem.mScreen_OriginX;
	theScreenRect.right = mCoordItem.mScreen_OriginX + mCoordItem.mScreenWidth;
	theScreenRect.top = mCoordItem.mScreen_OriginY - mCoordItem.mScreenHeight;
	theScreenRect.bottom = mCoordItem.mScreen_OriginY;

	for (i = 0; i < Count; i++)
	{
		xxx = m_PointArrayX.GetAt(i);
		yyy = m_PointArrayY.GetAt(i);
		theRect.left = dfConvertLogicalXToScreen(xxx);
		theRect.top = dfConvertLogicalYToScreen(yyy);
		theRect.right = theRect.left+SCATTER_DOT_SIZE;
		theRect.bottom = theRect.top+SCATTER_DOT_SIZE;
		if (theScreenRect.PtInRect(theRect.TopLeft()))
		{
			pDC->FillSolidRect(theRect, RGB(255, 0, 0));

		}
	}

}


//void CGraphWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	g_WndMoveFlag = 1;
//	CString theStr;
//	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
//	theStr.Format("OnNcLButtonDown：%x", g_WndMoveFlag);
//	pApp->m_LogMgnP->Addlog(theStr);
//
//	CWnd::OnNcLButtonDown(nHitTest, point);
//}




//void CGraphWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (g_WndMoveFlag == 1)
//		g_WndMoveFlag = 2;
//
//#if 0
//	CString theStr;
//	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
//	theStr.Format("OnNcMouseMove：%x,%x", g_WndMoveFlag, nHitTest);
//	pApp->m_LogMgnP->Addlog(theStr);
//#endif
//
//	CWnd::OnNcMouseMove(nHitTest, point);
//}

void CGraphWnd::AddXY(double x, double y)
{
	m_PointArrayX.Add(x);
	m_PointArrayY.Add(y);
	//Invalidate();
}




void CGraphWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_GRAWND_DRAW_HISDATA,0,0);
	CWnd::OnTimer(nIDEvent);
}

void CGraphWnd::UpdateAEData(int chno, int hitIndex)
{
	if (chno != mChannelNo)
		return;


	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	HIT_ITEM_HEAD theHitHead;
	pApp->mHitManageP->LockBuf();
	theHitHead = pApp->mHitManageP->GetHitItem(chno, hitIndex, mAmpBuf);
	pApp->mHitManageP->UnlockBuf();
#if 0
	CString theStr;
	theStr.Format("UpdateAEData：chno %d, hitindex :%d,theHitHead.iHitLength:%d,mYAixState:%d", chno, hitIndex, theHitHead.iHitLength, mYAixState);
	pApp->m_LogMgnP->Addlog(theStr);
#endif
	if (theHitHead.iChannelNo == -1)
		return;
	int i,j;
	BOOL  brease = false;
	double x, y,temp,sum;
	CString theStr;
	if (mYAixState == 1)
	{
		m_PointArrayX.RemoveAll();
		m_PointArrayY.RemoveAll();
		j = 0;
		for (i = 0; i < theHitHead.iHitLength; i++)
		{
			if (j == 0 && pApp->mAppState ==1)
			{
				x = (double)i / pApp->mSampleRate;
				y = *(mAmpBuf + i) * 1000;

				m_PointArrayX.Add(x);
				m_PointArrayY.Add(y);
			//	theStr.Format("chno: %d dianya: %f,gain:%f", chno, y);
			//	pApp->m_LogMgnP->Addlog(theStr);

			}
			j++;
			if (j >= MIN_SHOW_DOTS_BS &&  pApp->mAppState == 1)
			{
				j = 0;
			}

		}
	}
	else if (mYAixState == 2)
	{
		m_PointArrayX.RemoveAll();
		m_PointArrayY.RemoveAll();
		ConvertToFFT(mAmpBuf, theHitHead.iHitLength, &temp, mAmpFFTBuf);
		for (i = 0; i < theHitHead.iHitLength /2; i++)
		{
			if (pApp->mAppState == 1)
			{
				x = (double)i / pApp->mSampleTime;
				y = *(mAmpFFTBuf + i);
				m_PointArrayX.Add(x);
				if (y < mCoordItem.mAxisYMin)
					y = mCoordItem.mAxisYMin;
				m_PointArrayY.Add(y);
			}
		}
	}
	else if (mYAixState == 3)
	{
		if (theHitHead.douTot >= (double)mCoordItem.mAxisXMax)
		{
			mCoordItem.mAxisXMax += 30;
			SetCoordParamter();
			brease = true;
			Invalidate();
		}
		x = theHitHead.douTot;
		y = 0.0;
		sum = 0.0;
		for (i = 0; i < theHitHead.iHitLength; i++)
		{
			temp = fabs(*(mAmpBuf + i));
			if ( temp > y)
				y = temp;
			sum += temp;
		}
		//	ConvertToFFT(mAmpBuf, theHitHead.iHitLength, &y, mAmpFFTBuf);
		sum = sum / theHitHead.iHitLength;
		theStr.Format("average :%f max v :%f", sum, y);
		pApp->m_LogMgnP->Addlog(theStr);
		y -= sum;
		y = 20 * log10(y * 1000 *1000);
		y -= (double)pApp->m_arrayChannel_Item[chno].iAmpGain;
		y -= (double)pApp->m_arrayChannel_Item[chno].mSoftAmpGain;
		if (pApp->mDeZaoshangFlag)
		{
			if (y > 29.0 && y < 40.0)
				y -= 10.0;
			else if (y < 50.0)
				y -= 20;
		}

		theStr.Format("chno:%d  db:%f", chno, y);
		pApp->m_LogMgnP->Addlog(theStr);

		AddXY(x,y);

	}
	if(!brease)
	 InvalidDisplayArea();
}

LRESULT  CGraphWnd::GraWnd_Process_Data(WPARAM x, LPARAM y)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
//	CString theStr;
//	theStr.Format("GraWnd_Process_Data：selfchno %d,  Chno :%d hitindex :%d", mChannelNo, x, y);
//	pApp->m_LogMgnP->Addlog(theStr);
	UpdateAEData(x, y);
	return 0;
}

void CGraphWnd::AddSensorData_Show()
{
	CDC * pDC = GetDC();
	if (mGraphState == 0)
		DrawWaveform_graph(pDC);
	else
		Draw2D_Scatter(pDC);
	ReleaseDC(pDC);
}

//实部与虚部
#define REAL 0
#define IMAG 1


void CGraphWnd::ConvertToFFT(double inBuf[], int Len, double * outMaxAmp, double * outAmpBuf)
{
//	fftw_complex* outBuf = new fftw_complex[Len];
	fftw_plan plan = fftw_plan_dft_r2c_1d(Len, inBuf, m_outBuf, FFTW_ESTIMATE);
	fftw_execute(plan);

	*outMaxAmp = 0.0;
	int i;
	for (i = 0; i < Len / 2; i++)
	{
		outAmpBuf[i] = sqrt(m_outBuf[i][REAL] * m_outBuf[i][REAL] + m_outBuf[i][IMAG] * m_outBuf[i][IMAG]);
		if (i == 0)
		{
			outAmpBuf[i] = outAmpBuf[i] / Len;

		}
		else
			outAmpBuf[i] = 2.0 * outAmpBuf[i] / Len;

		outAmpBuf[i] = 20 * log10(outAmpBuf[i] / 0.000001);
		if (outAmpBuf[i] > *outMaxAmp)
			*outMaxAmp = outAmpBuf[i];

	}

	fftw_destroy_plan(plan);
//	delete[]outBuf;
}

void CGraphWnd::NotifyStartPlay()
{
	m_PointArrayX.RemoveAll();
	m_PointArrayY.RemoveAll();
	mSelectNode = -1;
	SetWndTitle();
	Invalidate();
}


void CGraphWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();
	//	SetActiveWindow();
//	CString theStr;
//	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
//	theStr.Format("CGraphWnd::OnLButtonDown");
	//pApp->m_LogMgnP->Addlog(theStr);
	mStartPt = point;
	CWnd::OnLButtonDown(nFlags, point);
}


void CGraphWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	if (pApp->mAppState == 1)
		return;
	mEndPt = point;
	CRect theRect(mStartPt, mEndPt);
	int retflag,HitIndex;
	if (abs(theRect.Width()) < 100)
	{
		retflag = JudgeIfSelectNode(point);
		if (retflag != -1)
		{
			double tottemp = m_PointArrayX.GetAt(mSelectNode);
			HitIndex = pApp->mHitManageP->FindHitIndexByTot(mChannelNo, tottemp);
			if (HitIndex != -1)
			{
				CAESSCrollView * pView = (CAESSCrollView *)GetParent();
				pView->PostMessage(WM_NOTIFY_GRAWND_DRAW_DOTDATA,mChannelNo, HitIndex);
			}
		}
		InvalidDisplayArea();
	}
	else
	{
		if (mStartPt.x > mEndPt.x)
		{
			SetWndTitle();
			Invalidate();
		}
		else
		{
			CPoint pts = ptConvertScreenToLogical(mStartPt);
	//		theStr.Format("CGraphWnd::OnLButtonUp start x:%d,y:%d", pts.x, pts.y);
	//		pApp->m_LogMgnP->Addlog(theStr);
			CPoint pte = ptConvertScreenToLogical(mEndPt);
	//		theStr.Format("CGraphWnd::OnLButtonUp end x:%d,y:%d", pte.x, pte.y);
//			pApp->m_LogMgnP->Addlog(theStr);

			mCoordItem.mAxisXMin = pts.x;
			mCoordItem.mAxisXMax = pte.x;
			mCoordItem.mAxisYMin = pte.y;
			mCoordItem.mAxisYMax = pts.y;
			SetCoordParamter();
			Invalidate();
		}
	}
	//CWnd::OnLButtonUp(nFlags, point);
}


void CGraphWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	if (pApp->mAppState == 1)
		return;

	if (MK_LBUTTON & nFlags)
	{
	//	theStr.Format("CGraphWnd::OnMouseMove");
//		pApp->m_LogMgnP->Addlog(theStr);
		mEndPt = point;
		DrawMouseMoveRect();
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CGraphWnd::DrawMouseMoveRect()
{
	CDC * pDC = GetDC();
	CRect theRect(mStartPt, mEndPt);
	pDC->Rectangle(theRect);
	ReleaseDC(pDC);
}

int CGraphWnd::JudgeIfSelectNode(CPoint pt)
{
	if (mYAixState != 3 || mGraphState != 1)
		return -1;
	int i, Count = m_PointArrayX.GetSize();
	double xxx, yyy;
	mSelectNode = -1;
	for (i = 0; i < Count; i++)
	{
		xxx = m_PointArrayX.GetAt(i);
		yyy = m_PointArrayY.GetAt(i);
		mSelectRect.left = dfConvertLogicalXToScreen(xxx);
		mSelectRect.top = dfConvertLogicalYToScreen(yyy);
		mSelectRect.right = mSelectRect.left + SCATTER_DOT_SIZE;
		mSelectRect.bottom = mSelectRect.top + SCATTER_DOT_SIZE;
		if (mSelectRect.PtInRect(pt))
		{
			mSelectNode = i;
			break;
		}
	}
	return mSelectNode;
}

void CGraphWnd::InvalidDisplayArea()
{
	CRect theRect;
	theRect.left = mCoordItem.mScreen_OriginX;
	theRect.right = mCoordItem.mScreen_OriginX + mCoordItem.mScreenWidth;
	theRect.top = mCoordItem.mScreen_OriginY - mCoordItem.mScreenHeight;
	theRect.bottom = mCoordItem.mScreen_OriginY;
	InvalidateRect(theRect);
}

void CGraphWnd::DrawSelectNode(CDC* pDC)
{
	if (mYAixState != 3 || mGraphState != 1)
		return;
	if (mSelectNode != -1)
	{
		pDC->FillSolidRect(mSelectRect, RGB(0, 0, 255));
	}

}

void CGraphWnd::ReCalculateSelectRect()
{
	if (mYAixState != 3 || mGraphState != 1)
		return;
	if (mSelectNode != -1)
	{
		int Count = m_PointArrayX.GetSize();
		if (mSelectNode >= Count)
			return;
		double xxx, yyy;
		xxx = m_PointArrayX.GetAt(mSelectNode);
		yyy = m_PointArrayY.GetAt(mSelectNode);
		mSelectRect.left = dfConvertLogicalXToScreen(xxx);
		mSelectRect.top = dfConvertLogicalYToScreen(yyy);
		mSelectRect.right = mSelectRect.left + SCATTER_DOT_SIZE;
		mSelectRect.bottom = mSelectRect.top + SCATTER_DOT_SIZE;
	}

}

void CGraphWnd::DrawWave_DataDot(int chno, int hitIndex)
{
	if (chno != mChannelNo)
		return;

	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	HIT_ITEM_HEAD theHitHead;
	theHitHead = pApp->mHitManageP->GetHitItem(chno, hitIndex, mAmpBuf);
	if (theHitHead.iChannelNo == -1)
		return;
	int i,j,k,m;
	double x, y,temp,max;
	CString theStr;
//	theStr.Format("CGraphWnd::DrawWave_DataDot chno:%d,hitIndex:%d");
//	pApp->m_LogMgnP->Addlog(theStr);
	if (mYAixState == 1)
	{
		m_PointArrayX.RemoveAll();
		m_PointArrayY.RemoveAll();
		max = 0.0;
		k = -1;
		m = 0;
		for (i = 0,j = 0; i < theHitHead.iHitLength; i++)
		{
				x = (double)i / pApp->mSampleRate;
				y = *(mAmpBuf + i) * 1000;
				if (y > pApp->m_arrayChannel_Item[theHitHead.iChannelNo].iTiggerYuValue)
				{
					if (k == -1)
					{
						k = i;

					}
					j++;
				}
				if (y > max)
				{
					max = y;
					m = i;
				}

				m_PointArrayX.Add(x);
				m_PointArrayY.Add(y);
				//	theStr.Format("chno: %d dianya: %f,gain:%f", chno, y);
				//	pApp->m_LogMgnP->Addlog(theStr);

		}
		theStr.Format("chno: %d 序号: %d,振铃次数:%d,pdt:%d", chno, hitIndex,j,m-k);
		pApp->m_LogMgnP->Addlog(theStr);
	}
	else if (mYAixState == 2)
	{
		m_PointArrayX.RemoveAll();
		m_PointArrayY.RemoveAll();
		ConvertToFFT(mAmpBuf, theHitHead.iHitLength, &temp, mAmpFFTBuf);
		for (i = 0; i < theHitHead.iHitLength / 2; i++)
		{
				x = (double)i / pApp->mSampleTime;
				y = *(mAmpFFTBuf + i);
				if (y < mCoordItem.mAxisYMin)
					y = mCoordItem.mAxisYMin;
				m_PointArrayX.Add(x);
				m_PointArrayY.Add(y);
		}
	}
	InvalidDisplayArea();

}

void CGraphWnd::Start_DrawHistory_Scatter()
{
	if (mYAixState != 3)
		return;
	if (mGraphState == 0)
		return;
	mStartHistoryIndex = 0;
	m_PointArrayX.RemoveAll();
	m_PointArrayY.RemoveAll();
	mTimeHistory = SetTimer(0, 500, NULL);
}

LRESULT CGraphWnd::GraWnd_Draw_HisData(WPARAM x, LPARAM y)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	HIT_ITEM_HEAD theHitHead;
	theHitHead = pApp->mHitManageP->GetHitItem(mChannelNo, mStartHistoryIndex, mAmpBuf);
//	CString theStr;
//	theStr.Format("mChannelNo: %d theHitHead.iChannelNo: %d,mStartHistoryIndex:%d", mChannelNo, theHitHead.iChannelNo, mStartHistoryIndex);
//	pApp->m_LogMgnP->Addlog(theStr);

	if (theHitHead.iChannelNo == -1)
	{
		KillTimer(mTimeHistory);
		return 1;
	}
	if (theHitHead.douTot >= (double)mCoordItem.mAxisXMax)
	{
		mCoordItem.mAxisXMax += 30;
		SetCoordParamter();
	}
	double xx, yy,sum,temp;
	int i;

	xx = theHitHead.douTot;
	yy = 0.0;
	sum = 0.0;
	for (i = 0; i < theHitHead.iHitLength; i++)
	{
		temp = fabs(*(mAmpBuf + i));
		if (temp > yy)
			yy = temp;
		sum += temp;
	}
	//	ConvertToFFT(mAmpBuf, theHitHead.iHitLength, &y, mAmpFFTBuf);
	sum = sum / theHitHead.iHitLength;
	yy -= sum;
	yy = 20 * log10(yy * 1000 * 1000);
	yy -= (double)pApp->m_arrayChannel_Item[mChannelNo].iAmpGain;
	yy -= (double)pApp->m_arrayChannel_Item[mChannelNo].mSoftAmpGain;

	if (pApp->mDeZaoshangFlag)
	{
		if (yy > 29.0 && yy < 40.0)
			yy -= 10.0;
		else if (yy < 50.0)
			yy -= 20;
	}
	AddXY(xx, yy);

	mStartHistoryIndex++;
	Invalidate();
	return 0;
}


void CGraphWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL bSuc = false;
	switch (nChar)
	{
		case VK_LEFT :
			if (mCoordItem.mAxisXMin - 1 > mCoordItem.mInitAxisXMin)
			{
				mCoordItem.mAxisXMin--;
				mCoordItem.mAxisXMax--;
				bSuc = true;
			}
			break;
		case VK_RIGHT:
			if (mCoordItem.mAxisXMax + 1 < mCoordItem.mInitAxisXMax)
			{
				mCoordItem.mAxisXMax++;
				mCoordItem.mAxisXMin++;
				bSuc = true;
			}
			break;
		case VK_UP:
			if (mCoordItem.mAxisYMax + 1 < mCoordItem.mInitAxisYMax)
			{
				mCoordItem.mAxisYMin++;
				mCoordItem.mAxisYMax++;
				bSuc = true;
			}
			break;
		case VK_DOWN:
			if (mCoordItem.mAxisYMin - 1 > mCoordItem.mInitAxisYMin)
			{
				mCoordItem.mAxisYMin--;
				mCoordItem.mAxisYMax--;
				bSuc = true;
			}
			break;
		default:break;
	}
	if (bSuc)
		Invalidate();
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGraphWnd::UpdateCoordPara()
{
	SetWndTitle();
	Invalidate();
}

