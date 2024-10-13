#include "geant4source.h"

Geant4Source::Geant4Source(float radius, float width, float enegy): m_radius(radius), m_width(width), m_enegy(enegy)
{
    setUseUserColor(true);
    generateGeometryInfo();  // 初始化几何信息
}

int Geant4Source::getTypeID()
{
    return 5;
}

float Geant4Source::getRadius() const
{
    return m_radius;
}

void Geant4Source::setRadius(float newRadius)
{
    m_radius = newRadius;
    generateGeometryInfo();
}

float Geant4Source::getWidth() const
{
    return m_width;
}

void Geant4Source::setWidth(float newWidth)
{
    m_width = newWidth;
    generateGeometryInfo();
}

float Geant4Source::getEnegy() const
{
    return m_enegy;
}

void Geant4Source::setEnegy(float newEnegy)
{
    m_enegy = newEnegy;
}

QString Geant4Source::getType() const
{
    return type;
}

void Geant4Source::setType(const QString &newType)
{
    type = newType;
}

void Geant4Source::generateGeometryInfo()
{
    // 清空之前的 mesh 数据
    meshs.clear();

    // 创建 Box 的顶点
    QVector<OpenGLMesh::Vertex> vertices;
    // 创建索引数组，定义每个面的两个三角形
    QVector<quint32> indices;

    float r = m_radius > 0 ? m_radius : 0.01;
    float l = m_width > 0 ? m_width : 0.01;
    for (int j = 0; j <= 90; ++j)
    {
        float theta = 2 * M_PI * j / 90;
        float y = r * sin(theta);
        float x = r * cos(theta); // 外半径的 x 坐标
        float z = l / 2; // 外半径的 z 坐标

        // 添加外层顶点
        vertices.append({ QVector3D(x, y, z), QVector3D(0, 0, 1).normalized()});
        vertices.append({ QVector3D(x, y, -z), QVector3D(0, -1, 1).normalized()});
    }
    vertices.append({ QVector3D(0, 0, l / 2), QVector3D(0, 0, 1).normalized()});
    vertices.append({ QVector3D(0, 0, -l / 2), QVector3D(0, -1, 1).normalized()});

    for (int j = 0; j < 90; ++j)
    {
        // 添加三角形的索引
        indices.append(j * 2);
        indices.append(j * 2 + 1);
        indices.append(j * 2 + 2);
        indices.append(j * 2 + 2);
        indices.append(j * 2 + 1);
        indices.append(j * 2 + 3);

        indices.append(j * 2);
        indices.append(j * 2 + 2);
        indices.append(182);

        indices.append(j * 2 + 3);
        indices.append(j * 2 + 1);
        indices.append(183);
    }

    // 创建 OpenGLMesh 对象并设置数据
    OpenGLMesh mesh;
    mesh.setData(vertices, indices, {});  // 这里可以添加纹理数据，如果有的话

    // 将生成的 Mesh 添加到 meshs 成员
    meshs.append(mesh);
}
