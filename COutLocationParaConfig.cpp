#include "stdafx.h"
#include <iostream>     // 标准输入输出流
#include <fstream>      // 文件流操作
#include "AESWin.h"
#include "COutLocationParaConfig.h"


COutLocationParaConfig::COutLocationParaConfig()
{

}


COutLocationParaConfig::~COutLocationParaConfig()
{

}

void COutLocationParaConfig::OutoutConfig()
{
    CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();

    char conFileName[256];
    strcpy_s(conFileName, pApp->m_szSaveFilePath);
    strcat_s(conFileName, "myconfig.txt");

    CStdioFile m_outFile;

    m_outFile.Open(conFileName, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::typeText);

    CString theStr;

    theStr.Format("Speed: %d\n", pApp->mAudioLocate.iSoundVelocity);
    m_outFile.WriteString(theStr);

    if(pApp->mAudioLocate.iLocateType == 2)
        theStr.Format("Mode:: 3D\n");
    else
        theStr.Format("Mode:: 2D\n");
    m_outFile.WriteString(theStr);

    theStr.Format("Threshold: 100000000\n");
    m_outFile.WriteString(theStr);

    theStr.Format("AutoRejection: 0\n");
    m_outFile.WriteString(theStr);

    theStr.Format("UseFreqFilter: 0\n");
    m_outFile.WriteString(theStr);
    theStr.Format("MinFreq: 100.0\n");
    m_outFile.WriteString(theStr);
    theStr.Format("MaxFreq: 400.0\n");
    m_outFile.WriteString(theStr);

    theStr.Format("UseAnisotropy: 0\n");
    m_outFile.WriteString(theStr);
    theStr.Format("Vx: 1.0\n");
    m_outFile.WriteString(theStr);
    theStr.Format("Vy: 1.0\n");
    m_outFile.WriteString(theStr);
    theStr.Format("Vz: 1.0\n");
    m_outFile.WriteString(theStr);

    theStr.Format("Sensors:\n");
    m_outFile.WriteString(theStr);

    for (int i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
    {
        theStr.Format("%d,%d,%d,%d,0.0\n", pApp->mAudioLocate.sProbePara[i].iChNo, pApp->mAudioLocate.sProbePara[i].iX, pApp->mAudioLocate.sProbePara[i].iY, pApp->mAudioLocate.sProbePara[i].iZ);
        m_outFile.WriteString(theStr);
    }

    m_outFile.Close();
}

void COutLocationParaConfig::OutoutData(int ArriveCount, SENSOR_ARRIVE  pSensorArrive[])
{
    CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();

    char conFileName[256];
    strcpy_s(conFileName, pApp->m_szSaveFilePath);
    strcat_s(conFileName, "mydata.csv");
    DeleteFile(conFileName);


    std::ofstream resFile(conFileName); // 准备结果文件
    resFile << "EventID,SensorID,Time,Frequency\n"; // 写入表头，第一列为时间(s)
    int i, EventID;
    EventID = 1;
    for (i = 0; i < ArriveCount; i++)
    {
        resFile << EventID << "," << pSensorArrive[i].iChNo << "," << pSensorArrive[i].duTime << "," << pSensorArrive[i].aveFuzhi << "\n"; // 输出 CSV

    }
    resFile.close();
#if 0
    int    EventID,iChNo;        //探头通道号
    double   duTime;       //到达时间
    double  aveFuzhi;     //平均幅值

    EventID = 1;
    iChNo = 1;
    duTime = 0.123;
    aveFuzhi = 1.2;

    resFile << EventID << "," << iChNo << "," << duTime << "," << aveFuzhi  << "\n"; // 输出 CSV

    iChNo = 2;
    duTime = 0.3453;
    aveFuzhi = 1.3;
    resFile << EventID << "," << iChNo << "," << duTime << "," << aveFuzhi << "\n"; // 输出 CSV

    iChNo = 3;
    duTime = 0.3456;
    aveFuzhi = 1.8;
    resFile << EventID << "," << iChNo << "," << duTime << "," << aveFuzhi << "\n"; // 输出 CSV
#endif
}



