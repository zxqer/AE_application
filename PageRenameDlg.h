#pragma once


// CPageRenameDlg 对话框

class CPageRenameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPageRenameDlg)

public:
	CPageRenameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageRenameDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PAGE_RENAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString mPageName;
};
