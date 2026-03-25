
// AESWin.h : AESWin 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CAESWinApp:
// 有关此类的实现，请参阅 AESWin.cpp
//

#define MAX_CHANNELS    8

typedef struct tagCHANNEL_ITEM {
	int    iMonitor_Flag;
	int    iTiggerYuValue;       //触发阈值  单位dB
	int    iAmpGain;     //增益
	double m_nMinVoltage;
	double m_nMaxVoltage;
	char   m_Name[16];
	int   mUperLimit;  //数字滤波上限  kHz
	int   mLowerLimit;  //数字滤波下限 kHz
	int   mSoftAmpGain; //数字放大dB
}CHANNEL_Item, *CHANNEL_ITEM;


typedef struct tagAUDIO_PARA {
	int    iLastTime;        //信号持续时间（微秒）
	int    iLowerYuValue;       //信号结束阈值（毫伏）不用
	int    iOverTime;        //自动门限定位倍数
	int    iEnergySum;     //信号能量和
	int    iZlLowCnt;         //振铃最小次数
}AUDIO_PARA, *AUDIO_PARAP;

typedef struct tagPROBE_PARA {
	int    iChNo;        //探头通道号
	int    iX;       //探头位置 X
	int    iY;        //探头位置 Y
	int    iZ;     // 探头位置  Z
}PROBE_PARA, *PROBE_PARAP;

typedef struct tagAUDIO_LOCATE {
	int     iLocateType;        //定位类型  0:线性  1：平面   2:立体
	int    iSoundVelocity;     //声音传播速度  
	int    iSoundVelocityY;     //声音传播速度  
	int     iLocateStruct;        //定位结构 0： 平面; 1:立方体
	int    iAxiXMin;
	int    iAxiXMax;
	int    iAxiYMin;
	int    iAxiYMax;
	int    iAxiZMin;
	int    iAxiZMax;
	int     iProbe_Count;       //探头数量
	PROBE_PARA  sProbePara[MAX_CHANNELS];

}AUDIO_LOCATE, *AUDIO_LOCATEP;


class CLogMgn;
class CRawSignalBuf;
class CHitManage;
//class CFiberSocketThread;
class CRawSignalThread;
class CArtCardDriver;
class COutLocationParaConfig;
class CAESWinApp : public CWinAppEx
{
public:
	CAESWinApp();

	char m_szSaveFilePath[256];

	CLogMgn * m_LogMgnP;
	CRawSignalBuf * mRawSignalBufP;
	CHitManage * mHitManageP;
//	CFiberSocketThread * mFiberSocketThreadP;
	CRawSignalThread * mRawSignalThreadP;
	CArtCardDriver *mArtCardDriverP;
	COutLocationParaConfig* mOutLocationParaConfigP;

	CHANNEL_Item  m_arrayChannel_Item[MAX_CHANNELS];

	AUDIO_PARA mAudioPara;
	AUDIO_LOCATE  mAudioLocate;

	int    mAppState;   //1：采集状态; 2:定位状态
	int    mSampleRate;     //采样率
	int    mArtReadDots;    //通道每次读取采样点数
	int    mSampleTime;   //一次处理信号采样时间 单位毫秒

	int   mCoordMinVolt, mCoordMaxVolt;
	int   mSaveDataFlag;
	int  mDeZaoshangFlag;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChannelSet();
	afx_msg void OnArtParaset();
	afx_msg void OnAudioTezPara();
	afx_msg void OnLocationPara();
	afx_msg void OnSetCoord();
//	afx_msg void OnFileSave();
//	afx_msg void OnFileSaveAs();
};

extern CAESWinApp theApp;
