#ifndef MCSDRAGGABLERECT_H
#define MCSDRAGGABLERECT_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QtCharts/QChart>
#include <QObject>

using namespace QtCharts;

class MCSDraggableRect : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    explicit MCSDraggableRect(QGraphicsItem *parent = nullptr);

    void setLimitChart(QChart *newLimitChart);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QChart *limitChart;

signals:
    void rectChanged();

};

#endif // MCSDRAGGABLERECT_H
