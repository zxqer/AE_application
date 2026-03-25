// SetChannelDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "SetChannelDialog.h"
#include "afxdialogex.h"


// CSetChannelDialog 对话框

IMPLEMENT_DYNAMIC(CSetChannelDialog, CDialogEx)

CSetChannelDialog::CSetChannelDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_CHANNEL, pParent)
	, mMonitorCheck1(FALSE)
	, mMonitorCheck2(FALSE)
	, mMonitorCheck3(FALSE)
	, mMonitorCheck4(FALSE)
	, mMonitorCheck5(FALSE)
	, mMonitorCheck6(FALSE)
	, mMonitorCheck7(FALSE)
	, mMonitorCheck8(FALSE)
	, mMenkan_1(0)
	, mMenkan_2(0)
	, mMenkan_3(0)
	, mMenkan_4(0)
	, mMenkan_5(0)
	, mMenkan_6(0)
	, mMenkan_7(0)
	, mMenkan_8(0)
	, mGain1(0)
	, mGain2(0)
	, mGain3(0)
	, mGain4(0)
	, mGain5(0)
	, mGain6(0)
	, mGain7(0)
	, mGain8(0)
	, mLowerLimit_1(0)
	, mLowerLimit_2(0)
	, mLowerLimit_3(0)
	, mLowerLimit_4(0)
	, mLowerLimit_5(0)
	, mLowerLimit_6(0)
	, mLowerLimit_7(0)
	, mLowerLimit_8(0)
	, mUperLimit_1(0)
	, mUperLimit_2(0)
	, mUperLimit_3(0)
	, mUperLimit_4(0)
	, mUperLimit_5(0)
	, mUperLimit_6(0)
	, mUperLimit_7(0)
	, mUperLimit_8(0)
	, mSoftAmpGain_1(0)
	, mSoftAmpGain_2(0)
	, mSoftAmpGain_3(0)
	, mSoftAmpGain_4(0)
	, mSoftAmpGain_5(0)
	, mSoftAmpGain_6(0)
	, mSoftAmpGain_7(0)
	, mSoftAmpGain_8(0)
{

}

CSetChannelDialog::~CSetChannelDialog()
{
}

void CSetChannelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, mMonitorCheck1);
	DDX_Check(pDX, IDC_CHECK2, mMonitorCheck2);
	DDX_Check(pDX, IDC_CHECK3, mMonitorCheck3);
	DDX_Check(pDX, IDC_CHECK4, mMonitorCheck4);
	DDX_Check(pDX, IDC_CHECK5, mMonitorCheck5);
	DDX_Check(pDX, IDC_CHECK6, mMonitorCheck6);
	DDX_Check(pDX, IDC_CHECK7, mMonitorCheck7);
	DDX_Check(pDX, IDC_CHECK8, mMonitorCheck8);
	DDX_Text(pDX, IDC_EDIT1, mMenkan_1);
	DDX_Text(pDX, IDC_EDIT3, mMenkan_2);
	DDX_Text(pDX, IDC_EDIT5, mMenkan_3);
	DDX_Text(pDX, IDC_EDIT7, mMenkan_4);
	DDX_Text(pDX, IDC_EDIT9, mMenkan_5);
	DDX_Text(pDX, IDC_EDIT11, mMenkan_6);
	DDX_Text(pDX, IDC_EDIT13, mMenkan_7);
	DDX_Text(pDX, IDC_EDIT15, mMenkan_8);
	DDX_Text(pDX, IDC_EDIT2, mGain1);
	DDX_Text(pDX, IDC_EDIT4, mGain2);
	DDX_Text(pDX, IDC_EDIT6, mGain3);
	DDX_Text(pDX, IDC_EDIT8, mGain4);
	DDX_Text(pDX, IDC_EDIT10, mGain5);
	DDX_Text(pDX, IDC_EDIT12, mGain6);
	DDX_Text(pDX, IDC_EDIT14, mGain7);
	DDX_Text(pDX, IDC_EDIT16, mGain8);
	DDX_Control(pDX, IDC_COMBO2, mComBox[0]);
	DDX_Control(pDX, IDC_COMBO3, mComBox[1]);
	DDX_Control(pDX, IDC_COMBO4, mComBox[2]);
	DDX_Control(pDX, IDC_COMBO5, mComBox[3]);
	DDX_Control(pDX, IDC_COMBO6, mComBox[4]);
	DDX_Control(pDX, IDC_COMBO7, mComBox[5]);
	DDX_Control(pDX, IDC_COMBO8, mComBox[6]);
	DDX_Control(pDX, IDC_COMBO9, mComBox[7]);
	DDX_Text(pDX, IDC_EDIT17, mLowerLimit_1);
	DDX_Text(pDX, IDC_EDIT18, mLowerLimit_2);
	DDX_Text(pDX, IDC_EDIT19, mLowerLimit_3);
	DDX_Text(pDX, IDC_EDIT20, mLowerLimit_4);
	DDX_Text(pDX, IDC_EDIT21, mLowerLimit_5);
	DDX_Text(pDX, IDC_EDIT22, mLowerLimit_6);
	DDX_Text(pDX, IDC_EDIT23, mLowerLimit_7);
	DDX_Text(pDX, IDC_EDIT24, mLowerLimit_8);
	DDX_Text(pDX, IDC_EDIT25, mUperLimit_1);
	DDX_Text(pDX, IDC_EDIT26, mUperLimit_2);
	DDX_Text(pDX, IDC_EDIT27, mUperLimit_3);
	DDX_Text(pDX, IDC_EDIT28, mUperLimit_4);
	DDX_Text(pDX, IDC_EDIT29, mUperLimit_5);
	DDX_Text(pDX, IDC_EDIT30, mUperLimit_6);
	DDX_Text(pDX, IDC_EDIT31, mUperLimit_7);
	DDX_Text(pDX, IDC_EDIT32, mUperLimit_8);
	DDX_Text(pDX, IDC_EDIT33, mSoftAmpGain_1);
	DDX_Text(pDX, IDC_EDIT34, mSoftAmpGain_2);
	DDX_Text(pDX, IDC_EDIT35, mSoftAmpGain_3);
	DDX_Text(pDX, IDC_EDIT36, mSoftAmpGain_4);
	DDX_Text(pDX, IDC_EDIT37, mSoftAmpGain_5);
	DDX_Text(pDX, IDC_EDIT38, mSoftAmpGain_6);
	DDX_Text(pDX, IDC_EDIT39, mSoftAmpGain_7);
	DDX_Text(pDX, IDC_EDIT40, mSoftAmpGain_8);
}


BEGIN_MESSAGE_MAP(CSetChannelDialog, CDialogEx)
END_MESSAGE_MAP()


// CSetChannelDialog 消息处理程序


BOOL CSetChannelDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		if (pApp->m_arrayChannel_Item[i].m_nMaxVoltage < 2.5)
		{
			mComBox[i].SetCurSel(0);
		}
		else if (pApp->m_arrayChannel_Item[i].m_nMaxVoltage < 5.0)
		{
			mComBox[i].SetCurSel(1);

		}
		else if (pApp->m_arrayChannel_Item[i].m_nMaxVoltage < 10.0)
		{
			mComBox[i].SetCurSel(2);
		}
		else
		{
			mComBox[i].SetCurSel(3);
		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetChannelDialog::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	for (int i = 0; i < MAX_CHANNELS; i++)
	{
		if (mComBox[i].GetCurSel() == 0)
		{
			pApp->m_arrayChannel_Item[i].m_nMinVoltage = -1.25;
			pApp->m_arrayChannel_Item[i].m_nMaxVoltage = 1.25;
		}
		else if (mComBox[i].GetCurSel() == 1)
		{
			pApp->m_arrayChannel_Item[i].m_nMinVoltage = -2.5;
			pApp->m_arrayChannel_Item[i].m_nMaxVoltage = 2.5;
		}
		else if (mComBox[i].GetCurSel() == 2)
		{
			pApp->m_arrayChannel_Item[i].m_nMinVoltage = -5;
			pApp->m_arrayChannel_Item[i].m_nMaxVoltage = 5;
		}
		else
		{
			pApp->m_arrayChannel_Item[i].m_nMinVoltage = -10;
			pApp->m_arrayChannel_Item[i].m_nMaxVoltage = 10;
		}
	}

	CDialogEx::OnOK();
}
