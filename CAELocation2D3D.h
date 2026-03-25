#pragma once
#include <iostream>     // 标准输入输出流
#include <vector>       // 动态数组容器
#include <string>       // 字符串类
#include <fstream>      // 文件流操作
#include <sstream>      // 字符串流解析
#include <cmath>        // 数学函数库
#include <map>          // 键值对映射容器
#include <future>       // 异步多线程
#include <mutex>        // 互斥锁（确保写文件线程安全）
#include <iomanip>      // 输出格式控制
#include <algorithm>    // 算法库（用于寻找最早时间）
#include <Eigen/Dense>  // Eigen矩阵运算库

using namespace Eigen;  // 使用 Eigen 命名空间

struct AEHit {          // 定义单次传感器触发数据
    int sensorID;       // 传感器编号
    double time;        // 触发时间 (ms)
    double frequency;   // 主频 (kHz)
};

struct Sensor {         // 定义传感器硬件参数
    int id;             // 传感器编号
    Vector3d pos;       // 传感器三维坐标 (mm)
    double timeOffset;  // 线缆延迟补偿 (ms)
};

struct LocalizationResult { // 定义定位计算结果
    int eventID;            // 事件原始编号
    double absoluteTime;    // 事件发生的绝对时间 (s)
    bool valid = false;     // 定位结果是否通过残差校验
    Vector3d pos = Vector3d::Zero(); // 计算得出的三维坐标 (mm)
    double residual = 9999.0;        // 定位残差（拟合误差）
    int sensorsUsed = 0;             // 最终参与计算的传感器数量
};
class CAELocation2D3D
{
public:
    CAELocation2D3D();
	~CAELocation2D3D();

    int Calculate2D3DLocation(int& xx, int& yy, int& zz, int* chNos = NULL, double* times = NULL, int count = 0);
};

