// AESSCrollView.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "AESSCrollView.h"
#include "GraphWnd.h"
#include "LogMgn.h"


// CAESSCrollView

IMPLEMENT_DYNCREATE(CAESSCrollView, CScrollView)

CAESSCrollView::CAESSCrollView()
{
	mWndIDIndex = 32802;
	mLogFlag = 0;
}

CAESSCrollView::~CAESSCrollView()
{
}


BEGIN_MESSAGE_MAP(CAESSCrollView, CScrollView)
	ON_COMMAND(ID_NEW_GRAPH, &CAESSCrollView::OnNewGraph)
	ON_WM_SIZE()
	ON_WM_CHILDACTIVATE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_AUTO_GRAWND, &CAESSCrollView::OnAutoGrawnd)
	ON_MESSAGE(WM_NOTIFY_GRAWND_DRAW_DOTDATA, NotifyGraWnd_Draw_DataDot)
	ON_COMMAND(ID_DELETE_GRAWND, &CAESSCrollView::OnDeleteGrawnd)
END_MESSAGE_MAP()


// CAESSCrollView 绘图

void CAESSCrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
//	SetTimer(0, 1000, NULL);

}

void CAESSCrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码

#if 0
	CGraphWnd * pWnd;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->Invalidate();
	}
#endif
}

CView * CAESSCrollView::GetTabView()
{
	CMFCTabCtrl* pTabWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, GetParent());
	ASSERT_VALID(pTabWnd);

	CView *pParentView = (CView*)pTabWnd->GetParent();

	ASSERT_VALID(pParentView);
	// CAESWinView 消息处理程序
	return pParentView;
}


// CAESSCrollView 诊断

#ifdef _DEBUG
void CAESSCrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CAESSCrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAESSCrollView 消息处理程序


void CAESSCrollView::Serialize(CArchive& ar)
{
	CGraphWnd * pWnd;
	int i, Count, oldCnt;
	CRect theRect;
	if (ar.IsStoring())
	{	// storing code
		Count = mObArray.GetCount();
		ar << Count;
		for (i = 0; i < Count; i++)
		{
			pWnd = (CGraphWnd*)mObArray.GetAt(i);
			pWnd->GetWindowRect(theRect);
			pWnd->Serialize(ar);
			ar << theRect;
		}
	}
	else
	{	// loading code
		oldCnt = mObArray.GetCount();
		for (i = 0; i < oldCnt; i++)
		{
			pWnd = (CGraphWnd*)mObArray.GetAt(i);
			delete pWnd;
		}
		mObArray.RemoveAll();
		ar >> Count;
		CRect rc;
		for (i = 0; i < Count; i++)
		{
			pWnd = new CGraphWnd;
			pWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_THICKFRAME | WS_CAPTION, rc, this, mWndIDIndex);
			pWnd->Serialize(ar);
			mObArray.Add(pWnd);
			mWndIDIndex++;
			ar >> theRect;
			ScreenToClient(theRect);
			pWnd->MoveWindow(theRect);
		}
	}
}


void CAESSCrollView::OnNewGraph()
{
	// TODO: 在此添加命令处理程序代码
	CGraphWnd * pWnd;
	CRect rc;
	pWnd = new CGraphWnd;
	rc.left = 100;
	rc.top = 100;
	rc.bottom = (mWndHeight - 100) / 2 + 100;
	rc.right = (mWndWidth - 100) / 2 + 100;
	CString theStr;
	theStr = _T("波形");

	pWnd->Create(NULL, theStr, WS_VISIBLE | WS_CHILD | WS_THICKFRAME | WS_CAPTION, rc, this, mWndIDIndex);
	mWndIDIndex++;
	mObArray.Add(pWnd);

	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	theStr.Format("创建图形窗口：%x", pWnd->m_hWnd);
	pApp->m_LogMgnP->Addlog(theStr);
	SIZE  theSize;
	theSize.cx = mWndWidth * 2;
	theSize.cy = mWndHeight;
	SetScrollSizes(MM_TEXT, theSize);
	mLogFlag = 1;

}


void CAESSCrollView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	mWndWidth = cx;
	mWndHeight = cy;
	// TODO: 在此处添加消息处理程序代码
}


BOOL CAESSCrollView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (CScrollView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	else
	{

		CGraphWnd * pWnd;
		CString theStr;
		pWnd = (CGraphWnd *)GetFocus();
		if (pWnd != NULL)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CGraphWnd)))
			{
				CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
				//	theStr.Format("获取活动窗口：%x", pWnd->m_hWnd);
				//	pApp->m_LogMgnP->Addlog(theStr);
				return pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
			}
		}

	}
	return FALSE;

}

void  CAESSCrollView::DeleteAllGraphWnd()
{
	CGraphWnd * pWnd;
	int i,oldCnt = mObArray.GetCount();
	for (i = 0; i < oldCnt; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		delete pWnd;
	}
	mObArray.RemoveAll();

}


void CAESSCrollView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_nMapMode = MM_TEXT;
	CScrollView::OnPrepareDC(pDC, pInfo);
}


void CAESSCrollView::OnChildActivate()
{
	CScrollView::OnChildActivate();

	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	theStr.Format("CAESSCrollView OnChildActivate");
	pApp->m_LogMgnP->Addlog(theStr);

	// TODO: 在此处添加消息处理程序代码
}

void  CAESSCrollView::UpdateRealData(int chno, int index)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	CGraphWnd * pWnd;
	if (pApp->mAppState != 1)
		return;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->PostMessage(WM_GRAWND_PROCESS_DATA, chno, index);
	//	theStr.Format("CAESSCrollView UpdateRealData Chno :%d hitindex :%d", chno, index);
//		pApp->m_LogMgnP->Addlog(theStr);

	}

}

void  CAESSCrollView::NotifyStartPlay()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	CGraphWnd * pWnd;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->NotifyStartPlay();
	}

}


void CAESSCrollView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	CGraphWnd * pWnd;
	int i,Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->UpdateArtPara();
	}
}


void CAESSCrollView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	theStr.Format("CAESSCrollView ::OnLButtonUp");
	pApp->m_LogMgnP->Addlog(theStr);

	CScrollView::OnLButtonUp(nFlags, point);
}


void CAESSCrollView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	Invalidate();
	CScrollView::OnTimer(nIDEvent);
}


void CAESSCrollView::OnAutoGrawnd()
{
	// TODO: 在此添加命令处理程序代码
	int i,j,Rows,Cols,Count = mObArray.GetCount();
	CGraphWnd * pWnd;
	CPoint  startPt;
	int graWidth, graHeight,cc;
	if (Count == 1)
	{
		Rows = 1;
		Cols = 1;
	}
	else if (Count == 2)
	{
		Rows = 1;
		Cols = 2;
	}
	else if (Count == 3)
	{
		Rows = 1;
		Cols = 3;
	}
	else if (Count == 4 )
	{
		Rows = 2;
		Cols = 2;
	}
	else if (Count == 8)
	{
		Rows = 2;
		Cols = 4;
	}
	else
	{
		Rows = 3;
		Cols = 3;
	}
	CString theStr;
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();

	theStr.Format("OnAutoGrawnd Count:%d,Rows:%d,Cols:%d", Count, Rows, Cols);
	pApp->m_LogMgnP->Addlog(theStr);

	startPt.y = 0;
	graWidth = mWndWidth / Cols;
	graHeight = mWndHeight / Rows;
	for (i = 0; i < Rows; i++)
	{
		startPt.x = 0;
		for (j = 0; j < Cols; j++)
		{
			cc = i * Cols + j;
			if (cc < Count)
			{
				pWnd = (CGraphWnd*)mObArray.GetAt(cc);

				pWnd->MoveWindow(startPt.x, startPt.y, graWidth, graHeight, TRUE);
			//	theStr.Format("index:%d,startPt.x:%d,startPt.y:%d,graWidth:%d,graHeight:%d", cc, startPt.x, startPt.y, graWidth,graHeight);
			//	pApp->m_LogMgnP->Addlog(theStr);
				startPt.x += graWidth;
			}
			else
				break;
		}
		startPt.y += graHeight;
	}
}

LRESULT CAESSCrollView::NotifyGraWnd_Draw_DataDot(WPARAM x, LPARAM y)
{

	CGraphWnd * pWnd;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->DrawWave_DataDot(x,y);
	}
	return 0;
}

void CAESSCrollView::NotifyStartDrawHistory()
{
	CGraphWnd * pWnd;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->Start_DrawHistory_Scatter();
	}
	return;
}

void CAESSCrollView::NotifyUpdateCoord()
{
	CGraphWnd * pWnd;
	int i, Count = mObArray.GetCount();
	for (i = 0; i < Count; i++)
	{
		pWnd = (CGraphWnd*)mObArray.GetAt(i);
		pWnd->UpdateCoordPara();
	}
	return;
}




void CAESSCrollView::OnDeleteGrawnd()
{
	// TODO: 在此添加命令处理程序代码
	CGraphWnd * pGraWnd = NULL;
	CWnd * pWnd = GetFocus();
	int i, Count = mObArray.GetCount();
	CString theStr;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	for (i = 0; i < Count; i++)
	{
		pGraWnd = (CGraphWnd*)mObArray.GetAt(i);
		if (pWnd == pGraWnd)
		{
			theStr.Format("CAESSCrollView ::OnDeleteGrawnd");
			pApp->m_LogMgnP->Addlog(theStr);
			break;
		}
	}
	if (i != Count)
	{
		delete pGraWnd;
		mObArray.RemoveAt(i, 1);
	}
}
