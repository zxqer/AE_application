#pragma once


#define WM_LOCATION_PROCESS  WM_USER+300

typedef struct tagSENSOR_ARRIVE {
	int    iChNo;        //探头通道号
	double   duTime;       //到达时间
	double  aveFuzhi;     //平均幅值
}SENSOR_ARRIVE, *SENSOR_ARRIVEP;

// CLocationView 视图
class CSensorWnd;
class CLocSignalWnd;
class CLocationView : public CView
{
	DECLARE_DYNCREATE(CLocationView)

	void  UpdateRealData(int chno, int index);
	void  NotifyStartLocation();
	void  LocationAnalysis();

	SENSOR_ARRIVE  mSensorArrive[MAX_CHANNELS];
	int mSensorArriveCount;
	double  closeTime;  //事件闭锁时间
	int mScreenWidth, mScreenHeight;
	int mS1, mS2, mS3, mS4; //横轴s1 -- s2 ; 纵轴 s3  - s4

	int  mLocationflag;

	void ConstrainToBoundary(int& x, int& y, int xMin, int xMax, int yMin, int yMax);
protected:
	CLocationView();           // 动态创建所使用的受保护的构造函数
	virtual ~CLocationView();

	CSensorWnd * pSensorWnd;
	CListCtrl * pListCtrl;
	CLocSignalWnd* pLocSignalWnd;
	int LocSignalWnd_showFlag;
	void   InitListCtrl();
	void   AdjustListColumnWidth();
	void   SeekS1S3();
	int   GetSensorArriveIndex(int chNo);
	double   AddListCtrlHit(int Chno,int HitIndex, double EnergySum,int zlCnt,double maxAmp,int pdt, int plt, int arrivet, double tot);
	void   AnalysisAEData(int chno, int hitIndex);
	int	   Analysis_Audio_Signal(int start, int maxbufLen, double * pAmBuf,double * outSum, double triggerYuzhi, int * outZlCnt,double * maxAmp,int* pdt, int * arrivetime);
	double  *mAmpBuf;
	void   AddSensor(int chno, double duTime,double fuzhi);


public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT onLocationProcessData(WPARAM x, LPARAM y);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLocSignal();
};


