#ifndef MCSDEVICE_H
#define MCSDEVICE_H

#include <QObject>
#include <QtCharts/QScatterSeries>
#include "mcsscaledchartview.h"
#include "mcsoriginchartview.h"
#include "mcsspectrum.h"
#include "mcsanalyzealgorithm.h"

struct Parameter
{
    int windowSize;
    int polyOrder;
    double sigma;
    double threshold;
    double minPerkHeight;
    double minPerkWidth;
};

class MCSDevice : public QObject
{
    Q_OBJECT

public:
    explicit MCSDevice(QObject *parent = nullptr);
    ~MCSDevice();

    void resetView();

    void resetCurcor();

    int getHandleChannel();

    void setHandleChannel(int channel);

    void setDisplayLine(bool show);

    void clearSpectrum();

    void loadSpectrumFromFile();

    void saveSpectrumToFile();

    void saveSubtractToFile();

    void showReducedSpectrum();

    void hideReducedSpectrum();

    MCSScaledChartView *getScaledChartView() const;

    MCSOriginChartView *getOriginChartView() const;

    int getResKeV() const;
    void setResKeV(int newResKeV);

signals:
    void newCursorInfo(QString info);

public slots:
    void onCurcorMoved(double c1, double c2);
    void onNewDataReceived(QVector<double> data);
    void onNewAlgorithmParameters(Parameter parameter);

private:
    int resKeV;
    int handleChannel;
    Parameter parameter;
    MCSScaledChartView *scaledChartView;
    MCSOriginChartView *originChartView;
    MCSAnalyzeAlgorithm *analyzeAlgorith;
    MCSSpectrum *mainSpectrum;
    MCSSpectrum *backgroundSpectrum;

    QScatterSeries *getLine(MCSSpectrum *spectrum);

};

#endif // MCSDEVICE_H
