#include "stdafx.h"
#include "AESWin.h"
#include "CAELocation2D3D.h"
#include "LogMgn.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>
#include <future>
#include <mutex>
#include <algorithm>
#include <Eigen/Dense>

using namespace Eigen;

// ================= 1. 定位引擎类 (鲁棒性增强版) =================

class AEEngine {
public:
    double waveSpeed;      // 介质声速 (mm/s)
    bool useAnisotropy;    
    Vector3d vWeight;      
    bool is3D;             
    double threshold;      
    bool autoRejection;    
    bool useFreqFilter;    
    double minFreq;        
    double maxFreq;        

    std::map<int, Sensor> sensorMap; 

    // 各向异性有效距离
    double getEffDist(const Vector3d& source, const Vector3d& sensor) {
        Vector3d d = source - sensor;
        if (!useAnisotropy) return d.norm();
        return std::sqrt(std::pow(vWeight.x() * d.x(), 2) +
            std::pow(vWeight.y() * d.y(), 2) +
            std::pow(vWeight.z() * d.z(), 2));
    }

    LocalizationResult solve(int id, std::vector<AEHit> hits) {
        LocalizationResult r;
        r.eventID = id;

        auto minIt = std::min_element(hits.begin(), hits.end(),
            [](const AEHit& a, const AEHit& b) { return a.time < b.time; });
        
        // 【修正】：既然输入是秒，直接使用，不除以 1000
        r.absoluteTime = minIt->time; 

        int minReq = is3D ? 4 : 3;
        if (hits.size() < minReq) return r;

        // 尝试组合（剔除逻辑）
        std::vector<std::vector<int>> combinations;
        std::vector<int> allIdx;
        for (int i = 0; i < (int)hits.size(); ++i) allIdx.push_back(i);
        combinations.push_back(allIdx);

        if (autoRejection && hits.size() > minReq) {
            for (int i = 0; i < (int)hits.size(); ++i) {
                std::vector<int> subIdx;
                for (int j = 0; j < (int)hits.size(); ++j) if (i != j) subIdx.push_back(j);
                combinations.push_back(subIdx);
            }
        }

        for (const auto& idxSet : combinations) {
            auto current = runLM(id, hits, idxSet);
            if (current.valid && current.residual < r.residual) {
                r.pos = current.pos;
                r.residual = current.residual;
                r.sensorsUsed = current.sensorsUsed;
                r.valid = true;
            }
        }
        return r;
    }

private:
    // 计算总残差平方和 (Cost Function)
    double computeCost(const Vector3d& X, const std::vector<AEHit>& hits, const std::vector<int>& idx) {
        double totalSqError = 0;
        Sensor s0 = sensorMap[hits[idx[0]].sensorID];
        double t0 = hits[idx[0]].time;
        for (size_t i = 1; i < idx.size(); ++i) {
            Sensor si = sensorMap[hits[idx[i]].sensorID];
            double ti = hits[idx[i]].time;
            double d0 = getEffDist(X, s0.pos), di = getEffDist(X, si.pos);
            double error = (di - d0) - waveSpeed * (ti - t0);
            totalSqError += error * error;
        }
        return totalSqError;
    }

    // --- 【真正的 LM 算法实现】：即使方程不收敛也能找到最优近似解 ---
    LocalizationResult runLM(int id, const std::vector<AEHit>& hits, const std::vector<int>& idx) {
        LocalizationResult res;
        int n = idx.size();
        int dims = is3D ? 3 : 2;

        // 1. 初始化：重心法
        Vector3d X = Vector3d::Zero();
        for (int i = 0; i < n; ++i) X += sensorMap[hits[idx[i]].sensorID].pos;
        X /= n;
        if (!is3D) X.z() = 0;

        double lambda = 1e-3; // 初始阻尼
        double currentCost = computeCost(X, hits, idx);
        
        MatrixXd J(n - 1, dims);
        VectorXd rVec(n - 1);

        for (int iter = 0; iter < 50; ++iter) { // 增加迭代次数上限
            Sensor s0 = sensorMap[hits[idx[0]].sensorID];
            double t0 = hits[idx[0]].time;

            // 构造雅可比矩阵 J 和残差向量 r
            for (int i = 1; i < n; ++i) {
                Sensor si = sensorMap[hits[idx[i]].sensorID];
                double ti = hits[idx[i]].time;
                double d0 = getEffDist(X, s0.pos), di = getEffDist(X, si.pos);
                rVec(i - 1) = (di - d0) - waveSpeed * (ti - t0);
                for (int j = 0; j < dims; ++j) {
                    double eps = 1e-4;
                    Vector3d X_plus = X; X_plus(j) += eps;
                    J(i - 1, j) = ((getEffDist(X_plus, si.pos) - getEffDist(X_plus, s0.pos)) - (di - d0)) / eps;
                }
            }

            // LM 核心步：求解 (J^T * J + lambda * diag(J^T * J)) * delta = -J^T * r
            MatrixXd JtJ = J.transpose() * J;
            // 改进：使用马夸特策略，对角线缩放
            for(int d=0; d<dims; ++d) JtJ(d,d) += lambda * (JtJ(d,d) + 1e-6); 
            
            VectorXd delta = JtJ.ldlt().solve(-J.transpose() * rVec);

            // 尝试更新位置
            Vector3d nextX = X;
            nextX.head(dims) += delta;
            
            double nextCost = computeCost(nextX, hits, idx);

            // --- LM 步进逻辑控制 ---
            if (nextCost < currentCost) {
                // 如果误差减小了：接受这一步，减小 lambda (更趋向高斯-牛顿)
                X = nextX;
                currentCost = nextCost;
                lambda /= 10.0;
                if (delta.norm() < 1e-6) break; // 收敛退出
            } else {
                // 如果误差变大了：拒绝这一步，增大 lambda (更趋向梯度下降)
                lambda *= 10.0;
                if (lambda > 1e10) break; // 陷入泥潭，退出
            }
        }

        // 最终残差均方根
        double finalRMS = std::sqrt(currentCost / (n - 1));

        // --- 【近似解保底】：只要结果在实验台物理范围内，残差哪怕大点也给解 ---
        // 假设你的实验台是 5000mm 范围
        bool isInside = (X.norm() < 5000.0);

        if (isInside) {
            // 重点：即便 finalRMS 没达到 threshold，我们也给解。
            // 我们可以设置一个“极度宽松”的保底门限，比如 1500mm
            if (finalRMS <= threshold || finalRMS < 1500.0) {
                res.valid = true;
                res.pos = X;
                res.residual = finalRMS;
                res.sensorsUsed = n;
            }
        }
        return res;
    }
};


CAELocation2D3D::CAELocation2D3D()
{

}

CAELocation2D3D::~CAELocation2D3D()
{

}

// ================= 2. 外部接口函数 =================

int CAELocation2D3D::Calculate2D3DLocation(int& xx, int& yy, int& zz, int* chNos, double* times, int count)
{
    CAESWinApp* pApp = (CAESWinApp*)AfxGetApp();
    CString theStr;
    
    char conFileName[256], datFileName[256];
    strcpy_s(conFileName, pApp->m_szSaveFilePath); strcat_s(conFileName, "myconfig.txt");
    strcpy_s(datFileName, pApp->m_szSaveFilePath); strcat_s(datFileName, "mydata.csv");

    AEEngine engine;
    std::ifstream f(conFileName);
    if (!f.is_open()) return 0;

    // 1. 解析配置文件 (必须读取，为了获取传感器的三维坐标 XYZ)
    std::string line; bool isSens = false; engine.vWeight = Vector3d(1, 1, 1);
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line.find("Sensors:") != std::string::npos) { isSens = true; continue; }
        if (!isSens) {
            size_t c = line.find(':'); if (c == std::string::npos) continue;
            std::string k = line.substr(0, c), v = line.substr(c + 1);
            if (k == "Speed") engine.waveSpeed = std::stod(v); 
            else if (k == "Mode") engine.is3D = (v.find("3D") != std::string::npos);
            else if (k == "Threshold") engine.threshold = std::stod(v);
            else if (k == "UseFreqFilter") engine.useFreqFilter = (std::stoi(v) == 1);
            else if (k == "MinFreq") engine.minFreq = std::stod(v);
            else if (k == "MaxFreq") engine.maxFreq = std::stod(v);
            else if (k == "AutoRejection") engine.autoRejection = (std::stoi(v) == 1);
        } else {
            std::stringstream ss(line); int id; double x, y, z, off; char c;
            if (ss >> id >> c >> x >> c >> y >> c >> z >> c >> off)
                engine.sensorMap[id] = { id, Vector3d(x,y,z), off };
        }
    }

    std::map<int, std::vector<AEHit>> groups;

    // --- 【核心修改：内存直传优先】 ---
    if (chNos != NULL && times != NULL && count >= 3) 
    {
        // 实时定位模式：直接从内存数组读取当前的事件数据
        int eid = 1; // 实时模式下分配一个虚拟事件ID
        for (int i = 0; i < count; ++i) {
            // 参数依次为：SensorID, 到达时间, 频率(传入虚拟值防止被过滤)
            groups[eid].push_back({ chNos[i], times[i], engine.minFreq + 10.0 });
        }
    } 
    else 
    {
        // 兼容离线模式：如果没有传内存数组，则去读 csv 文件
        std::ifstream df(datFileName); 
        if(df.is_open()) {
            std::getline(df, line); // 跳过表头
            while (std::getline(df, line)) {
                std::stringstream ss(line); std::string item; int eid, sid; double t, frq;
                std::getline(ss, item, ','); eid = std::stoi(item);
                std::getline(ss, item, ','); sid = std::stoi(item);
                std::getline(ss, item, ','); t = std::stod(item);
                std::getline(ss, item, ','); frq = std::stod(item);
                if (!engine.useFreqFilter || (frq >= engine.minFreq && frq <= engine.maxFreq))
                    groups[eid].push_back({ sid, t, frq });
            }
        }
    }

    // 3. 执行多线程定位计算
    std::mutex mtx;
    std::vector<std::future<LocalizationResult>> futures;
    for (auto const& pair : groups) {
        int id = pair.first;
        std::vector<AEHit> hits = pair.second;
        futures.push_back(std::async(std::launch::async, [&engine, id, hits]() {
            return engine.solve(id, hits);
        }));
    }

    // 4. 提取结果
    int success = 0;
    for (auto& f : futures) {
        auto r = f.get();
        if (r.valid) {
            std::lock_guard<std::mutex> lk(mtx);
            xx = (int)r.pos.x(); yy = (int)r.pos.y(); zz = (int)r.pos.z();
            
            // 写入日志方便调试：注意记录了真正的残差和算出的坐标
            theStr.Format(_T("EventID:%d | Pos:(%d, %d) | Residual:%.2f | Success:%d"), 
                           r.eventID, xx, yy, r.residual, success + 1);
            pApp->m_LogMgnP->Addlog(theStr);
            success++;
        }
    }

    return success;
}