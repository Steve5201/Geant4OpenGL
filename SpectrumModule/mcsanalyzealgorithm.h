#ifndef MCSANALYZEALGORITHM_H
#define MCSANALYZEALGORITHM_H
#define _USE_MATH_DEFINES

#include "mcsspectrum.h"
#include <vector>

class MCSAnalyzeAlgorithm
{

public:
    virtual ~MCSAnalyzeAlgorithm();

    // 高斯函数
    virtual double gaussian(double x, double sigma);

    // 高斯平滑法平滑处理
    virtual MCSSpectrum gaussianSmooth(const MCSSpectrum& spectrum, int windowSize = 5, double sigma = 1);

    // 平滑窗口算法
    virtual MCSSpectrum smoothingWindow(MCSSpectrum& spectrum, int windowSize = 5);

    // 重心算法
    virtual MCSSpectrum centroid(const MCSSpectrum& spectrum, int windowSize = 5);

    // 滤波光滑算法
    virtual MCSSpectrum savitzkyGolaySmooth(const MCSSpectrum& spectrum, int windowSize = 5, int polyOrder = 1);

    // 计算数据的一阶导数
    virtual std::vector<double> computeFirstDerivative(const std::vector<double>& data);

    // 一阶导数法计算峰值
    virtual std::vector<std::tuple<double, double, double, double>> findPeaksFirstDerivative(
        const MCSSpectrum& spectrum, double countThreshold = 50, int minPeakWidth = 10);

    // 计算峰的边界
    virtual std::tuple<double, double, int, int> findPeakBoundaries(const std::vector<double>& energies,
        const std::vector<double>& data, size_t peakIndex, double minPeakHeight);

    // 扣除本地本底
    virtual MCSSpectrum removeBackground(const MCSSpectrum& spectrum);

    // 获取背景谱
    virtual MCSSpectrum getBackGround(const MCSSpectrum& spectrum);

    // 计算背景曲线
    virtual std::vector<double> getBackGround(const std::vector<double>& data, int k = 30);

private:

};

inline MCSAnalyzeAlgorithm::~MCSAnalyzeAlgorithm(){}

#endif // MCSANALYZEALGORITHM_H
