#pragma once


// CARTDialogPara 对话框

class CARTDialogPara : public CDialogEx
{
	DECLARE_DYNAMIC(CARTDialogPara)

public:
	CARTDialogPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CARTDialogPara();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int mSampleRate;
	int mArtReaadDots;
	int mShowTime;
	BOOL mSaveData;
};
