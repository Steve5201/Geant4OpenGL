#ifndef MCSCURSORITEM_H
#define MCSCURSORITEM_H

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QtCharts/QChart>
#include <QCursor>
#include <QObject>

using namespace QtCharts;

class MCSCursorItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    MCSCursorItem(const QString &text, const QColor col = Qt::black, QGraphicsItem *parent = nullptr, QObject *OP = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setText(const QString &text);
    QString text() const;

    QColor getColor() const;
    void setColor(const QColor &newColor);

    QPointF getPosX() const;

    void setLimitChart(QChart *newLimitChart);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void posMoved();

private:
    QGraphicsTextItem *textItem;
    QGraphicsLineItem *vline;
    bool dragging;
    QColor color;
    QChart *limitChart;

};

#endif // MCSCURSORITEM_H
