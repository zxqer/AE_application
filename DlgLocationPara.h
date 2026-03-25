#pragma once
#include "afxwin.h"


// CDlgLocationPara 对话框

class CDlgLocationPara : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLocationPara)

public:
	CDlgLocationPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLocationPara();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOCAPARA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox mComBoxLocateType;
	CComboBox mComboxStruct;
	int mSoundVel;
	int mAxiXMin;
	int mAxiXMax;
	int mAxiYMin;
	int mAxiYMax;
	virtual void OnOK();
	afx_msg void OnBnClickedButton1();
	int mSoundVeltY;
	int iAxiZMin;
	int iAxiZMax;
};
