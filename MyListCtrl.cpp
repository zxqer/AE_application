// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "MyListCtrl.h"
#include "DlgSensor.h"



// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{

}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CMyListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()



// CMyListCtrl 消息处理程序




void CMyListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int i = 0;
//	CString theStr;
	CDlgSensor *pDlgSet = (CDlgSensor *)GetParent();
	if (pNMLV->uNewState & LVIS_SELECTED)
	{
		i = pNMLV->iItem;
		pDlgSet->UpdateSelectedSensorData(i);
//		theStr = GetItemText(i,0);
	}
	*pResult = 0;
}
