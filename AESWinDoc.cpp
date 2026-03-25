
// AESWinDoc.cpp : CAESWinDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "AESWin.h"
#endif

#include "AESWinDoc.h"
#include "AESSCrollView.h"
#include "RawSignalBuf.h"
#include "AETabView.h"
#include "LocationView.h"
#include "LogMgn.h"


#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAESWinDoc

IMPLEMENT_DYNCREATE(CAESWinDoc, CDocument)

BEGIN_MESSAGE_MAP(CAESWinDoc, CDocument)
//	ON_COMMAND(ID_UPDATE_VIEW, &CAESWinDoc::OnUpdateView)
//ON_COMMAND(ID_FILE_SAVE, &CAESWinDoc::OnFileSave)
//ON_COMMAND(ID_FILE_SAVE_AS, &CAESWinDoc::OnFileSaveAs)
END_MESSAGE_MAP()


// CAESWinDoc 构造/析构

CAESWinDoc::CAESWinDoc()
{
	// TODO: 在此添加一次性构造代码

}

CAESWinDoc::~CAESWinDoc()
{
}

BOOL CAESWinDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CAESWinDoc 序列化

void CAESWinDoc::Serialize(CArchive& ar)
{
	CAESSCrollView * pView = (CAESSCrollView *)GetAEView();
	CAETabView* pTabView = (CAETabView *)pView->GetTabView();
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	int i;
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码

	//	CString theStr;
	//	theStr.Format("Doc Serialize");
	//	pApp->m_LogMgnP->Addlog(theStr);

		ar << pApp->mSampleRate;
		ar << pApp->mArtReadDots;
		ar << pApp->mSampleTime;
		ar << pApp->mCoordMinVolt;
		ar << pApp->mCoordMaxVolt;
		for (i = 0; i < 8; i++)
		{
			ar << pApp->m_arrayChannel_Item[i].iMonitor_Flag;
			ar << pApp->m_arrayChannel_Item[i].iTiggerYuValue;
			ar << pApp->m_arrayChannel_Item[i].iAmpGain;
			ar << pApp->m_arrayChannel_Item[i].m_nMinVoltage;
			ar << pApp->m_arrayChannel_Item[i].m_nMaxVoltage;
			ar << pApp->m_arrayChannel_Item[i].mSoftAmpGain;
			ar << pApp->m_arrayChannel_Item[i].mLowerLimit;
			ar << pApp->m_arrayChannel_Item[i].mUperLimit;
		}
		ar << pApp->mAudioPara.iLastTime;
		ar << pApp->mAudioPara.iLowerYuValue;
		ar << pApp->mAudioPara.iOverTime;
		ar << pApp->mAudioPara.iEnergySum;
		ar << pApp->mAudioPara.iZlLowCnt;

		ar << pApp->mAudioLocate.iLocateType;
		ar << pApp->mAudioLocate.iSoundVelocity;
		ar << pApp->mAudioLocate.iSoundVelocityY;
		ar << pApp->mAudioLocate.iLocateStruct;

		ar << pApp->mAudioLocate.iAxiXMin;
		ar << pApp->mAudioLocate.iAxiXMax;
		ar << pApp->mAudioLocate.iAxiYMin;
		ar << pApp->mAudioLocate.iAxiYMax;

		ar << pApp->mAudioLocate.iAxiZMin;
		ar << pApp->mAudioLocate.iAxiZMax;

		ar << pApp->mAudioLocate.iProbe_Count;



		for (i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
		{
			ar << pApp->mAudioLocate.sProbePara[i].iChNo;
			ar << pApp->mAudioLocate.sProbePara[i].iX;
			ar << pApp->mAudioLocate.sProbePara[i].iY;
			ar << pApp->mAudioLocate.sProbePara[i].iZ;
		}

		pTabView->Serialize(ar);
	}
	else
	{
		// TODO: 在此添加加载代码
		ar >> pApp->mSampleRate;
		ar >> pApp->mArtReadDots;
		ar >> pApp->mSampleTime;
		ar >> pApp->mCoordMinVolt;
		ar >> pApp->mCoordMaxVolt;

		for (i = 0; i < 8; i++)
		{
			ar >> pApp->m_arrayChannel_Item[i].iMonitor_Flag;
			ar >> pApp->m_arrayChannel_Item[i].iTiggerYuValue;
			ar >> pApp->m_arrayChannel_Item[i].iAmpGain;
			ar >> pApp->m_arrayChannel_Item[i].m_nMinVoltage;
			ar >> pApp->m_arrayChannel_Item[i].m_nMaxVoltage;
			ar >> pApp->m_arrayChannel_Item[i].mSoftAmpGain;
			ar >> pApp->m_arrayChannel_Item[i].mLowerLimit;
			ar >> pApp->m_arrayChannel_Item[i].mUperLimit;
		}
		ar >> pApp->mAudioPara.iLastTime;
		ar >> pApp->mAudioPara.iLowerYuValue;
		ar >> pApp->mAudioPara.iOverTime;
		ar >> pApp->mAudioPara.iEnergySum;
		ar >> pApp->mAudioPara.iZlLowCnt;

		ar >> pApp->mAudioLocate.iLocateType;
		ar >> pApp->mAudioLocate.iSoundVelocity;
		ar >> pApp->mAudioLocate.iSoundVelocityY;
		ar >> pApp->mAudioLocate.iLocateStruct;
		ar >> pApp->mAudioLocate.iAxiXMin;
		ar >> pApp->mAudioLocate.iAxiXMax;
		ar >> pApp->mAudioLocate.iAxiYMin;
		ar >> pApp->mAudioLocate.iAxiYMax;
		ar >> pApp->mAudioLocate.iAxiZMin;
		ar >> pApp->mAudioLocate.iAxiZMax;

		ar >> pApp->mAudioLocate.iProbe_Count;

		for (i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
		{
			ar >> pApp->mAudioLocate.sProbePara[i].iChNo;
			ar >> pApp->mAudioLocate.sProbePara[i].iX;
			ar >> pApp->mAudioLocate.sProbePara[i].iY;
			ar >> pApp->mAudioLocate.sProbePara[i].iZ;
		}

		pApp->mRawSignalBufP->UpdateArtPara();
		pTabView->Serialize(ar);
	}
}

CView * CAESWinDoc::GetAEView()
{
	POSITION pos = GetFirstViewPosition();
	CView * pView = NULL;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
	}
	return pView;
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CAESWinDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CAESWinDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CAESWinDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAESWinDoc 诊断

#ifdef _DEBUG
void CAESWinDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAESWinDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAESWinDoc 命令


//void CAESWinDoc::OnUpdateView()
//{
//	// TODO: 在此添加命令处理程序代码
//	UpdateAllViews(NULL);
//}

void  CAESWinDoc::NotifyAllViewUpdate(int chno, int hitindex)
{
	POSITION pos = GetFirstViewPosition();
	CAETabView * pTabView = NULL;
	while (pos != NULL)
	{
		pTabView = (CAETabView *)GetNextView(pos);
		break;
	}
	CView * pView = pTabView->GetActiveView();
	if (pView->IsKindOf(RUNTIME_CLASS(CAESSCrollView)))
	{
		CAESSCrollView * pScrollView = (CAESSCrollView *)pView;
		pScrollView->UpdateRealData(chno, hitindex);

	}
	else if (pView->IsKindOf(RUNTIME_CLASS(CLocationView)))
	{
		CLocationView * pLocationView = (CLocationView *)pView;
		pLocationView->UpdateRealData(chno, hitindex);
	}
}

void  CAESWinDoc::NotifyStartPlay()
{
	POSITION pos = GetFirstViewPosition();
	CAETabView * pTabView = NULL;
	while (pos != NULL)
	{
		pTabView = (CAETabView *)GetNextView(pos);
		break;
	}
	CView * pView = pTabView->GetActiveView();
	if (pView->IsKindOf(RUNTIME_CLASS(CAESSCrollView)))
	{
		CAESSCrollView * pScrollView = (CAESSCrollView *)pView;
		pScrollView->NotifyStartPlay();

	}
	else if (pView->IsKindOf(RUNTIME_CLASS(CLocationView)))
	{
		CLocationView * pLocationView = (CLocationView *)pView;
		pLocationView->NotifyStartLocation();
	}

}

void CAESWinDoc::NotifyAllViewDrawHistory()
{
	POSITION pos = GetFirstViewPosition();
	CView * pView = NULL;
	CAESSCrollView * pScrollView;
	BOOL bSuc;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		bSuc = pView->IsKindOf(RUNTIME_CLASS(CAESSCrollView));
		if (bSuc)
		{
			pScrollView = (CAESSCrollView *)pView;
			pScrollView->NotifyStartDrawHistory();
		}
	}
}

void CAESWinDoc::NotifyAllViewUpdateCoord()
{
	POSITION pos = GetFirstViewPosition();
	CView * pView = NULL;
	CAESSCrollView * pScrollView;
	BOOL bSuc;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		bSuc = pView->IsKindOf(RUNTIME_CLASS(CAESSCrollView));
		if (bSuc)
		{
			pScrollView = (CAESSCrollView *)pView;
			pScrollView->NotifyUpdateCoord();
		}
	}
}


void CAESWinDoc::AddLocationView()
{
	POSITION pos = GetFirstViewPosition();
	CView * pView = NULL;
	CAETabView * pTabView;
	BOOL bSuc;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		bSuc = pView->IsKindOf(RUNTIME_CLASS(CAETabView));
		if (bSuc)
		{
			pTabView = (CAETabView *)pView;
			pTabView->AddLocationView();
			break;
		}
	}
}
void CAESWinDoc::RemoveLocationView()
{
	POSITION pos = GetFirstViewPosition();
	CView * pView = NULL;
	CAETabView * pTabView;
	BOOL bSuc;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		bSuc = pView->IsKindOf(RUNTIME_CLASS(CAETabView));
		if (bSuc)
		{
			pTabView = (CAETabView *)pView;
			pTabView->RemoveLocationView(3);
			break;
		}
	}
}




BOOL CAESWinDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

	CString theStr;

	return CDocument::OnSaveDocument(lpszPathName);

	CString  thePathName = GetPathName();
	if (thePathName.IsEmpty())
	{
		return CDocument::OnSaveDocument(lpszPathName);
	}
	else if (strcmp(thePathName, lpszPathName))
	{
	//	theStr.Format("OnSaveDocument:%s,%s", lpszPathName,thePathName);
	//	pApp->m_LogMgnP->Addlog(theStr);

		return CDocument::OnSaveDocument(lpszPathName);
	}
	return false;
}


BOOL CAESWinDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

//	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();

//	CString theStr;
//	theStr.Format("SaveModified");
//	pApp->m_LogMgnP->Addlog(theStr);
	return CDocument::SaveModified();
}


//void CAESWinDoc::OnFileSave()
//{
//	// TODO: 在此添加命令处理程序代码
//	int i = 0;
//}


//void CAESWinDoc::OnFileSaveAs()
//{
//	// TODO: 在此添加命令处理程序代码
//	int i = 0;
//}


BOOL CAESWinDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	SetPathName(lpszPathName,TRUE);
	return TRUE;
}
