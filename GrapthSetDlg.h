#pragma once


// CGrapthSetDlg 对话框

class CGrapthSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGrapthSetDlg)

public:
	CGrapthSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGrapthSetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_GRAPH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int mComboValue;
	int mChannelNo;
	int m_GraphComBox;
};
