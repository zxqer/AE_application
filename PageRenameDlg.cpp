// PageRenameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "PageRenameDlg.h"
#include "afxdialogex.h"


// CPageRenameDlg 对话框

IMPLEMENT_DYNAMIC(CPageRenameDlg, CDialogEx)

CPageRenameDlg::CPageRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PAGE_RENAME, pParent)
	, mPageName(_T(""))
{

}

CPageRenameDlg::~CPageRenameDlg()
{
}

void CPageRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mPageName);
}


BEGIN_MESSAGE_MAP(CPageRenameDlg, CDialogEx)
END_MESSAGE_MAP()


// CPageRenameDlg 消息处理程序
