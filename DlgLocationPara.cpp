// DlgLocationPara.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "DlgLocationPara.h"
#include "afxdialogex.h"
#include "DlgSensor.h"


// CDlgLocationPara 对话框

IMPLEMENT_DYNAMIC(CDlgLocationPara, CDialogEx)

CDlgLocationPara::CDlgLocationPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOCAPARA, pParent)
	, mSoundVel(0)
	, mAxiXMin(0)
	, mAxiXMax(0)
	, mAxiYMin(0)
	, mAxiYMax(0)
	, mSoundVeltY(0)
	, iAxiZMin(0)
	, iAxiZMax(0)
{

}

CDlgLocationPara::~CDlgLocationPara()
{
}

void CDlgLocationPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, mComBoxLocateType);
	DDX_Control(pDX, IDC_COMBO2, mComboxStruct);
	DDX_Text(pDX, IDC_EDIT1, mSoundVel);
	DDX_Text(pDX, IDC_EDIT2, mAxiXMin);
	DDX_Text(pDX, IDC_EDIT4, mAxiXMax);
	DDX_Text(pDX, IDC_EDIT5, mAxiYMin);
	DDX_Text(pDX, IDC_EDIT6, mAxiYMax);
	DDX_Text(pDX, IDC_EDIT7, mSoundVeltY);
	DDX_Text(pDX, IDC_EDIT8, iAxiZMin);
	DDX_Text(pDX, IDC_EDIT9, iAxiZMax);
}


BEGIN_MESSAGE_MAP(CDlgLocationPara, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgLocationPara::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgLocationPara 消息处理程序


BOOL CDlgLocationPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	mComBoxLocateType.SetCurSel(pApp->mAudioLocate.iLocateType);
	mComboxStruct.SetCurSel(pApp->mAudioLocate.iLocateStruct);
	mSoundVel = pApp->mAudioLocate.iSoundVelocity;
	mSoundVeltY = pApp->mAudioLocate.iSoundVelocityY;
	mAxiXMin = pApp->mAudioLocate.iAxiXMin;
	mAxiXMax = pApp->mAudioLocate.iAxiXMax;
	mAxiYMin = pApp->mAudioLocate.iAxiYMin;
	mAxiYMax = pApp->mAudioLocate.iAxiYMax;

	iAxiZMin = pApp->mAudioLocate.iAxiZMin;
	iAxiZMax = pApp->mAudioLocate.iAxiZMax;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
			  // 异常: OCX 属性页应返回 FALSE
}


void CDlgLocationPara::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	pApp->mAudioLocate.iLocateType = mComBoxLocateType.GetCurSel();
	pApp->mAudioLocate.iLocateStruct = mComboxStruct.GetCurSel();
	pApp->mAudioLocate.iSoundVelocity = mSoundVel;
	pApp->mAudioLocate.iSoundVelocityY = mSoundVeltY;
	pApp->mAudioLocate.iAxiXMin = mAxiXMin;
	pApp->mAudioLocate.iAxiXMax = mAxiXMax;
	pApp->mAudioLocate.iAxiYMin = mAxiYMin;
	pApp->mAudioLocate.iAxiYMax = mAxiYMax;
	pApp->mAudioLocate.iAxiZMin = iAxiZMin;
	pApp->mAudioLocate.iAxiZMax = iAxiZMax;

	CDialogEx::OnOK();
}


void CDlgLocationPara::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	pApp->mAudioLocate.iLocateType = mComBoxLocateType.GetCurSel();
	pApp->mAudioLocate.iLocateStruct = mComboxStruct.GetCurSel();
	pApp->mAudioLocate.iSoundVelocity = mSoundVel;
	pApp->mAudioLocate.iSoundVelocityY = mSoundVeltY;
	pApp->mAudioLocate.iAxiXMin = mAxiXMin;
	pApp->mAudioLocate.iAxiXMax = mAxiXMax;
	pApp->mAudioLocate.iAxiYMin = mAxiYMin;
	pApp->mAudioLocate.iAxiYMax = mAxiYMax;
	pApp->mAudioLocate.iAxiZMin = iAxiZMin;
	pApp->mAudioLocate.iAxiZMax = iAxiZMax;

	CDlgSensor theDialog;
	if (theDialog.DoModal() == IDOK)
	{
	}

}
