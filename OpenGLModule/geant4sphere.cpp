#include "geant4sphere.h"

Geant4Sphere::Geant4Sphere(float innerRadius, float outerRadius, float startPhi, float lenPhi, float startTheta, float lenTheta)
    : m_innerRadius(innerRadius), m_outerRadius(outerRadius),
    m_startTheta(startTheta), m_lenTheta(lenTheta),
    m_startPhi(startPhi), m_lenPhi(lenPhi)
{
    setUseUserColor(true);
    if(m_startPhi > 360)
    {
        m_startPhi = 360;
    }
    if(m_startPhi + m_lenPhi > 360)
    {
        m_lenPhi = 360 - m_startPhi;
    }
    if(m_startTheta > 180)
    {
        m_startTheta = 180;
    }
    if(m_startTheta + m_lenTheta > 180)
    {
        m_lenTheta = 180 - m_startTheta;
    }
    generateGeometryInfo();
}

float Geant4Sphere::getInnerRadius() const
{
    return m_innerRadius;
}

void Geant4Sphere::setInnerRadius(float innerRadius)
{
    m_innerRadius = innerRadius;
    generateGeometryInfo();
}

float Geant4Sphere::getOuterRadius() const
{
    return m_outerRadius;
}

void Geant4Sphere::setOuterRadius(float outerRadius)
{
    m_outerRadius = outerRadius;
    generateGeometryInfo();
}

float Geant4Sphere::getStartTheta() const
{
    return m_startTheta;
}

void Geant4Sphere::setStartTheta(float startTheta)
{
    m_startTheta = startTheta;
    if(m_startTheta > 180)
    {
        m_startTheta = 180;
    }
    if(m_startTheta + m_lenTheta > 180)
    {
        m_lenTheta = 180 - m_startTheta;
    }
    generateGeometryInfo();
}

float Geant4Sphere::getLenTheta() const
{
    return m_lenTheta;
}

void Geant4Sphere::setLenTheta(float endTheta)
{
    m_lenTheta = endTheta;
    if(m_startTheta > 180)
    {
        m_startTheta = 180;
    }
    if(m_startTheta + m_lenTheta > 180)
    {
        m_lenTheta = 180 - m_startTheta;
    }
    generateGeometryInfo();
}

float Geant4Sphere::getStartPhi() const
{
    return m_startPhi;
}

void Geant4Sphere::setStartPhi(float startPhi)
{
    m_startPhi = startPhi;
    if(m_startPhi > 360)
    {
        m_startPhi = 360;
    }
    if(m_startPhi + m_lenPhi > 360)
    {
        m_lenPhi = 360 - m_startPhi;
    }
    generateGeometryInfo();
}

float Geant4Sphere::getLenPhi() const
{
    return m_lenPhi;
}

void Geant4Sphere::setLenPhi(float endPhi)
{
    m_lenPhi = endPhi;
    if(m_startPhi > 360)
    {
        m_startPhi = 360;
    }
    if(m_startPhi + m_lenPhi > 360)
    {
        m_lenPhi = 360 - m_startPhi;
    }
    generateGeometryInfo();
}

int Geant4Sphere::getTypeID()
{
    return 2;
}

void Geant4Sphere::generateGeometryInfo()
{
    // 清空之前的 mesh 数据
    meshs.clear();

    // 定义球体的参数
    const int segments = 360; // 球体的纬度段数
    const int rings = 180;     // 球体的经度环数
    const float radius_out = m_outerRadius; // 球体外半径
    const float radius_in = m_innerRadius; // 球体内半径

    QVector<OpenGLMesh::Vertex> vertices;
    QVector<quint32> indices;

    // 生成球体的顶点
    for (int i = 0; i <= rings; ++i)
    {
        float phi = M_PI * i / rings; // 纬度角
        if((phi - (m_lenTheta + m_startTheta) / 180 * M_PI) < 0.001  && (phi - m_startTheta / 180 * M_PI) > -0.001)
        {
            for (int j = 0; j <= segments; ++j)
            {
                float theta = 2.0f * M_PI * j / segments; // 经度角
                if((theta - (m_lenPhi + m_startPhi) / 180 * M_PI) < 0.001 && (theta - m_startPhi / 180 * M_PI) > -0.001)
                {
                    // 计算球体外表面的顶点坐标
                    float z = radius_out * cos(phi);
                    float x = radius_out * sin(phi) * cos(theta);
                    float y = radius_out * sin(phi) * sin(theta);
                    // 计算球体内表面的顶点坐标
                    float z2 = radius_in * cos(phi);
                    float x2 = radius_in * sin(phi) * cos(theta);
                    float y2 = radius_in * sin(phi) * sin(theta);

                    // 法向量（单位向量）
                    QVector3D normal = QVector3D(x, y, z).normalized();

                    // 添加顶点
                    vertices.append({ QVector3D(x, y, z), normal});
                    vertices.append({ QVector3D(x2, y2, z2), normal});
                }
            }
        }
    }

    // 生成球体的索引
    int start;
    for (int i = 0; i < m_lenTheta; ++i)
    {
        start = (m_lenPhi + 1) * i * 2;
        for (int j = 0; j < m_lenPhi; ++j)
        {
            indices.append(start + 2 * j);
            indices.append(start + 2 * j + m_lenPhi * 2 + 2);
            indices.append(start + 2 * j + 2);
            indices.append(start + 2 * j + m_lenPhi * 2 + 2);
            indices.append(start + 2 * j + m_lenPhi * 2 + 4);
            indices.append(start + 2 * j + 2);

            indices.append(start + 2 * j + 1);
            indices.append(start + 2 * j + 3);
            indices.append(start + 2 * j + m_lenPhi * 2 + 3);
            indices.append(start + 2 * j + m_lenPhi * 2 + 3);
            indices.append(start + 2 * j + 3);
            indices.append(start + 2 * j + m_lenPhi * 2 + 5);


        }
    }
    for (int j = 0; j < m_lenPhi; ++j)
    {
        if(qAbs(qSin(m_startTheta / 180 * M_PI) - 0) < 0.00000001)
        {
            break;
        }
        indices.append(2 * j + 2);
        indices.append(2 * j);
        indices.append(2 * j + 1);
        indices.append(2 * j + 1);
        indices.append(2 * j + 3);
        indices.append(2 * j + 2);
    }
    start = (m_lenPhi + 1) * (m_lenTheta) * 2;
    for (int j = 0; j < m_lenPhi; ++j)
    {
        if(qAbs(qSin((m_startTheta + m_lenTheta) / 180 * M_PI)) - 0 < 0.00000001)
        {
            break;
        }
        indices.append(start + 2 * j + 2);
        indices.append(start + 2 * j);
        indices.append(start + 2 * j + 1);
        indices.append(start + 2 * j + 1);
        indices.append(start + 2 * j + 3);
        indices.append(start + 2 * j + 2);
    }
    for (int j = 0; j < m_lenTheta; ++j)
    {
        start = (m_lenPhi + 1) * j * 2;
        if(static_cast<int>(m_lenPhi) % 360 == 0)
        {
            break;
        }
        indices.append(start + m_lenPhi * 2 + 2);
        indices.append(start);
        indices.append(start + 1);
        indices.append(start + m_lenPhi * 2 + 2);
        indices.append(start + 1);
        indices.append(start + m_lenPhi * 2 + 3);
    }
    for (int j = 0; j < m_lenTheta; ++j)
    {
        start = (m_lenPhi + 1) * j * 2;
        if(static_cast<int>(m_lenPhi) % 360 == 0)
        {
            break;
        }
        indices.append(start + m_lenPhi * 2);
        indices.append(start + m_lenPhi * 4 + 2);
        indices.append(start + m_lenPhi * 2 + 1);
        indices.append(start + m_lenPhi * 2 + 1);
        indices.append(start + m_lenPhi * 4 + 2);
        indices.append(start + m_lenPhi * 4 + 3);
    }

    // 创建 OpenGLMesh 对象并设置数据
    OpenGLMesh mesh;
    mesh.setData(vertices, indices, {}); // 这里可以添加纹理数据，如果有的话

    // 将生成的 Mesh 添加到 meshs 成员
    meshs.append(mesh);
}
