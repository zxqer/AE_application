#include "stdafx.h"
#include "LogMgn.h"
#include "AESWin.h"


#define MAX_SAVE_LINES   10000
CLogMgn::CLogMgn()
{
	CreateLogFile();

	CString msLine;
	m_EnableLog = 1;
	if (m_EnableLog == 0)
	{
		msLine = "Î´ÉèÖÃlogÊä³ö";
		m_outFile.WriteString(msLine);
	}
	m_lines = 0;
	hMutex = CreateMutex(NULL, FALSE, NULL);

}


CLogMgn::~CLogMgn()
{
	m_outFile.Close();
}

void CLogMgn::Addlog(CString msLine)
{
	if (m_EnableLog == 0)
		return;

	LockBuf();
	msLine += "\n";
	m_outFile.WriteString(msLine);
	m_lines++;
	if (m_lines >= MAX_SAVE_LINES)
	{
		m_outFile.Close();
		m_lines = 0;
		CreateLogFile();
	}
	UnlockBuf();
}

void  CLogMgn::LockBuf()
{
	WaitForSingleObject(hMutex, INFINITE);
}
void CLogMgn::UnlockBuf()
{
	ReleaseMutex(hMutex);
}

void CLogMgn::CreateLogFile()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	char logFileName[256];

	strcpy_s(logFileName, pApp->m_szSaveFilePath);
	strcat_s(logFileName, "log\\");

	CTime tt = CTime::GetCurrentTime();
	CString msLine;
	msLine = tt.Format("%Y%m%d_%H%M%S") + ".txt";
	strcat_s(logFileName, msLine);

	m_outFile.Open(logFileName, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::typeText);

}
