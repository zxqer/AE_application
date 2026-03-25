/**
 * SensorWnd.cpp : 传感器布局与 3D 立体定位显示窗口实现文件
 * * 功能描述：
 * 1. 支持 2D (GDI) 与 3D (OpenGL) 模式切换。
 * 2. 3D 模式下支持白色背景、动态量程网格、坐标数值标注。
 * 3. 实现了鼠标左键旋转视角和滚轮缩放功能。
 */

#include "stdafx.h"
#include "AESWin.h"
#include "SensorWnd.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --- 静态常量定义 ---
#define SENSOR_RADIUS    12   // 传感器/定位点在视图中的半径
#define MAXCOLUMNS       5    // 坐标轴默认划分为 5 个刻度段

IMPLEMENT_DYNAMIC(CSensorWnd, CWnd)

CSensorWnd::CSensorWnd()
{
    // 初始化定位坐标为 -1 (代表初始无定位数据)
    mLocationX = mLocationY = mLocationZ = -1;

    // 初始化 2D/3D 共享的逻辑坐标系量程 (默认 0-100)
    mAxisXMin = 0; mAxisXMax = 100;
    mAxisYMin = 0; mAxisYMax = 100;
    mAxisZMin = 0; mAxisZMax = 100;

    // 初始化显示模式
    m_DisplayMode = MODE_2D_WAVE;

    // 初始化 3D 视角参数
    m_fRotX = 25.0f;     // 俯视角
    m_fRotY = -35.0f;    // 偏航角
    m_fScale = 1.0f;     // 默认缩放倍率
    
    m_hDC = NULL;
    m_hRC = NULL;
    m_FontListBase = 0;
}

CSensorWnd::~CSensorWnd() {}

// --- MFC 消息映射 ---
BEGIN_MESSAGE_MAP(CSensorWnd, CWnd)
    ON_WM_PAINT()        // 处理绘图请求
    ON_WM_SIZE()         // 窗口大小改变时调整 OpenGL 投影
    ON_WM_ERASEBKGND()   // 拦截背景擦除以防止闪烁
    ON_WM_LBUTTONDOWN()  // 记录鼠标旋转起点
    ON_WM_MOUSEMOVE()    // 实现鼠标拖拽旋转
    ON_WM_MOUSEWHEEL()   // 实现滚轮缩放
    ON_WM_CREATE()       // 初始化 OpenGL 环境
    ON_WM_DESTROY()      // 释放资源
END_MESSAGE_MAP()

/**
 * @brief 设置物理监控区域的尺寸范围
 * @param zMin/zMax 决定了 3D 空间中厚度/深度方向的显示范围
 */
void CSensorWnd::SetCoordSize(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
{
    mAxisXMin = xMin; mAxisXMax = xMax;
    mAxisYMin = yMin; mAxisYMax = yMax;
    mAxisZMin = zMin; mAxisZMax = zMax;

    // 计算 X, Y, Z 各轴的刻度间隔宽度
    mAxisXUint = max(1, (mAxisXMax - mAxisXMin) / MAXCOLUMNS);
    mAxisYUint = max(1, (mAxisYMax - mAxisYMin) / MAXCOLUMNS);
    mAxisZUint = max(1, (mAxisZMax - mAxisZMin) / MAXCOLUMNS);

    Invalidate(); // 立即触发窗口重绘
}

// --- OpenGL 文字渲染引擎 ---

/**
 * @brief 创建 3D 位图字体映射
 * 将 Windows 字体映射为 OpenGL 显示列表，解决 OpenGL 无法直接绘制文字的问题
 */
void CSensorWnd::BuildFont()
{
    wglMakeCurrent(m_hDC, m_hRC);
    m_FontListBase = glGenLists(96); // 生成 96 个字符的显示列表

    // 创建高质量的 Windows 逻辑字体
    HFONT font = CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                            ANSI_CHARSET, 0, 0, 0, 0, _T("Arial"));

    HFONT oldfont = (HFONT)SelectObject(m_hDC, font);
    // 生成 ASCII 32 到 128 范围的位图字体
    wglUseFontBitmaps(m_hDC, 32, 96, m_FontListBase);
    SelectObject(m_hDC, oldfont);
    DeleteObject(font);
}

void CSensorWnd::KillFont() 
{
    if (m_FontListBase) glDeleteLists(m_FontListBase, 96);
}

/**
 * @brief 在 3D 空间特定坐标处绘制文本
 */
void CSensorWnd::PrintText3D(float x, float y, float z, const char* fmt, ...)
{
    char text[256];
    va_list ap;
    if (fmt == NULL) return;

    va_start(ap, fmt);
    vsprintf_s(text, fmt, ap);
    va_end(ap);

    // 设置文字在 3D 场景中的光栅化位置
    glRasterPos3f(x, y, z);

    glPushAttrib(GL_LIST_BIT);
    glListBase(m_FontListBase - 32); // 指向对应字符的显示列表
    glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
    glPopAttrib();
}

// --- 3D 渲染核心实现 ---

/**
 * @brief 绘制动态 3D 立方体量程网格及数值标签
 */
void CSensorWnd::Draw3DGrid() 
{
    // 1. 绘制灰色参考框架线
    glLineWidth(1.0f);
    glColor3f(0.85f, 0.85f, 0.85f); // 浅灰色，适合白底
    glBegin(GL_LINES);
    
    // 绘制底面 (Zmin) 和顶面 (Zmax) 的 XY 平面网格
    for (int x = mAxisXMin; x <= mAxisXMax; x += mAxisXUint) {
        glVertex3f((float)x, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)x, (float)mAxisYMax, (float)mAxisZMin);
        glVertex3f((float)x, (float)mAxisYMin, (float)mAxisZMax); glVertex3f((float)x, (float)mAxisYMax, (float)mAxisZMax);
    }
    for (int y = mAxisYMin; y <= mAxisYMax; y += mAxisYUint) {
        glVertex3f((float)mAxisXMin, (float)y, (float)mAxisZMin); glVertex3f((float)mAxisXMax, (float)y, (float)mAxisZMin);
        glVertex3f((float)mAxisXMin, (float)y, (float)mAxisZMax); glVertex3f((float)mAxisXMax, (float)y, (float)mAxisZMax);
    }
    // 绘制 Z 轴垂直连接线，构成闭合立方体
    glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMax);
    glVertex3f((float)mAxisXMax, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)mAxisXMax, (float)mAxisYMin, (float)mAxisZMax);
    glVertex3f((float)mAxisXMin, (float)mAxisYMax, (float)mAxisZMin); glVertex3f((float)mAxisXMin, (float)mAxisYMax, (float)mAxisZMax);
    glVertex3f((float)mAxisXMax, (float)mAxisYMax, (float)mAxisZMin); glVertex3f((float)mAxisXMax, (float)mAxisYMax, (float)mAxisZMax);
    glEnd();

    // 2. 绘制彩色主轴线 (增强方向感)
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.2f, 0.2f); glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)mAxisXMax + 20, (float)mAxisYMin, (float)mAxisZMin); // X:红
    glColor3f(0.0f, 0.7f, 0.0f); glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)mAxisXMin, (float)mAxisYMax + 20, (float)mAxisZMin); // Y:绿
    glColor3f(0.2f, 0.2f, 1.0f); glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMin); glVertex3f((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMax + 20); // Z:蓝
    glEnd();

    // 3. 绘制刻度数值 (文字颜色设为黑色)
    glColor3f(0.0f, 0.0f, 0.0f); 

    // X 轴数值刻度标注
    for (int x = mAxisXMin; x <= mAxisXMax; x += mAxisXUint)
        PrintText3D((float)x, (float)mAxisYMin - 12, (float)mAxisZMin, "%d", x);
    PrintText3D((float)mAxisXMax + 25, (float)mAxisYMin, (float)mAxisZMin, "X(mm)");

    // Y 轴数值刻度标注
    for (int y = mAxisYMin; y <= mAxisYMax; y += mAxisYUint)
        PrintText3D((float)mAxisXMin - 35, (float)y, (float)mAxisZMin, "%d", y);
    PrintText3D((float)mAxisXMin, (float)mAxisYMax + 25, (float)mAxisZMin, "Y(mm)");

    // Z 轴数值刻度标注
    for (int z = mAxisZMin; z <= mAxisZMax; z += mAxisZUint)
        PrintText3D((float)mAxisXMin - 15, (float)mAxisYMin - 10, (float)z, "%d", z);
    PrintText3D((float)mAxisXMin, (float)mAxisYMin, (float)mAxisZMax + 25, "Z(mm)");
}

/**
 * @brief 3D 渲染主循环入口
 */
void CSensorWnd::RenderScene3D()
{
    // 清理颜色和深度缓冲区 (背景色已在 InitOpenGL 中设为白色)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 计算量程几何中心点，使场景绕中心旋转，而不是原点旋转
    float cX = (mAxisXMin + mAxisXMax) / 2.0f;
    float cY = (mAxisYMin + mAxisYMax) / 2.0f;
    float cZ = (mAxisZMin + mAxisZMax) / 2.0f;

    // --- 视角变换矩阵 ---
    glTranslatef(0.0f, 0.0f, -450.0f);       // 1. 设置观察距离
    glScalef(m_fScale, m_fScale, m_fScale); // 2. 设置缩放倍率
    glRotatef(m_fRotX, 1.0f, 0.0f, 0.0f);   // 3. 绕 X 轴旋转
    glRotatef(m_fRotY, 0.0f, 1.0f, 0.0f);   // 4. 绕 Y 轴旋转
    
    // 关键位移：将物理量程的中心平移到观察窗口的中心
    glTranslatef(-cX, -cY, -cZ);

    Draw3DGrid();           // 绘制立体量程网格
    Draw3DSensors();        // 绘制实际传感器位置
    Draw3DLocationPoints(); // 绘制声源定位结果点
}

// --- OpenGL 初始化与消息响应 ---

BOOL CSensorWnd::InitOpenGL()
{
    m_hDC = ::GetDC(m_hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR), 1, 
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
        PFD_TYPE_RGBA, 24, 0,0,0,0,0,0, 0,0, 0,0,0,0,0, 32, 0,0, 
        PFD_MAIN_PLANE, 0,0,0,0 
    };
    SetPixelFormat(m_hDC, ChoosePixelFormat(m_hDC, &pfd), &pfd);
    m_hRC = wglCreateContext(m_hDC);
    wglMakeCurrent(m_hDC, m_hRC);

    // 【核心设置】3D 背景设为白色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glEnable(GL_DEPTH_TEST); // 开启深度测试，确保点线遮挡正确
    return TRUE;
}

void CSensorWnd::OnPaint() 
{ 
    CPaintDC dc(this);
    if (m_DisplayMode == MODE_3D_LOCATION) {
        // --- 3D 渲染管线 ---
        wglMakeCurrent(m_hDC, m_hRC); 
        RenderScene3D(); 
        SwapBuffers(m_hDC); // 交换双缓冲，防止画面撕裂
    } else {
        // --- 2D 绘图管线 ---
        DrawAxisAndText(&dc); 
        DrawSensor(&dc); 
        DrawLocation(&dc);
    }
}

void CSensorWnd::OnSize(UINT nType, int cx, int cy) 
{
    CWnd::OnSize(nType, cx, cy); 
    mScreenWidth = cx; mScreenHeight = cy;
    if (m_hRC && cy > 0) {
        wglMakeCurrent(m_hDC, m_hRC); 
        glViewport(0, 0, cx, cy); // 更新视口大小
        glMatrixMode(GL_PROJECTION); 
        glLoadIdentity();
        // 设置 45 度透视投影，远裁剪面设为 1500 以兼容大数据量程
        gluPerspective(45.0, (GLfloat)cx / (GLfloat)cy, 1.0, 1500.0);
        glMatrixMode(GL_MODELVIEW);
    }
}

// --- 交互处理 ---

void CSensorWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
    // 如果左键按下且处于 3D 模式，根据位移更新旋转角度
    if (m_DisplayMode == MODE_3D_LOCATION && (nFlags & MK_LBUTTON)) {
        m_fRotY += (point.x - m_ptLastMouse.x) * 0.5f; 
        m_fRotX += (point.y - m_ptLastMouse.y) * 0.5f;
        m_ptLastMouse = point; 
        Invalidate(FALSE); // 使用 FALSE 参数避免 GDI 擦除导致闪烁
    }
}

BOOL CSensorWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    if (m_DisplayMode == MODE_3D_LOCATION) {
        // zDelta 为正代表滚轮前推，放大场景
        m_fScale += (zDelta > 0) ? 0.1f : -0.1f; 
        m_fScale = max(0.1f, m_fScale); // 最小缩放限制
        Invalidate(FALSE); 
        return TRUE;
    } 
    return FALSE;
}

BOOL CSensorWnd::OnEraseBkgnd(CDC* p) { return TRUE; } // 拦截，防止 2D 绘图闪烁

// --- 数据同步辅助 ---

void CSensorWnd::Draw3DSensors() 
{
    CAESWinApp * pApp = (CAESWinApp *)AfxGetApp();
    glPointSize(10.0f); 
    glBegin(GL_POINTS); 
    glColor3f(0.0f, 0.8f, 0.0f); // 传感器显示为绿色大点
    for (int i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
        glVertex3f((float)pApp->mAudioLocate.sProbePara[i].iX, 
                   (float)pApp->mAudioLocate.sProbePara[i].iY, 
                   (float)pApp->mAudioLocate.sProbePara[i].iZ); // 平铺在底面
    glEnd();
}

void CSensorWnd::Draw3DLocationPoints() 
{
    // 如果存在有效的定位数据 (X != -1)
    if (mLocationX != -1) {
        glPointSize(15.0f); 
        glBegin(GL_POINTS); 
        glColor3f(1.0f, 0.4f, 0.0f); // 定位声源显示为橙色醒目大点
        glVertex3f((float)mLocationX, (float)mLocationY, (float)mLocationZ);
        glEnd();
    }
}

void CSensorWnd::DrawAxisAndText(CDC* pDC)
{
    CRect theRect;
    theRect.left = 0;
    theRect.top = 0;
    theRect.right = mScreenWidth;
    theRect.bottom = mScreenHeight;
    pDC->FillSolidRect(theRect, RGB(255, 255, 255));

    CPen NewPen1(PS_SOLID, 1, RGB(235, 235, 235));
    CPen* pOldPen = pDC->SelectObject(&NewPen1);
    CPoint ScreenPt, LogicalPt;

    int start = mAxisXMin;
    while (start < mAxisXMax)
    {
        LogicalPt.x = start;
        LogicalPt.y = mAxisYMin;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        pDC->MoveTo(ScreenPt.x, ScreenPt.y);

        LogicalPt.y = mAxisYMax;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        pDC->LineTo(ScreenPt.x, ScreenPt.y);
        start += mAxisXUint;
    }
    start = mAxisYMin;
    while (start < mAxisYMax)
    {
        LogicalPt.x = mAxisXMin;
        LogicalPt.y = start;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        pDC->MoveTo(ScreenPt.x, ScreenPt.y);
        LogicalPt.x = mAxisXMax;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        pDC->LineTo(ScreenPt.x, ScreenPt.y);
        start += mAxisYUint;
    }

    pDC->SelectObject(pOldPen);

    CString theStr;
    int yValue;
    pDC->SetTextColor(RGB(255, 0, 0));
    pDC->SetBkMode(TRANSPARENT);
    start = mAxisYMin;
    yValue = mAxisYMin;
    while (start < mAxisYMax)
    {
        LogicalPt.x = mAxisXMin;
        LogicalPt.y = start;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        //	ScreenPt.x -= WND_MARGIN;
        ScreenPt.y -= 15;
        theStr.Format("%d", yValue);
        pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
        start += mAxisYUint;
        yValue += mAxisYUint;
    }
    pDC->SetTextColor(RGB(0, 0, 0));
    yValue = mAxisXMin;
    start = mAxisXMin;
    while (start < mAxisXMax)
    {
        LogicalPt.x = start;
        LogicalPt.y = mAxisYMin;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        ScreenPt.y -= 20;
        ScreenPt.x -= 10;
        theStr.Format("%d", yValue);
        pDC->TextOut(ScreenPt.x, ScreenPt.y, theStr);
        yValue += mAxisXUint;
        start += mAxisXUint;
    }
}

void CSensorWnd::DrawSensor(CDC* pDC)
{
    CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
    CPoint ScreenPt, LogicalPt;
    int i;
    CBrush brush;
    brush.CreateSolidBrush(RGB(0, 255, 0));
    CBrush* pOrigBrush = (CBrush*)pDC->SelectObject(&brush);
    for (i = 0; i < pApp->mAudioLocate.iProbe_Count; i++)
    {
        LogicalPt.x = pApp->mAudioLocate.sProbePara[i].iX;
        LogicalPt.y = pApp->mAudioLocate.sProbePara[i].iY;
        ScreenPt = ptConvertLogicalToScreen(LogicalPt);
        pDC->Ellipse(ScreenPt.x - SENSOR_RADIUS, ScreenPt.y - SENSOR_RADIUS, ScreenPt.x + SENSOR_RADIUS, ScreenPt.y + SENSOR_RADIUS);
    }
    pDC->SelectObject(pOrigBrush);
}

void CSensorWnd::DrawLocation(CDC* pDC)
{
    if (mLocationX == -1 && mLocationY == -1)
        return;
    CPoint ScreenPt, LogicalPt;
    CBrush brush;
    brush.CreateSolidBrush(RGB(255, 0, 0));
    CBrush* pOrigBrush = (CBrush*)pDC->SelectObject(&brush);
    LogicalPt.x = mLocationX;
    LogicalPt.y = mLocationY;
    ScreenPt = ptConvertLogicalToScreen(LogicalPt);
    pDC->Ellipse(ScreenPt.x - SENSOR_RADIUS, ScreenPt.y - SENSOR_RADIUS, ScreenPt.x + SENSOR_RADIUS, ScreenPt.y + SENSOR_RADIUS);
    pDC->SelectObject(pOrigBrush);
}

CPoint  CSensorWnd::ptConvertLogicalToScreen(CPoint pt)
{
    CPoint rePt;
    double x, y;
    pt.x -= mAxisXMin;
    pt.y -= mAxisYMin;
    x = (double)pt.x * mScreenWidth / (mAxisXMax - mAxisXMin);
    y = (double)pt.y * mScreenHeight / (mAxisYMax - mAxisYMin);
    rePt.x = (int)x;
    rePt.y = mScreenHeight - (int)y;
    return rePt;
}

int  CSensorWnd::dfConvertLogicalXToScreen(double x)
{
    int rex;
    x -= (double)mAxisXMin;
    x = x * mScreenWidth / (mAxisXMax - mAxisXMin);
    rex = (int)x;
    return rex;
}
int CSensorWnd::dfConvertLogicalYToScreen(double y)
{
    int rey;
    y -= (double)mAxisYMin;
    y = y * mScreenHeight / (mAxisYMax - mAxisYMin);
    rey = mScreenHeight - (int)y;
    return rey;
}


// 其余 OnCreate, OnDestroy 等管理函数实现...
int CSensorWnd::OnCreate(LPCREATESTRUCT lp) { CWnd::OnCreate(lp); InitOpenGL(); BuildFont(); return 0; }
void CSensorWnd::OnDestroy() { KillFont(); wglDeleteContext(m_hRC); CWnd::OnDestroy(); }
void CSensorWnd::OnLButtonDown(UINT n, CPoint p) { m_ptLastMouse = p; CWnd::OnLButtonDown(n, p); }
void CSensorWnd::AddLocationPt(int xx, int yy, int zz)
{ 
    mLocationX = xx; mLocationY = yy; mLocationZ = zz; Invalidate(); 
}
void CSensorWnd::SetDisplayMode(DisplayMode mode) 
{
    m_DisplayMode = mode; 
    Invalidate();
}