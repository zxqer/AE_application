#include "stdafx.h"
#include "AESWin.h"
#include "ArtCardDriver.h"
#include "RawSignalBuf.h"
#include "LogMgn.h"


#define ArtDAQErrChk(functionCall) if( ArtDAQFailed(error=(functionCall)) ) goto Error; else


int32 ART_CALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	CArtCardDriver * pCardDriver = (CArtCardDriver *)callbackData;
	int32       error = 0;
	int32       read = 0;
	long numSampsPerChan = pApp->mArtReadDots;
	CString strTemp;
	double *dTemBufP;
	/*********************************************/
	// ArtDAQ_ Read Code
	/*********************************************/
	ArtDAQErrChk(ArtDAQ_ReadAnalogF64(taskHandle, numSampsPerChan, -1, ArtDAQ_Val_GroupByChannel, pCardDriver->m_ReaddataP, numSampsPerChan*pCardDriver->NumChannels, &read, NULL));


//	strTemp.Format("收到数据字节=%d\r", read);
//	pApp->m_LogMgnP->Addlog(strTemp);
	if (read>0)
	{
		// 采集数据写到LOG
	//	pApp->ProcessData(m_ReaddataP, pApp->GetWaveTriggerLength(0));
		int i;
		dTemBufP = pCardDriver->m_ReaddataP;
		for (i = 0; i < MAX_CHANNELS; i++)
		{
			if (pApp->m_arrayChannel_Item[i].iMonitor_Flag == 1)
			{
				pApp->mRawSignalBufP->LockBuf(i);
				pApp->mRawSignalBufP->AddLineDot(i, numSampsPerChan, dTemBufP);
				pApp->mRawSignalBufP->UnlockBuf(i);
				dTemBufP += numSampsPerChan;
			}
		}
	}

Error:
	if (ArtDAQFailed(error)) 
	{
		/*********************************************/
		// ArtDAQ_ Stop Code
		/*********************************************/
		ArtDAQ_StopTask(taskHandle);
		ArtDAQ_ClearTask(taskHandle);
	}
	return 0;
}

// 采集卡回调函数，停止数据采集
int32 ART_CALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	int32   error = 0;
	char    errBuff[2048] = { '\0' };

	// Check to see if an error stopped the task.
	ArtDAQErrChk(status);

Error:
	if (ArtDAQFailed(error)) {
		ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
		ArtDAQ_ClearTask(taskHandle);
	}
	return 0;
}


CArtCardDriver::CArtCardDriver()
{
	m_ReaddataP = new double[ART_READDATA_BUF];
	m_TaskHandle = 0;
}


CArtCardDriver::~CArtCardDriver()
{
	delete[]m_ReaddataP;
}

BOOL CArtCardDriver::InitCard()
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };
	CString strTemp;
	if (m_TaskHandle != 0)
		return false;
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	ArtDAQErrChk(ArtDAQ_CreateTask("", &m_TaskHandle));

	int i;
	NumChannels = 0;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (pApp->m_arrayChannel_Item[i].iMonitor_Flag == 1)
		{
			// 创建通道
			strTemp.Format("通道号：%d  Name= %s\r", i, pApp->m_arrayChannel_Item[i].m_Name);
			pApp->m_LogMgnP->Addlog(strTemp);
			ArtDAQErrChk(ArtDAQ_CreateAIVoltageChan(m_TaskHandle, pApp->m_arrayChannel_Item[i].m_Name, "", ArtDAQ_Val_Cfg_Default, pApp->m_arrayChannel_Item[i].m_nMinVoltage, pApp->m_arrayChannel_Item[i].m_nMaxVoltage, ArtDAQ_Val_Volts, NULL));
			NumChannels++;
			//ArtDAQErrChk (ArtDAQ_CreateAIVoltageChan(m_TaskHandle,"Dev1/ai0","",ArtDAQ_Val_Cfg_Default,-10.0,10.0,ArtDAQ_Val_Volts,NULL));

		}
	}
	// 配置时钟
	int nWaveSamplingRate = pApp->mSampleRate * 1000;
	ArtDAQErrChk(ArtDAQ_CfgSampClkTiming(m_TaskHandle, "", nWaveSamplingRate, ArtDAQ_Val_Rising, ArtDAQ_Val_ContSamps, ART_READDATA_BUF));

	ArtDAQErrChk(ArtDAQ_RegisterEveryNSamplesEvent(m_TaskHandle, ArtDAQ_Val_Acquired_Into_Buffer, pApp->mArtReadDots, 0, EveryNCallback, this));
	ArtDAQErrChk(ArtDAQ_RegisterDoneEvent(m_TaskHandle, 0, DoneCallback, this));


	strTemp.Format("创建通道成功\r", errBuff);
	pApp->m_LogMgnP->Addlog(strTemp);
	return true;

Error:
	if (ArtDAQFailed(error))
	{
		ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
		strTemp.Format("创建通道失败=%s\r", errBuff);
		pApp->m_LogMgnP->Addlog(strTemp);

	}
	m_TaskHandle = 0;
	return false;
}

// 启动阿尔泰采集卡，开始采集数据
bool  CArtCardDriver::StartAcquisition()
{
	if (0 == m_TaskHandle)
		return false;
	int32   error = 0;
	char    errBuff[2048] = { '\0' };
	ArtDAQErrChk(ArtDAQ_StartTask(m_TaskHandle));
	return true;
Error:
	if (ArtDAQFailed(error))
	{
		ArtDAQ_StopTask(m_TaskHandle);
		ArtDAQ_ClearTask(m_TaskHandle);
		ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
		CString str(errBuff);
#ifdef WRITELOG
		m_LogMgnP->W(str);
#endif
		AfxMessageBox(str);
	}
	m_TaskHandle = 0;
	return false;

}

// 停止采集
void  CArtCardDriver::StopAcquisition()
{
	if (0 != m_TaskHandle)
	{
		ArtDAQ_StopTask(m_TaskHandle);
		ArtDAQ_ClearTask(m_TaskHandle);
		m_TaskHandle = 0;
	}
}


