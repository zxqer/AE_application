#pragma once


// CAudioParaDlg 对话框

class CAudioParaDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAudioParaDlg)

public:
	CAudioParaDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAudioParaDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AUDIO_PARA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int mLastTime;
//	int mLowerLimit;
//	int mOverTime;
	int mEnergySum;
	int mZlLowCnt;
	int zidongmenxian;
};
