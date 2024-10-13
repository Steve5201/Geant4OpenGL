#ifndef MCSORIGINCHARTVIEW_H
#define MCSORIGINCHARTVIEW_H

#include <QWheelEvent>
#include <QMouseEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QScatterSeries>
#include "mcsdraggablerect.h"

class MCSOriginChartView : public QChartView
{
    Q_OBJECT

public:
    MCSOriginChartView(QWidget *parent = nullptr);
    ~MCSOriginChartView();

    void refreshView();
    void setChannelRange(int begin, int end);
    void removeMainSeries();
    void removeBackgroundSeries();
    void setMainSeries(QScatterSeries *series);
    void setBackgroundSeries(QScatterSeries *backgroundSeries);
    void showDifferenceSeries();
    void hideDifferenceSeries();
    void addRoiSeries(QAreaSeries *series);
    void removeRoiSeries(QAreaSeries *series);

    QScatterSeries *getMainSeries() const;
    QScatterSeries *getBackgroundSeries() const;
    QScatterSeries *getDifferenceSeries() const;

signals:
    void rectangleChanged(QPointF topleft, QPointF downright);

public slots:
    void onRectPosChanged();
    void onResetRectRequest();
    void onNewPosDelta(QPointF delta);
    void onChangeRect(QPointF topleft, QPointF downright);

private:
    MCSDraggableRect *draggableRect;
    QScatterSeries *mainSeries;
    QScatterSeries *backgroundSeries;
    QScatterSeries *differenceSeries;
    QScatterSeries *dm,*db;
    QChart *chartV;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QScatterSeries *scaleSeries;
    qreal max1,max2;

    void calculateDifferenceSeries();

};

#endif // MCSORIGINCHARTVIEW_H
