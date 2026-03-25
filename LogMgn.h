#pragma once
class CLogMgn
{
public:
	CLogMgn();
	~CLogMgn();

	int m_EnableLog;
	int m_lines;
	HANDLE   hMutex;

	void  Addlog(CString msLine);
	void  LockBuf();
	void  UnlockBuf();
	void  CreateLogFile();

	CStdioFile m_outFile;
};

