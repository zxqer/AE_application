#pragma once


// CAETabView 视图

class CAETabView : public CTabView
{

protected:
	CAETabView();           // 动态创建所使用的受保护的构造函数
	DECLARE_DYNCREATE(CAETabView)

	virtual ~CAETabView();

	CStringArray   mStrPageNameArray;
public:
	void  AddPage();
	void  DeletePage();
	void  SetActivePageName(CString inPageName);
	void  SetPageNameByIndex(int index,CString inPageName);
	void  AddLocationView();
	void  RemoveLocationView(int flag);

	int  mLocationViewIndex;
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void Serialize(CArchive& ar);
};


