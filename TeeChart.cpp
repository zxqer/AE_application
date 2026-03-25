#include "stdafx.h"
#include "TeeChart.h"

#define  TEE_COLOR_AXIS    RGB(212,212,212)
#define TEE_AXIS_SCREEN_DARW_LEN    10  //×ø±êÖáÆÁÄ»ÏßÑÓÉê³¤¶È  
#define TEE_WND_MARGIN          50   //´°¿Ú±ß¿Õ  

#define TEE_AXIS_SCREEN_UNITX    80  //×ø±êÖáÆÁÄ»µ¥Î»  
#define TEE_AXIS_SCREEN_UNITY    30  //×ø±êÖáÆÁÄ»µ¥Î»  

#define  TEE_LINE_WIDTH  2


CTeeChart::CTeeChart()
{
	XAxis_ShowFlag = 0;
	mStartTime = 0.0;
	audio_start_pos = -1;
}


CTeeChart::~CTeeChart()
{
}

void CTeeChart::SetCoordinate_System(CRect inRect,int AxisXMin,int AxisXMax, int AxisYMin, int AxisYMax)
{
	mAxisScreenRect = inRect;
	mAxisYMin = AxisYMin;
	mAxisYMax = AxisYMax;
	mAxisXMin = AxisXMin;
	mAxisXMax = AxisXMax;
	SetCoordParamter();

}

void CTeeChart::SetYCoordinate(int AxisYMin, int AxisYMax)
{
	mAxisYMin = AxisYMin;
	mAxisYMax = AxisYMax;
	SetCoordParamter();
}


void  CTeeChart::SetCoordParamter()
{
	if (mAxisXMin >= 0)
		mScreen_OriginX = mAxisScreenRect.left;
	else
		mScreen_OriginX = mAxisScreenRect.left + mAxisScreenRect.Width() / 2;

	if (mAxisYMin >= 0)
		mScreen_OriginY = mAxisScreenRect.bottom;
	else
		mScreen_OriginY = mAxisScreenRect.bottom - mAxisScreenRect.Height() / 2;

	int xxx = TEE_AXIS_SCREEN_UNITX;
	while (1)
	{
		mAxisXUint = (int)((double)(mAxisXMax - mAxisXMin) / mAxisScreenRect.Width()*xxx);
		if (mAxisXUint <= 5)
			break;
		if ((mAxisXUint % 5) == 0)
			break;
		xxx++;
	}
	if (mAxisXUint == 0) mAxisXUint = 1;
	xxx = TEE_AXIS_SCREEN_UNITY;
	while (1)
	{
		mAxisYUint = (int)((double)(mAxisYMax - mAxisYMin) / mAxisScreenRect.Height() * xxx);
		if (mAxisYUint <= 5)
			break;
		if ((mAxisYUint % 5) == 0)
			break;
		xxx++;
	}
	if (mAxisYUint == 0) mAxisYUint = 1;

}


void CTeeChart::SetLineColor(DWORD inColor)
{
	mLineColor = inColor;
}

void CTeeChart::SetGraphState(int inState)
{
	mGraphState = inState;
}


int  CTeeChart::dfConvertLogicalXToScreen(double x)
{
	int rex;
	x = x * mAxisScreenRect.Width() / (mAxisXMax - mAxisXMin);
	rex = (int)x + mScreen_OriginX;
	return rex;
}
int CTeeChart::dfConvertLogicalYToScreen(double y)
{
	int rey;
	y = y *  mAxisScreenRect.Height() / (mAxisYMax - mAxisYMin);
	rey = mScreen_OriginY - (int)y;
	return rey;
}

CPoint  CTeeChart::ptConvertLogicalToScreen(CPoint pt)
{
	CPoint rePt;
	double x, y;
	x = (double)pt.x * mAxisScreenRect.Width() / (mAxisXMax - mAxisXMin);
	y = (double)pt.y *  mAxisScreenRect.Height() / (mAxisYMax - mAxisYMin);
	rePt.x = (int)x + mScreen_OriginX;
	rePt.y = mScreen_OriginY - (int)y;
	return rePt;
}

void  CTeeChart::OutputAxis(CDC* pDC)
{
	pDC->FillSolidRect(mAxisScreenRect, TEE_COLOR_AXIS);
	CPen NewPen1(PS_SOLID, 1, RGB(235, 235, 235));
	CPen *pOldPen = pDC->SelectObject(&NewPen1);
	CPoint ScreenPt, LogicalPt;

	int i, start = mAxisXUint;
	int logicalWidth = mAxisXMax - mAxisXMin;
	int logicalHeight = mAxisYMax - mAxisYMin;
	for (i = 0; i < logicalWidth / mAxisXUint; i++)
	{
		LogicalPt.x = start;
		LogicalPt.y = mAxisYMin;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);

		LogicalPt.y = mAxisYMax;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mAxisXUint;
	}
	start = 0;
	while (start <= mAxisYMax)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		LogicalPt.x = logicalWidth;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mAxisYUint;
	}
	start = -mAxisYUint;
	while (start >= mAxisYMin)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		LogicalPt.x = logicalWidth;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start -= mAxisYUint;
	}

	pDC->SelectObject(pOldPen);
	if (XAxis_ShowFlag)
	{
		CPen NewPen2(PS_SOLID, 1, RGB(0, 0, 0));
		pOldPen = pDC->SelectObject(&NewPen2);
		start = 0;
		for (i = 0; i <= logicalWidth / mAxisXUint; i++)
		{
			LogicalPt.x = start;
			LogicalPt.y = mAxisYMin;
			ScreenPt = ptConvertLogicalToScreen(LogicalPt);
			pDC->MoveTo(ScreenPt.x, ScreenPt.y);
			ScreenPt.y += TEE_AXIS_SCREEN_DARW_LEN;
			pDC->LineTo(ScreenPt.x, ScreenPt.y);

			start += mAxisXUint;
		}
	}

	start = 0;
	while (start <= mAxisYMax)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		ScreenPt.x -= TEE_AXIS_SCREEN_DARW_LEN;
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start += mAxisYUint;
	}
	start = -mAxisYUint;
	while (start >= mAxisYMin)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		pDC->MoveTo(ScreenPt.x, ScreenPt.y);
		ScreenPt.x -= TEE_AXIS_SCREEN_DARW_LEN;
		pDC->LineTo(ScreenPt.x, ScreenPt.y);
		start -= mAxisYUint;
	}

	pDC->SelectObject(pOldPen);

	CString theStr;
	int yValue = 0;
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	start = 0;
	while (start <= mAxisYMax)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		ScreenPt.x -= TEE_WND_MARGIN;
		ScreenPt.y -= 10;
		theStr.Format("%d", yValue);
		pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
		start += mAxisYUint;
		yValue += mAxisYUint;
	}
	start = -mAxisYUint;
	yValue = -mAxisYUint;
	while (start >= mAxisYMin)
	{
		LogicalPt.x = 0;
		LogicalPt.y = start;
		ScreenPt = ptConvertLogicalToScreen(LogicalPt);
		ScreenPt.x -= TEE_WND_MARGIN;
		ScreenPt.y -= 10;
		theStr.Format("%d", yValue);
		pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
		start -= mAxisYUint;
		yValue -= mAxisYUint;

	}

	if (XAxis_ShowFlag)
	{
		pDC->SetTextColor(RGB(0, 0, 0));
		yValue = mAxisXMin;
		start = 0;
		for (i = 0; i <= logicalWidth / mAxisXUint; i++)
		{
			LogicalPt.x = start;
			LogicalPt.y = mAxisYMin;
			ScreenPt = ptConvertLogicalToScreen(LogicalPt);
			ScreenPt.y += TEE_AXIS_SCREEN_DARW_LEN;
			ScreenPt.x -= 5;
			theStr.Format("%d", yValue);
			pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
			yValue += mAxisXUint;
			start += mAxisXUint;
		}
	}
}

void  CTeeChart::OnlyAddData(double x, double y)
{
	if (x >= (double)mAxisXMax)
	{
		mAxisXMax += 30;
		SetCoordParamter();
	}
	if (y < mAxisYMin)
		y = mAxisYMin;
	if (y > mAxisYMax)
		y = mAxisYMax;

	m_PointArrayX.Add(x);
	m_PointArrayY.Add(y);

}

int CTeeChart::AddData(CDC * pDC,double x, double y)
{
	int reflag = 0;
	if (x >= (double)mAxisXMax)
	{
		mAxisXMax += 30.0;
		SetCoordParamter();
		reflag = 1;
	}

	if (y < mAxisYMin)
		y = mAxisYMin;
	if (y > mAxisYMax)
		y = mAxisYMax;
	m_PointArrayX.Add(x);
	m_PointArrayY.Add(y);
	double xxx, yyy;
	int x1, y1;
	int count = m_PointArrayX.GetSize();
	if (reflag == 0 && count >=2)
	{
		CPen NewPen1(PS_SOLID, TEE_LINE_WIDTH, mLineColor);
		CPen *pOldPen = pDC->SelectObject(&NewPen1);
		xxx = m_PointArrayX.GetAt(count-2);
		yyy = m_PointArrayY.GetAt(count-2);
		x1 = dfConvertLogicalXToScreen(xxx);
		y1 = dfConvertLogicalYToScreen(yyy);
		pDC->MoveTo(x1, y1);
		xxx = m_PointArrayX.GetAt(count - 1);
		yyy = m_PointArrayY.GetAt(count - 1);
		x1 = dfConvertLogicalXToScreen(xxx);
		y1 = dfConvertLogicalYToScreen(yyy);
		pDC->LineTo(x1, y1);

		pDC->SelectObject(pOldPen);
	}

	return reflag;

}

void  CTeeChart::DrawWaveform_graph(CDC* pDC)
{
	int i, Count = m_PointArrayX.GetSize();
	if (Count == 0)
		return;
	double xxx, yyy;
	int x, y;
	CPen NewPen1(PS_SOLID, TEE_LINE_WIDTH, mLineColor);
	CPen *pOldPen = pDC->SelectObject(&NewPen1);
	for (i = 0; i < Count; i++)
	{
		xxx = m_PointArrayX.GetAt(i);
		yyy = m_PointArrayY.GetAt(i);
		x = dfConvertLogicalXToScreen(xxx);
		y = dfConvertLogicalYToScreen(yyy);
		if (i == 0)
			pDC->MoveTo(x, y);
		else
			pDC->LineTo(x, y);
	}
	pDC->SelectObject(pOldPen);

}
void CTeeChart::DrawBar_graph(CDC* pDC)
{
	int i, Count = m_PointArrayX.GetSize();
	if (Count == 0)
		return;
	double xxx, yyy;
	int x, y;
	CPen NewPen1(PS_SOLID, 1, mLineColor);
	CPen *pOldPen = pDC->SelectObject(&NewPen1);
	for (i = 0; i < Count; i++)
	{
		xxx = m_PointArrayX.GetAt(i);
		yyy = m_PointArrayY.GetAt(i);
		x = dfConvertLogicalXToScreen(xxx);
		y = dfConvertLogicalYToScreen(yyy);
		pDC->MoveTo(x, y);
		yyy = 0;
		y = dfConvertLogicalYToScreen(yyy);
		pDC->LineTo(x, y);
	}
	pDC->SelectObject(pOldPen);
}

void CTeeChart::OnDraw(CDC* pDC)
{
	if (mGraphState == 0)
	{
		OutputAxis(pDC);
		DrawWaveform_graph(pDC);
		DrawAudio_Start_Pos(pDC);
	}
	else
	{
		OutputAxis(pDC);
		DrawBar_graph(pDC);
	}

}

void CTeeChart::SetAxis_ShowFlag(int show_flag)
{
	XAxis_ShowFlag = show_flag;
}

void CTeeChart::ClearData()
{
	m_PointArrayX.RemoveAll();
	m_PointArrayY.RemoveAll();
//	mStartTime = 0.0;
}

void CTeeChart::DrawAudio_Start_Pos(CDC* pDC)
{
	if (audio_start_pos == -1)
		return;
	CPen NewPen1(PS_SOLID, TEE_LINE_WIDTH, RGB(255,0,0));
	CPen* pOldPen = pDC->SelectObject(&NewPen1);
	int x, y;
	x = dfConvertLogicalXToScreen(audio_start_pos);
	y = dfConvertLogicalYToScreen(0.0);
	pDC->MoveTo(x, y);

	y = dfConvertLogicalYToScreen((double)mAxisYMax*2/3);

	pDC->LineTo(x, y);

	pDC->SelectObject(pOldPen);
}




