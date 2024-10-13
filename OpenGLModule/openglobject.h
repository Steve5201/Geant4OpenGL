#ifndef OPENGLOBJECT_H
#define OPENGLOBJECT_H

#include <QtMath>
#include <QMatrix4x4>
#include "openglmesh.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/EXporter.hpp>
#include <assimp/postprocess.h>

class OpenGLObject
{
public:
    OpenGLObject();
    OpenGLObject(QString file);
    OpenGLObject(const OpenGLObject &other);
    ~OpenGLObject();
    OpenGLObject& operator=(const OpenGLObject &other);

    virtual int getTypeID();
    void reset();
    bool isValid() const;
    bool getConfiged() const;
    void loadModel(QString file);
    bool exportModel(QString file);
    QString getFileName() const;
    QMatrix4x4 getTransformationMat() const;
    QVector<OpenGLMesh> getMeshs() const;
    QVector<OpenGLMesh>& getMeshsRef();
    void setScaleFactor(float factor);
    float getScaleFactor() const;
    QVector3D getPosition() const;
    void setPosition(const QVector3D &newPosition);
    QVector3D getRotation() const;
    void setRotation(const QVector3D &newRotation);
    QVector<QVector3D> getRect3D() const;
    QColor getUserColor() const;
    void setUserColor(const QColor &newUserColor);
    bool getUseUserColor() const;
    void setUseUserColor(bool newUseUserColor);
    QVector3D getUpDirection() const;
    QVector3D getFrontDirection() const;
    QVector3D getRightDirection() const;
    QString getOjectName() const;
    void setObjectName(const QString &newOjectName);
    QString getCustomerName1() const;
    void setCustomerName1(const QString &newCustomerName1);
    QString getCustomerName2() const;
    void setCustomerName2(const QString &newCustomerName2);

protected:
    QVector<OpenGLMesh> meshs;

private:
    bool valid;
    bool useUserColor;
    float scaleFactor;
    QString ojectName;
    QString fileName;
    QString customerName1;
    QString customerName2;
    QColor userColor;
    QVector3D position;
    QVector3D rotation;
    QVector3D upDirection;
    QVector3D frontDirection;
    QVector3D rightDirection;
    QMatrix4x4 *transMat,*rotMat;

    void rotate(float angle, const QVector3D &vector);
    void processNode(aiNode *node, const aiScene *scene);
    OpenGLMesh processMesh(aiMesh *mesh, const aiScene *scene);
    QVector<OpenGLMesh::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

};

#endif // OPENGLOBJECT_H
