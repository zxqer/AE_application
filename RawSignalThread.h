#pragma once
#include "fftw3.h"

class CRawSignalThread
{
public:
	CRawSignalThread();
	~CRawSignalThread();

	HANDLE  hRawSignalThread;
	HANDLE hRawSignalEvent;
	BOOL  Play_Flag;
	HANDLE m_GlobalMem;
	double *pArrayAmpBuf;
	int mReadDots;
	fftw_complex* m_outFFtBuf;

	double  mArrayTot[MAX_CHANNELS];
	double  mArrayYuVaule[MAX_CHANNELS];
	int arrayFirstReadDots[MAX_CHANNELS];

	void StartPlay();
	void StopPlay();
	void FFT_LvBoProcess(int LowLimit, int UperLimit, int SoftGain, double in_outBuf[], int Len, int SampleTime);

};

