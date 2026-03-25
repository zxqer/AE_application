#include "stdafx.h"
#include <ws2tcpip.h>
#include "AESWin.h"
#include "RawSignalBuf.h"
#include "FiberSocketThread.h"
#include "LogMgn.h"


#pragma comment(lib, "Ws2_32.lib")

char receivebuf[BUFFER__REC_SIZE];
char sendBuf[BUFFER__SEND_SIZE];

unsigned char* ureceivebuf;
unsigned char* usendBuf;


#define LOCAL_PORT  8006
DWORD WINAPI Fiber_Socket_ThreadFun(LPVOID lpParam)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CFiberSocketThread * pThread = (CFiberSocketThread *)lpParam;
	WSADATA wsaData;

	struct sockaddr_in local,from;
	SOCKET listen_socket;
	CString strTemp;
	int fromlen,ret;
	int iResult,recbuflen;
	unsigned char outip[8];
	char theIpAddr[32];



	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		strTemp.Format("Error at WSAStartup()\n");
		pThread->m_LogMgnP->Addlog(strTemp);
		return 1;
	}

	memset(&local, 0, sizeof(local));
	memset(&from, 0, sizeof(from));


	local.sin_family = AF_INET;

	strcpy_s(theIpAddr, "192.168.1.34");
	inet_pton(AF_INET, theIpAddr, outip);
	memcpy(&(local.sin_addr), outip, 4);

	//local.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
	local.sin_port = htons(LOCAL_PORT);  //端口号，需要网络序转换

	listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); /* Open a socket */
	if (listen_socket <0)
	{
		strTemp.Format("Client: Error Opening socket: Error %d\n",WSAGetLastError());
		pThread->m_LogMgnP->Addlog(strTemp);
		WSACleanup();
		return -1;
	}

	int defRcvBufSize = 256*1024;
	int optlen = sizeof(defRcvBufSize);

	if (setsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char*)&defRcvBufSize, optlen) < 0)
	{
		strTemp.Format("setsockopt recbufsize error %d\n", WSAGetLastError());
		pThread->m_LogMgnP->Addlog(strTemp);
	}

	defRcvBufSize = -1;
	if (getsockopt(listen_socket, SOL_SOCKET, SO_RCVBUF, (char*)&defRcvBufSize, &optlen) < 0)
	{
		strTemp.Format("getsockopt recbufsize error %d\n", WSAGetLastError());
		pThread->m_LogMgnP->Addlog(strTemp);
	}

	if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local))== SOCKET_ERROR)
	{
		strTemp.Format("bind() failed with error %d\n", WSAGetLastError());
		pThread->m_LogMgnP->Addlog(strTemp);
		WSACleanup();
		return -1;
	}

#if 1
	u_long iMode = 1;

	iResult = ioctlsocket(listen_socket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		strTemp.Format("ioctlsocket failed with error: %ld\n", iResult);
		pThread->m_LogMgnP->Addlog(strTemp);

	}

#endif

	ureceivebuf = reinterpret_cast<unsigned char*>(receivebuf);
	usendBuf = reinterpret_cast<unsigned char*>(sendBuf);
	fromlen = sizeof(from);
	recbuflen = 0;
	while (1)
	{
		iResult = recvfrom(listen_socket, receivebuf+recbuflen, BUFFER__REC_SIZE, 0, (struct sockaddr *)&from, &fromlen);

		if(iResult != SOCKET_ERROR)
		{
			pApp->mRawSignalBufP->LockBuf(0);
			recbuflen += iResult;
	//		ret = pApp->mRawSignalBufP->AddLineDot(0,recbuflen, ureceivebuf);
			ret = 0;
			pApp->mRawSignalBufP->UnlockBuf(0);
			if (ret == 0)
			{
				Sleep(50);
			}
			else
			{
				recbuflen = 0;
			}
		}
		else
		{

			Sleep(35); //old  is 35
		//	iResult = WSAGetLastError();
		//	strTemp.Format("receive failure  : %d\n", iResult);
		//	pThread->m_LogMgnP->Addlog(strTemp);
		}
	}

	closesocket(listen_socket);
	WSACleanup();

	return 0;
}

CFiberSocketThread::CFiberSocketThread(CLogMgn * pLogMgn)
{
	DWORD  threadId;
	m_LogMgnP = pLogMgn;



	hSocketThread = CreateThread(NULL, 0, Fiber_Socket_ThreadFun, this, 0, &threadId);

}


CFiberSocketThread::~CFiberSocketThread()
{
}







