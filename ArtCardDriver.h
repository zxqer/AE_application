#pragma once
#include "dlltyp.h"
#include "Art_DAQ.h"

#define ART_READDATA_BUF   100*1024
class CArtCardDriver
{
public:
	CArtCardDriver();
	~CArtCardDriver();

	TaskHandle  m_TaskHandle;
	double * m_ReaddataP;
	int NumChannels;

	BOOL   InitCard();
	bool   StartAcquisition();
	void   StopAcquisition();

};

