#include "mcsoriginchartview.h"
#include <QApplication>
#include <QDebug>

MCSOriginChartView::MCSOriginChartView(QWidget *parent)
    : QChartView(parent), draggableRect(new MCSDraggableRect()), mainSeries(nullptr),
    backgroundSeries(nullptr), differenceSeries(new QScatterSeries()), dm(nullptr), db (nullptr), max1(0.0), max2(0.0)
{
    setMouseTracking(true);
    // 创建x轴
    axisX = new QValueAxis();
    axisX->setTitleText("");
    axisX->setTickCount(2);
    axisX->setLabelsVisible(false);
    axisX->setRange(0.0, 1024.0);
    // 创建y轴
    axisY = new QValueAxis();
    axisY->setTitleText("");
    axisY->setTickCount(2);
    axisY->setLabelsVisible(false);
    axisY->setRange(0.0, 10.0);
    // 创建表
    chartV = chart();
    chartV->addAxis(axisX, Qt::AlignBottom);
    chartV->addAxis(axisY, Qt::AlignLeft);
    chartV->legend()->hide();
    // 设置工具序列
    scaleSeries = new QScatterSeries(chartV);
    chartV->addSeries(scaleSeries);
    scaleSeries->attachAxis(axisX);
    scaleSeries->attachAxis(axisY);
    scaleSeries->hide();
    // differenceSeries->setUseOpenGL(true);
    differenceSeries->setMarkerSize(3);
    chartV->addSeries(differenceSeries);
    differenceSeries->attachAxis(axisX);
    differenceSeries->attachAxis(axisY);
    differenceSeries->hide();
    // 设置拖拽放大区
    chartV->scene()->addItem(draggableRect);
    draggableRect->setLimitChart(chartV);
    draggableRect->setBrush(QColor(103, 103, 103, 37));
    QPen pen = draggableRect->pen();
    pen.setColor(Qt::red);
    draggableRect->setPen(pen);
    onResetRectRequest();
    connect(draggableRect, &MCSDraggableRect::rectChanged, this, &MCSOriginChartView::onRectPosChanged);
}

MCSOriginChartView::~MCSOriginChartView()
{
    if(mainSeries)
    {
        mainSeries->deleteLater();
    }
    if(backgroundSeries)
    {
        backgroundSeries->deleteLater();
    }
}

void MCSOriginChartView::refreshView()
{
    chartV->setPlotArea(chartV->rect());
    chartV->setPlotAreaBackgroundVisible(true);
    chartV->setPlotAreaBackgroundBrush(QColor(55, 55, 100));
    onResetRectRequest();
}

void MCSOriginChartView::setChannelRange(int begin, int end)
{
    if(!axisX)
    {
        return;
    }
    axisX->setRange(begin, end);
    onRectPosChanged();
}

void MCSOriginChartView::removeMainSeries()
{
    if(mainSeries)
    {
        mainSeries->clear();
        mainSeries->hide();
        chart()->removeSeries(mainSeries);
        mainSeries = nullptr;
    }
    differenceSeries->hide();
    max1 = 0.0;
    if(max2 > 0)
    {
        axisY->setRange(0.0, max2 * 1.2);
    }
    onRectPosChanged();
}

void MCSOriginChartView::removeBackgroundSeries()
{
    if(backgroundSeries)
    {
        backgroundSeries->clear();
        backgroundSeries->hide();
        chart()->removeSeries(backgroundSeries);
        backgroundSeries = nullptr;
    }
    differenceSeries->hide();
    max2 = 0.0;
    if(max1 > 0)
    {
        axisY->setRange(0.0, max1 * 1.2);
    }
    onRectPosChanged();
}

void MCSOriginChartView::setMainSeries(QScatterSeries *series)
{
    if(!series)
    {
        return;
    }
    if(chart()->series().contains(mainSeries))
    {
        chart()->removeSeries(mainSeries);
    }
    series->setMarkerSize(3);
    series->setUseOpenGL(true);
    chart()->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setColor(QColor(255, 254 ,145));
    mainSeries = series;
    double max = 0.0;
    foreach (auto p, series->points())
    {
        if(p.y() > max)
        {
            max = p.y();
        }
    }
    if(max > max2)
    {
        axisY->setRange(0.0, max * 1.2);
    }
    else if(max2 > 0.0)
    {
        axisY->setRange(0.0, max2 * 1.2);
    }
    max1 = max;
    onRectPosChanged();
}

void MCSOriginChartView::setBackgroundSeries(QScatterSeries *series)
{
    if(!series)
    {
        return;
    }
    if(chart()->series().contains(backgroundSeries))
    {
        chart()->removeSeries(backgroundSeries);
    }
    series->setMarkerSize(3);
    series->setUseOpenGL(true);
    chart()->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setColor(QColor(161, 251 ,142));
    backgroundSeries = series;
    double max = 0.0;
    foreach (auto p, series->points())
    {
        if(p.y() > max)
        {
            max = p.y();
        }
    }
    if(max > max1)
    {
        axisY->setRange(0.0, max * 1.2);
    }
    else if(max1 > 0.0)
    {
        axisY->setRange(0.0, max1 * 1.2);
    }
    max2 = max;
    onRectPosChanged();
}

void MCSOriginChartView::showDifferenceSeries()
{
    if(!mainSeries || !backgroundSeries)
    {
        return;
    }
    differenceSeries->hide();
    calculateDifferenceSeries();
    differenceSeries->show();
    differenceSeries->setColor(Qt::gray);
}

void MCSOriginChartView::hideDifferenceSeries()
{
    differenceSeries->hide();
}

void MCSOriginChartView::addRoiSeries(QAreaSeries *series)
{
    chartV->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}

void MCSOriginChartView::removeRoiSeries(QAreaSeries *series)
{
    if(chartV->series().contains(series))
    {
        chartV->removeSeries(series);
    }
}

void MCSOriginChartView::onRectPosChanged()
{
    QPointF topLeft = draggableRect->sceneBoundingRect().topLeft();
    QPointF bottomRight = draggableRect->sceneBoundingRect().bottomRight();
    double tlx = chart()->mapToValue(topLeft, scaleSeries).x();
    double drx = chart()->mapToValue(bottomRight, scaleSeries).x();
    double tly = chart()->mapToValue(topLeft, scaleSeries).y();
    double dry = chart()->mapToValue(bottomRight, scaleSeries).y();
    emit rectangleChanged({tlx, tly}, {drx, dry});
}

void MCSOriginChartView::onResetRectRequest()
{
    qreal xmax = axisX->max();
    qreal ymax = axisY->max();
    qreal xmin = axisX->min();
    qreal ymin = axisY->min();
    QPointF tl = chart()->mapToPosition({xmin, ymax}, scaleSeries);
    QPointF dr = chart()->mapToPosition({xmax, ymin}, scaleSeries);
    draggableRect->setRect(draggableRect->mapRectFromScene({tl, dr}));
    onRectPosChanged();
}

void MCSOriginChartView::onNewPosDelta(QPointF delta)
{
    QRectF dragRect = draggableRect->sceneBoundingRect();
    qreal dragLeft = chartV->mapToValue(dragRect.topLeft(), scaleSeries).x();
    qreal dragRight = chartV->mapToValue(dragRect.bottomRight(), scaleSeries).x();
    qreal dragUp = chartV->mapToValue(dragRect.topLeft(), scaleSeries).y();
    qreal dragDown = chartV->mapToValue(dragRect.bottomRight(), scaleSeries).y();
    bool canlr = dragLeft + delta.x() >= axisX->min() && dragRight + delta.x() <= axisX->max();
    bool canud = dragUp + delta.y() <= axisY->max() && dragDown + delta.y() >= axisY->min();
    if(canlr)
    {
        QPointF posV = chartV->mapToValue(draggableRect->scenePos(), scaleSeries);
        QPointF dx = {delta.x(), 0};
        QPointF pos = chartV->mapToPosition(posV + dx, scaleSeries);
        draggableRect->setPos(pos);
        onRectPosChanged();
    }
    if(canud)
    {
        QPointF posV = chartV->mapToValue(draggableRect->scenePos(), scaleSeries);
        QPointF dy = {0, delta.y()};
        QPointF pos = chartV->mapToPosition(posV + dy, scaleSeries);
        draggableRect->setPos(pos);
        onRectPosChanged();
    }
}

void MCSOriginChartView::onChangeRect(QPointF topleft, QPointF downright)
{
    QPointF tl = chart()->mapToPosition(topleft, scaleSeries);
    QPointF dr = chart()->mapToPosition(downright, scaleSeries);
    draggableRect->setRect(draggableRect->mapRectFromScene({tl, dr}));
    onRectPosChanged();
}

QScatterSeries *MCSOriginChartView::getDifferenceSeries() const
{
    return differenceSeries;
}

QScatterSeries *MCSOriginChartView::getBackgroundSeries() const
{
    return backgroundSeries;
}

QScatterSeries *MCSOriginChartView::getMainSeries() const
{
    return mainSeries;
}

void MCSOriginChartView::calculateDifferenceSeries()
{
    if(!mainSeries || !backgroundSeries)
    {
        return;
    }
    if(dm != mainSeries || db != backgroundSeries)
    {
        chartV->removeSeries(differenceSeries);
        differenceSeries->clear();
        QList<QPointF> mainps = mainSeries->points();
        QList<QPointF> backgroundps = backgroundSeries->points();
        for (int i = 0; i < axisX->max(); ++i)
        {
            if(i < mainps.size() && i < backgroundps.size())
            {
                qreal y = mainps[i].y() - backgroundps[i].y();
                differenceSeries->append(i, y > 0 ? y : 0);
            }
        }
        chartV->addSeries(differenceSeries);
        differenceSeries->attachAxis(axisX);
        differenceSeries->attachAxis(axisY);
        dm = mainSeries;
        db = backgroundSeries;
    }
}
