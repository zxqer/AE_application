// DialogSetCoord.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "DialogSetCoord.h"
#include "afxdialogex.h"


// CDialogSetCoord 对话框

IMPLEMENT_DYNAMIC(CDialogSetCoord, CDialogEx)

CDialogSetCoord::CDialogSetCoord(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SETCOORD, pParent)
	, mMinVolt(0)
	, mMaxVolt(0)
{

}

CDialogSetCoord::~CDialogSetCoord()
{
}

void CDialogSetCoord::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mMinVolt);
	DDX_Text(pDX, IDC_EDIT2, mMaxVolt);
}


BEGIN_MESSAGE_MAP(CDialogSetCoord, CDialogEx)
END_MESSAGE_MAP()


// CDialogSetCoord 消息处理程序
