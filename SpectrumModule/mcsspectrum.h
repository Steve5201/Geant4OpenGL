#ifndef MCSSPECTRUM_H
#define MCSSPECTRUM_H

#include <map>
#include <string>

class MCSSpectrum
{
public:
    MCSSpectrum();
    ~MCSSpectrum();

    // 添加能谱数据
    void addDataPoint(double energy, double count);

    // 删除能谱数据
    void removeDataPoint(double energy);

    // 清空能谱数据
    void clear();

    // 获取能谱数据
    double getData(double energy) const;

    // 合并能谱数据
    void mergeData(double energy, double count);

    // 获取属性
    std::string getProperty(std::string p) const;

    // 设置属性
    void setProperty(std::string p, std::string v);

    // 获取最大计数
    double getMaxCount() const;

    // 获取最大能量
    double getMaxEnergy() const;

    // 获取内部数据
    std::map<double, double> getInnerMap() const;

    // 设置内部数据
    void setInnerMap(const std::map<double, double>& map);

    // 获取范围计数
    double getSubArea(double energyMin, double energyMax) const;

    // 获取子谱数据
    MCSSpectrum getSubSpectrum(double energyMin, double energyMax) const;

    // 从本地文件读取能谱数据
    void loadFromJSON(const std::string& filename);

    // 存储能谱数据到本地文件
    void saveToJSON(const std::string& filename) const;  // 写入文件

    // 重载[]操作符
    double& operator[](double energy);

    // 合并另一个谱的数据
    void mergeSpectrum(const MCSSpectrum& other);

    // 属性map设置
    std::map<std::string, std::string> getProperties() const;

    // 属性map设置
    void setProperties(const std::map<std::string, std::string> &newProperties);

private:
    std::map<double, double> spectrumData; // 能谱数据
    std::map<std::string, std::string> properties; //属性

};

#endif // MCSSPECTRUM_H
