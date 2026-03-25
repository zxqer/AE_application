// AETabView.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "AETabView.h"
#include "AESSCrollView.h"
#include "LocationView.h"
#include "LogMgn.h"



// CAETabView

IMPLEMENT_DYNCREATE(CAETabView, CTabView)

BEGIN_MESSAGE_MAP(CAETabView, CTabView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


CAETabView::CAETabView()
{
	mLocationViewIndex = -1;
}

CAETabView::~CAETabView()
{
}



// CAETabView 诊断

#ifdef _DEBUG
void CAETabView::AssertValid() const
{
	CTabView::AssertValid();
}

#ifndef _WIN32_WCE
void CAETabView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAETabView 消息处理程序




void  CAETabView::AddPage()
{
	CString theStr;
	theStr = _T("新页面");
	mStrPageNameArray.Add(theStr);
	AddView(RUNTIME_CLASS(CAESSCrollView),theStr, -1);
}




int CAETabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CString theStr;
//	theStr = _T("定位");
//	AddView(RUNTIME_CLASS(CLocationView), theStr, -1);
	theStr = _T("新页面");
	mStrPageNameArray.Add(theStr);
	AddView(RUNTIME_CLASS(CAESSCrollView), theStr, -1);
	return 0;
}

void  CAETabView::AddLocationView()
{
	if (mLocationViewIndex != -1)
	{
		SetActiveView(mLocationViewIndex);
	}
	else
	{
		CString theStr;
		theStr = _T("定位");
		mLocationViewIndex = AddView(RUNTIME_CLASS(CLocationView), theStr, -1);
		SetActiveView(mLocationViewIndex);
	}
}

void CAETabView::RemoveLocationView(int flag)
{
#if 1
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	theStr.Format("RemoveLocationView :%d!!!",flag);
	pApp->m_LogMgnP->Addlog(theStr);
#endif
	SetActiveView(0);
	if(mLocationViewIndex != -1)
		RemoveView(mLocationViewIndex);
	mLocationViewIndex = -1;
}


void CAETabView::Serialize(CArchive& ar)
{
	CMFCTabCtrl&  pTabWnd = GetTabControl();
	CAESSCrollView*  pView;
	CString theStr;
	int i,j,Count,oldcnt,removecnt,addcnt;
	if (ar.IsStoring())
	{	// storing code
		if (mLocationViewIndex != -1)
			RemoveLocationView(1);
		Count = pTabWnd.GetTabsNum();
		ar << Count;
		for (i = 0; i < Count; i++)
		{
			theStr = mStrPageNameArray.GetAt(i);
			ar << theStr;
			pView = (CAESSCrollView*)pTabWnd.GetTabWnd(i);
			pView->Serialize(ar);
		}
	}
	else
	{	// loading code
		if (mLocationViewIndex != -1)
			RemoveLocationView(2);
		ar >> Count;
		oldcnt = mStrPageNameArray.GetCount();
		if (oldcnt > Count)
		{
			removecnt = oldcnt - Count;
			for (j = 0; j < removecnt; j++)
			{
				RemoveView(Count);

			}

		}
		else if(oldcnt < Count)
		{
			addcnt = Count - oldcnt;
			for (j = 0; j < addcnt; j++)
			{
				theStr = _T("新页面");
				AddView(RUNTIME_CLASS(CAESSCrollView), theStr, -1);
			}
		}
		mStrPageNameArray.RemoveAll();
		for (i = 0; i < Count; i++)
		{
			ar >> theStr;
			SetPageNameByIndex(i,theStr);
			mStrPageNameArray.Add(theStr);
			pView = (CAESSCrollView*)pTabWnd.GetTabWnd(i);
			pView->Serialize(ar);
		}

	}
}

void CAETabView::SetActivePageName(CString inPageName)
{
	CMFCTabCtrl&  pTabWnd = GetTabControl();
	int index = pTabWnd.GetActiveTab();
	pTabWnd.SetTabLabel(index, inPageName);
	mStrPageNameArray.SetAt(index, inPageName);

}

void  CAETabView::SetPageNameByIndex(int index, CString inPageName)
{
	CMFCTabCtrl&  pTabWnd = GetTabControl();
	pTabWnd.SetTabLabel(index, inPageName);

}




void CAETabView::DeletePage()
{
	// TODO: 在此添加命令处理程序代码
	if (mStrPageNameArray.GetSize() > 1)
	{
		CMFCTabCtrl&  pTabWnd = GetTabControl();
		CAESSCrollView * pView = (CAESSCrollView *)GetActiveView();
		pView->DeleteAllGraphWnd();
		int index = pTabWnd.GetActiveTab();
		RemoveView(index);
	}
}
