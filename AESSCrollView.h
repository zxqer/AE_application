#pragma once



// CAESSCrollView 视图
#define WM_NOTIFY_GRAWND_DRAW_DOTDATA  WM_USER+200


class CAESSCrollView : public CScrollView
{
	DECLARE_DYNCREATE(CAESSCrollView)



protected:
	CAESSCrollView();           // 动态创建所使用的受保护的构造函数
	virtual ~CAESSCrollView();

public:

	CObArray  mObArray;
	int  mWndIDIndex;
	int  mWndWidth, mWndHeight;
	int  mLogFlag;

	CView * GetTabView();
	void  DeleteAllGraphWnd();
	void  UpdateRealData(int chno,int index);
	void  NotifyStartPlay();
	void  NotifyStartDrawHistory();
	void  NotifyUpdateCoord();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
public:
	virtual void Serialize(CArchive& ar);
	afx_msg void OnNewGraph();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnChildActivate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnAutoGrawnd();
	afx_msg LRESULT NotifyGraWnd_Draw_DataDot(WPARAM x, LPARAM y);
	afx_msg void OnDeleteGrawnd();
};


