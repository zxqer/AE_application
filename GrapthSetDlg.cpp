// GrapthSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "GrapthSetDlg.h"
#include "afxdialogex.h"


// CGrapthSetDlg 对话框

IMPLEMENT_DYNAMIC(CGrapthSetDlg, CDialogEx)

CGrapthSetDlg::CGrapthSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_GRAPH, pParent)
	, mComboValue(0)
	, mChannelNo(0)
	, m_GraphComBox(0)
{

}

CGrapthSetDlg::~CGrapthSetDlg()
{
}

void CGrapthSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO1, mComboValue);
	DDX_Text(pDX, IDC_EDIT1, mChannelNo);
	DDX_CBIndex(pDX, IDC_COMBO2, m_GraphComBox);
}


BEGIN_MESSAGE_MAP(CGrapthSetDlg, CDialogEx)
END_MESSAGE_MAP()


// CGrapthSetDlg 消息处理程序
