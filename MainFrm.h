
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#define WM_AESMOUSE_LBUTTON_UP WM_USER+1
#define WM_AES_UPDATE_VIEW WM_USER+2
#define WM_AES_DRAW_HISTORY WM_USER+3


class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

	void  NotifyAllViewUpdate_ARTPara();
	void  NotifyAllViewUpdate_CoordPara();
	void  SetDocModifiedFlag();

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNewPage();
	afx_msg void OnRenamePage();
	afx_msg LRESULT AESMouseLbuttonUp(WPARAM, LPARAM);
	afx_msg LRESULT AES_UpdateView(WPARAM x, LPARAM y);
	afx_msg LRESULT AES_DrawHistory(WPARAM x, LPARAM y);
	afx_msg void OnDeletePage();
	afx_msg void OnStartPlay();
	afx_msg void OnStopPlay();
	afx_msg void OnReplayHistory();
	afx_msg void OnRefreshAespage();
	afx_msg void OnStartLocation();
	afx_msg void OnStopLocation();
	afx_msg void OnUpdateStopPlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStopLocation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStartLocation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStartPlay(CCmdUI *pCmdUI);
	afx_msg void OnSaveText();
};


