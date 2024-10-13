#ifndef OPENGLCAMERA_H
#define OPENGLCAMERA_H

#include <QMatrix4x4>

class OpenGLCamera
{
public:
    OpenGLCamera();
    OpenGLCamera(const OpenGLCamera &other);
    ~OpenGLCamera();
    OpenGLCamera& operator=(const OpenGLCamera &other);

    QMatrix4x4 getViewMat() const;
    QVector3D getCameraUp() const;
    QVector3D getCameraRight() const;
    QVector3D getCameraPosition() const;
    QVector3D getCameraDirection() const;

    void moveRight(float steps);
    void moveLeft(float steps);
    void moveFront(float steps);
    void moveBack(float steps);
    void rotate(float angle, const QVector3D &vector);
    void moveTo(const QVector3D &pos);
    void lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up);

private:
    QMatrix4x4 *viewMat;
    QVector3D cameraUp;
    QVector3D cameraRight;
    QVector3D cameraPosition;
    QVector3D cameraDirection;

    void updateCamera();

};

#endif // OPENGLCAMERA_H
