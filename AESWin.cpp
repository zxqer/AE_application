
// AESWin.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "AESWin.h"
#include "MainFrm.h"

#include "AESWinDoc.h"
#include "AETabView.h"
#include "LogMgn.h"
#include "SetChannelDialog.h"
#include "RawSignalBuf.h"
#include "HitManage.h"
#include "FiberSocketThread.h"
#include "RawSignalThread.h"
#include "ArtCardDriver.h"
#include "ARTDialogPara.h"
#include "AudioParaDlg.h"
#include "DlgLocationPara.h"
#include "DialogSetCoord.h"
#include "CAELocation2D3D.h"
#include "COutLocationParaConfig.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAESWinApp

BEGIN_MESSAGE_MAP(CAESWinApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAESWinApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_CHANNEL_SET, &CAESWinApp::OnChannelSet)
	ON_COMMAND(ID_ART_PARASET, &CAESWinApp::OnArtParaset)
	ON_COMMAND(ID_AUDIO_TEZ_PARA, &CAESWinApp::OnAudioTezPara)
	ON_COMMAND(ID_LOCATION_PARA, &CAESWinApp::OnLocationPara)
	ON_COMMAND(ID_SET_COORD, &CAESWinApp::OnSetCoord)
//	ON_COMMAND(ID_FILE_SAVE, &CWinAppEx::OnFileSave)
//	ON_COMMAND(ID_FILE_SAVE_AS, &CWinAppEx::OnFileSaveAs)
END_MESSAGE_MAP()


// CAESWinApp 构造

CAESWinApp::CAESWinApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("AESWin.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CAESWinApp 对象

CAESWinApp theApp;


// CAESWinApp 初始化

BOOL CAESWinApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAESWinDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CAETabView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CAESWinApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	delete m_LogMgnP;
	return CWinAppEx::ExitInstance();
}

// CAESWinApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CAESWinApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAESWinApp 自定义加载/保存方法

void CAESWinApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

char fileset[32] = "setup.ini";
char BaySpec_sys_settings[32] = "系统设置";
char File_Name_ini[256];

void CAESWinApp::LoadCustomState()
{
	HINSTANCE hInst = NULL;

	hInst = AfxGetApp()->m_hInstance;
	char path_buffer[_MAX_PATH];
	GetModuleFileName(hInst, path_buffer, sizeof(path_buffer));

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(path_buffer, drive, dir, fname, ext);

	strcpy_s(File_Name_ini, drive);
	strcat_s(File_Name_ini, dir);
	strcpy_s(m_szSaveFilePath, File_Name_ini);


	strcat_s(File_Name_ini, "configure\\");
	strcat_s(File_Name_ini, fileset);


	CString theStr;
	CString buf,theTemp;
	int i,j;
	j = 0;
	theStr.Format("SampleRate");
	mSampleRate = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 450, File_Name_ini);


	theStr.Format("ArtReadDots");
	mArtReadDots = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 8192, File_Name_ini);

	theStr.Format("SampleTime");
	mSampleTime = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 500, File_Name_ini);

	theStr.Format("SaveDataFlag");
	mSaveDataFlag = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 0, File_Name_ini);

	for (i = 0; i < MAX_CHANNELS; i++)
	{
		theStr.Format("Monitor_Flag%d", i + 1);
		m_arrayChannel_Item[i].iMonitor_Flag = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 0, File_Name_ini);
		if (m_arrayChannel_Item[i].iMonitor_Flag == 1)
			j++;

		theStr.Format("TiggerYuValue%d", i + 1);
		m_arrayChannel_Item[i].iTiggerYuValue = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 10, File_Name_ini);

		theStr.Format("AmpGain%d", i + 1);
		m_arrayChannel_Item[i].iAmpGain = GetPrivateProfileInt(BaySpec_sys_settings, theStr, 0, File_Name_ini);

		theStr.Format("m_nMinVoltage%d", i + 1);
		GetPrivateProfileString(BaySpec_sys_settings, theStr, "-1.25", buf.GetBuffer(10), 10, File_Name_ini);
		m_arrayChannel_Item[i].m_nMinVoltage = (double)atof(buf);

		theStr.Format("m_nMaxVoltage%d", i + 1);
		GetPrivateProfileString(BaySpec_sys_settings, theStr, "1.25", buf.GetBuffer(10), 10, File_Name_ini);
		m_arrayChannel_Item[i].m_nMaxVoltage = (double)atof(buf);

		theStr.Format("Name%d", i + 1);
		theTemp.Format("Dev1/ai%d", i);
		GetPrivateProfileString(BaySpec_sys_settings, theStr, theTemp, buf.GetBuffer(10), 10, File_Name_ini);
		lstrcpy(m_arrayChannel_Item[i].m_Name, buf);

		m_arrayChannel_Item[i].mUperLimit = 0;
		m_arrayChannel_Item[i].mLowerLimit = 0;
		m_arrayChannel_Item[i].mSoftAmpGain = 0;
	}
	mDeZaoshangFlag = 0;
	mAudioPara.iEnergySum = 50;
	mAudioPara.iLastTime = 3000;
	mAudioPara.iLowerYuValue = 100;
	mAudioPara.iOverTime = 5;
	mAudioPara.iZlLowCnt = 30;

	mAudioLocate.iLocateType = 0;
	mAudioLocate.iProbe_Count = 0;
	mAudioLocate.iSoundVelocity = 300000;
	mAudioLocate.iSoundVelocityY = 1640;

	mAudioLocate.iLocateStruct = 0;

	mAudioLocate.iAxiXMin = 0;
	mAudioLocate.iAxiXMax = 100;
	mAudioLocate.iAxiYMin = 0;
	mAudioLocate.iAxiYMax = 100;

	mAudioLocate.iAxiZMin = 0;
	mAudioLocate.iAxiZMax = 100;

	mCoordMinVolt = -2000;
	mCoordMaxVolt = 2000;


	m_LogMgnP = new CLogMgn;
	mRawSignalBufP = new CRawSignalBuf;
	mHitManageP = new CHitManage;
//	mFiberSocketThreadP = new CFiberSocketThread(m_LogMgnP);
	mRawSignalThreadP = new CRawSignalThread;
	mArtCardDriverP = new CArtCardDriver;
	mOutLocationParaConfigP = new COutLocationParaConfig;
	mAppState = 0;
	mRawSignalBufP->UpdateArtPara();

}

void CAESWinApp::SaveCustomState()
{
	CString theStr,buf;
	int i;
	//use doc save 


	theStr.Format("SaveDataFlag");
	buf.Format("%d", mSaveDataFlag);
	WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

	theStr.Format("SampleRate");
	buf.Format("%d", mSampleRate);
	WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

	theStr.Format("ArtReadDots");
	buf.Format("%d", mArtReadDots);
	WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

	theStr.Format("SampleTime");
	buf.Format("%d", mSampleTime);
	WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

	for (i = 0; i < MAX_CHANNELS; i++)
	{

		theStr.Format("Monitor_Flag%d", i + 1);
		buf.Format("%d", m_arrayChannel_Item[i].iMonitor_Flag);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

		theStr.Format("TiggerYuValue%d", i + 1);
		buf.Format("%d", m_arrayChannel_Item[i].iTiggerYuValue);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);
		
		theStr.Format("AmpGain%d", i + 1);
		buf.Format("%d", m_arrayChannel_Item[i].iAmpGain);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

		theStr.Format("m_nMinVoltage%d", i + 1);
		buf.Format("%f", m_arrayChannel_Item[i].m_nMinVoltage);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

		theStr.Format("m_nMaxVoltage%d", i + 1);
		buf.Format("%f", m_arrayChannel_Item[i].m_nMaxVoltage);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, buf, File_Name_ini);

		theStr.Format("Name%d", i + 1);
		WritePrivateProfileString(BaySpec_sys_settings, theStr, m_arrayChannel_Item[i].m_Name, File_Name_ini);
	}
}

// CAESWinApp 消息处理程序





//BOOL CAboutDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CDialogEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//}


void CAESWinApp::OnChannelSet()
{
	// TODO: 在此添加命令处理程序代码
	CSetChannelDialog theDialog;
	theDialog.mMonitorCheck1 = m_arrayChannel_Item[0].iMonitor_Flag;
	theDialog.mMonitorCheck2 = m_arrayChannel_Item[1].iMonitor_Flag;
	theDialog.mMonitorCheck3 = m_arrayChannel_Item[2].iMonitor_Flag;
	theDialog.mMonitorCheck4 = m_arrayChannel_Item[3].iMonitor_Flag;
	theDialog.mMonitorCheck5 = m_arrayChannel_Item[4].iMonitor_Flag;
	theDialog.mMonitorCheck6 = m_arrayChannel_Item[5].iMonitor_Flag;
	theDialog.mMonitorCheck7 = m_arrayChannel_Item[6].iMonitor_Flag;
	theDialog.mMonitorCheck8 = m_arrayChannel_Item[7].iMonitor_Flag;
	theDialog.mMenkan_1 = m_arrayChannel_Item[0].iTiggerYuValue;
	theDialog.mMenkan_2 = m_arrayChannel_Item[1].iTiggerYuValue;
	theDialog.mMenkan_3 = m_arrayChannel_Item[2].iTiggerYuValue;
	theDialog.mMenkan_4 = m_arrayChannel_Item[3].iTiggerYuValue;
	theDialog.mMenkan_5 = m_arrayChannel_Item[4].iTiggerYuValue;
	theDialog.mMenkan_6 = m_arrayChannel_Item[5].iTiggerYuValue;
	theDialog.mMenkan_7 = m_arrayChannel_Item[6].iTiggerYuValue;
	theDialog.mMenkan_8 = m_arrayChannel_Item[7].iTiggerYuValue;
	theDialog.mGain1 = m_arrayChannel_Item[0].iAmpGain;
	theDialog.mGain2 = m_arrayChannel_Item[1].iAmpGain;
	theDialog.mGain3 = m_arrayChannel_Item[2].iAmpGain;
	theDialog.mGain4 = m_arrayChannel_Item[3].iAmpGain;
	theDialog.mGain5 = m_arrayChannel_Item[4].iAmpGain;
	theDialog.mGain6 = m_arrayChannel_Item[5].iAmpGain;
	theDialog.mGain7 = m_arrayChannel_Item[6].iAmpGain;
	theDialog.mGain8 = m_arrayChannel_Item[7].iAmpGain;

	theDialog.mSoftAmpGain_1 = m_arrayChannel_Item[0].mSoftAmpGain;
	theDialog.mSoftAmpGain_2 = m_arrayChannel_Item[1].mSoftAmpGain;
	theDialog.mSoftAmpGain_3 = m_arrayChannel_Item[2].mSoftAmpGain;
	theDialog.mSoftAmpGain_4 = m_arrayChannel_Item[3].mSoftAmpGain;
	theDialog.mSoftAmpGain_5 = m_arrayChannel_Item[4].mSoftAmpGain;
	theDialog.mSoftAmpGain_6 = m_arrayChannel_Item[5].mSoftAmpGain;
	theDialog.mSoftAmpGain_7 = m_arrayChannel_Item[6].mSoftAmpGain;
	theDialog.mSoftAmpGain_8 = m_arrayChannel_Item[7].mSoftAmpGain;

	theDialog.mLowerLimit_1 = m_arrayChannel_Item[0].mLowerLimit;
	theDialog.mLowerLimit_2 = m_arrayChannel_Item[1].mLowerLimit;
	theDialog.mLowerLimit_3 = m_arrayChannel_Item[2].mLowerLimit;
	theDialog.mLowerLimit_4 = m_arrayChannel_Item[3].mLowerLimit;
	theDialog.mLowerLimit_5 = m_arrayChannel_Item[4].mLowerLimit;
	theDialog.mLowerLimit_6 = m_arrayChannel_Item[5].mLowerLimit;
	theDialog.mLowerLimit_7 = m_arrayChannel_Item[6].mLowerLimit;
	theDialog.mLowerLimit_8 = m_arrayChannel_Item[7].mLowerLimit;

	theDialog.mUperLimit_1 = m_arrayChannel_Item[0].mUperLimit;
	theDialog.mUperLimit_2 = m_arrayChannel_Item[1].mUperLimit;
	theDialog.mUperLimit_3 = m_arrayChannel_Item[2].mUperLimit;
	theDialog.mUperLimit_4 = m_arrayChannel_Item[3].mUperLimit;
	theDialog.mUperLimit_5 = m_arrayChannel_Item[4].mUperLimit;
	theDialog.mUperLimit_6 = m_arrayChannel_Item[5].mUperLimit;
	theDialog.mUperLimit_7 = m_arrayChannel_Item[6].mUperLimit;
	theDialog.mUperLimit_8 = m_arrayChannel_Item[7].mUperLimit;

	if (theDialog.DoModal() == IDOK)
	{
		m_arrayChannel_Item[0].iMonitor_Flag = theDialog.mMonitorCheck1;
		m_arrayChannel_Item[1].iMonitor_Flag = theDialog.mMonitorCheck2;
		m_arrayChannel_Item[2].iMonitor_Flag = theDialog.mMonitorCheck3;
		m_arrayChannel_Item[3].iMonitor_Flag = theDialog.mMonitorCheck4;
		m_arrayChannel_Item[4].iMonitor_Flag = theDialog.mMonitorCheck5;
		m_arrayChannel_Item[5].iMonitor_Flag = theDialog.mMonitorCheck6;
		m_arrayChannel_Item[6].iMonitor_Flag = theDialog.mMonitorCheck7;
		m_arrayChannel_Item[7].iMonitor_Flag = theDialog.mMonitorCheck8;
		m_arrayChannel_Item[0].iTiggerYuValue = theDialog.mMenkan_1;
		m_arrayChannel_Item[1].iTiggerYuValue = theDialog.mMenkan_2;
		m_arrayChannel_Item[2].iTiggerYuValue = theDialog.mMenkan_3;
		m_arrayChannel_Item[3].iTiggerYuValue = theDialog.mMenkan_4;
		m_arrayChannel_Item[4].iTiggerYuValue = theDialog.mMenkan_5;
		m_arrayChannel_Item[5].iTiggerYuValue = theDialog.mMenkan_6;
		m_arrayChannel_Item[6].iTiggerYuValue = theDialog.mMenkan_7;
		m_arrayChannel_Item[7].iTiggerYuValue = theDialog.mMenkan_8;
	
		m_arrayChannel_Item[0].iAmpGain = theDialog.mGain1;
		m_arrayChannel_Item[1].iAmpGain = theDialog.mGain2;
		m_arrayChannel_Item[2].iAmpGain = theDialog.mGain3;
		m_arrayChannel_Item[3].iAmpGain = theDialog.mGain4;
		m_arrayChannel_Item[4].iAmpGain = theDialog.mGain5;
		m_arrayChannel_Item[5].iAmpGain = theDialog.mGain6;
		m_arrayChannel_Item[6].iAmpGain = theDialog.mGain7;
		m_arrayChannel_Item[7].iAmpGain = theDialog.mGain8;

		m_arrayChannel_Item[0].mSoftAmpGain = theDialog.mSoftAmpGain_1;
		m_arrayChannel_Item[1].mSoftAmpGain = theDialog.mSoftAmpGain_2;
		m_arrayChannel_Item[2].mSoftAmpGain = theDialog.mSoftAmpGain_3;
		m_arrayChannel_Item[3].mSoftAmpGain = theDialog.mSoftAmpGain_4;
		m_arrayChannel_Item[4].mSoftAmpGain = theDialog.mSoftAmpGain_5;
		m_arrayChannel_Item[5].mSoftAmpGain = theDialog.mSoftAmpGain_6;
		m_arrayChannel_Item[6].mSoftAmpGain = theDialog.mSoftAmpGain_7;
		m_arrayChannel_Item[7].mSoftAmpGain = theDialog.mSoftAmpGain_8;

		m_arrayChannel_Item[0].mLowerLimit = theDialog.mLowerLimit_1;
		m_arrayChannel_Item[1].mLowerLimit = theDialog.mLowerLimit_2;
		m_arrayChannel_Item[2].mLowerLimit = theDialog.mLowerLimit_3;
		m_arrayChannel_Item[3].mLowerLimit = theDialog.mLowerLimit_4;
		m_arrayChannel_Item[4].mLowerLimit = theDialog.mLowerLimit_5;
		m_arrayChannel_Item[5].mLowerLimit = theDialog.mLowerLimit_6;
		m_arrayChannel_Item[6].mLowerLimit = theDialog.mLowerLimit_7;
		m_arrayChannel_Item[7].mLowerLimit = theDialog.mLowerLimit_8;

		m_arrayChannel_Item[0].mUperLimit = theDialog.mUperLimit_1;
		m_arrayChannel_Item[1].mUperLimit = theDialog.mUperLimit_2;
		m_arrayChannel_Item[2].mUperLimit = theDialog.mUperLimit_3;
		m_arrayChannel_Item[3].mUperLimit = theDialog.mUperLimit_4;
		m_arrayChannel_Item[4].mUperLimit = theDialog.mUperLimit_5;
		m_arrayChannel_Item[5].mUperLimit = theDialog.mUperLimit_6;
		m_arrayChannel_Item[6].mUperLimit = theDialog.mUperLimit_7;
		m_arrayChannel_Item[7].mUperLimit = theDialog.mUperLimit_8;



		mRawSignalBufP->UpdateArtPara();
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SetDocModifiedFlag();

	}

}


void CAESWinApp::OnArtParaset()
{
	// TODO: 在此添加命令处理程序代码
	CARTDialogPara theDialog;
	theDialog.mSampleRate = mSampleRate;
	theDialog.mArtReaadDots = mArtReadDots;
	theDialog.mShowTime = mSampleTime;
	theDialog.mSaveData = mSaveDataFlag;
	if (theDialog.DoModal() == IDOK)
	{
		mSampleRate = theDialog.mSampleRate;
		mArtReadDots = theDialog.mArtReaadDots;
		mSampleTime = theDialog.mShowTime;
		mSaveDataFlag = theDialog.mSaveData;
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SetDocModifiedFlag();
		pMainFrame->NotifyAllViewUpdate_ARTPara();
	}

}


void CAESWinApp::OnAudioTezPara()
{
	// TODO: 在此添加命令处理程序代码
	CAudioParaDlg theDialog;
	theDialog.mEnergySum = mAudioPara.iEnergySum;
	theDialog.mLastTime = mAudioPara.iLastTime;
	theDialog.mZlLowCnt = mAudioPara.iZlLowCnt;
	theDialog.zidongmenxian = mAudioPara.iOverTime;
	if (theDialog.DoModal() == IDOK)
	{
		mAudioPara.iEnergySum = theDialog.mEnergySum;
		mAudioPara.iLastTime = theDialog.mLastTime;
		mAudioPara.iZlLowCnt = theDialog.mZlLowCnt;
		mAudioPara.iOverTime = theDialog.zidongmenxian;
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SetDocModifiedFlag();
	}
}


void CAESWinApp::OnLocationPara()
{
	// TODO: 在此添加命令处理程序代码
	CDlgLocationPara theDialog;
	if (theDialog.DoModal() == IDOK)
	{
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SetDocModifiedFlag();
	}
}


void CAESWinApp::OnSetCoord()
{
	// TODO: 在此添加命令处理程序代码
	CDialogSetCoord theDialog;
	theDialog.mMinVolt = mCoordMinVolt;
	theDialog.mMaxVolt = mCoordMaxVolt;
	if (theDialog.DoModal() == IDOK)
	{
		mCoordMinVolt = theDialog.mMinVolt;
		mCoordMaxVolt = theDialog.mMaxVolt;
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); 
		pMainFrame->NotifyAllViewUpdate_CoordPara();
		pMainFrame->SetDocModifiedFlag();
	}
}


//void CAESWinApp::OnFileSave()
//{
//	// TODO: 在此添加命令处理程序代码
//	int i = 0;
//}


//void CAESWinApp::OnFileSaveAs()
//{
//	// TODO: 在此添加命令处理程序代码
//	int i = 0;
//}
