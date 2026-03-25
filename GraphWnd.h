#pragma once

#include "fftw3.h"

// CGraphWnd
#define WM_GRAWND_PROCESS_DATA WM_USER+100
#define WM_GRAWND_DRAW_HISDATA WM_USER+101


typedef struct tagCoord_ITEM {
	int    mWndClientWidth;    ////窗口客户区宽度
	int    mWndClientHeight;    //窗口客户区高度
	int    mScreenWidth;     // 屏幕显示区宽度
	int    mScreenHeight;    //屏幕区高度
	int    mScreen_OriginX;     //屏幕显示原点x
	int    mScreen_OriginY;     //屏幕显示原点y
	int  mAxisXMin, mAxisXMax;     //X轴最小和最大物理量
	int  mAxisYMin, mAxisYMax;     //Y轴最小和最大物理量
	int  mInitAxisXMin, mInitAxisXMax;     //X轴初始化最小和最大物理量
	int  mInitAxisYMin, mInitAxisYMax;     //Y轴初始化最小和最大物理量
	int  mAxisXUint;        //X物理单位   
	int  mAxisYUint;        //Y物理单位   
}COORD_Item, *COORD_ITEM;


class CGraphWnd : public CWnd
{
	DECLARE_DYNAMIC(CGraphWnd)

public:
	CGraphWnd();
	virtual ~CGraphWnd();

	int   mGraphState;   //0  波形图；  1： 二维散点图；
	int   mYAixState;     // 0 撞击   1： 电压毫伏   2： 电源dB   3：幅值dB
	int   mChannelNo;     //-1 所有通道
	COORD_Item  mCoordItem;

	CArray<double, double&> m_PointArrayX;
	CArray<double, double&> m_PointArrayY;
	void  AddXY(double x, double y);
	double  *mAmpBuf;
	double  *mAmpFFTBuf;
	fftw_complex* m_outBuf;

	int mStartHistoryIndex;
	UINT_PTR  mTimeHistory;

	CPoint mStartPt, mEndPt;
	int mSelectNode;  //当前选中点   
	CRect  mSelectRect;
	void  DrawMouseMoveRect();
	int   JudgeIfSelectNode(CPoint pt);
	void  DrawSelectNode(CDC* pDC);
	void  ReCalculateSelectRect();

   void  UpdateAEData(int chno, int hitIndex);
   void  DrawWave_DataDot(int chno, int hitIndex);

   void  UpdateArtPara();    //更新采集参数
   void  UpdateCoordPara();    //更新坐标参数
   void  NotifyStartPlay();
   void  ConvertToFFT(double inBuf[], int Len, double * outMaxAmp, double * outAmpBuf);
   void  AddSensorData_Show();
   void  ClearData();
   void  Start_DrawHistory_Scatter();

   void  InvalidDisplayArea();
   void  SetWndTitle();
   void  SetCoordParamter();
   void  DrawAxisAndText(CDC* pDC);
   void  DrawWaveform_graph(CDC* pDC);   //显示波形图
   void  Draw2D_Scatter(CDC* pDC);   //显示二维散点图

	CPoint  ptConvertScreenToLogical(CPoint pt);
	CPoint  ptConvertLogicalToScreen(CPoint pt);
	int    dfConvertLogicalXToScreen(double x);
	int    dfConvertLogicalYToScreen(double y);

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void Serialize(CArchive& ar);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetGraph();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT GraWnd_Process_Data(WPARAM x, LPARAM y);
	afx_msg LRESULT GraWnd_Draw_HisData(WPARAM x, LPARAM y);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


