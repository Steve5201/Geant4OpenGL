#include "mcsdraggablerect.h"
#include <QGraphicsScene>

MCSDraggableRect::MCSDraggableRect(QGraphicsItem *parent)
    : QGraphicsRectItem(parent), limitChart(nullptr)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptHoverEvents(true);
}

void MCSDraggableRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF offset = event->pos() - event->lastPos();
    qreal speedFactor = 1.0;
    offset *= speedFactor;
    if (limitChart)
    {
        QRectF limitRect = limitChart->plotArea();
        QRectF thisRect = sceneBoundingRect();
        qreal lux1 = thisRect.topLeft().x();
        qreal lux2 = limitRect.topLeft().x();
        qreal luy1 = thisRect.topLeft().y();
        qreal luy2 = limitRect.topLeft().y();
        qreal drx1 = thisRect.bottomRight().x();
        qreal drx2 = limitRect.bottomRight().x();
        qreal dry1 = thisRect.bottomRight().y();
        qreal dry2 = limitRect.bottomRight().y();
        bool canlr = lux1 + offset.x() >= lux2 && drx1 + offset.x() <= drx2;
        bool canud = luy1 + offset.y() >= luy2 && dry1 + offset.y() <= dry2;
        if(canlr)
        {
            setPos(pos() + QPointF(offset.x(), 0));
        }
        if(canud)
        {
            setPos(pos() + QPointF(0, offset.y()));
        }
    }
    emit rectChanged();
}

void MCSDraggableRect::setLimitChart(QChart *newLimitChart)
{
    limitChart = newLimitChart;
}
