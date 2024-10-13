#ifndef MCSSCALEDCHARTVIEW_H
#define MCSSCALEDCHARTVIEW_H

#include <QWheelEvent>
#include <QMouseEvent>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QScatterSeries>
#include "mcscursoritem.h"

class MCSScaledChartView : public QChartView
{
    Q_OBJECT

public:
    MCSScaledChartView(QWidget *parent = nullptr);
    ~MCSScaledChartView();

    void setDisplayLine(bool show);
    void setChannelRange(int begin, int end);
    void resetCurcor();
    void refreshView();
    void removeMainSeries();
    void removeBackgroundSeries();
    void setMainSeries(QScatterSeries *series);
    void setBackgroundSeries(QScatterSeries *backgroundSeries);
    void showDifferenceSeries();
    void hideDifferenceSeries();
    void addRoiSeries(QAreaSeries *series);
    void removeRoiSeries(QAreaSeries *series);

    QVector2D getCurcorsPos();
    QScatterSeries *getMainSeries() const;
    QScatterSeries *getBackgroundSeries() const;
    QScatterSeries *getDifferenceSeries() const;

signals:
    void requestResetRect();
    void curcorMoved(double c1, double c2);
    void newPosDelta(QPointF delta);
    void newRoiSelected(double xMin, double xMax, bool add);
    void newRectSelected(QPointF topleft, QPointF downright);

public slots:
    void onCurcorPosMoved();
    void onChangeRect(QPointF topleft, QPointF downright);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isDragging;
    QScatterSeries *mainSeries;
    QScatterSeries *backgroundSeries;
    QScatterSeries *differenceSeries;
    QScatterSeries *dm,*db;
    MCSCursorItem *c1,*c2;
    QPoint origin;
    QPoint lastMousePos;
    QChart *chartV;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QScatterSeries *scaleSeries;
    QGraphicsRectItem *rubberBand;

    void calculateDifferenceSeries();

};

#endif // MCSSCALEDCHARTVIEW_H
