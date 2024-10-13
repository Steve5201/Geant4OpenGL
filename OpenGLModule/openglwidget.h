#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QHash>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include "openglcamera.h"
#include "openglobject.h"

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void startInView();
    void stopInView();
    void addObject(OpenGLObject *obj);
    void addObject(OpenGLObject *obj, QVector3D pos);
    bool addObjectAtScreenPos(OpenGLObject *obj, QPoint pos = {0, 0});
    void removeAllObject();
    void removeObject(const int objidx);
    void removeObject(OpenGLObject *obj);
    OpenGLObject *getObjectByName(QString name) const;

public:
    QVector<OpenGLObject*> getObjects() const;
    QVector<OpenGLObject*>& getObjectsRef();
    float getFov() const;
    void setFov(float newFov);
    float getFod() const;
    void setFod(float newFod);
    OpenGLCamera getCamera() const;
    void setCamera(const OpenGLCamera &newCamera);
    float getCameraRotateSpeed() const;
    void setCameraRotateSpeed(float newCameraRotateSpeed);
    float getCameraMoveSpeed() const;
    void setCameraMoveSpeed(float newCameraMoveSpeed);
    QMatrix4x4 getProjectionMat() const;
    void setProjectionMat(const QMatrix4x4 &newProjectionMat);
    OpenGLObject *getCurrentObject() const;
    void setCurrentObject(OpenGLObject *newCurrentObject);
    QColor getAmbientColor() const;
    void setAmbientColor(const QColor &newAmbientColor);
    GLfloat getAmbientStrength() const;
    void setAmbientStrength(GLfloat newAmbientStrength);
    QColor getAmbientLightColor() const;
    void setAmbientLightColor(const QColor &newAmbientLightColor);
    QVector3D getAmbientLightDirection() const;
    void setAmbientLightDirection(const QVector3D &newAmbientLightDirection);
    bool getDrawLineMode() const;
    void setDrawLineMode(bool newDrawLineMode);
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> getUserLines() const;
    void setUserLines(const QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> &newUserLines);
    void setDrawUserLines(bool newDrawUserLines);

signals:
    void sourceRemoved();
    void objectRemoved(QString objname);
    void selectObjectChanged(OpenGLObject *obj);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void refresh();

private:
    bool programValid;
    bool inputEnabled;
    bool drawLineMode;
    bool drawUserLines;
    float fov,fod;
    float cameraMoveSpeed;
    float cameraRotateSpeed;
    bool moveR,moveL,moveF,moveB;
    double rotateY;

    QTimer *updateTimer;
    QOpenGLTexture *texture;
    OpenGLObject *currentObject;
    QOpenGLShaderProgram *program;

    QString log;
    QPoint lastPos;
    QColor AmbientColor;
    QColor AmbientLightColor;
    QVector3D AmbientLightDirection;
    GLfloat AmbientStrength;
    OpenGLCamera camera;
    QMatrix4x4 projectionMat;
    QVector<OpenGLObject*> objects;
    QHash<QString,QOpenGLTexture*> loadedTextures;
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> userLines;

    void drawUserLinesOnScene();
    void drawCurrentObjectFrame();
    void configMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo = nullptr);
    void drawMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo = nullptr);
    void configObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo = nullptr);
    void drawObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo = nullptr);
    void configProgram();
    void updateEnvironment();
    void updateProgramVertexs();
    void updateUserColor(bool use, QColor color, bool drawfra = false);
    void updateProgramModel(const QMatrix4x4 &mat);
    void updateProgramView(const QMatrix4x4 &mat);
    void updateProgramprojection(const QMatrix4x4 &mat);
    void enableSelectMode(bool select);
    void setSelectId(const QVector4D &id);
    OpenGLObject* getSelectObject(QPointF pos);

};

#endif // OPENGLWIDGET_H
