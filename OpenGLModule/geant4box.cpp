#include "geant4box.h"

Geant4Box::Geant4Box(float width, float height, float depth)
    : m_halfWidth(width), m_halfHeight(height), m_halfDepth(depth)
{
    setUseUserColor(true);
    generateGeometryInfo();  // 初始化几何信息
}

float Geant4Box::getHalfWidth() const
{
    return m_halfWidth;
}

void Geant4Box::setHalfWidth(float width)
{
    m_halfWidth = width;
    generateGeometryInfo();
}

float Geant4Box::getHalfHeight() const
{
    return m_halfHeight;
}

void Geant4Box::setHalfHeight(float height)
{
    m_halfHeight = height;
    generateGeometryInfo();
}

float Geant4Box::getHalfDepth() const
{
    return m_halfDepth;
}

void Geant4Box::setHalfDepth(float depth)
{
    m_halfDepth = depth;
    generateGeometryInfo();
}

int Geant4Box::getTypeID()
{
    return 1;
}

void Geant4Box::generateGeometryInfo()
{
    // 清空之前的 mesh 数据
    meshs.clear();

    // 创建 Box 的顶点
    QVector<OpenGLMesh::Vertex> vertices;

    // 定义 Box 的八个顶点（假设宽度、高度和深度分别为 m_width、m_height 和 m_depth）
    // 前面四个顶点
    vertices.append({ QVector3D(-m_halfWidth, -m_halfHeight, -m_halfDepth), QVector3D(-1, -1, -1), QVector2D(0, 0) }); // 0
    vertices.append({ QVector3D(m_halfWidth, -m_halfHeight, -m_halfDepth), QVector3D(1, -1, -1), QVector2D(1, 0) });  // 1
    vertices.append({ QVector3D(m_halfWidth, m_halfHeight, -m_halfDepth), QVector3D(1, 1, -1), QVector2D(1, 1) });   // 2
    vertices.append({ QVector3D(-m_halfWidth, m_halfHeight, -m_halfDepth), QVector3D(-1, 1, -1), QVector2D(0, 1) });  // 3

    // 后面四个顶点
    vertices.append({ QVector3D(-m_halfWidth, -m_halfHeight, m_halfDepth), QVector3D(-1, -1, 1), QVector2D(0, 0) });  // 4
    vertices.append({ QVector3D(m_halfWidth, -m_halfHeight, m_halfDepth), QVector3D(1, -1, 1), QVector2D(1, 0) });   // 5
    vertices.append({ QVector3D(m_halfWidth, m_halfHeight, m_halfDepth), QVector3D(1, 1, 1), QVector2D(1, 1) });    // 6
    vertices.append({ QVector3D(-m_halfWidth, m_halfHeight, m_halfDepth), QVector3D(-1, 1, 1), QVector2D(0, 1) });   // 7

    // 创建索引数组，定义每个面的两个三角形
    QVector<quint32> indices;

    // 前面
    indices.append(0); indices.append(2); indices.append(1);
    indices.append(0); indices.append(3); indices.append(2);

    // 后面
    indices.append(4); indices.append(5); indices.append(6);
    indices.append(4); indices.append(6); indices.append(7);

    // 左侧
    indices.append(4); indices.append(1); indices.append(5);
    indices.append(4); indices.append(0); indices.append(1);

    // 右侧
    indices.append(3); indices.append(6); indices.append(2);
    indices.append(3); indices.append(7); indices.append(6);

    // 顶部
    indices.append(3); indices.append(4); indices.append(7);
    indices.append(3); indices.append(0); indices.append(4);

    // 底部
    indices.append(1); indices.append(6); indices.append(5);
    indices.append(1); indices.append(2); indices.append(6);

    // 创建 OpenGLMesh 对象并设置数据
    OpenGLMesh mesh;
    mesh.setData(vertices, indices, {});  // 这里可以添加纹理数据，如果有的话

    // 将生成的 Mesh 添加到 meshs 成员
    meshs.append(mesh);
}
