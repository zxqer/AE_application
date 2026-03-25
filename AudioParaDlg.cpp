// AudioParaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "AudioParaDlg.h"
#include "afxdialogex.h"


// CAudioParaDlg 对话框

IMPLEMENT_DYNAMIC(CAudioParaDlg, CDialogEx)

CAudioParaDlg::CAudioParaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_AUDIO_PARA, pParent)
	, mLastTime(0)
	, mEnergySum(0)
	, mZlLowCnt(0)
	, zidongmenxian(0)
{

}

CAudioParaDlg::~CAudioParaDlg()
{
}

void CAudioParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mLastTime);
	//  DDX_Text(pDX, IDC_EDIT2, mLowerLimit);
	//  DDX_Text(pDX, IDC_EDIT3, mOverTime);
	DDX_Text(pDX, IDC_EDIT4, mEnergySum);
	DDX_Text(pDX, IDC_EDIT5, mZlLowCnt);
	DDX_Text(pDX, IDC_EDIT2, zidongmenxian);
}


BEGIN_MESSAGE_MAP(CAudioParaDlg, CDialogEx)
END_MESSAGE_MAP()


// CAudioParaDlg 消息处理程序
