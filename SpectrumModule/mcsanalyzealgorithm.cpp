#include "mcsanalyzealgorithm.h"
#include <cmath>
#include <vector>

// 高斯函数
double MCSAnalyzeAlgorithm::gaussian(double x, double sigma)
{
    return std::exp(-0.5 * (x * x) / (sigma * sigma)) / (sigma * std::sqrt(2.0 * M_PI));
}

// 高斯平滑法平滑处理
MCSSpectrum MCSAnalyzeAlgorithm::gaussianSmooth(const MCSSpectrum& spectrum, int windowSize, double sigma)
{
    MCSSpectrum smoothedSpectrum;
    std::map<double, double> dataMap = spectrum.getInnerMap();
    std::vector<std::pair<double, double>> data(dataMap.begin(), dataMap.end());

    int window = windowSize;

    for (size_t i = 0; i < data.size(); ++i)
    {
        double sum = 0.0;
        double weightSum = 0.0;
        for (int j = -window; j <= window; ++j)
        {
            if (i + j >= 0 && i + j < data.size())
            {
                double weight = gaussian(j, sigma);
                sum += weight * data[i + j].second;
                weightSum += weight;
            }
        }
        smoothedSpectrum.addDataPoint(data[i].first, sum / weightSum);
    }
    return smoothedSpectrum;
}

// 平滑窗口算法
MCSSpectrum MCSAnalyzeAlgorithm::smoothingWindow(MCSSpectrum &spectrum, int windowSize)
{
    MCSSpectrum rt;
    if (windowSize <= 1 || spectrum.getInnerMap().size() < windowSize)
    {
        return rt;
    }

    std::map<double, double> smoothedData;
    const auto& data = spectrum.getInnerMap();

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        double sum = 0.0;
        int count = 0;
        for (int i = -windowSize / 2; i <= windowSize / 2; ++i)
        {
            if (std::distance(it, data.end()) > i)
            {
                auto neighborIt = std::next(it, i);
                if (neighborIt != data.end() && neighborIt != data.begin())
                {
                    sum += neighborIt->second;
                    ++count;
                }
            }
        }
        smoothedData[it->first] = sum / count;
    }

    rt.setInnerMap(smoothedData);
    return rt;
}

// 重心算法
MCSSpectrum MCSAnalyzeAlgorithm::centroid(const MCSSpectrum &spectrum, int windowSize)
{
    MCSSpectrum rt;
    if (windowSize <= 1 || spectrum.getInnerMap().size() < windowSize)
    {
        return rt;
    }

    std::map<double, double> smoothedData;
    const auto& data = spectrum.getInnerMap();

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        double sum = 0.0;
        int count = 0;
        for (int i = -windowSize / 2; i <= windowSize / 2; ++i)
        {
            if (std::distance(it, data.end()) > i)
            {
                auto neighborIt = std::next(it, i);
                if (neighborIt != data.end() && neighborIt != data.begin())
                {
                    sum += neighborIt->second * 1.0 / (std::abs(i) + 1);
                    ++count;
                }
            }
        }
        smoothedData[it->first] = sum / count;
    }

    rt.setInnerMap(smoothedData);
    return rt;
}

// 滤波光滑算法
MCSSpectrum MCSAnalyzeAlgorithm::savitzkyGolaySmooth(const MCSSpectrum &spectrum, int windowSize, int polyOrder)
{
    MCSSpectrum rt;
    if (windowSize <= 1 || spectrum.getInnerMap().size() < windowSize)
    {
        return rt;
    }

    std::vector<double> coefficients(windowSize);
    int halfWindow = windowSize / 2;

    // 生成Savitzky-Golay滤波器的系数
    for (int i = -halfWindow; i <= halfWindow; ++i)
    {
        double numerator = 0;
        for (int j = 0; j <= polyOrder; ++j)
        {
            numerator += pow(i, j);
        }
        coefficients[halfWindow + i] = numerator;
    }

    std::map<double, double> smoothedData;
    const auto& data = spectrum.getInnerMap();

    auto it = data.begin();
    for (int i = 0; i < halfWindow; ++i, ++it)
    {
        smoothedData[it->first] = it->second;
    }

    for (it = std::next(data.begin(), halfWindow); it != std::prev(data.end(), halfWindow); ++it)
    {
        double smoothedValue = 0.0;
        int k = 0;
        for (int i = -halfWindow; i <= halfWindow; ++i)
        {
            auto neighborIt = std::next(it, i);
            smoothedValue += coefficients[k++] * neighborIt->second;
        }
        smoothedData[it->first] = smoothedValue;
    }

    for (; it != data.end(); ++it)
    {
        smoothedData[it->first] = it->second;
    }

    rt.setInnerMap(smoothedData);
    return rt;
}

// 计算数据的一阶导数
std::vector<double> MCSAnalyzeAlgorithm::computeFirstDerivative(const std::vector<double>& data)
{
    size_t n = data.size();
    std::vector<double> firstDerivative(n - 1, 0.0);

    for (size_t i = 0; i < n - 1; ++i)
    {
        firstDerivative[i] = data[i + 1] - data[i];
    }

    return firstDerivative;
}

// 计算峰的边界
std::tuple<double, double, int, int> MCSAnalyzeAlgorithm::findPeakBoundaries(
    const std::vector<double>& energies,
    const std::vector<double>& data,
    size_t peakIndex,
    double minPeakHeight)
{
    size_t n = data.size();

    // 查找左边界
    size_t left = peakIndex;
    while (left > 0)
    {
        if (data[left - 1] > data[left]) break;
        --left;
    }

    // 查找右边界
    size_t right = peakIndex;
    while (right < n - 1)
    {
        if (data[right + 1] > data[right]) break;
        ++right;
    }

    return {energies[left], energies[right], left, right};
}

// 一阶导数法寻峰
std::vector<std::tuple<double, double, double, double>> MCSAnalyzeAlgorithm::findPeaksFirstDerivative(
    const MCSSpectrum& spectrum, double countThreshold, int minPeakWidth)
{
    std::vector<std::tuple<double, double, double, double>> peaks;
    std::vector<double> energies;
    std::vector<double> data;

    // 提取能谱数据
    for (const auto& pair : spectrum.getInnerMap())
    {
        energies.push_back(pair.first);
        data.push_back(pair.second);
    }

    // 计算一阶导数
    std::vector<double> firstDerivative = computeFirstDerivative(data);

    size_t n = firstDerivative.size();
    for (size_t i = 1; i < n - 1; ++i)
    {
        // 检查峰值条件：一阶导数由正变负且峰值计数大于阈值
        if (firstDerivative[i - 1] > 0 && firstDerivative[i] <= 0 && data[i] > countThreshold)
        {
            // 计算峰的边界
            auto [leftBound, rightBound, leftIndex, rightIndex] = findPeakBoundaries(energies, data, i, countThreshold);
            // 计算峰的宽度
            double peakWidth = rightBound - leftBound;

            // 只有在峰宽度大于最小宽度时才记录峰值
            double peakEnergy = energies[i];
            double peakArea = 0;
            for (int j = leftIndex; j < rightIndex; ++j)
            {
                peakArea += data[j];
            }
            if (peakWidth >= minPeakWidth)
            {
                peaks.push_back(std::make_tuple(peakEnergy, peakArea, leftBound, rightBound));
            }
            double pc = data[i];
            double lc = data[leftIndex];
            double rc = data[rightIndex];
            double maxc = spectrum.getMaxCount();
            if(pc - lc > maxc / 5 && pc - rc > maxc / 5)
            {
                peaks.push_back(std::make_tuple(peakEnergy, peakArea, leftBound, rightBound));
            }
        }
    }

    return peaks;
}

// 扣除本地本底
MCSSpectrum MCSAnalyzeAlgorithm::removeBackground(const MCSSpectrum &spectrum)
{
    MCSSpectrum rt;
    std::map<double, double> data = spectrum.getInnerMap();
    std::vector<double> cdata;
    for (const auto& pair : data)
    {
        cdata.push_back(pair.second);
    }
    std::vector<double> bdata = getBackGround(cdata, 77);
    for (const auto& pair : data)
    {
        rt[pair.first] = cdata[pair.first] - bdata[pair.first];
    }
    return rt;
}

// 获取背景谱
MCSSpectrum MCSAnalyzeAlgorithm::getBackGround(const MCSSpectrum &spectrum)
{
    MCSSpectrum rt;
    std::map<double, double> data = spectrum.getInnerMap();
    std::vector<double> cdata;
    std::vector<double> edata;
    for (const auto& pair : data)
    {
        cdata.push_back(pair.second);
        edata.push_back(pair.first);
    }
    std::vector<double> bdata = getBackGround(cdata, 77);
    for (int i = 0; i < bdata.size() && i < edata.size(); ++i)
    {
        rt[edata[i]] = bdata[i];
    }
    return rt;
}

// 计算背景曲线
std::vector<double> MCSAnalyzeAlgorithm::getBackGround(const std::vector<double> &data, int k)
{
    // 初始化 y 为输入数据 data
    std::vector<double> y = data;
    if(y.size() == 0)
    {
        return y;
    }

    // 逐渐减小平滑范围 i，从 k 到 0
    for (int i = k; i >= 0; --i)
    {
        // 复制 y 到 x
        std::vector<double> x(y);

        // 对 y 进行平滑处理，从 i 到 y.size() - i
        for (size_t j = i; j < y.size() - i; ++j)
        {
            // 比较当前点 y[j] 与其左右对称点的均值
            if (y[j] < (y[j + i] + y[j - i]) / 2)
            {
                x[j] = y[j];  // 保持原值
            }
            else
            {
                x[j] = (y[j + i] + y[j - i]) / 2;  // 使用对称点均值进行平滑
            }
        }

        // 更新 y 为 x，准备进行下一次迭代
        y = x;
    }

    // 返回处理后的背景数据 y
    return y;
}
