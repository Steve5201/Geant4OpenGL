#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include <QBuffer>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include "openglobject.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), currentObject(nullptr)
{
    ui->setupUi(this);
    hasSource = false;
    ui->propertya->setCurrentIndex(6);
    // 配置gl窗口
    glWidget = new OpenGLWidget;
    ui->OGL->layout()->addWidget(glWidget);
    connect(glWidget,SIGNAL(sourceRemoved()),this,SLOT(onSourceRemoved()));
    connect(glWidget,SIGNAL(objectRemoved(QString)),this,SLOT(onObjectRemoved(QString)));
    connect(glWidget,SIGNAL(selectObjectChanged(OpenGLObject*)),this,SLOT(onSelectObjectChanged(OpenGLObject*)));
    ui->cameraMoveSpeed->setValue(0.1);
    ui->cameraMoveSpeed->setSingleStep(0.01);
    ui->cameraRotateSpeed->setValue(0.1);
    ui->cameraRotateSpeed->setSingleStep(0.01);
    // 配置形状拖动窗口
    label = nullptr;
    ui->shapeList->addItem("立方体");
    ui->shapeList->addItem("球体");
    ui->shapeList->addItem("圆柱体");
    ui->shapeList->addItem("圆环");
    ui->shapeList->addItem("源");
    ui->shapeList->item(0)->setTextAlignment(Qt::AlignCenter);
    ui->shapeList->item(1)->setTextAlignment(Qt::AlignCenter);
    ui->shapeList->item(2)->setTextAlignment(Qt::AlignCenter);
    ui->shapeList->item(3)->setTextAlignment(Qt::AlignCenter);
    ui->shapeList->item(4)->setTextAlignment(Qt::AlignCenter);
    ui->shapeList->viewport()->installEventFilter(this);
    // 配置G4
    g4Manager = nullptr;
    // 配置谱显示
    device = new MCSDevice;
    ui->spt1->layout()->addWidget(device->getOriginChartView());
    ui->spt2->layout()->addWidget(device->getScaledChartView());
    connect(device,SIGNAL(newCursorInfo(QString)),this,SLOT(onNewInfo(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete g4Manager;
}

void MainWindow::setG4Manager(Geant4Manager *newG4Manager)
{
    g4Manager = newG4Manager;
}

void MainWindow::onNewInfo(QString info)
{
    ui->statusbar->showMessage(info);
}

void MainWindow::onSourceRemoved()
{
    hasSource = false;
}

void MainWindow::onObjectRemoved(QString objname)
{
    for (int i = 0; i < ui->itemList->count(); ++i)
    {
        if(objname == ui->itemList->item(i)->text())
        {
            QListWidgetItem *item = ui->itemList->takeItem(i);
            if(item)
            {
                delete item;
            }
        }
    }
    loadNames.remove(loadNames.indexOf(objname));
}

void MainWindow::onSelectObjectChanged(OpenGLObject *obj)
{
    currentObject = obj;
    if(!currentObject)
    {
        ui->propertyb->setEnabled(false);
        ui->propertya->setCurrentIndex(6);
        return;
    }
    ui->propertyb->setEnabled(true);
    ui->px->blockSignals(true);
    ui->py->blockSignals(true);
    ui->pz->blockSignals(true);
    ui->rx->blockSignals(true);
    ui->ry->blockSignals(true);
    ui->rz->blockSignals(true);
    ui->sf->blockSignals(true);
    ui->cr->blockSignals(true);
    ui->cg->blockSignals(true);
    ui->cb->blockSignals(true);
    ui->objname->setText(obj->getOjectName());
    ui->px->setValue(obj->getPosition().x());
    ui->py->setValue(obj->getPosition().y());
    ui->pz->setValue(obj->getPosition().z());
    ui->rx->setValue(obj->getRotation().x());
    ui->ry->setValue(obj->getRotation().y());
    ui->rz->setValue(obj->getRotation().z());
    ui->sf->setValue(obj->getScaleFactor());
    ui->cr->setValue(obj->getUserColor().red());
    ui->cg->setValue(obj->getUserColor().green());
    ui->cb->setValue(obj->getUserColor().blue());
    ui->px->blockSignals(false);
    ui->py->blockSignals(false);
    ui->pz->blockSignals(false);
    ui->rx->blockSignals(false);
    ui->ry->blockSignals(false);
    ui->rz->blockSignals(false);
    ui->sf->blockSignals(false);
    ui->cr->blockSignals(false);
    ui->cg->blockSignals(false);
    ui->cb->blockSignals(false);
    ui->propertya->setCurrentIndex(0);
    if(g4Manager)
    {
        ui->mats0->blockSignals(true);
        ui->mats0->clear();
        ui->mats0->addItems(g4Manager->getMaterialNames());
        ui->mats0->blockSignals(false);
        QString matstr = obj->getCustomerName1();
        if(!g4Manager->getMaterialNames().contains(matstr))
        {
            matstr = "G4_AIR";
            obj->setCustomerName1(matstr);
        }
        ui->mats0->setCurrentText(matstr);
    }
    ui->tcq0->blockSignals(true);
    ui->tcq0->setChecked(!obj->getCustomerName2().isEmpty());
    ui->tcq0->blockSignals(false);
    switch (obj->getTypeID())
    {
    case 1:
        setBox();
        break;
    case 2:
        setSphere();
        break;
    case 3:
        setTubs();
        break;
    case 4:
        setTorus();
    case 5:
        setSource();
        break;
    default:
        break;
    }
}

void MainWindow::on_resetCamera_triggered()
{
    glWidget->setCamera(OpenGLCamera());
}

void MainWindow::on_load_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, "选择文件");
    if(file.isEmpty())
    {
        return;
    }
    OpenGLObject *obj = new OpenGLObject;
    obj->loadModel(file);
    if(obj->isValid())
    {
        QString name = getName(obj->getTypeID());
        loadNames.append(name);
        obj->setObjectName(name);
        if(glWidget->addObjectAtScreenPos(obj, {glWidget->width()/2,glWidget->height()/2}))
        {
            ui->itemList->addItem(name);
        }
    }
    else
    {
        QMessageBox::warning(this,"提示","导入失败");
    }
}

void MainWindow::on_save_triggered()
{
    QString file = QFileDialog::getSaveFileName(this, "选择文件");
    if(file.isEmpty())
    {
        return;
    }
    if(currentObject)
    {
        if(currentObject->exportModel(file))
        {
            QMessageBox::warning(this,"提示","导出成功");
        }
        else
        {
            QMessageBox::warning(this,"提示","导出失败");
        }
    }
}

void MainWindow::on_remove_triggered()
{
    OpenGLObject *obj = glWidget->getCurrentObject();
    if(obj)
    {
        int idx = loadNames.indexOf(obj->getOjectName());
        if(idx != -1)
        {
            loadNames.removeAt(idx);
        }
        for (int i = 0; i < ui->itemList->count(); ++i)
        {
            if(obj->getOjectName() == ui->itemList->item(i)->text())
            {
                QListWidgetItem *item = ui->itemList->takeItem(i);
                if(item)
                {
                    delete item;
                }
            }
        }
        glWidget->removeObject(obj);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if(event->size().isValid())
    {
        device->getOriginChartView()->refreshView();
        device->getScaledChartView()->refreshView();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->shapeList->viewport())
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent)
        {
            if(event->type() == QEvent::MouseButtonPress)
            {
                QPoint pos = mouseEvent->pos(); // 相对当前组件的坐标
                QListWidgetItem *item = ui->shapeList->itemAt(pos);
                if(item)
                {
                    ui->shapeList->setCurrentItem(item);
                    QPoint mainwinPos = ui->shapeList->viewport()->mapTo(this,pos);
                    label = new QLabel(this);
                    switch (ui->shapeList->currentRow())
                    {
                    case 0:
                        label->setPixmap(QPixmap(":/shape/imgs/box.png").scaled(80,80));
                        break;
                    case 1:
                        label->setPixmap(QPixmap(":/shape/imgs/sphere.png").scaled(80,80));
                        break;
                    case 2:
                        label->setPixmap(QPixmap(":/shape/imgs/tubs.png").scaled(80,80));
                        break;
                    case 3:
                        label->setPixmap(QPixmap(":/shape/imgs/torus.png").scaled(80,80));
                        break;
                    case 4:
                        label->setPixmap(QPixmap(":/shape/imgs/source.png").scaled(80,80));
                        break;
                    default:
                        break;
                    }
                    label->setFixedSize(80,80);
                    label->move(mainwinPos - QPoint{40,40});
                    label->raise();
                    label->show();
                    ui->shapeList->setEnabled(false);
                }
            }
            if(event->type() == QEvent::MouseMove)
            {
                if(label)
                {
                    QPoint mainwinPos = ui->shapeList->viewport()->mapTo(this,mouseEvent->pos());
                    label->move( mainwinPos - QPoint{40,40});
                }
            }
            if(event->type() == QEvent::MouseButtonRelease)
            {
                if(label)
                {
                    QPoint mainwinPos = ui->shapeList->viewport()->mapTo(this,mouseEvent->pos());
                    QPoint glPos = glWidget->mapFrom(this,mainwinPos);
                    int index = ui->shapeList->currentRow();
                    if(index == 0)
                    {
                        Geant4Box *shape = new Geant4Box;
                        QString name = getName(shape->getTypeID());
                        loadNames.append(name);
                        shape->setObjectName(name);
                        if(glWidget->addObjectAtScreenPos(shape, glPos))
                        {
                            ui->itemList->addItem(name);
                        }
                    }
                    if(index == 1)
                    {
                        Geant4Sphere *shape = new Geant4Sphere;
                        QString name = getName(shape->getTypeID());
                        loadNames.append(name);
                        shape->setObjectName(name);
                        if(glWidget->addObjectAtScreenPos(shape, glPos))
                        {
                            ui->itemList->addItem(name);
                        }
                    }
                    if(index == 2)
                    {
                        Geant4Tubs *shape = new Geant4Tubs;
                        QString name = getName(shape->getTypeID());
                        loadNames.append(name);
                        shape->setObjectName(name);
                        if(glWidget->addObjectAtScreenPos(shape, glPos))
                        {
                            ui->itemList->addItem(name);
                        }
                    }
                    if(index == 3)
                    {
                        Geant4Torus *shape = new Geant4Torus;
                        QString name = getName(shape->getTypeID());
                        loadNames.append(name);
                        shape->setObjectName(name);
                        if(glWidget->addObjectAtScreenPos(shape, glPos))
                        {
                            ui->itemList->addItem(name);
                        }
                    }
                    if(index == 4)
                    {
                        if(!hasSource)
                        {
                            Geant4Source *shape = new Geant4Source;
                            QString name = getName(shape->getTypeID());
                            loadNames.append(name);
                            shape->setObjectName(name);
                            if(glWidget->addObjectAtScreenPos(shape, glPos))
                            {
                                ui->itemList->addItem(name);
                                hasSource = true;
                            }
                        }
                    }
                    delete label;
                    label = nullptr;
                    ui->shapeList->setEnabled(true);
                }
            }
        }
    }
    QMainWindow::eventFilter(watched,event);
    return false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_V:
        break;
    default:
        break;
    }
}

QString MainWindow::getName(int type)
{
    QString name;
    static int sufix = 0;
    static int sufix1 = 0;
    static int sufix2 = 0;
    static int sufix3 = 0;
    static int sufix4 = 0;
    switch (type)
    {
    case 0:
        name = QString("obj_model_%1").arg(sufix++);
        break;
    case 1:
        name = QString("obj_box_%1").arg(sufix1++);
        break;
    case 2:
        name = QString("obj_sphere_%1").arg(sufix2++);
        break;
    case 3:
        name = QString("obj_tubs_%1").arg(sufix3++);
        break;
    case 4:
        name = QString("obj_torus_%1").arg(sufix4++);
        break;
    case 5:
        name = QString("obj_source_%1").arg(sufix4++);
        break;
    default:
        break;
    }
    if(loadNames.contains(name))
    {
        name = getName(type);
    }
    loadNames.append(name);
    return name;
}

void MainWindow::setBox()
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        ui->propertya->setCurrentIndex(1);
        ui->halflen->setValue(box->getHalfDepth());
        ui->halfwidth->setValue(box->getHalfWidth());
        ui->halfheight->setValue(box->getHalfHeight());
        if(g4Manager)
        {
            ui->mats1->blockSignals(true);
            ui->mats1->clear();
            ui->mats1->addItems(g4Manager->getMaterialNames());
            ui->mats1->blockSignals(false);
            QString matstr = box->getCustomerName1();
            if(!g4Manager->getMaterialNames().contains(matstr))
            {
                matstr = "G4_AIR";
                box->setCustomerName1(matstr);
            }
            ui->mats1->setCurrentText(matstr);
        }
        ui->tcq1->blockSignals(true);
        ui->tcq1->setChecked(!box->getCustomerName2().isEmpty());
        ui->tcq1->blockSignals(false);
    }
}

void MainWindow::setSphere()
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        ui->inR->blockSignals(true);
        ui->outR->blockSignals(true);
        ui->propertya->setCurrentIndex(2);
        ui->inR->setValue(sphere->getInnerRadius());
        ui->outR->setValue(sphere->getOuterRadius());
        ui->sphi->setValue(sphere->getStartPhi());
        ui->dphi->setValue(sphere->getLenPhi());
        ui->stheta->setValue(sphere->getStartTheta());
        ui->dtheta->setValue(sphere->getLenTheta());
        if(g4Manager)
        {
            ui->mats2->blockSignals(true);
            ui->mats2->clear();
            ui->mats2->addItems(g4Manager->getMaterialNames());
            ui->mats2->blockSignals(false);
            QString matstr = sphere->getCustomerName1();
            if(!g4Manager->getMaterialNames().contains(matstr))
            {
                matstr = "G4_AIR";
                sphere->setCustomerName1(matstr);
            }
            ui->mats2->setCurrentText(matstr);
        }
        ui->tcq2->blockSignals(true);
        ui->tcq2->setChecked(!sphere->getCustomerName2().isEmpty());
        ui->tcq2->blockSignals(false);
        ui->inR->blockSignals(false);
        ui->outR->blockSignals(false);
    }
}

void MainWindow::setTubs()
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        ui->tubinR->blockSignals(true);
        ui->tuboutR->blockSignals(true);
        ui->propertya->setCurrentIndex(3);
        ui->tubinR->setValue(tub->getInnerRadius());
        ui->tuboutR->setValue(tub->getOuterRadius());
        ui->tubhalfheight->setValue(tub->getHalfHeight());
        ui->tubstartdeg->setValue(tub->getStartAngle());
        ui->tublendeg->setValue(tub->getLenAngle());
        if(g4Manager)
        {
            ui->mats3->blockSignals(true);
            ui->mats3->clear();
            ui->mats3->addItems(g4Manager->getMaterialNames());
            ui->mats3->blockSignals(false);
            QString matstr = tub->getCustomerName1();
            if(!g4Manager->getMaterialNames().contains(matstr))
            {
                matstr = "G4_AIR";
                tub->setCustomerName1(matstr);
            }
            ui->mats3->setCurrentText(matstr);
        }
        ui->tcq3->blockSignals(true);
        ui->tcq3->setChecked(!tub->getCustomerName2().isEmpty());
        ui->tcq3->blockSignals(false);
        ui->tubinR->blockSignals(false);
        ui->tuboutR->blockSignals(false);
    }
}

void MainWindow::setTorus()
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        ui->torinR->blockSignals(true);
        ui->toroutR->blockSignals(true);
        ui->propertya->setCurrentIndex(4);
        ui->torinR->setValue(tor->getRMin());
        ui->toroutR->setValue(tor->getRMax());
        ui->torR->setValue(tor->getRTor());
        ui->torstartdeg->setValue(tor->getSPhi());
        ui->torlendeg->setValue(tor->getDPhi());
        if(g4Manager)
        {
            ui->mats4->blockSignals(true);
            ui->mats4->clear();
            ui->mats4->addItems(g4Manager->getMaterialNames());
            ui->mats4->blockSignals(false);
            QString matstr = tor->getCustomerName1();
            if(!g4Manager->getMaterialNames().contains(matstr))
            {
                matstr = "G4_AIR";
                tor->setCustomerName1(matstr);
            }
            ui->mats4->setCurrentText(matstr);
        }
        ui->tcq4->blockSignals(true);
        ui->tcq4->setChecked(!tor->getCustomerName2().isEmpty());
        ui->tcq4->blockSignals(false);
        ui->torinR->blockSignals(false);
        ui->toroutR->blockSignals(false);
    }
}

void MainWindow::setSource()
{
    Geant4Source *sor = dynamic_cast<Geant4Source*>(currentObject);
    if(sor)
    {
        ui->propertya->setCurrentIndex(5);
        ui->sorR->setValue(sor->getRadius());
        ui->sorL->setValue(sor->getWidth());
        ui->sorE->setValue(sor->getEnegy());
        if(g4Manager)
        {
            ui->sorT->blockSignals(true);
            ui->sorT->clear();
            ui->sorT->addItems(g4Manager->getParticleNames());
            ui->sorT->blockSignals(false);
            QString sorTstr = sor->getType();
            if(!g4Manager->getParticleNames().contains(sorTstr))
            {
                sorTstr = "gamma";
                sor->setType(sorTstr);
            }
            ui->sorT->setCurrentText(sorTstr);
        }
    }
}

void MainWindow::sendSceneToG4()
{
    if(glWidget->getObjectsRef().empty())
    {
        return;
    }
    detectors.clear();
    ui->detectors->blockSignals(true);
    ui->detectors->clear();
    ui->detectors->addItem("不显示");
    g4Manager->clearGeometry();
    QHash<G4LogicalVolume*, std::string> dtmap;
    G4NistManager* nist = G4NistManager::Instance();
    G4LogicalVolume* logicWorld = g4Manager->getDetectorConstruction()->getWorldLogicalVolume();
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
    foreach (auto obj, glWidget->getObjectsRef())
    {
        G4ThreeVector pos(obj->getPosition().x() * cm, obj->getPosition().y() * cm, obj->getPosition().z() * cm);
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateX(-obj->getRotation().x() * deg);
        rot->rotateY(-obj->getRotation().y() * deg);
        rot->rotateZ(-obj->getRotation().z() * deg);
        G4Colour col(obj->getUserColor().redF(), obj->getUserColor().greenF(), obj->getUserColor().blueF());
        float sf = obj->getScaleFactor();
        std::string name = obj->getOjectName().toStdString();
        G4Material* material = nist->FindOrBuildMaterial("G4_AIR");
        QString mat = obj->getCustomerName1();
        if(!mat.isEmpty())
        {
            material = nist->FindOrBuildMaterial(obj->getCustomerName1().toStdString());
        }
        QString tcq = obj->getCustomerName2();
        if(obj->getTypeID() == 0)
        {
            G4TessellatedSolid* complexSolid = new G4TessellatedSolid(name);
            for (int i = 0; i < obj->getMeshsRef().size(); ++i)
            {
                OpenGLMesh &mesh = obj->getMeshsRef()[i];
                for (int j = 0; j < mesh.getIndicesSize(); j += 3)
                {
                    QVector3D p1 = mesh.getVerticesRef()[mesh.getIndicesRef()[j]].position;
                    QVector3D p2 = mesh.getVerticesRef()[mesh.getIndicesRef()[j + 1]].position;
                    QVector3D p3 = mesh.getVerticesRef()[mesh.getIndicesRef()[j + 2]].position;
                    G4ThreeVector v1(p1.x() * cm * sf, p1.y() * cm * sf, p1.z() * cm * sf);
                    G4ThreeVector v2(p2.x() * cm * sf, p2.y() * cm * sf, p2.z() * cm * sf);
                    G4ThreeVector v3(p3.x() * cm * sf, p3.y() * cm * sf, p3.z() * cm * sf);
                    G4TriangularFacet* facet = new G4TriangularFacet(v1, v2, v3, ABSOLUTE);
                    complexSolid->AddFacet((G4VFacet*)facet);
                }
            }
            complexSolid->SetSolidClosed(true);
            G4LogicalVolume* logicComplex = new G4LogicalVolume(complexSolid, material, name);
            new G4PVPlacement(rot, pos, logicComplex, name, logicWorld, false, 0, false);
            G4VisAttributes* visAttr = new G4VisAttributes(col); // 绿色
            visAttr->SetForceSolid(true);
            logicComplex->SetVisAttributes(col);
            if(!tcq.isEmpty())
            {
                dtmap.insert(logicComplex, name);
                ui->detectors->addItem(obj->getOjectName());
            }
        }
        else if(obj->getTypeID() == 1)
        {
            Geant4Box *item = dynamic_cast<Geant4Box*>(obj);
            if(item)
            {
                G4Box* box = new G4Box(name,
                                       item->getHalfWidth() * cm * sf,
                                       item->getHalfHeight() * cm * sf,
                                       item->getHalfDepth() * cm * sf);
                G4LogicalVolume* logicBox = new G4LogicalVolume(box, material, name);
                new G4PVPlacement(rot,
                                  pos,
                                  logicBox,
                                  name,
                                  logicWorld,
                                  false,
                                  0,
                                  false);
                logicBox->SetVisAttributes(col);
                if(!tcq.isEmpty())
                {
                    dtmap.insert(logicBox, name);
                    ui->detectors->addItem(obj->getOjectName());
                }
            }
        }
        else if(obj->getTypeID() == 2)
        {
            Geant4Sphere *item = dynamic_cast<Geant4Sphere*>(obj);
            if(item)
            {
                G4Sphere* sph = new G4Sphere(name,
                                             item->getInnerRadius() * cm * sf,
                                             item->getOuterRadius() * cm * sf,
                                             item->getStartPhi() * deg,
                                             item->getLenPhi() * deg,
                                             item->getStartTheta() * deg,
                                             item->getLenTheta() * deg);
                G4LogicalVolume* logicSph = new G4LogicalVolume(sph, material, name);
                new G4PVPlacement(rot,
                                  pos,
                                  logicSph,
                                  name,
                                  logicWorld,
                                  false,
                                  0,
                                  false);
                logicSph->SetVisAttributes(col);
                if(!tcq.isEmpty())
                {
                    dtmap.insert(logicSph, name);
                    ui->detectors->addItem(obj->getOjectName());
                }
            }
        }
        else if(obj->getTypeID() == 3)
        {
            Geant4Tubs *item = dynamic_cast<Geant4Tubs*>(obj);
            if(item)
            {
                G4Tubs* tub = new G4Tubs(name,
                                         item->getInnerRadius() * cm * sf,
                                         item->getOuterRadius() * cm * sf,
                                         item->getHalfHeight() * cm * sf,
                                         item->getStartAngle() * deg,
                                         item->getLenAngle() * deg);
                G4LogicalVolume* logicTub = new G4LogicalVolume(tub, material, name);
                new G4PVPlacement(rot,
                                  pos,
                                  logicTub,
                                  name,
                                  logicWorld,
                                  false,
                                  0,
                                  false);
                logicTub->SetVisAttributes(col);
                if(!tcq.isEmpty())
                {
                    dtmap.insert(logicTub, name);
                    ui->detectors->addItem(obj->getOjectName());
                }
            }
        }
        else if(obj->getTypeID() == 4)
        {
            Geant4Torus *item = dynamic_cast<Geant4Torus*>(obj);
            if(item)
            {
                G4Torus* tor = new G4Torus(name,
                                           item->getRMin() * cm * sf,
                                           item->getRMax() * cm * sf,
                                           item->getRTor() * cm * sf,
                                           item->getSPhi() * deg,
                                           item->getDPhi() * deg);
                G4LogicalVolume* logicTor = new G4LogicalVolume(tor, material, name);
                new G4PVPlacement(rot,
                                  pos,
                                  logicTor,
                                  name,
                                  logicWorld,
                                  false,
                                  0,
                                  false);
                logicTor->SetVisAttributes(col);
                if(!tcq.isEmpty())
                {
                    dtmap.insert(logicTor, name);
                    ui->detectors->addItem(obj->getOjectName());
                }
            }
        }
        else if(obj->getTypeID() == 5)
        {
            Geant4Source *item = dynamic_cast<Geant4Source*>(obj);
            if(item)
            {
                auto *ac = g4Manager->getActionInitialization();
                ac->setPosition(pos);
                G4ThreeVector rotate(item->getRotation().x(), item->getRotation().y(), item->getRotation().z());
                ac->setRotation(rotate);
                ac->setEnergy(item->getEnegy());
                ac->setRadius(item->getRadius() * sf);
                ac->setThickness(item->getWidth() * sf);
                ac->setParticleName(item->getType().toStdString());
            }
        }
    }

    ui->detectors->blockSignals(false);
    g4Manager->getDetectorConstruction()->setDetectorNames(dtmap);
    g4Manager->getRunManager()->GeometryHasBeenModified();
    g4Manager->getRunManager()->ReinitializeGeometry();
}

QString MainWindow::getSceneInfo()
{
    QString rt;
    QBuffer buffer;
    if(!buffer.open(QIODevice::WriteOnly))
    {
        return rt;
    }
    QTextStream out(&buffer);
    out.setCodec("UTF-8");
    QVector3D pos = glWidget->getCamera().getCameraPosition();
    QVector3D dit = glWidget->getCamera().getCameraDirection();
    out << pos.x() << "  " << pos.y() << "  " << pos.z() << "\n";
    out << dit.x() << "  " << dit.y() << "  " << dit.z() << "\n";
    foreach (auto obj, glWidget->getObjectsRef())
    {
        out << "begin  ";
        out << obj->getTypeID() << "  ";
        out << obj->getPosition().x() << "  ";
        out << obj->getPosition().y() << "  ";
        out << obj->getPosition().z() << "  ";
        out << obj->getRotation().x() << "  ";
        out << obj->getRotation().y() << "  ";
        out << obj->getRotation().z() << "  ";
        out << obj->getUserColor().red() << "  ";
        out << obj->getUserColor().green() << "  ";
        out << obj->getUserColor().blue() << "  ";
        out << obj->getScaleFactor() << "  ";
        out << obj->getOjectName() << "  ";
        if(obj->getTypeID() == 0)
        {
            QString mat = obj->getCustomerName1();
            if(mat.isEmpty())
            {
                mat = "null";
            }
            out << mat << "  ";
            if(obj->getCustomerName2().isEmpty())
            {
                out << 0 << "  ";
            }
            else
            {
                out << 1 << "  ";
            }
            out << obj->getFileName() << "  ";
        }
        else if(obj->getTypeID() == 1)
        {
            Geant4Box *box = dynamic_cast<Geant4Box*>(obj);
            if(box)
            {
                QString mat = box->getCustomerName1();
                if(mat.isEmpty())
                {
                    mat = "null";
                }
                out << mat << "  ";
                if(box->getCustomerName2().isEmpty())
                {
                    out << 0 << "  ";
                }
                else
                {
                    out << 1 << "  ";
                }
                out << box->getHalfDepth() << "  ";
                out << box->getHalfWidth() << "  ";
                out << box->getHalfHeight() << "  ";
            }
        }
        else if(obj->getTypeID() == 2)
        {
            Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(obj);
            if(sphere)
            {
                QString mat = sphere->getCustomerName1();
                if(mat.isEmpty())
                {
                    mat = "null";
                }
                out << mat << "  ";
                if(sphere->getCustomerName2().isEmpty())
                {
                    out << 0 << "  ";
                }
                else
                {
                    out << 1 << "  ";
                }
                out << sphere->getInnerRadius() << "  ";
                out << sphere->getOuterRadius() << "  ";
                out << sphere->getStartPhi() << "  ";
                out << sphere->getLenPhi() << "  ";
                out << sphere->getStartTheta() << "  ";
                out << sphere->getLenTheta() << "  ";
            }
        }
        else if(obj->getTypeID() == 3)
        {
            Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(obj);
            if(tub)
            {
                QString mat = tub->getCustomerName1();
                if(mat.isEmpty())
                {
                    mat = "null";
                }
                out << mat << "  ";
                if(tub->getCustomerName2().isEmpty())
                {
                    out << 0 << "  ";
                }
                else
                {
                    out << 1 << "  ";
                }
                out << tub->getInnerRadius() << "  ";
                out << tub->getOuterRadius() << "  ";
                out << tub->getHalfHeight() << "  ";
                out << tub->getStartAngle() << "  ";
                out << tub->getLenAngle() << "  ";
            }
        }
        else if(obj->getTypeID() == 4)
        {
            Geant4Torus *tor = dynamic_cast<Geant4Torus*>(obj);
            if(tor)
            {
                QString mat = tor->getCustomerName1();
                if(mat.isEmpty())
                {
                    mat = "null";
                }
                out << mat << "  ";
                if(tor->getCustomerName2().isEmpty())
                {
                    out << 0 << "  ";
                }
                else
                {
                    out << 1 << "  ";
                }
                out << tor->getRMin() << "  ";
                out << tor->getRMax() << "  ";
                out << tor->getRTor() << "  ";
                out << tor->getSPhi() << "  ";
                out << tor->getDPhi() << "  ";
            }
        }
        else if(obj->getTypeID() == 5)
        {
            Geant4Source *sor = dynamic_cast<Geant4Source*>(obj);
            if(sor)
            {
                QString type = sor->getType();
                if(type.isEmpty())
                {
                    type = "null";
                }
                out << type << "  ";
                out << sor->getRadius() << "  ";
                out << sor->getWidth() << "  ";
                out << sor->getEnegy() << "  ";
            }
        }
        out << "end\n";
    }
    out.flush();
    rt = buffer.data();
    return rt;
}

QVector<OpenGLObject*> MainWindow::getSceneObjectsByText(QString info, OpenGLCamera *cam)
{
    QVector<OpenGLObject*> rt;
    QBuffer buffer;
    buffer.setData(info.toUtf8());
    if(!buffer.open(QIODevice::ReadOnly))
    {
        return rt;
    }
    QTextStream in(&buffer);
    in.setCodec("UTF-8");
    double x,y,z;
    x = y = z = 0.0;
    in >> x >> y >> z;
    QVector3D pos(x, y, z);
    in >> x >> y >> z;
    QVector3D dit(x, y, z);
    if(cam)
    {
        cam->lookAt(pos, pos + dit, {0, 1, 0});
    }
    QString str;
    float val;
    while (!in.atEnd())
    {
        in >> str;
        if(str == "begin")
        {
            float px = 0.0;
            float py = 0.0;
            float pz = 0.0;
            float rx = 0.0;
            float ry = 0.0;
            float rz = 0.0;
            float sf = 0.0;
            int r = 0;
            int g = 0;
            int b = 0;
            in >> val >> px >> py >> pz >> rx >> ry >> rz >> r >> g >> b >> sf >> str;
            loadNames.append(str);
            if(val == 0)
            {
                OpenGLObject *obj = new OpenGLObject;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setCustomerName1(str);
                in >> r;
                if(r == 0)
                {
                    obj->setCustomerName2("");
                }
                else
                {
                    obj->setCustomerName2(obj->getOjectName());
                }
                in >> str;
                obj->loadModel(str);
                rt.append(obj);
            }
            else if(val == 1)
            {
                Geant4Box *obj = new Geant4Box;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setCustomerName1(str);
                in >> r;
                if(r == 0)
                {
                    obj->setCustomerName2("");
                }
                else
                {
                    obj->setCustomerName2(obj->getOjectName());
                }
                in >> val;
                obj->setHalfDepth(val);
                in >> val;
                obj->setHalfWidth(val);
                in >> val;
                obj->setHalfHeight(val);
                rt.append(obj);
            }
            else if(val == 2)
            {
                Geant4Sphere *obj = new Geant4Sphere;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setCustomerName1(str);
                in >> r;
                if(r == 0)
                {
                    obj->setCustomerName2("");
                }
                else
                {
                    obj->setCustomerName2(obj->getOjectName());
                }
                in >> val;
                obj->setInnerRadius(val);
                in >> val;
                obj->setOuterRadius(val);
                in >> val;
                obj->setStartPhi(val);
                in >> val;
                obj->setLenPhi(val);
                in >> val;
                obj->setStartTheta(val);
                in >> val;
                obj->setLenTheta(val);
                rt.append(obj);
            }
            else if(val == 3)
            {
                Geant4Tubs *obj = new Geant4Tubs;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setCustomerName1(str);
                in >> r;
                if(r == 0)
                {
                    obj->setCustomerName2("");
                }
                else
                {
                    obj->setCustomerName2(obj->getOjectName());
                }
                in >> val;
                obj->setInnerRadius(val);
                in >> val;
                obj->setOuterRadius(val);
                in >> val;
                obj->setHalfHeight(val);
                in >> val;
                obj->setStartAngle(val);
                in >> val;
                obj->setLenAngle(val);
                rt.append(obj);
            }
            else if(val == 4)
            {
                Geant4Torus *obj = new Geant4Torus;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setCustomerName1(str);
                in >> r;
                if(r == 0)
                {
                    obj->setCustomerName2("");
                }
                else
                {
                    obj->setCustomerName2(obj->getOjectName());
                }
                in >> val;
                obj->setRMin(val);
                in >> val;
                obj->setRMax(val);
                in >> val;
                obj->setRTor(val);
                in >> val;
                obj->setSPhi(val);
                in >> val;
                obj->setDPhi(val);
                rt.append(obj);
            }
            else if(val == 5)
            {
                Geant4Source *obj = new Geant4Source;
                obj->setPosition({px,py,pz});
                obj->setRotation({rx,ry,rz});
                obj->setUserColor({r,g,b});
                obj->setScaleFactor(sf);
                obj->setObjectName(str);
                in >> str;
                if(str == "null")
                {
                    str = "";
                }
                obj->setType(str);
                in >> val;
                obj->setRadius(val);
                in >> val;
                obj->setWidth(val);
                in >> val;
                obj->setEnegy(val);
                rt.append(obj);
            }
        }
        in >> str;
    }
    return rt;
}

void MainWindow::on_inView_triggered()
{
    glWidget->startInView();
}

void MainWindow::on_bInView_clicked()
{
    on_inView_triggered();
}

void MainWindow::on_bResetView_clicked()
{
    on_resetCamera_triggered();
}

void MainWindow::on_cameraMoveSpeed_valueChanged(double arg1)
{
    glWidget->setCameraMoveSpeed(arg1);
}

void MainWindow::on_cameraRotateSpeed_valueChanged(double arg1)
{
    glWidget->setCameraRotateSpeed(arg1);
}

void MainWindow::on_sf_valueChanged(double arg1)
{
    if(currentObject)
    {
        currentObject->setScaleFactor(arg1);
    }
}

void MainWindow::on_px_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D pos(ui->px->value(),ui->py->value(),ui->pz->value());
        currentObject->setPosition(pos);
    }
}

void MainWindow::on_py_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D pos(ui->px->value(),ui->py->value(),ui->pz->value());
        currentObject->setPosition(pos);
    }
}

void MainWindow::on_pz_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D pos(ui->px->value(),ui->py->value(),ui->pz->value());
        currentObject->setPosition(pos);
    }
}

void MainWindow::on_rx_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D rot(ui->rx->value(),ui->ry->value(),ui->rz->value());
        currentObject->setRotation(rot);
    }
}

void MainWindow::on_ry_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D rot(ui->rx->value(),ui->ry->value(),ui->rz->value());
        currentObject->setRotation(rot);
    }
}

void MainWindow::on_rz_valueChanged(double arg1)
{
    if(currentObject)
    {
        QVector3D rot(ui->rx->value(),ui->ry->value(),ui->rz->value());
        currentObject->setRotation(rot);
    }
}

void MainWindow::on_cr_valueChanged(int arg1)
{
    if(currentObject)
    {
        QColor col(ui->cr->value(),ui->cg->value(),ui->cb->value());
        currentObject->setUserColor(col);
    }
}

void MainWindow::on_cg_valueChanged(int arg1)
{
    if(currentObject)
    {
        QColor col(ui->cr->value(),ui->cg->value(),ui->cb->value());
        currentObject->setUserColor(col);
    }
}

void MainWindow::on_cb_valueChanged(int arg1)
{
    if(currentObject)
    {
        QColor col(ui->cr->value(),ui->cg->value(),ui->cb->value());
        currentObject->setUserColor(col);
    }
}

void MainWindow::on_mats0_currentTextChanged(const QString &arg1)
{
    if(currentObject)
    {
        currentObject->setCustomerName1(arg1);
    }
}

void MainWindow::on_tcq0_stateChanged(const int arg1)
{
    if(currentObject)
    {
        if(arg1 == Qt::Checked)
        {
            currentObject->setCustomerName2(currentObject->getOjectName());
        }
        else
        {
            currentObject->setCustomerName2("");
        }
    }
}

void MainWindow::on_halflen_valueChanged(double arg1)
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        box->setHalfDepth(arg1);
    }
}

void MainWindow::on_halfwidth_valueChanged(double arg1)
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        box->setHalfWidth(arg1);
    }
}

void MainWindow::on_halfheight_valueChanged(double arg1)
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        box->setHalfHeight(arg1);
    }
}

void MainWindow::on_mats1_currentTextChanged(const QString &arg1)
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        box->setCustomerName1(arg1);
    }
}

void MainWindow::on_tcq1_stateChanged(const int arg1)
{
    Geant4Box *box = dynamic_cast<Geant4Box*>(currentObject);
    if(box)
    {
        if(arg1 == Qt::Checked)
        {
            box->setCustomerName2(box->getOjectName());
        }
        else
        {
            box->setCustomerName2("");
        }
    }
}

void MainWindow::on_inR_valueChanged(double arg1)
{
    if(arg1 >= ui->outR->value())
    {
        ui->outR->setValue(arg1 + 0.01);
    }
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setInnerRadius(arg1);
    }
}

void MainWindow::on_outR_valueChanged(double arg1)
{
    if(arg1 <= ui->inR->value())
    {
        ui->inR->setValue(arg1 - 0.01);
    }
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setOuterRadius(arg1);
    }
}

void MainWindow::on_sphi_valueChanged(double arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setStartPhi(arg1);
    }
}

void MainWindow::on_dphi_valueChanged(double arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setLenPhi(arg1);
    }
}

void MainWindow::on_stheta_valueChanged(double arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setStartTheta(arg1);
    }
}

void MainWindow::on_dtheta_valueChanged(double arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setLenTheta(arg1);
    }
}

void MainWindow::on_mats2_currentTextChanged(const QString &arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        sphere->setCustomerName1(arg1);
    }
}

void MainWindow::on_tcq2_stateChanged(const int arg1)
{
    Geant4Sphere *sphere = dynamic_cast<Geant4Sphere*>(currentObject);
    if(sphere)
    {
        if(arg1 == Qt::Checked)
        {
            sphere->setCustomerName2(sphere->getOjectName());
        }
        else
        {
            sphere->setCustomerName2("");
        }
    }
}

void MainWindow::on_tubinR_valueChanged(double arg1)
{
    if(arg1 >= ui->tuboutR->value())
    {
        ui->tuboutR->setValue(arg1 + 0.01);
    }
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setInnerRadius(arg1);
    }
}

void MainWindow::on_tuboutR_valueChanged(double arg1)
{
    if(arg1 <= ui->tubinR->value())
    {
        ui->tubinR->setValue(arg1 - 0.01);
    }
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setOuterRadius(arg1);
    }
}

void MainWindow::on_tubhalfheight_valueChanged(double arg1)
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setHalfHeight(arg1);
    }
}

void MainWindow::on_tubstartdeg_valueChanged(double arg1)
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setStartAngle(arg1);
    }
}

void MainWindow::on_tublendeg_valueChanged(double arg1)
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setLenAngle(arg1);
    }
}

void MainWindow::on_mats3_currentTextChanged(const QString &arg1)
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        tub->setCustomerName1(arg1);
    }
}

void MainWindow::on_tcq3_stateChanged(const int arg1)
{
    Geant4Tubs *tub = dynamic_cast<Geant4Tubs*>(currentObject);
    if(tub)
    {
        if(arg1 == Qt::Checked)
        {
            tub->setCustomerName2(tub->getOjectName());
        }
        else
        {
            tub->setCustomerName2("");
        }
    }
}

void MainWindow::on_torinR_valueChanged(double arg1)
{
    if(arg1 >= ui->toroutR->value())
    {
        ui->toroutR->setValue(arg1 + 0.01);
    }
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setRMin(arg1);
    }
}

void MainWindow::on_toroutR_valueChanged(double arg1)
{
    if(arg1 <= ui->torinR->value())
    {
        ui->torinR->setValue(arg1 - 0.01);
    }
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setRMax(arg1);
    }
}

void MainWindow::on_torR_valueChanged(double arg1)
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setRTor(arg1);
    }
}

void MainWindow::on_torstartdeg_valueChanged(double arg1)
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setSPhi(arg1);
    }
}

void MainWindow::on_torlendeg_valueChanged(double arg1)
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setDPhi(arg1);
    }
}

void MainWindow::on_mats4_currentTextChanged(const QString &arg1)
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        tor->setCustomerName1(arg1);
    }
}

void MainWindow::on_tcq4_stateChanged(const int arg1)
{
    Geant4Torus *tor = dynamic_cast<Geant4Torus*>(currentObject);
    if(tor)
    {
        if(arg1 == Qt::Checked)
        {
            tor->setCustomerName2(tor->getOjectName());
        }
        else
        {
            tor->setCustomerName2("");
        }
    }
}

void MainWindow::on_itemList_currentTextChanged(const QString &currentText)
{
    OpenGLObject *obj = glWidget->getObjectByName(currentText);
    if(obj)
    {
        currentObject = obj;
        glWidget->setCurrentObject(obj);
    }
}

void MainWindow::on_itemList_itemClicked(QListWidgetItem *item)
{
    OpenGLObject *obj = glWidget->getObjectByName(item->text());
    if(obj)
    {
        currentObject = obj;
        glWidget->setCurrentObject(obj);
    }
}

void MainWindow::on_sorR_valueChanged(double arg1)
{
    Geant4Source *sor = dynamic_cast<Geant4Source*>(currentObject);
    if(sor)
    {
        sor->setRadius(arg1);
    }
}

void MainWindow::on_sorL_valueChanged(double arg1)
{
    Geant4Source *sor = dynamic_cast<Geant4Source*>(currentObject);
    if(sor)
    {
        sor->setWidth(arg1);
    }
}

void MainWindow::on_sorE_valueChanged(double arg1)
{
    Geant4Source *sor = dynamic_cast<Geant4Source*>(currentObject);
    if(sor)
    {
        sor->setEnegy(arg1);
    }
}

void MainWindow::on_sorT_currentTextChanged(const QString &arg1)
{
    Geant4Source *sor = dynamic_cast<Geant4Source*>(currentObject);
    if(sor)
    {
        sor->setType(arg1);
    }
}

void MainWindow::on_loadScene_clicked()
{
    glWidget->removeAllObject();
    QString fname = QFileDialog::getOpenFileName(this,"","","*.scene");
    if(fname.isEmpty())
    {
        return;
    }
    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString info = in.readAll();
    OpenGLCamera cam;
    auto objlist = getSceneObjectsByText(info, &cam);
    foreach (auto obj, objlist)
    {
        glWidget->addObject(obj);
        ui->itemList->addItem(obj->getOjectName());
        if(obj->getTypeID() == 5)
        {
            hasSource = true;
        }
    }
    glWidget->setCamera(cam);
    QMessageBox::information(this,"提示","导入成功");
}

void MainWindow::on_saveScene_clicked()
{
    QString fname = QFileDialog::getSaveFileName(this,"","","*.scene");
    if(fname.isEmpty())
    {
        return;
    }
    QString info = getSceneInfo();
    QFile file(fname);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << info;
    QMessageBox::information(this,"提示","导出成功");
}

void MainWindow::on_clearScene_clicked()
{
    glWidget->removeAllObject();
}

void MainWindow::on_startSimulate_clicked()
{
    setEnabled(false);
    QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    g4Manager->getActionInitialization()->getRunAction()->setRunNumber(ui->simulateCount->value());
    g4Manager->getRunManager()->BeamOn(ui->simulateCount->value());
    setEnabled(true);
    Geant4UserRunAction *runAction = g4Manager->getActionInitialization()->getRunAction();
    if(runAction)
    {
        QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> traks = runAction->getTraks();
        glWidget->setDrawUserLines(true);
        glWidget->setUserLines(traks);
    }
    ui->detectors->setCurrentIndex(0);
    g4Manager->getDetectorConstruction()->setMerged(false);
    detectors = g4Manager->getDetectorConstruction()->getDetectorMap();
}

void MainWindow::on_prepareG4_clicked()
{
    if(glWidget->getObjectsRef().empty())
    {
        return;
    }
    device->clearSpectrum();
    glWidget->setDrawUserLines(false);
    sendSceneToG4();
    ui->startSimulate->setEnabled(true);
}

void MainWindow::on_drawMode_clicked()
{
    if(ui->drawMode->text() == "一般模式")
    {
        glWidget->setDrawLineMode(true);
        ui->drawMode->setText("线框模式");
    }
    else
    {
        glWidget->setDrawLineMode(false);
        ui->drawMode->setText("一般模式");
    }
}

void MainWindow::on_detectors_currentTextChanged(const QString &arg1)
{
    if(detectors.contains(arg1.toStdString()))
    {
        device->onNewDataReceived(detectors[arg1.toStdString()]);
        device->getScaledChartView()->refreshView();
    }
    else
    {
        device->clearSpectrum();
    }
}

void MainWindow::on_loadSpectrum_clicked()
{
    device->loadSpectrumFromFile();
}

void MainWindow::on_saveSpectrum_clicked()
{
    device->saveSpectrumToFile();
}

void MainWindow::on_objname_textEdited(const QString &arg1)
{
    if(currentObject)
    {
        if(currentObject->getOjectName() != arg1)
        {
            if(loadNames.contains(arg1))
            {
                ui->objname->setText(currentObject->getOjectName());
            }
            else
            {
                QString oldname = currentObject->getOjectName();
                currentObject->setObjectName(arg1);
                for (int i = 0; i < ui->itemList->count(); ++i)
                {
                    if(ui->itemList->item(i)->text() == oldname)
                    {
                        ui->itemList->item(i)->setText(arg1);
                    }
                }
            }
        }
    }
}

void MainWindow::on_trakDisplay_valueChanged(int arg1)
{
    g4Manager->getActionInitialization()->getRunAction()->setDrawNumber(arg1);
}

void MainWindow::on_resKeV_valueChanged(int arg1)
{
    g4Manager->getDetectorConstruction()->setResKeV(arg1);
    device->setResKeV(arg1);
}
