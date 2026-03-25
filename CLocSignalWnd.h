#pragma once
#include "TeeChart.h"

// CLocSignalWnd

#define MAX_TEECHART_GRA  4
#define MAX_EVENT_ARRIVE  8


typedef struct tagEVENT_ARRIVE {
	int    iChNo;        //通道号
	int    iHitIndex;       //撞击序号
	int    iArriveTime;     //到达时间
}EVENT_ARRIVE, * EVENT_ARRIVEP;


class CLocSignalWnd : public CWnd
{
	DECLARE_DYNAMIC(CLocSignalWnd)

public:
	CLocSignalWnd();
	virtual ~CLocSignalWnd();

	int mWndClientWidth,mWndClientHeight;

	CTeeChart mTeeChart[MAX_TEECHART_GRA];
	EVENT_ARRIVE mArrEvent_Arrive[MAX_EVENT_ARRIVE];
	int mEvent_Arrive_Count,mStartIndex;

	void  SetTeeChartCoordParamter();
	void  AddEventArrive(int Chno, int HitIndex, int ArriveTime);
	void  ShowEventData();
	void  DrawChno(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};


