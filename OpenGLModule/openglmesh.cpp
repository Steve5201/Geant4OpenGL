#include "openglmesh.h"

OpenGLMesh::OpenGLMesh()
{
    VAO = nullptr;
    VBO = nullptr;
    EBO = nullptr;
    rect3D.resize(8);
}

OpenGLMesh::OpenGLMesh(const OpenGLMesh &other)
{
    fileName = other.fileName;
    vertices = other.vertices;
    indices = other.indices;
    textures = other.textures;
    rect3D = other.rect3D;
    VAO = nullptr;
    VBO = nullptr;
    EBO = nullptr;
}

OpenGLMesh::OpenGLMesh(const QVector<Vertex> &v, const QVector<quint32> &i, const QVector<Texture> &t)
{
    VAO = nullptr;
    VBO = nullptr;
    EBO = nullptr;
    rect3D.resize(8);
    setData(v,i,t);
}

OpenGLMesh::~OpenGLMesh()
{
    delete VAO;
    delete VBO;
    delete EBO;
}

OpenGLMesh &OpenGLMesh::operator=(const OpenGLMesh &other)
{
    fileName = other.fileName;
    vertices = other.vertices;
    indices = other.indices;
    textures = other.textures;
    rect3D = other.rect3D;
    VAO = nullptr;
    VBO = nullptr;
    EBO = nullptr;
    return *this;
}

bool OpenGLMesh::isNull() const
{
    return textures.isEmpty() && indices.isEmpty() && textures.isEmpty();
}

bool OpenGLMesh::getConfiged() const
{
    bool tex_config = true;
    foreach (auto v, textures)
    {
        if(v.ptex == nullptr)
        {
            tex_config = false;
            break;
        }
    }
    return tex_config && VAO != nullptr && VBO != nullptr && EBO != nullptr;
}

uint OpenGLMesh::getIndicesSize() const
{
    return indices.size();
}

uint OpenGLMesh::getVerticesSize() const
{
    return vertices.size();
}

QVector<OpenGLMesh::Vertex> OpenGLMesh::getVertices() const
{
    return vertices;
}

QVector<OpenGLMesh::Vertex> &OpenGLMesh::getVerticesRef()
{
    return vertices;
}

void OpenGLMesh::setVertices(const QVector<Vertex> &newVertices)
{
    vertices = newVertices;
}

QVector<quint32> OpenGLMesh::getIndices() const
{
    return indices;
}

QVector<quint32> &OpenGLMesh::getIndicesRef()
{
    return indices;
}

void OpenGLMesh::setIndices(const QVector<quint32> &newIndices)
{
    indices = newIndices;
}

QVector<OpenGLMesh::Texture> OpenGLMesh::getTextures() const
{
    return textures;
}

QVector<OpenGLMesh::Texture> &OpenGLMesh::getTexturesRef()
{
    return textures;
}

void OpenGLMesh::setTextures(const QVector<Texture> &newTextures)
{
    textures = newTextures;
}

void OpenGLMesh::setData(const QVector<Vertex> &v, const QVector<quint32> &i, const QVector<Texture> &t)
{
    if(v.empty())
    {
        return;
    }
    vertices = v;
    indices = i;
    textures = t;
    qreal minx = vertices[0].position.x();
    qreal miny = vertices[0].position.y();
    qreal minz = vertices[0].position.z();
    qreal maxx = minx;
    qreal maxy = miny;
    qreal maxz = minz;
    foreach (auto vet, vertices)
    {
        qreal x = vet.position.x();
        qreal y = vet.position.y();
        qreal z = vet.position.z();
        if(x > maxx)
        {
            maxx = x;
        }
        if(x < minx)
        {
            minx = x;
        }
        if(y > maxy)
        {
            maxy = y;
        }
        if(y < miny)
        {
            miny = y;
        }
        if(z > maxz)
        {
            maxz = z;
        }
        if(z < minz)
        {
            minz = z;
        }
    }
    rect3D.clear();
    rect3D.append(QVector3D(minx,miny,minz));
    rect3D.append(QVector3D(minx,maxy,minz));
    rect3D.append(QVector3D(minx,miny,maxz));
    rect3D.append(QVector3D(minx,maxy,maxz));
    rect3D.append(QVector3D(maxx,miny,minz));
    rect3D.append(QVector3D(maxx,maxy,minz));
    rect3D.append(QVector3D(maxx,miny,maxz));
    rect3D.append(QVector3D(maxx,maxy,maxz));
}

QString OpenGLMesh::getFileName() const
{
    return fileName;
}

void OpenGLMesh::setFileName(const QString &newFileName)
{
    fileName = newFileName;
}

QOpenGLBuffer* OpenGLMesh::getVBO() const
{
    return VBO;
}

void OpenGLMesh::setVBO(QOpenGLBuffer *newVBO)
{
    VBO = newVBO;
}

QOpenGLBuffer* OpenGLMesh::getEBO() const
{
    return EBO;
}

void OpenGLMesh::setEBO(QOpenGLBuffer *newEBO)
{
    EBO = newEBO;
}

QOpenGLVertexArrayObject* OpenGLMesh::getVAO() const
{
    return VAO;
}

void OpenGLMesh::setVAO(QOpenGLVertexArrayObject *newVAO)
{
    VAO = newVAO;
}

QVector<QVector3D> OpenGLMesh::getRect3D() const
{
    return rect3D;
}
