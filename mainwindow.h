#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QListWidgetItem>
#include "mcsdevice.h"
#include "openglwidget.h"
#include "geant4manager.h"
#include "geant4shapes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setG4Manager(Geant4Manager *newG4Manager);

private slots:
    void onNewInfo(QString info);

    void onSourceRemoved();

    void onObjectRemoved(QString objname);

    void onSelectObjectChanged(OpenGLObject *obj);

    void on_resetCamera_triggered();

    void on_load_triggered();

    void on_save_triggered();

    void on_remove_triggered();

    void on_inView_triggered();

    void on_bInView_clicked();

    void on_bResetView_clicked();

    void on_cameraMoveSpeed_valueChanged(double arg1);

    void on_cameraRotateSpeed_valueChanged(double arg1);

    void on_sf_valueChanged(double arg1);

    void on_px_valueChanged(double arg1);

    void on_py_valueChanged(double arg1);

    void on_pz_valueChanged(double arg1);

    void on_rx_valueChanged(double arg1);

    void on_ry_valueChanged(double arg1);

    void on_rz_valueChanged(double arg1);

    void on_cr_valueChanged(int arg1);

    void on_cg_valueChanged(int arg1);

    void on_cb_valueChanged(int arg1);

    void on_mats0_currentTextChanged(const QString &arg1);

    void on_tcq0_stateChanged(const int arg1);

    void on_halflen_valueChanged(double arg1);

    void on_halfwidth_valueChanged(double arg1);

    void on_halfheight_valueChanged(double arg1);

    void on_mats1_currentTextChanged(const QString &arg1);

    void on_tcq1_stateChanged(const int arg1);

    void on_inR_valueChanged(double arg1);

    void on_outR_valueChanged(double arg1);

    void on_sphi_valueChanged(double arg1);

    void on_dphi_valueChanged(double arg1);

    void on_stheta_valueChanged(double arg1);

    void on_dtheta_valueChanged(double arg1);

    void on_mats2_currentTextChanged(const QString &arg1);

    void on_tcq2_stateChanged(const int arg1);

    void on_tubinR_valueChanged(double arg1);

    void on_tuboutR_valueChanged(double arg1);

    void on_tubhalfheight_valueChanged(double arg1);

    void on_tubstartdeg_valueChanged(double arg1);

    void on_tublendeg_valueChanged(double arg1);

    void on_mats3_currentTextChanged(const QString &arg1);

    void on_tcq3_stateChanged(const int arg1);

    void on_torinR_valueChanged(double arg1);

    void on_toroutR_valueChanged(double arg1);

    void on_torR_valueChanged(double arg1);

    void on_torstartdeg_valueChanged(double arg1);

    void on_torlendeg_valueChanged(double arg1);

    void on_mats4_currentTextChanged(const QString &arg1);

    void on_tcq4_stateChanged(const int arg1);

    void on_itemList_currentTextChanged(const QString &currentText);

    void on_itemList_itemClicked(QListWidgetItem *item);

    void on_sorR_valueChanged(double arg1);

    void on_sorL_valueChanged(double arg1);

    void on_sorE_valueChanged(double arg1);

    void on_sorT_currentTextChanged(const QString &arg1);

    void on_loadScene_clicked();

    void on_saveScene_clicked();

    void on_clearScene_clicked();

    void on_startSimulate_clicked();

    void on_prepareG4_clicked();

    void on_drawMode_clicked();

    void on_detectors_currentTextChanged(const QString &arg1);

    void on_loadSpectrum_clicked();

    void on_saveSpectrum_clicked();

    void on_objname_textEdited(const QString &arg1);

    void on_trakDisplay_valueChanged(int arg1);

    void on_resKeV_valueChanged(int arg1);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    bool hasSource;
    QLabel *label;
    OpenGLWidget *glWidget;
    OpenGLObject *currentObject;
    QVector<QString> loadNames;

    MCSDevice *device;
    Geant4Manager *g4Manager;
    QMap<std::string, QVector<double>> detectors;

    QString getName(int type);
    void setBox();
    void setSphere();
    void setTubs();
    void setTorus();
    void setSource();
    void sendSceneToG4();
    QString getSceneInfo();
    QVector<OpenGLObject*> getSceneObjectsByText(QString info, OpenGLCamera *camera = nullptr);

};
#endif // MAINWINDOW_H
