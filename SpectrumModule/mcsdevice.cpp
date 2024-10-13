#include "mcsdevice.h"
#include <QVector3D>
#include <QFileDialog>
#include <QGridLayout>
#include <QApplication>
#include <QMessageBox>
#include <QRandomGenerator>

MCSDevice::MCSDevice(QObject *parent)
    : QObject(parent), handleChannel(0), mainSpectrum(nullptr), backgroundSpectrum(nullptr)
{
    // 算法对象
    analyzeAlgorith = new MCSAnalyzeAlgorithm;
    // 原始视图窗口
    originChartView = new MCSOriginChartView;
    originChartView->setRenderHint(QPainter::Antialiasing, true);
    // 缩放视图窗口
    scaledChartView = new MCSScaledChartView;
    scaledChartView->setRenderHint(QPainter::Antialiasing, true);
    connect(scaledChartView, &MCSScaledChartView::curcorMoved, this, &MCSDevice::onCurcorMoved);
    // 关联两个视图窗口
    connect(originChartView, &MCSOriginChartView::rectangleChanged, scaledChartView, &MCSScaledChartView::onChangeRect);
    connect(scaledChartView, &MCSScaledChartView::newRectSelected, originChartView, &MCSOriginChartView::onChangeRect);
    connect(scaledChartView, &MCSScaledChartView::newPosDelta, originChartView, &MCSOriginChartView::onNewPosDelta);
    connect(scaledChartView, &MCSScaledChartView::requestResetRect, originChartView, &MCSOriginChartView::onResetRectRequest);
    // 初始化算法参数
    parameter.windowSize = 11;
    parameter.sigma = 3.7;
    parameter.minPerkHeight = 10.0;
    parameter.minPerkWidth = 30.0;
    parameter.polyOrder = 1;
    parameter.threshold = 0.0;
}

MCSDevice::~MCSDevice()
{
    delete analyzeAlgorith;
    scaledChartView->deleteLater();
    originChartView->deleteLater();
}

void MCSDevice::resetView()
{
    originChartView->onResetRectRequest();
}

void MCSDevice::resetCurcor()
{
    scaledChartView->resetCurcor();
}

int MCSDevice::getHandleChannel()
{
    return handleChannel;
}

void MCSDevice::setHandleChannel(int channel)
{
    handleChannel = channel;
}

void MCSDevice::setDisplayLine(bool show)
{
    scaledChartView->setDisplayLine(show);
}

void MCSDevice::clearSpectrum()
{
    if(handleChannel == 0)
    {
        if(mainSpectrum)
        {
            scaledChartView->removeMainSeries();
            originChartView->removeMainSeries();
            delete mainSpectrum;
            mainSpectrum = nullptr;
        }
    }
    else
    {
        if(backgroundSpectrum)
        {
            scaledChartView->removeBackgroundSeries();
            originChartView->removeBackgroundSeries();
            delete backgroundSpectrum;
            backgroundSpectrum = nullptr;
        }
    }
}

void MCSDevice::loadSpectrumFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("保存到"), QApplication::applicationDirPath(), tr("谱文件 (*.dat)"));
    if(fileName.isEmpty())
    {
        return;
    }
    MCSSpectrum *s1 = new MCSSpectrum();
    s1->loadFromJSON(fileName.toStdString());
    QScatterSeries *oline = getLine(s1);
    QScatterSeries *sline = getLine(s1);
    if(handleChannel == 0)
    {
        scaledChartView->setMainSeries(sline);
        originChartView->setMainSeries(oline);
        oline->setName("主线");
        sline->setName("主线");
        if(mainSpectrum)
        {
            delete mainSpectrum;
        }
        mainSpectrum = s1;
    }
    else
    {
        scaledChartView->setBackgroundSeries(sline);
        originChartView->setBackgroundSeries(oline);
        oline->setName("背景线");
        sline->setName("背景线");
        if(backgroundSpectrum)
        {
            delete backgroundSpectrum;
        }
        backgroundSpectrum = s1;
    }
}

void MCSDevice::saveSpectrumToFile()
{
    QString fn = QFileDialog::getSaveFileName(nullptr, tr("保存到"), QApplication::applicationDirPath(), tr("谱文件 (*.dat)"));
    if(fn.isEmpty())
    {
        return;
    }
    std::string fileName = fn.toStdString();
    if(handleChannel == 0)
    {
        if(mainSpectrum)
        {
            mainSpectrum->saveToJSON(fileName);
        }
    }
    else
    {
        if(backgroundSpectrum)
        {
            backgroundSpectrum->saveToJSON(fileName);
        }
    }
    QMessageBox::information(nullptr,"提示","保存成功");
}

void MCSDevice::saveSubtractToFile()
{
    if(!mainSpectrum || !backgroundSpectrum)
    {
        return;
    }
    QString fn = QFileDialog::getSaveFileName(nullptr, tr("保存到"), QApplication::applicationDirPath(), tr("谱文件 (*.dat)"));
    if(fn.isEmpty())
    {
        return;
    }
    MCSSpectrum sub;
    foreach (auto v, mainSpectrum->getInnerMap())
    {
        double c = v.second - backgroundSpectrum->getData(v.first);
        sub.addDataPoint(v.first, c > 0 ? c : 0);
    }
    sub.saveToJSON(fn.toStdString());
    QMessageBox::information(nullptr,"提示","保存成功");
}

void MCSDevice::showReducedSpectrum()
{
    scaledChartView->showDifferenceSeries();
    originChartView->showDifferenceSeries();
}

void MCSDevice::hideReducedSpectrum()
{
    scaledChartView->hideDifferenceSeries();
    originChartView->hideDifferenceSeries();
}

MCSScaledChartView *MCSDevice::getScaledChartView() const
{
    return scaledChartView;
}

MCSOriginChartView *MCSDevice::getOriginChartView() const
{
    return originChartView;
}

void MCSDevice::onCurcorMoved(double c1, double c2)
{
    double e1 = c1;
    double e2 = c2;
    double ct1 = 0.0;
    double ct2 = 0.0;
    if(handleChannel == 0)
    {
        if(mainSpectrum)
        {
            ct1 = mainSpectrum->getData(e1);
            ct2 = mainSpectrum->getData(e2);
        }
    }
    else
    {
        if(backgroundSpectrum)
        {
            ct1 = backgroundSpectrum->getData(e1);
            ct2 = backgroundSpectrum->getData(e2);
        }
    }
    QString posinfo = QString::asprintf("光标1：道址%.2f，能量%.2f，计数%.2f\t光标2：道址%.2f，能量%.2f，计数%.2f", c1, e1, ct1 ,c2, e2, ct2);
    emit newCursorInfo(posinfo);
}

void MCSDevice::onNewDataReceived(QVector<double> data)
{
    if(data.empty())
    {
        return;
    }
    clearSpectrum();
    data[0] = 0.0;
    QScatterSeries *oline = nullptr;
    QScatterSeries *sline = nullptr;
    if(handleChannel == 0)
    {
        if(!mainSpectrum)
        {
            mainSpectrum = new MCSSpectrum;
        }
        for (int i = 0; i < data.size(); ++i)
        {
            double v = data[i];
            mainSpectrum->mergeData(i, v);
        }
        oline = getLine(mainSpectrum);
        sline = getLine(mainSpectrum);
        scaledChartView->setMainSeries(sline);
        originChartView->setMainSeries(oline);
        oline->setName("主线");
        sline->setName("主线");
        originChartView->setChannelRange(0, mainSpectrum->getMaxEnergy());
    }
    else
    {
        if(!backgroundSpectrum)
        {
            backgroundSpectrum = new MCSSpectrum;
        }
        for (int i = 0; i < data.size(); ++i)
        {
            double v = data[i];
            backgroundSpectrum->mergeData(i, v);
        }
        oline = getLine(backgroundSpectrum);
        sline = getLine(backgroundSpectrum);
        scaledChartView->setMainSeries(sline);
        originChartView->setMainSeries(oline);
        oline->setName("主线");
        sline->setName("主线");
        originChartView->setChannelRange(0, backgroundSpectrum->getMaxEnergy());
    }
}

void MCSDevice::onNewAlgorithmParameters(Parameter p)
{
    parameter = p;
}

QScatterSeries *MCSDevice::getLine(MCSSpectrum *spectrum)
{
    int step = 1;
    if(step < 1)
    {
        step = 1;
    }
    QScatterSeries *rt = new QScatterSeries;
    auto map = spectrum->getInnerMap();
    auto it = map.begin();
    int i = 0;
    while(it != map.end())
    {
        double v = 0.0;
        for (int i = 0; i < step; ++i)
        {
            if(it != map.end())
            {
                v += it->second;
                it++;
            }
        }
        rt->append(i, v);
        i++;
    }
    return rt;
}
