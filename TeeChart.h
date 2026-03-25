#pragma once
class CTeeChart
{
public:
	CTeeChart();
	~CTeeChart();

	CRect  mAxisScreenRect;     // 坐标轴显示区
	int    mScreen_OriginX;     //坐标轴原点x
	int    mScreen_OriginY;     //坐标轴原点y
	int  mAxisYMin, mAxisYMax;     //Y轴最小和最大物理量
	int  mAxisXMin, mAxisXMax;     //X轴最小和最大物理量
	int  mAxisXUint;        //X物理单位   
	int  mAxisYUint;        //Y物理单位   

	int   mGraphState;   //0  波形图；  1： 柱状图；

	DWORD   mLineColor;
	int XAxis_ShowFlag;
	double  audio_start_pos;//声发射起点位置

	double mStartTime;

	CArray<double, double&> m_PointArrayX;
	CArray<double, double&> m_PointArrayY;

	int  AddData(CDC * pDC,double x, double y);
	void  OnlyAddData(double x, double y);
	void  SetCoordParamter();

	void  SetCoordinate_System(CRect inRect, int AxisXMin, int AxisXMax, int AxisYMin, int AxisYMax);
	void  SetYCoordinate(int AxisYMin, int AxisYMax);
	void  SetLineColor(DWORD inColor);
	void  SetGraphState(int inState);
	void  SetAxis_ShowFlag(int show_flag);
	int    dfConvertLogicalXToScreen(double x);
	int    dfConvertLogicalYToScreen(double y);
	CPoint  ptConvertLogicalToScreen(CPoint pt);
	void  ClearData();

    void  OnDraw(CDC* pDC);      
	void  OutputAxis(CDC* pDC);
	void  DrawWaveform_graph(CDC* pDC);
	void  DrawBar_graph(CDC* pDC);
	void  DrawAudio_Start_Pos(CDC* pDC);
};

