// ARTDialogPara.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "ARTDialogPara.h"
#include "afxdialogex.h"


// CARTDialogPara 对话框

IMPLEMENT_DYNAMIC(CARTDialogPara, CDialogEx)

CARTDialogPara::CARTDialogPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ART, pParent)
	, mSampleRate(0)
	, mArtReaadDots(0)
	, mShowTime(0)
	, mSaveData(FALSE)
{

}

CARTDialogPara::~CARTDialogPara()
{
}

void CARTDialogPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mSampleRate);
	DDX_Text(pDX, IDC_EDIT2, mArtReaadDots);
	DDX_Text(pDX, IDC_EDIT3, mShowTime);
	DDX_Check(pDX, IDC_CHECK1, mSaveData);
}


BEGIN_MESSAGE_MAP(CARTDialogPara, CDialogEx)
END_MESSAGE_MAP()


// CARTDialogPara 消息处理程序
