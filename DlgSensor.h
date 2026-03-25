#pragma once
#include "afxcmn.h"
#include "MyListCtrl.h"
#include "afxwin.h"


// CDlgSensor 对话框
class CSensorWnd;
class CDlgSensor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSensor)

public:
	CDlgSensor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSensor();

	CSensorWnd * pSensorWnd;
	void   InitListCtrl();
	void  UpdateSelectedSensorData(int Number);
	int m_SelectNo;
	int screenwidth,screenheight;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SENSOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CMyListCtrl mListCtrl;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	int mAxiX;
	int mAxiY;
	int mChNo;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton mButtonOk;
	int mAxiZ;
};
