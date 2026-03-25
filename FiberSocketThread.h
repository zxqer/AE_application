#pragma once
class CLogMgn;
#define BUFFER__REC_SIZE 8192*1000
#define BUFFER__SEND_SIZE 256

class CFiberSocketThread
{
public:
	CFiberSocketThread(CLogMgn * pLogMgn);
	~CFiberSocketThread();



	HANDLE  hSocketThread;
	CLogMgn * m_LogMgnP;
};

