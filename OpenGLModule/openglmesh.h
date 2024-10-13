#ifndef OPENGLMESH_H
#define OPENGLMESH_H

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class OpenGLMesh
{
public:

    enum TextureType {DIFFUSE = 2048, SPECULAR};
    struct Vertex
    {
        QVector3D position;
        QVector3D normal;
        QVector2D texCoords;
    };

    struct Texture
    {
        QString path;
        TextureType type;
        QOpenGLTexture *ptex;
    };

    OpenGLMesh();
    OpenGLMesh(const OpenGLMesh &other);
    OpenGLMesh(const QVector<Vertex> &v, const QVector<quint32> &i, const QVector<Texture> &t);
    ~OpenGLMesh();
    OpenGLMesh& operator=(const OpenGLMesh &other);

    bool isNull() const;
    bool getConfiged() const;
    uint getIndicesSize() const;
    uint getVerticesSize() const;
    QVector<Vertex> getVertices() const;
    QVector<Vertex>& getVerticesRef();
    void setVertices(const QVector<Vertex> &newVertices);
    QVector<quint32> getIndices() const;
    QVector<quint32>& getIndicesRef();
    void setIndices(const QVector<quint32> &newIndices);
    QVector<Texture> getTextures() const;
    QVector<Texture>& getTexturesRef();
    void setTextures(const QVector<Texture> &newTextures);
    void setData(const QVector<Vertex> &v, const QVector<quint32> &i, const QVector<Texture> &t);
    QString getFileName() const;
    void setFileName(const QString &newFileName);
    QOpenGLBuffer* getVBO() const;
    void setVBO(QOpenGLBuffer *newVBO);
    QOpenGLBuffer* getEBO() const;
    void setEBO(QOpenGLBuffer *newEBO);
    QOpenGLVertexArrayObject* getVAO() const;
    void setVAO(QOpenGLVertexArrayObject *newVAO);
    QVector<QVector3D> getRect3D() const;

private:
    QString fileName;
    QVector<Vertex> vertices;
    QVector<quint32> indices;
    QVector<Texture> textures;
    QOpenGLBuffer *VBO,*EBO;
    QOpenGLVertexArrayObject *VAO;
    QVector<QVector3D> rect3D;

};

#endif // OPENGLMESH_H
