#include "mcsspectrum.h"
#include <string>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QMessageBox>

MCSSpectrum::MCSSpectrum()
{
}

MCSSpectrum::~MCSSpectrum()
{
}

// 添加能谱数据
void MCSSpectrum::addDataPoint(double energy, double count)
{
    spectrumData[energy] = count;
}

// 删除能谱数据
void MCSSpectrum::removeDataPoint(double energy)
{
    spectrumData.erase(energy);
}

// 清空能谱数据
void MCSSpectrum::clear()
{
    spectrumData.clear();
}

// 获取能谱数据
double MCSSpectrum::getData(double energy) const
{
    auto it = spectrumData.begin();
    double c = 0.0;
    double min = 999999999999999.0;
    while(it != spectrumData.end())
    {
        if(std::abs(it->first - energy) < min)
        {
            c = it->second;
            min = std::abs(it->first - energy);
        }
        it++;
    }
    return c;
}

// 合并能谱数据
void MCSSpectrum::mergeData(double energy, double count)
{
    auto it = spectrumData.find(energy);
    if (it == spectrumData.end())
    {
        spectrumData[energy] = 0.0;
    }
    spectrumData[energy] += count;
}

// 获取属性
std::string MCSSpectrum::getProperty(std::string p) const
{
    auto it = properties.find(p);
    if(it != properties.end())
    {
        std::string property = it->second;
        return property;
    }
    return std::string();
}

// 设置属性
void MCSSpectrum::setProperty(std::string p, std::string v)
{
    properties[p] = v;
}

// 获取最大计数
double MCSSpectrum::getMaxCount() const
{
    double rt = 0.0;
    for (const auto& pair : spectrumData)
    {
        if (pair.second > rt)
        {
            rt = pair.second;
        }
    }
    return rt;
}

// 获取最大能量
double MCSSpectrum::getMaxEnergy() const
{
    double rt = 0.0;
    for (const auto& pair : spectrumData)
    {
        if (pair.first > rt)
        {
            rt = pair.first;
        }
    }
    return rt;
}

// 获取内部数据
std::map<double, double> MCSSpectrum::getInnerMap() const
{
    return spectrumData;
}

// 设置内部数据
void MCSSpectrum::setInnerMap(const std::map<double, double> &map)
{
    spectrumData = map;
}

// 获取范围计数
double MCSSpectrum::getSubArea(double energyMin, double energyMax) const
{
    double rt = 0.0;
    for (const auto& pair : spectrumData)
    {
        if (pair.first > energyMin && pair.first < energyMax)
        {
            rt += pair.second;
        }
    }
    return rt;
}

// 获取子谱数据
MCSSpectrum MCSSpectrum::getSubSpectrum(double energyMin, double energyMax) const
{
    MCSSpectrum subSpectrum;
    for (const auto& pair : spectrumData)
    {
        if (pair.first >= energyMin && pair.first <= energyMax)
        {
            subSpectrum.addDataPoint(pair.first, pair.second);
        }
    }
    return subSpectrum;
}

// 从本地文件读取能谱数据
void MCSSpectrum::loadFromJSON(const std::string& filename)
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(nullptr, "提示", "文件未打开");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        QMessageBox::information(nullptr, "提示", "文件解析出错，文件损坏: " + parseError.errorString());
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 清空数据
    this->clear();
    properties.clear();

    // 读取 "org_data"
    if (jsonObj.contains("org_data") && jsonObj["org_data"].isArray())
    {
        QJsonArray orgDataArray = jsonObj["org_data"].toArray();
        for (int i = 0; i < orgDataArray.size(); ++i)
        {
            this->addDataPoint(static_cast<double>(i), orgDataArray[i].toDouble());
        }
    }

    // 读取其他属性
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it)
    {
        QString key = it.key();
        if (key != "org_data")  // "org_data" 已经单独处理过
        {
            properties[key.toStdString()] = it.value().toString().toStdString();
        }
    }
}

// 存储能谱数据到本地文件
void MCSSpectrum::saveToJSON(const std::string& filename) const
{
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(nullptr, "提示", "文件保存失败");
        return;
    }

    QJsonObject jsonObj;

    // 写入基本属性
    for (const auto& [key, value] : properties)
    {
        jsonObj[QString::fromStdString(key)] = QString::fromStdString(value);
    }

    // 写入 org_data
    QJsonArray orgDataArray;
    for (const auto& [energy, count] : spectrumData)
    {
        orgDataArray.append(count);
    }
    jsonObj["org_data"] = orgDataArray;

    // 将 JSON 对象写入文件
    QJsonDocument jsonDoc(jsonObj);
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}

// 重载[]操作符
double& MCSSpectrum::operator[](double energy)
{
    return spectrumData[energy];
}

// 合并另一个谱的数据
void MCSSpectrum::mergeSpectrum(const MCSSpectrum& other)
{
    for (const auto& pair : other.spectrumData)
    {
        spectrumData[pair.first] += pair.second;
    }
}

// 属性map设置
std::map<std::string, std::string> MCSSpectrum::getProperties() const
{
    return properties;
}

// 属性map设置
void MCSSpectrum::setProperties(const std::map<std::string, std::string> &newProperties)
{
    properties = newProperties;
}

