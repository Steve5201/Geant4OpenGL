#include "mcscursoritem.h"
#include <QGraphicsSceneMouseEvent>

MCSCursorItem::MCSCursorItem(const QString &text, const QColor col, QGraphicsItem *parent, QObject *OP)
    : QObject(OP), QGraphicsItem(parent), dragging(false), limitChart(nullptr)
{
    textItem = new QGraphicsTextItem(text, this);
    textItem->setDefaultTextColor(col);
    textItem->setFont(QFont("Arial", 12));
    textItem->setPos(-textItem->boundingRect().width() + 23, -textItem->boundingRect().height() + 10);
    vline = new QGraphicsLineItem(this);
    QLineF vl{{0.0, -10000.0}, {0.0, 10000.0}};
    vline->setLine(vl);
    vline->setPos(0.0, 0.0);
    setCursor(Qt::OpenHandCursor);
    setFlag(QGraphicsItem::ItemIsMovable);
    color = col;
    vline->setPen(QPen(color));
}

QRectF MCSCursorItem::boundingRect() const
{
    return QRectF(-30, -20, 60, 30);
}

void MCSCursorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(QPen(color));
}

void MCSCursorItem::setText(const QString &text)
{
    textItem->setPlainText(text);
    textItem->setPos(-textItem->boundingRect().width() / 2, -textItem->boundingRect().height() - 5);
}

QString MCSCursorItem::text() const
{
    return textItem->toPlainText();
}

void MCSCursorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void MCSCursorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setCursor(Qt::OpenHandCursor);
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void MCSCursorItem::setLimitChart(QChart *newLimitChart)
{
    limitChart = newLimitChart;
}

QColor MCSCursorItem::getColor() const
{
    return color;
}

void MCSCursorItem::setColor(const QColor &newColor)
{
    color = newColor;
    vline->setPen(QPen(color));
    textItem->setDefaultTextColor(color);
}

QPointF MCSCursorItem::getPosX() const
{
    return vline->scenePos();
}

void MCSCursorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF offset = event->pos() - event->lastPos();
    qreal speedFactor = 1.0;
    offset *= speedFactor;
    if (limitChart)
    {
        QRectF limitRect = limitChart->plotArea();
        QPointF pos0 = vline->scenePos();
        bool canlr = limitRect.topLeft().x() <= pos0.x() + offset.x() && limitRect.bottomRight().x() >= pos0.x() + offset.x();
        bool canud = limitRect.topLeft().y() <= pos0.y() + offset.y() && limitRect.bottomRight().y() >= pos0.y() + offset.y();
        if(canlr)
        {
            setPos(pos() + QPointF(offset.x(), 0));
        }
        if(canud)
        {
            setPos(pos() + QPointF(0, offset.y()));
        }
    }
    emit posMoved();
}
