#pragma once
#include "afxwin.h"


// CSetChannelDialog 对话框

class CSetChannelDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSetChannelDialog)

public:
	CSetChannelDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetChannelDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_CHANNEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL mMonitorCheck1;
	BOOL mMonitorCheck2;
	BOOL mMonitorCheck3;
	BOOL mMonitorCheck4;
	BOOL mMonitorCheck5;
	BOOL mMonitorCheck6;
	BOOL mMonitorCheck7;
	BOOL mMonitorCheck8;
	int mMenkan_1;
	int mMenkan_2;
	int mMenkan_3;
	int mMenkan_4;
	int mMenkan_5;
	int mMenkan_6;
	int mMenkan_7;
	int mMenkan_8;
	int mGain1;
	int mGain2;
	int mGain3;
	int mGain4;
	int mGain5;
	int mGain6;
	int mGain7;
	int mGain8;
	CComboBox mComBox[MAX_CHANNELS];
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	int mLowerLimit_1;
	int mLowerLimit_2;
	int mLowerLimit_3;
	int mLowerLimit_4;
	int mLowerLimit_5;
	int mLowerLimit_6;
	int mLowerLimit_7;
	int mLowerLimit_8;
	int mUperLimit_1;
	int mUperLimit_2;
	int mUperLimit_3;
	int mUperLimit_4;
	int mUperLimit_5;
	int mUperLimit_6;
	int mUperLimit_7;
	int mUperLimit_8;
	int mSoftAmpGain_1;
	int mSoftAmpGain_2;
	int mSoftAmpGain_3;
	int mSoftAmpGain_4;
	int mSoftAmpGain_5;
	int mSoftAmpGain_6;
	int mSoftAmpGain_7;
	int mSoftAmpGain_8;
};
