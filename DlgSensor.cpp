// DlgSensor.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "DlgSensor.h"
#include "afxdialogex.h"
#include "SensorWnd.h"


// CDlgSensor 对话框

IMPLEMENT_DYNAMIC(CDlgSensor, CDialogEx)

CDlgSensor::CDlgSensor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SENSOR, pParent)
	, mAxiX(0)
	, mAxiY(0)
	, mChNo(0)
	, mAxiZ(0)
{

}

CDlgSensor::~CDlgSensor()
{
}

void CDlgSensor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, mListCtrl);
	DDX_Text(pDX, IDC_EDIT1, mAxiX);
	DDX_Text(pDX, IDC_EDIT2, mAxiY);
	DDX_Text(pDX, IDC_EDIT3, mChNo);
	DDX_Control(pDX, IDOK, mButtonOk);
	DDX_Text(pDX, IDC_EDIT4, mAxiZ);
}


BEGIN_MESSAGE_MAP(CDlgSensor, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSensor::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgSensor::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgSensor::OnBnClickedButton3)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgSensor 消息处理程序


BOOL CDlgSensor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect theListRect,theButtonRect;
	mListCtrl.GetWindowRect(theListRect);
	mButtonOk.GetWindowRect(theButtonRect);
	ScreenToClient(theListRect);
	ScreenToClient(theButtonRect);

	CRect rc;
	rc.left = theListRect.right + 10;
	rc.top = 10;
	rc.right = rc.left + (screenwidth - theListRect.right - 15);
	rc.bottom = theButtonRect.top - 10;
	pSensorWnd = new CSensorWnd;
	pSensorWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_THICKFRAME, rc, this, 1);

	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	pSensorWnd->SetCoordSize(pApp->mAudioLocate.iAxiXMin, pApp->mAudioLocate.iAxiXMax, pApp->mAudioLocate.iAxiYMin, pApp->mAudioLocate.iAxiYMax, pApp->mAudioLocate.iAxiZMin, pApp->mAudioLocate.iAxiZMax);
	if (pApp->mAudioLocate.iLocateType == 2)
	{
		pSensorWnd->SetDisplayMode(MODE_3D_LOCATION);
	}

	mListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CString strColumn;

	strColumn.Format(_T("通道"));
	mListCtrl.InsertColumn(0, strColumn, LVCFMT_LEFT, 120);
	strColumn.Format(_T("坐标X"));
	mListCtrl.InsertColumn(1, strColumn, LVCFMT_LEFT, 150);
	strColumn.Format(_T("坐标Y"));
	mListCtrl.InsertColumn(2, strColumn, LVCFMT_LEFT, 150);
	strColumn.Format(_T("坐标Z"));
	mListCtrl.InsertColumn(3, strColumn, LVCFMT_LEFT, 150);


	InitListCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgSensor::InitListCtrl()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	int i;
	CString strColumn;

	for (i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
	{
		strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iChNo + 1);
		mListCtrl.InsertItem(i, strColumn, 0);
		strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iX);
		mListCtrl.SetItemText(i, 1,strColumn);
		strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iY);
		mListCtrl.SetItemText(i, 2,strColumn);
		strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iZ);
		mListCtrl.SetItemText(i, 3, strColumn);
	}
}



//增加
void CDlgSensor::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	UpdateData(TRUE);
	if (mChNo <= 0)
		return;
	int i = pApp->mAudioLocate.iProbe_Count;
	pApp->mAudioLocate.sProbePara[i].iChNo = mChNo - 1;
	pApp->mAudioLocate.sProbePara[i].iX = mAxiX;
	pApp->mAudioLocate.sProbePara[i].iY = mAxiY;
	pApp->mAudioLocate.sProbePara[i].iZ = mAxiZ;

	CString strColumn;
	int k = mListCtrl.GetItemCount();
	strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iChNo + 1);
	mListCtrl.InsertItem(k, strColumn, 0);
	strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iX);
	mListCtrl.SetItemText(k, 1, strColumn);
	strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iY);
	mListCtrl.SetItemText(k, 2, strColumn);
	strColumn.Format(_T("%d"), pApp->mAudioLocate.sProbePara[i].iZ);
	mListCtrl.SetItemText(k, 3, strColumn);
	pApp->mAudioLocate.iProbe_Count++;
	pSensorWnd->Invalidate();
}

//删除
void CDlgSensor::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	UpdateData(TRUE);
	if (m_SelectNo == -1)
		return;
	if (m_SelectNo >= pApp->mAudioLocate.iProbe_Count)
		return;
	mListCtrl.DeleteItem(m_SelectNo);
	int k = m_SelectNo;

	for (k = m_SelectNo + 1; k < pApp->mAudioLocate.iProbe_Count; k++)
	{
		pApp->mAudioLocate.sProbePara[k-1].iChNo = pApp->mAudioLocate.sProbePara[k].iChNo;
		pApp->mAudioLocate.sProbePara[k-1].iX = pApp->mAudioLocate.sProbePara[k].iX;
		pApp->mAudioLocate.sProbePara[k-1].iY = pApp->mAudioLocate.sProbePara[k].iY;
		pApp->mAudioLocate.sProbePara[k-1].iZ = pApp->mAudioLocate.sProbePara[k].iZ;
	}
	pApp->mAudioLocate.iProbe_Count--;
	m_SelectNo = -1;
	pSensorWnd->Invalidate();

}

//修改
void CDlgSensor::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	UpdateData(TRUE);
	if (m_SelectNo == -1)
		return;
	if (m_SelectNo >= pApp->mAudioLocate.iProbe_Count)
		return;
	pApp->mAudioLocate.sProbePara[m_SelectNo].iChNo = mChNo - 1;
	pApp->mAudioLocate.sProbePara[m_SelectNo].iX = mAxiX;
	pApp->mAudioLocate.sProbePara[m_SelectNo].iY = mAxiY;
	pApp->mAudioLocate.sProbePara[m_SelectNo].iZ = mAxiZ;

	CString strColumn;
	strColumn.Format(_T("%d"), mChNo);
	mListCtrl.SetItemText(m_SelectNo, 0, strColumn);

	strColumn.Format(_T("%d"), mAxiX);
	mListCtrl.SetItemText(m_SelectNo, 1, strColumn);

	strColumn.Format(_T("%d"), mAxiY);
	mListCtrl.SetItemText(m_SelectNo, 2, strColumn);

	strColumn.Format(_T("%d"), mAxiZ);
	mListCtrl.SetItemText(m_SelectNo, 3, strColumn);

	pSensorWnd->Invalidate();
}

void CDlgSensor::UpdateSelectedSensorData(int Number)
{
	CString theStr;

	theStr = mListCtrl.GetItemText(Number, 0);
	mChNo = atoi(theStr);
	theStr = mListCtrl.GetItemText(Number, 1);
	mAxiX = atoi(theStr);
	theStr = mListCtrl.GetItemText(Number, 2);
	mAxiY = atoi(theStr);
	theStr = mListCtrl.GetItemText(Number, 3);
	mAxiZ = atoi(theStr);

	m_SelectNo = Number;
	UpdateData(FALSE);
}



void CDlgSensor::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	screenwidth = cx;
	screenheight = cy;

#if 0
	CRect theRect;
	theRect.left = cx - 400;
	theRect.top = 10;
	theRect.right = theRect.left + 400;
	theRect.bottom = theRect.top + 230;
	pSensorWnd->MoveWindow(theRect.left, theRect.top,400,200);
#endif
	// TODO: 在此处添加消息处理程序代码
}
