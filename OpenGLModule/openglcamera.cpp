#include "openglcamera.h"

OpenGLCamera::OpenGLCamera()
{
    cameraUp = {0,1,0};
    cameraRight = {1,0,0};
    cameraPosition = {0,0,1};
    cameraDirection = {0,0,-1};
    viewMat = new QMatrix4x4;
    viewMat->lookAt(cameraPosition,cameraPosition + cameraDirection,cameraUp);
}

OpenGLCamera::OpenGLCamera(const OpenGLCamera &other)
{
    viewMat = new QMatrix4x4;
    *viewMat = *other.viewMat;
    cameraUp = other.cameraUp;
    cameraRight = other.cameraRight;
    cameraPosition = other.cameraPosition;
    cameraDirection = other.cameraDirection;
}

OpenGLCamera::~OpenGLCamera()
{
    delete viewMat;
}

OpenGLCamera &OpenGLCamera::operator=(const OpenGLCamera &other)
{
    *viewMat = *other.viewMat;
    cameraUp = other.cameraUp;
    cameraRight = other.cameraRight;
    cameraPosition = other.cameraPosition;
    cameraDirection = other.cameraDirection;
    return *this;
}

QMatrix4x4 OpenGLCamera::getViewMat() const
{
    return *viewMat;
}

QVector3D OpenGLCamera::getCameraUp() const
{
    return cameraUp;
}

QVector3D OpenGLCamera::getCameraRight() const
{
    return cameraRight;
}

QVector3D OpenGLCamera::getCameraPosition() const
{
    return cameraPosition;
}

QVector3D OpenGLCamera::getCameraDirection() const
{
    return cameraDirection;
}

void OpenGLCamera::moveRight(float steps)
{
    cameraPosition += steps * cameraRight;
    updateCamera();
}

void OpenGLCamera::moveLeft(float steps)
{
    cameraPosition -= steps * cameraRight;
    updateCamera();
}

void OpenGLCamera::moveFront(float steps)
{
    cameraPosition += steps * cameraDirection;
    updateCamera();
}

void OpenGLCamera::moveBack(float steps)
{
    cameraPosition -= steps * cameraDirection;
    updateCamera();
}

void OpenGLCamera::rotate(float angle, const QVector3D &vector)
{
    QMatrix4x4 rot_mat4;
    rot_mat4.rotate(angle,vector);
    cameraUp = (cameraUp.toVector4D() * rot_mat4).toVector3D();
    cameraUp.normalize();
    cameraRight = (cameraRight.toVector4D() * rot_mat4).toVector3D();
    cameraRight.normalize();
    cameraDirection = (cameraDirection.toVector4D() * rot_mat4).toVector3D();
    cameraDirection.normalize();
    updateCamera();
}

void OpenGLCamera::moveTo(const QVector3D &pos)
{
    cameraPosition = pos;
    lookAt(cameraPosition,cameraPosition + cameraDirection,cameraUp);
}

void OpenGLCamera::lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up)
{
    if(!QVector3D::crossProduct(center - eye, up).isNull())
    {
        delete viewMat;
        viewMat = new QMatrix4x4;
        viewMat->lookAt(eye,center,up);
        cameraPosition = eye;
        cameraDirection = center - eye;
        cameraDirection.normalize();
        cameraRight = QVector3D::normal(cameraDirection,up);
        cameraUp = QVector3D::normal(cameraRight,cameraDirection);
    }
}

void OpenGLCamera::updateCamera()
{
    if (!QVector3D::crossProduct(cameraDirection, cameraUp).isNull())
    {
        delete viewMat;
        viewMat = new QMatrix4x4;
        viewMat->lookAt(cameraPosition,cameraPosition + cameraDirection,cameraUp);
    }
}
