#include "geant4tubs.h"

Geant4Tubs::Geant4Tubs(float innerRadius, float outerRadius, float height, float startAngle, float lenAngle)
    : m_innerRadius(innerRadius), m_outerRadius(outerRadius),
    m_halfHeight(height), m_startAngle(startAngle), m_lenAngle(lenAngle)
{
    setUseUserColor(true);
    if(m_startAngle > 360)
    {
        m_startAngle = 360;
    }
    if(m_startAngle + m_lenAngle > 360)
    {
        m_lenAngle = 360 - m_startAngle;
    }
    generateGeometryInfo();
}

float Geant4Tubs::getInnerRadius() const
{
    return m_innerRadius;
}

void Geant4Tubs::setInnerRadius(float innerRadius)
{
    m_innerRadius = innerRadius;
    generateGeometryInfo();
}

float Geant4Tubs::getOuterRadius() const
{
    return m_outerRadius;
}

void Geant4Tubs::setOuterRadius(float outerRadius)
{
    m_outerRadius = outerRadius;
    generateGeometryInfo();
}

float Geant4Tubs::getHalfHeight() const
{
    return m_halfHeight;
}

void Geant4Tubs::setHalfHeight(float height)
{
    m_halfHeight = height;
    generateGeometryInfo();
}

float Geant4Tubs::getStartAngle() const
{
    return m_startAngle;
}

void Geant4Tubs::setStartAngle(float startAngle)
{
    m_startAngle = startAngle;
    if(m_startAngle > 360)
    {
        m_startAngle = 360;
    }
    if(m_startAngle + m_lenAngle > 360)
    {
        m_lenAngle = 360 - m_startAngle;
    }
    generateGeometryInfo();
}

float Geant4Tubs::getLenAngle() const
{
    return m_lenAngle;
}

void Geant4Tubs::setLenAngle(float endAngle)
{
    m_lenAngle = endAngle;
    if(m_startAngle > 360)
    {
        m_startAngle = 360;
    }
    if(m_startAngle + m_lenAngle > 360)
    {
        m_lenAngle = 360 - m_startAngle;
    }
    generateGeometryInfo();
}

int Geant4Tubs::getTypeID()
{
    return 3;
}

void Geant4Tubs::generateGeometryInfo()
{
    // 清空之前的 mesh 数据
    meshs.clear();

    // 定义圆柱体的参数
    const int segments = 360; // 圆周分割数
    const float rMin = m_innerRadius; // 内半径
    const float rMax = m_outerRadius; // 外半径
    const float height = m_halfHeight; // 高度
    const float sPhi = m_startAngle; // 起始角度
    const float dPhi = m_lenAngle; // 跨度角度

    QVector<OpenGLMesh::Vertex> vertices;
    QVector<quint32> indices;

    // 生成圆柱体的顶点
    float z = height; // 计算高度
    for (int j = 0; j <= segments; ++j)
    {
        float phi = 2 * M_PI * j / segments; // 纬度角
        if((phi - (sPhi + dPhi) / 180 * M_PI) < 0.001  && (phi - sPhi / 180 * M_PI) > -0.001)
        {
            float x = rMax * cos(phi); // 外半径的 x 坐标
            float y = rMax * sin(phi); // 外半径的 z 坐标

            // 添加外层顶点
            vertices.append({ QVector3D(x, y, z), QVector3D(x, y, z).normalized()});
            vertices.append({ QVector3D(x, y, -z), QVector3D(x, y, -z).normalized()});

            // 添加内层顶点
            x = rMin * cos(phi); // 内半径的 x 坐标
            y = rMin * sin(phi); // 内半径的 z 坐标

            vertices.append({ QVector3D(x, y, z), QVector3D(x, y, z).normalized()});
            vertices.append({ QVector3D(x, y, -z), QVector3D(x, y, -z).normalized()});
        }
    }

    // 生成圆柱体的索引
    for (int j = 0; j < dPhi; ++j)
    {
        // 添加两个三角形的索引
        indices.append(j * 4);
        indices.append(j * 4 + 4);
        indices.append(j * 4 + 1);
        indices.append(j * 4 + 4);
        indices.append(j * 4 + 5);
        indices.append(j * 4 + 1);

        indices.append(j * 4 + 6);
        indices.append(j * 4 + 2);
        indices.append(j * 4 + 3);
        indices.append(j * 4 + 3);
        indices.append(j * 4 + 7);
        indices.append(j * 4 + 6);

        indices.append(j * 4 + 4);
        indices.append(j * 4 + 0);
        indices.append(j * 4 + 2);
        indices.append(j * 4 + 2);
        indices.append(j * 4 + 6);
        indices.append(j * 4 + 4);

        indices.append(j * 4 + 1);
        indices.append(j * 4 + 5);
        indices.append(j * 4 + 3);
        indices.append(j * 4 + 3);
        indices.append(j * 4 + 5);
        indices.append(j * 4 + 7);
    }

    if(static_cast<int>(dPhi) % 360 != 0)
    {
        indices.append(0);
        indices.append(1);
        indices.append(2);
        indices.append(1);
        indices.append(3);
        indices.append(2);

        indices.append(4 * dPhi + 0);
        indices.append(4 * dPhi + 1);
        indices.append(4 * dPhi + 2);
        indices.append(4 * dPhi + 1);
        indices.append(4 * dPhi + 3);
        indices.append(4 * dPhi + 2);
    }

    // 创建 OpenGLMesh 对象并设置数据
    OpenGLMesh mesh;
    mesh.setData(vertices, indices, {}); // 这里可以添加纹理数据，如果有的话

    // 将生成的 Mesh 添加到 meshs 成员
    meshs.append(mesh);
}
