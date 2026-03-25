// LocationView.cpp : 实现文件
//

#include "stdafx.h"
#include "AESWin.h"
#include "LocationView.h"
#include "SensorWnd.h"
#include "HitManage.h"
#include "LogMgn.h"
#include "CLocSignalWnd.h"
#include "COutLocationParaConfig.h"
#include "CAELocation2D3D.h"

// CLocationView

IMPLEMENT_DYNCREATE(CLocationView, CView)

CLocationView::CLocationView()
{
	mAmpBuf = NULL;
	mLocationflag = -1;
	LocSignalWnd_showFlag = 0;
}

CLocationView::~CLocationView()
{
	if (mAmpBuf != NULL)
		delete[]mAmpBuf;
}

BEGIN_MESSAGE_MAP(CLocationView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_LOCATION_PROCESS, onLocationProcessData)
	ON_WM_CHAR()
	ON_COMMAND(ID_LOC_SIGNAL, &CLocationView::OnLocSignal)
END_MESSAGE_MAP()


// CLocationView 绘图

void CLocationView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// CLocationView 诊断

#ifdef _DEBUG
void CLocationView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLocationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLocationView 消息处理程序


//BOOL CLocationView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
//}


void CLocationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	InitListCtrl();
}


void CLocationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	CPoint startPt;

	mScreenWidth = cx;
	mScreenHeight = cy;

	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
//	CString strTemp;
//	strTemp.Format("OnSize   mScreenWidth :%d,mScreenHeight:%d", mScreenWidth, mScreenHeight);
//	pApp->m_LogMgnP->Addlog(strTemp);

	startPt.x = 100;
	startPt.y = 50;

	pListCtrl->MoveWindow(startPt.x, startPt.y, cx - 2 * startPt.x,300, TRUE);

	startPt.x = 100;
	startPt.y = 400;
	pSensorWnd->MoveWindow(startPt.x, startPt.y, cx - 2* startPt.x, cy - 450, TRUE);

	startPt.x = 200;
	startPt.y = 100;

	pLocSignalWnd->MoveWindow(startPt.x, startPt.y, cx - 2 * startPt.x, cy - 2 * startPt.y + 50, TRUE);

	
	AdjustListColumnWidth();
	// TODO: 在此处添加消息处理程序代码

}


int CLocationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rc;
	rc.left = 510;
	rc.top = 10;
	rc.right = 900;
	rc.bottom = 330;
	pSensorWnd = new CSensorWnd;
	pSensorWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_THICKFRAME, rc, this, 1);

	pLocSignalWnd = new CLocSignalWnd;
	pLocSignalWnd->Create(NULL, NULL, WS_CHILD | WS_THICKFRAME, rc, this, 1);

	pListCtrl = new CListCtrl;

	pListCtrl->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT ,rc, this, 2);
	pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES);

	InitListCtrl();

	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	pSensorWnd->SetCoordSize(pApp->mAudioLocate.iAxiXMin, pApp->mAudioLocate.iAxiXMax, pApp->mAudioLocate.iAxiYMin, pApp->mAudioLocate.iAxiYMax, pApp->mAudioLocate.iAxiZMin,pApp->mAudioLocate.iAxiZMax);
	
	if (pApp->mAudioLocate.iLocateType == 2)
	{
		pSensorWnd->SetDisplayMode(MODE_3D_LOCATION);
	}

	return 0;
}


void  CLocationView::NotifyStartLocation()
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	if (mAmpBuf != NULL)
	{
		delete[]mAmpBuf;
	}
	mAmpBuf = new double[pApp->mSampleRate*pApp->mSampleTime];
	if (mAmpBuf == NULL)
	{
		CString theStr;
		theStr.Format("CLocationView mAmpBuf allocate buf failed");
		pApp->m_LogMgnP->Addlog(theStr);

	}
	pListCtrl->DeleteAllItems();
	mSensorArriveCount = 0;
	int xx = (pApp->mAudioLocate.sProbePara[1].iX - pApp->mAudioLocate.sProbePara[0].iX);
	int yy = (pApp->mAudioLocate.sProbePara[1].iY - pApp->mAudioLocate.sProbePara[0].iY);
	double  distance = (double)xx*xx;
	distance += (double)yy*yy;
	distance = sqrt(distance);
	closeTime = distance * 2 / pApp->mAudioLocate.iSoundVelocity + 0.05;
	//closeTime = 0.5;

}

void  CLocationView::UpdateRealData(int chno, int index)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString theStr;
	if (pApp->mAppState != 2)
		return;
	PostMessage(WM_LOCATION_PROCESS,chno,index);
}

void CLocationView::AnalysisAEData(int chno, int hitIndex)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	HIT_ITEM_HEAD theHitHead;
	pApp->mHitManageP->LockBuf();
	theHitHead = pApp->mHitManageP->GetHitItem(chno, hitIndex, mAmpBuf);
	pApp->mHitManageP->UnlockBuf();
	if (theHitHead.iChannelNo == -1)
		return;
	int pos_fft = 0;
	int signal_Len;
	double triggerYuzhi = (double)pApp->m_arrayChannel_Item[chno].iTiggerYuValue / 1000;
	CString strTemp;
	double sumEnergy, mkEnergy,maxAmp,arrivetime,fuzhi;
	int ZlCnt,pdt,arrivet;
	mkEnergy = (double)pApp->mAudioPara.iEnergySum;
	do {
		if (*(mAmpBuf + pos_fft) > triggerYuzhi)
		{
			signal_Len = Analysis_Audio_Signal(pos_fft, theHitHead.iHitLength, mAmpBuf, &sumEnergy, triggerYuzhi, &ZlCnt, &maxAmp,&pdt,&arrivet);
			if (ZlCnt >= pApp->mAudioPara.iZlLowCnt && sumEnergy >= mkEnergy)
			{
				arrivetime = AddListCtrlHit(chno, hitIndex, sumEnergy, ZlCnt, maxAmp, pdt, signal_Len, arrivet, theHitHead.douTot);
				fuzhi = sumEnergy * 1000 / signal_Len;
				AddSensor(chno, arrivetime, fuzhi);
				pLocSignalWnd->AddEventArrive(chno, hitIndex, arrivet);
				if(mSensorArriveCount >= 2)
					LocationAnalysis();
				break;
			}
			else
			{
				if (signal_Len == 0)
					break;
				pos_fft += signal_Len;
			}
		}
		else
		{
			pos_fft++;
		}

	} while (pos_fft < theHitHead.iHitLength);
//	strTemp.Format("AnalysisAEData end");
//	pApp->m_LogMgnP->Addlog(strTemp);
}

int CLocationView::Analysis_Audio_Signal(int start, int maxbufLen, double * pAmBuf, double * outSum, double triggerYuzhi, int * outZlCnt,double* maxAmp,int* pdt,int * arrivetime)
{
	CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
	CString strTemp;

	double temp = (double)pApp->mSampleRate * 1000 / 1000000 * pApp->mAudioPara.iLastTime;
	int LastDots = (int)temp;

	double ddd,dzf;
	int i,j,k;
	while (start > 0)
	{
		dzf = *(pAmBuf + start) / *(pAmBuf + start - 1);
		if (dzf > (double)pApp->mAudioPara.iOverTime)
			break;
		start--;
	}
	i = start;
	j = 0;
	*pdt = 0;
	*outZlCnt = 0;
	*outSum = 0.0;
	*maxAmp = 0.0;
	k = 0;
	while (i < maxbufLen)
	{
		ddd = fabs(*(pAmBuf + i));
		dzf = *(pAmBuf + i);

		if (dzf > *maxAmp)
		{
			*maxAmp = dzf;
			*pdt  = k;
		}
		if (dzf >= triggerYuzhi)
			*outZlCnt = *outZlCnt + 1;
		*outSum += ddd;

		i++;
		k++;
		if (k >= LastDots)
			break;
	}
	*arrivetime = start;
//	strTemp.Format("Analysis_Audio_Signal LastDots:%d  start:%d , pdt:%d,arrivetime:%d,Energy:%f,Zlcs:%d", LastDots,start, *pdt, *arrivetime, *outSum,*outZlCnt);
//	pApp->m_LogMgnP->Addlog(strTemp);
	return (i - start);
}

void  CLocationView::InitListCtrl()
{
	CString strColumn;
	int theWidth = 120;
	strColumn.Format(_T("通道号"));
	pListCtrl->InsertColumn(0, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("撞击号"));
	pListCtrl->InsertColumn(1, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("能量计数"));
	pListCtrl->InsertColumn(2, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("上升时间(微秒)"));
	pListCtrl->InsertColumn(3, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("持续时间(微秒)"));
	pListCtrl->InsertColumn(4, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("振铃次数"));
	pListCtrl->InsertColumn(5, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("最高幅值(毫伏)"));
	pListCtrl->InsertColumn(6, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("平均幅值(毫伏)"));
	pListCtrl->InsertColumn(7, strColumn, LVCFMT_LEFT, theWidth);
	strColumn.Format(_T("到达最高幅值时间(秒)"));
	pListCtrl->InsertColumn(8, strColumn, LVCFMT_LEFT, theWidth);
}

void  CLocationView::AdjustListColumnWidth()
{
	int theWidth = (mScreenWidth - 200) / 16;
	for (int i = 0; i < 9; i++)
	{
		pListCtrl->SetColumnWidth(i, theWidth);
	}
}


double CLocationView::AddListCtrlHit(int Chno,int HitIndex,double EnergySum,int zlCnt,double maxAmp,int pdt,int plt,int arrivet,double tot)
{
	CAESWinApp *pApp = (CAESWinApp *)AfxGetApp();
	CString strColumn;
	int k = pListCtrl->GetItemCount();
	strColumn.Format(_T("%d"), Chno + 1);
	pListCtrl->InsertItem(k, strColumn, 0);
	strColumn.Format(_T("%d"), HitIndex);
	pListCtrl->SetItemText(k, 1, strColumn);
	strColumn.Format(_T("%f"), EnergySum);
	pListCtrl->SetItemText(k, 2, strColumn);
	int temp;
	temp = pdt * 1000 / pApp->mSampleRate;
	strColumn.Format(_T("%d"), temp);
	pListCtrl->SetItemText(k, 3, strColumn);



	temp = plt * 1000 / pApp->mSampleRate;
	strColumn.Format(_T("%d"), temp);
	pListCtrl->SetItemText(k, 4, strColumn);

	strColumn.Format(_T("%d"), zlCnt);
	pListCtrl->SetItemText(k, 5, strColumn);
	temp = (int)(maxAmp * 1000);
	strColumn.Format(_T("%d"), temp);
	pListCtrl->SetItemText(k, 6, strColumn);
	temp = EnergySum * 1000  / plt;
	strColumn.Format(_T("%d"), temp);
	pListCtrl->SetItemText(k, 7, strColumn);

	CString strTemp;

	double dutemp;
	dutemp = (double)arrivet / (pApp->mSampleRate * 1000);
	dutemp += tot;
	dutemp -= (double)pdt / pApp->mSampleRate / 1000;

	strTemp.Format("AddListCtrlHit Chno :%d arrivet:%d , tot:%f,time :%f", Chno, arrivet, tot, dutemp);
	pApp->m_LogMgnP->Addlog(strTemp);

	strColumn.Format(_T("%f"), dutemp);
	pListCtrl->SetItemText(k, 8, strColumn);
	return dutemp;

}

LRESULT CLocationView::onLocationProcessData(WPARAM x, LPARAM y)
{
	AnalysisAEData(x, y);
	return 0;
}

// ==========================================================
// 边界约束算法：将飞出边界的点沿原方向拉回平面边缘
// ==========================================================
void ConstrainToBoundary(int& x, int& y, int xMin, int xMax, int yMin, int yMax)
{
	// 1. 如果本来就在平面内，直接放行
	if (x >= xMin && x <= xMax && y >= yMin && y <= yMax)
		return;

	// 2. 计算可视化平面的几何中心作为参考点
	double xc = (xMin + xMax) / 2.0;
	double yc = (yMin + yMax) / 2.0;

	// 3. 计算从中心指向目标点的向量
	double dx = (double)x - xc;
	double dy = (double)y - yc;

	// 防止极其罕见的原点重合（分母为0的情况）
	if (fabs(dx) < 1e-6 && fabs(dy) < 1e-6) return;

	// 4. 计算 X 和 Y 方向触碰边界所需要的缩放比例 (Scale Factor)
	double sx = 1e9, sy = 1e9; // 初始化为一个巨大的数

	if (dx > 0) sx = (xMax - xc) / dx;
	else if (dx < 0) sx = (xMin - xc) / dx;

	if (dy > 0) sy = (yMax - yc) / dy;
	else if (dy < 0) sy = (yMin - yc) / dy;

	// 5. 核心：取 X 和 Y 比例中较小的那一个 (谁先撞到墙，就听谁的)
	double s = (sx < sy) ? sx : sy;

	// 6. 按照比例更新坐标，将其钉在边界上
	x = (int)(xc + s * dx);
	y = (int)(yc + s * dy);
}
void CLocationView::LocationAnalysis()
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	CString strTemp;

	// --- 获取当前动态调节的可视化范围边界 ---
	double xMin = (double)pApp->mAudioLocate.iAxiXMin;
	double xMax = (double)pApp->mAudioLocate.iAxiXMax;
	double yMin = (double)pApp->mAudioLocate.iAxiYMin;
	double yMax = (double)pApp->mAudioLocate.iAxiYMax;
	double zMin = (double)pApp->mAudioLocate.iAxiZMin;
	double zMax = (double)pApp->mAudioLocate.iAxiZMax;

	// --- 情况 A：直线定位 (2个传感器) ---
	if (pApp->mAudioLocate.iLocateType == 0 && pApp->mAudioLocate.iProbe_Count == 2 && mSensorArriveCount == 2)
	{
		double t1, t2;
		if (mSensorArrive[0].iChNo == pApp->mAudioLocate.sProbePara[0].iChNo)
		{
			t1 = mSensorArrive[0].duTime;
			t2 = mSensorArrive[1].duTime;
		}
		else
		{
			t1 = mSensorArrive[1].duTime;
			t2 = mSensorArrive[0].duTime;
		}

		double s1 = (t1 - t2) * (pApp->mAudioLocate.iSoundVelocity) / 2.0;
		double resX = 0, resY = 0;

		if (pApp->mAudioLocate.sProbePara[1].iX == pApp->mAudioLocate.sProbePara[0].iX)
		{
			resY = (pApp->mAudioLocate.sProbePara[1].iY + pApp->mAudioLocate.sProbePara[0].iY) / 2.0 + s1;
			resX = pApp->mAudioLocate.sProbePara[1].iX;
		}
		else if (pApp->mAudioLocate.sProbePara[1].iY == pApp->mAudioLocate.sProbePara[0].iY)
		{
			resX = (pApp->mAudioLocate.sProbePara[1].iX + pApp->mAudioLocate.sProbePara[0].iX) / 2.0 + s1;
			resY = pApp->mAudioLocate.sProbePara[1].iY;
		}

		// 直线定位越界约束 (简单 Clamp)
		if (resX < xMin) resX = xMin; if (resX > xMax) resX = xMax;
		if (resY < yMin) resY = yMin; if (resY > yMax) resY = yMax;

		strTemp.Format(_T("Line loc Success  X:%.1f Y:%.1f"), resX, resY);
		pApp->m_LogMgnP->Addlog(strTemp);
		pSensorWnd->AddLocationPt((int)resX, (int)resY);

		mSensorArriveCount = 0;
	}

	// --- 情况 B & C：平面定位 (Type 1) 或 立体定位 (Type 2) ---
	else if ((pApp->mAudioLocate.iLocateType == 1 || pApp->mAudioLocate.iLocateType == 2) && mSensorArriveCount == 4)
	{
		int xx = 0, yy = 0, zz = 0;

		// 1. 准备内存数据
		int chNos[MAX_CHANNELS];
		double times[MAX_CHANNELS];
		for (int i = 0; i < mSensorArriveCount; i++)
		{
			chNos[i] = mSensorArrive[i].iChNo;
			times[i] = mSensorArrive[i].duTime;
		}

		// 2. 导出配置与离线数据
		pApp->mOutLocationParaConfigP->OutoutConfig();
		pApp->mOutLocationParaConfigP->OutoutData(mSensorArriveCount, mSensorArrive);

		// 3. 调用 LM 算法引擎
		CAELocation2D3D theAELocation2D3D;
		int success = theAELocation2D3D.Calculate2D3DLocation(xx, yy, zz, chNos, times, mSensorArriveCount);

		if (success > 0)
		{
			// --- 【核心约束算法】：射线投影拉回法 ---
			double curX = (double)xx;
			double curY = (double)yy;
			double curZ = (double)zz;

			// 如果点在范围外，执行投影
			if (curX < xMin || curX > xMax || curY < yMin || curY > yMax || curZ < zMin || curZ > zMax)
			{
				// 计算当前可视化范围的中心点
				double cx = (xMin + xMax) / 2.0;
				double cy = (yMin + yMax) / 2.0;
				double cz = (zMin + zMax) / 2.0;

				// 计算从中心指向外漂移点的向量
				double dx = curX - cx;
				double dy = curY - cy;
				double dz = curZ - cz;

				// 分别计算 X, Y, Z 三个方向触碰边界所需的缩放比例
				double sx = 1e9, sy = 1e9, sz = 1e9;
				if (fabs(dx) > 1e-6) {
					if (dx > 0) sx = (xMax - cx) / dx;
					else sx = (xMin - cx) / dx;
				}
				if (fabs(dy) > 1e-6) {
					if (dy > 0) sy = (yMax - cy) / dy;
					else sy = (yMin - cy) / dy;
				}
				if (pApp->mAudioLocate.iLocateType == 2 && fabs(dz) > 1e-6) { // 仅 3D 模式考虑 Z 轴
					if (dz > 0) sz = (zMax - cz) / dz;
					else sz = (zMin - cz) / dz;
				}

				// 取最小的缩放比例 (即最先撞墙的那个轴)
				double s = sx;
				if (sy < s) s = sy;
				if (pApp->mAudioLocate.iLocateType == 2 && sz < s) s = sz;

				// 按比例拉回，正好落在边界上
				xx = (int)(cx + s * dx);
				yy = (int)(cy + s * dy);
				if (pApp->mAudioLocate.iLocateType == 2)
					zz = (int)(cz + s * dz);
	}

			// 画出约束后的点
			pSensorWnd->AddLocationPt(xx, yy, zz);
}

		strTemp.Format(_T("%s loc result: %d | FinalPos:(%d, %d, %d)"),
			(pApp->mAudioLocate.iLocateType == 1 ? _T("Plane") : _T("Cube")), success, xx, yy, zz);
		pApp->m_LogMgnP->Addlog(strTemp);

		mSensorArriveCount = 0;
	}
}
void CLocationView::AddSensor(int chno, double duTime,double fuzhi)
{
	int i;
	BOOL bFind = false;
	double maxTime = 0.0; 
	for (i = 0; i < mSensorArriveCount; i++)
	{
		if (mSensorArrive[i].iChNo == chno)
		{
			bFind = true;
		}
		if (mSensorArrive[i].duTime > maxTime)
			maxTime = mSensorArrive[i].duTime;
	}
	CAESWinApp *pApp = (CAESWinApp *)AfxGetApp();
	CString strTemp;
	strTemp.Format("add sensor  maxTime:%f duTime:%f ,closeTime:%f", maxTime, duTime, closeTime);
	pApp->m_LogMgnP->Addlog(strTemp);

	if (((duTime - maxTime) > closeTime) && mSensorArriveCount != 0)
	{
		strTemp.Format("remove sensor  delt:%f ,closeTime:%f", duTime - maxTime,closeTime);
		pApp->m_LogMgnP->Addlog(strTemp);
		mSensorArriveCount = 0;
		bFind = false;
	}
	if (bFind == false)
	{
		mSensorArrive[mSensorArriveCount].iChNo = chno;
		mSensorArrive[mSensorArriveCount].duTime = duTime;
		mSensorArrive[mSensorArriveCount].aveFuzhi = fuzhi;
		mSensorArriveCount++;
	}
}

void CLocationView::SeekS1S3()
{
	CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
	int i, j;   //横轴s1 -- s2 ; 纵轴 s3  - s4
	mS1 = -1;
	mS2 = -1;
	mS3 = -1;
	mS4 = -1;
	j = 0;
	for (i = 1; i < 4; i++)//寻找横轴，纵轴传感器
	{
		if (pApp->mAudioLocate.sProbePara[j].iY == pApp->mAudioLocate.sProbePara[i].iY)
		{
			if (pApp->mAudioLocate.sProbePara[j].iX < pApp->mAudioLocate.sProbePara[i].iX)
			{
				mS1 = j;
				mS2 = i;
			}
			else
			{
				mS1 = i;
				mS2 = j;
			}
		}
		else if (pApp->mAudioLocate.sProbePara[j].iX == pApp->mAudioLocate.sProbePara[i].iX)
		{
			if (pApp->mAudioLocate.sProbePara[j].iY < pApp->mAudioLocate.sProbePara[i].iY)
			{
				mS3 = j;
				mS4 = i;
			}
			else
			{
				mS3 = i;
				mS4 = j;
			}
		}
	}
	if (mS1 != -1)
	{
		for (i = 0; i < 4; i++)
		{
			if (i == mS1 || i == mS2)
			{
				continue;
			}
			else
			{
				mS3 = i;
				break;
			}
		}
		for (i = 0; i < 4; i++)
		{
			if (i == mS1 || i == mS2  || i == mS3)
			{
				continue;
			}
			else
			{
				mS4 = i;
				break;
			}
		}
		if (pApp->mAudioLocate.sProbePara[mS3].iY > pApp->mAudioLocate.sProbePara[mS4].iY)
		{
			i = mS3;
			mS3 = mS4;
			mS4 = i;
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			if (i == mS3 || i == mS4)
			{
				continue;
			}
			else
			{
				mS1 = i;
				break;
			}
		}
		for (i = 0; i < 4; i++)
		{
			if (i == mS1 || i == mS3 || i == mS4)
			{
				continue;
			}
			else
			{
				mS2 = i;
				break;
			}
		}
		if (pApp->mAudioLocate.sProbePara[mS1].iX > pApp->mAudioLocate.sProbePara[mS2].iX)
		{
			i = mS1;
			mS2 = mS1;
			mS1 = i;
		}
	}
}

int CLocationView::GetSensorArriveIndex(int chNo)
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		if (mSensorArrive[i].iChNo == chNo)
		{
			return i;
		}
	}
	return -1;
}




void CLocationView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == '1')
		mLocationflag = 0;
	else if(nChar == '2')
		mLocationflag = 1;
	else if(nChar == '3')
		mLocationflag = 2;


	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CLocationView::OnLocSignal()
{
	// TODO: 在此添加命令处理程序代码
	if (LocSignalWnd_showFlag == 0)
	{
		pLocSignalWnd->ShowEventData();
		pListCtrl->ShowWindow(SW_HIDE);
		pSensorWnd->ShowWindow(SW_HIDE);
		pLocSignalWnd->ShowWindow(SW_SHOW);
		LocSignalWnd_showFlag = 1;
	}
	else
	{
		pLocSignalWnd->ShowWindow(SW_HIDE);
		pListCtrl->ShowWindow(SW_SHOW);
		pSensorWnd->ShowWindow(SW_SHOW);
		LocSignalWnd_showFlag = 0;

	}
}
