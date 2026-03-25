#pragma once


// CDialogSetCoord 对话框

class CDialogSetCoord : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSetCoord)

public:
	CDialogSetCoord(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogSetCoord();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETCOORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int mMinVolt;
	int mMaxVolt;
};
