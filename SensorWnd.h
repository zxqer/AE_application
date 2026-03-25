/**
 * @file SensorWnd.h
 * @brief 传感器布局展示与 3D 声源定位显示窗口类
 * * 本类基于 MFC CWnd 构建，集成了 GDI (2D) 与 OpenGL (3D) 两套渲染引擎。
 * 用于实时展示声发射传感器的物理位置以及计算出的 3D 定位坐标。
 */

#pragma once

#include <afxwin.h>  // 包含 MFC 核心和标准组件，解决 BOOL 等类型识别问题
#include <GL/gl.h>   // OpenGL 核心库：提供基本的绘图命令（如 glBegin, glVertex）
#include <GL/glu.h>  // OpenGL 实用库：提供高级绘图功能（如 gluPerspective, gluLookAt）

// 自动链接 OpenGL 运行库
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

/**
 * @enum DisplayMode
 * @brief 定义窗口的显示模式
 */
enum DisplayMode {
	MODE_2D_WAVE = 0,     // 2D 模式：使用 GDI 绘制平面的传感器布局（XY 平面）
	MODE_3D_LOCATION = 1  // 3D 模式：使用 OpenGL 绘制立体的量程网格和空间定位点
};

/**
 * @class CSensorWnd
 * @brief 自定义窗口类，处理声发射定位的可视化
 */
class CSensorWnd : public CWnd
{
	DECLARE_DYNAMIC(CSensorWnd) // 支持 MFC 运行时类型识别

public:
	// --- 构造与析构 ---
	CSensorWnd();
	virtual ~CSensorWnd();

	// --- 核心显示状态 ---
	DisplayMode m_DisplayMode;            // 当前窗口处于 2D 还是 3D 模式
	void SetDisplayMode(DisplayMode mode); // 切换模式并触发重绘

	// --- 2D/3D 逻辑坐标系变量 (物理量程) ---
	// 这些变量定义了监控区域的实际物理尺寸（通常单位为 mm）
	int   mScreenWidth, mScreenHeight;    // 窗口当前的像素宽高
	int   mAxisXMin, mAxisXMax;           // X 轴物理量程范围
	int   mAxisYMin, mAxisYMax;           // Y 轴物理量程范围
	int   mAxisZMin, mAxisZMax;           // Z 轴物理量程范围 (3D 专用)
	int   mAxisXUint, mAxisYUint, mAxisZUint; // 坐标轴每个刻度代表的物理长度

	// --- 定位数据变量 ---
	int   mLocationX, mLocationY, mLocationZ; // 当前计算出的声源 3D 坐标位置

	// --- 基础接口函数 ---
	/**
	 * @brief 设置监控区域的物理尺寸
	 * @param xMin/xMax X轴范围, yMin/yMax Y轴范围, zMin/zMax Z轴范围 (厚度/深度)
	 */
	void  SetCoordSize(int xMin, int xMax, int yMin, int yMax, int zMin = 0, int zMax = 100);
	
	/**
	 * @brief 更新并绘制新的定位点坐标
	 */
	void  AddLocationPt(int xx, int yy, int zz = 0);

	// --- 2D 坐标转换与绘图辅助 (GDI) ---
	CPoint ptConvertLogicalToScreen(CPoint pt); // 将物理坐标转换为屏幕像素坐标
	int   dfConvertLogicalXToScreen(double x);  // 转换 X 逻辑坐标到屏幕
	int   dfConvertLogicalYToScreen(double y);  // 转换 Y 逻辑坐标到屏幕
	
	void DrawAxisAndText(CDC* pDC);      // 绘制 2D 模式下的坐标轴背景和刻度文字
	void DrawSensor(CDC* pDC);           // 绘制 2D 模式下的传感器阵列
	void DrawLocation(CDC* pDC);         // 绘制 2D 模式下的定位结果点



protected:
	// --- OpenGL 核心成员 ---
	HGLRC m_hRC;               // OpenGL 渲染上下文 (Rendering Context)
	HDC   m_hDC;               // Windows 设备上下文句柄 (Device Context)
	
	// --- 3D 视角交互变量 ---
	float m_fRotX;             // 视角绕 X 轴旋转的角度 (上下翻转)
	float m_fRotY;             // 视角绕 Y 轴旋转的角度 (左右旋转)
	float m_fScale;            // 整体场景的缩放比例
	CPoint m_ptLastMouse;      // 记录鼠标最后一次点击/移动的位置，用于计算旋转差值

	// --- OpenGL 3D 字体渲染支持 ---
	GLuint  m_FontListBase;    // 存储 96 个字符位图显示列表的起始 ID
	void    BuildFont();       // 初始化字体：将 Windows 字体映射到 OpenGL
	void    KillFont();        // 销毁字体资源
	/**
	 * @brief 在 3D 空间的 (x, y, z) 处打印文字
	 * @param fmt 格式化字符串，类似于 printf
	 */
	void    PrintText3D(float x, float y, float z, const char* fmt, ...);

	// --- 3D 绘图内部子程序 ---
	BOOL InitOpenGL();         // 初始化 OpenGL 像素格式及上下文
	void RenderScene3D();      // 3D 绘图总入口，处理平移、旋转、缩放
	void Draw3DGrid();         // 核心：动态绘制带刻度数值的 3D 立方体量程网格
	void Draw3DLocationPoints(); // 在立体空间中绘制声源定位点
	void Draw3DSensors();      // 在立体空间中绘制传感器阵列位置

	// --- MFC 消息映射声明 ---
	DECLARE_MESSAGE_MAP()

public:
	// --- MFC 系统消息响应函数 ---
	afx_msg void OnPaint();                 // 窗口绘图消息
	afx_msg void OnSize(UINT nType, int cx, int cy); // 窗口大小改变消息：用于更新 3D 视口
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);    // 拦截背景擦除：防止 2D 绘图闪烁
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct); // 窗口创建消息：初始化 OpenGL
	afx_msg void OnDestroy();               // 窗口销毁消息：释放资源

	// --- 鼠标交互响应 ---
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); // 鼠标左键按下：开始旋转视角
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);   // 鼠标移动：实现 3D 拖拽旋转
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt); // 鼠标滚轮：实现 3D 缩放
};