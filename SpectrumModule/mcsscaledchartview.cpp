#include "mcsscaledchartview.h"

MCSScaledChartView::MCSScaledChartView(QWidget *parent)
    : QChartView(parent), isDragging(false), mainSeries(nullptr), backgroundSeries(nullptr),
    differenceSeries(new QScatterSeries()), dm(nullptr), db (nullptr), rubberBand(nullptr)
{
    setMouseTracking(true);
    // 创建x轴
    axisX = new QValueAxis();
    axisX->setTitleText("");
    axisX->setTickCount(2);
    axisX->setLabelsColor(Qt::white);
    axisX->setRange(0.0, 1024.0);
    // 创建y轴
    axisY = new QValueAxis();
    axisY->setTitleText("");
    axisY->setTickCount(2);
    axisY->setLabelsColor(Qt::white);
    axisY->setRange(0.0, 10.0);
    axisY->setLabelsAngle(-90);
    // 创建表
    chartV = chart();
    chartV->addAxis(axisX, Qt::AlignBottom);
    chartV->addAxis(axisY, Qt::AlignLeft);
    chartV->legend()->hide();
    // 创建工具序列
    scaleSeries = new QScatterSeries(chart());
    scaleSeries->append({22,22});
    scaleSeries->setMarkerSize(20);
    // scaleSeries->setUseOpenGL(true);
    chartV->addSeries(scaleSeries);
    scaleSeries->attachAxis(axisX);
    scaleSeries->attachAxis(axisY);
    scaleSeries->hide();
    differenceSeries->setUseOpenGL(true);
    differenceSeries->setMarkerSize(3);
    chartV->addSeries(differenceSeries);
    differenceSeries->attachAxis(axisX);
    differenceSeries->attachAxis(axisY);
    differenceSeries->hide();
    QAreaSeries *a = new QAreaSeries(chartV);
    chartV->addSeries(a);
    a->hide();
    // 创建游标
    c1 = new MCSCursorItem("光标1", QColor(240, 135 ,132));
    c2 = new MCSCursorItem("光标2", QColor(126, 135 ,247));
    scene()->addItem(c1);
    scene()->addItem(c2);
    c1->setPos(width() / 4, height() / 2);
    c2->setPos(width() / 2, height() / 2);
    c1->setLimitChart(chart());
    c2->setLimitChart(chart());
    connect(c1, &MCSCursorItem::posMoved, this, &MCSScaledChartView::onCurcorPosMoved);
    connect(c2, &MCSCursorItem::posMoved, this, &MCSScaledChartView::onCurcorPosMoved);
}

MCSScaledChartView::~MCSScaledChartView()
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

void MCSScaledChartView::setDisplayLine(bool show)
{
    if(show)
    {
        axisX->setTickCount(5);
        axisY->setTickCount(6);
    }
    else
    {
        axisX->setTickCount(2);
        axisY->setTickCount(2);
    }
}

void MCSScaledChartView::setChannelRange(int begin, int end)
{
    axisX->setRange(begin > 0 ? begin : 0, end);
}

void MCSScaledChartView::resetCurcor()
{
    c1->setPos(width() / 4, height() / 2);
    c2->setPos(width() / 2, height() / 2);
    onCurcorPosMoved();
}

void MCSScaledChartView::refreshView()
{
    QRectF rect = {chartV->rect().topLeft() + QPointF(50.0, 70.0), chartV->rect().bottomRight() + QPointF(-70.0, -50.0)};
    chartV->setPlotArea(rect);
    chartV->setBackgroundVisible(true);
    chartV->setBackgroundBrush(QColor(55, 55, 100));
    chartV->setPlotAreaBackgroundVisible(true);
    chartV->setPlotAreaBackgroundBrush(QColor(55, 55, 100));
    if(!chartV->plotArea().contains(c1->pos()))
    {
        c1->setPos(chartV->pos() + QPointF(50.0, 50.0));
    }
    if(!chartV->plotArea().contains(c2->pos()))
    {
        c2->setPos(chartV->pos() + QPointF(100.0, 100.0));
    }
    resetCurcor();
    onCurcorPosMoved();
}

void MCSScaledChartView::removeMainSeries()
{
    if(mainSeries)
    {
        mainSeries->clear();
        mainSeries->hide();
        chart()->removeSeries(mainSeries);
        mainSeries = nullptr;
    }
    differenceSeries->hide();
}

void MCSScaledChartView::removeBackgroundSeries()
{
    if(backgroundSeries)
    {
        backgroundSeries->clear();
        backgroundSeries->hide();
        chart()->removeSeries(backgroundSeries);
        backgroundSeries = nullptr;
    }
    differenceSeries->hide();
}

void MCSScaledChartView::setMainSeries(QScatterSeries *series)
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
}

void MCSScaledChartView::setBackgroundSeries(QScatterSeries *series)
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
}

void MCSScaledChartView::showDifferenceSeries()
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

void MCSScaledChartView::hideDifferenceSeries()
{
    differenceSeries->hide();
}

void MCSScaledChartView::addRoiSeries(QAreaSeries *series)
{
    chartV->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}

void MCSScaledChartView::removeRoiSeries(QAreaSeries *series)
{
    if(chartV->series().contains(series))
    {
        chartV->removeSeries(series);
    }
}

QVector2D MCSScaledChartView::getCurcorsPos()
{
    QVector2D rt;
    double c1x = chart()->mapToValue(c1->getPosX(), scaleSeries).x();
    double c2x = chart()->mapToValue(c2->getPosX(), scaleSeries).x();
    rt.setX(c1x);
    rt.setY(c2x);
    return rt;
}

void MCSScaledChartView::mousePressEvent(QMouseEvent *event)
{
    QChartView::mousePressEvent(event);

    if(c1->isUnderMouse() || c2->isUnderMouse())
    {
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        origin = event->pos();
        if (!rubberBand && chartV->plotArea().contains(origin))
        {
            rubberBand = new QGraphicsRectItem();
            rubberBand->setBrush(QBrush(QColor(0, 0, 255, 50)));
            rubberBand->setPen(QPen(Qt::blue));
            chart()->scene()->addItem(rubberBand);
            rubberBand->setRect(QRectF(origin, QSize()));
        }
    }
    if (event->button() == Qt::RightButton)
    {
        isDragging = true;
        lastMousePos = event->pos();
    }
}

void MCSScaledChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        emit requestResetRect();
    }
}

void MCSScaledChartView::mouseMoveEvent(QMouseEvent *event)
{
    QChartView::mouseMoveEvent(event);

    if (rubberBand && (event->buttons() & Qt::LeftButton))
    {
        rubberBand->setRect(QRectF(origin, event->pos()).normalized());
    }
    if ((event->buttons() & Qt::RightButton) && isDragging)
    {
        QPointF delta = chart()->mapToValue(lastMousePos, scaleSeries) - chart()->mapToValue(event->pos(), scaleSeries);  // 计算移动的偏移量
        emit newPosDelta(delta);
        lastMousePos = event->pos();  // 更新鼠标位置
    }
}

void MCSScaledChartView::mouseReleaseEvent(QMouseEvent *event)
{
    QChartView::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton && rubberBand)
    {
        QPointF endPos = event->pos();
        if(endPos.x() < chartV->plotArea().topLeft().x())
        {
            endPos.setX(chartV->plotArea().topLeft().x() + 1);
        }
        if(endPos.x() > chartV->plotArea().bottomRight().x())
        {
            endPos.setX(chartV->plotArea().bottomRight().x() - 1);
        }
        if(endPos.y() < chartV->plotArea().topLeft().y())
        {
            endPos.setY(chartV->plotArea().topLeft().y() + 1);
        }
        if(endPos.y() > chartV->plotArea().bottomRight().y())
        {
            endPos.setY(chartV->plotArea().bottomRight().y() - 1);
        }
        if(chartV->plotArea().contains(endPos))
        {
            qreal x = origin.x() < endPos.x() ? origin.x() : endPos.x();
            qreal y = origin.y() < endPos.y() ? origin.y() : endPos.y();
            qreal w = abs(origin.x() - endPos.x());
            qreal h = abs(origin.y() - endPos.y());
            QRectF rect = {x,y,w,h};
            if (rect.isValid())
            {
                QPointF topLeft = rect.topLeft();
                QPointF bottomRight = rect.bottomRight();
                double tlx = chart()->mapToValue(topLeft, scaleSeries).x();
                double drx = chart()->mapToValue(bottomRight, scaleSeries).x();
                double tly = chart()->mapToValue(topLeft, scaleSeries).y();
                double dry = chart()->mapToValue(bottomRight, scaleSeries).y();
                if(event->modifiers() == Qt::ShiftModifier)
                {
                    emit newRoiSelected(tlx, drx, true);
                }
                else if(event->modifiers() == Qt::ControlModifier)
                {
                    emit newRoiSelected(tlx, drx, false);
                }
                else
                {
                    emit newRectSelected({tlx,tly},{drx,dry});
                }
            }
        }
        chart()->scene()->removeItem(rubberBand);
        delete rubberBand;
        rubberBand = nullptr;
    }
    if (event->button() == Qt::RightButton && isDragging)
    {
        isDragging = false;
    }
}

void MCSScaledChartView::calculateDifferenceSeries()
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

QScatterSeries *MCSScaledChartView::getDifferenceSeries() const
{
    return differenceSeries;
}

QScatterSeries *MCSScaledChartView::getBackgroundSeries() const
{
    return backgroundSeries;
}

QScatterSeries *MCSScaledChartView::getMainSeries() const
{
    return mainSeries;
}

void MCSScaledChartView::onCurcorPosMoved()
{
    double c1x = chart()->mapToValue(c1->getPosX(), scaleSeries).x();
    double c2x = chart()->mapToValue(c2->getPosX(), scaleSeries).x();
    emit curcorMoved(c1x, c2x);
}

void MCSScaledChartView::onChangeRect(QPointF topleft, QPointF downright)
{
    if(!axisX || !axisY)
    {
        return;
    }
    axisX->setRange(topleft.x() > 0 ? topleft.x() : 0, downright.x());
    axisY->setRange(downright.y() > 0 ? downright.y() : 0, topleft.y());
}
