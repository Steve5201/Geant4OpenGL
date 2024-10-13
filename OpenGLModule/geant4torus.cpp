#include "geant4torus.h"

Geant4Torus::Geant4Torus(float rMin, float rMax, float rTor, float sPhi, float dPhi)
    : m_rMin(rMin), m_rMax(rMax), m_rTor(rTor), m_sPhi(sPhi), m_dPhi(dPhi)
{
    setUseUserColor(true);
    if(m_sPhi > 360)
    {
        m_sPhi = 360;
    }
    if(m_sPhi + m_dPhi > 360)
    {
        m_dPhi = 360 - m_sPhi;
    }
    generateGeometryInfo();  // 初始化几何信息
}

float Geant4Torus::getRMin() const
{
    return m_rMin;
}

void Geant4Torus::setRMin(float rMin)
{
    m_rMin = rMin;
    generateGeometryInfo();
}

float Geant4Torus::getRMax() const
{
    return m_rMax;
}

void Geant4Torus::setRMax(float rMax)
{
    m_rMax = rMax;
    generateGeometryInfo();
}

float Geant4Torus::getRTor() const
{
    return m_rTor;
}

void Geant4Torus::setRTor(float rTor)
{
    m_rTor = rTor;
    generateGeometryInfo();
}

float Geant4Torus::getSPhi() const
{
    return m_sPhi;
}

void Geant4Torus::setSPhi(float sPhi)
{
    m_sPhi = sPhi;
    if(m_sPhi > 360)
    {
        m_sPhi = 360;
    }
    if(m_sPhi + m_dPhi > 360)
    {
        m_dPhi = 360 - m_sPhi;
    }
    generateGeometryInfo();
}

float Geant4Torus::getDPhi() const
{
    return m_dPhi;
}

void Geant4Torus::setDPhi(float dPhi)
{
    m_dPhi = dPhi;
    if(m_sPhi > 360)
    {
        m_sPhi = 360;
    }
    if(m_sPhi + m_dPhi > 360)
    {
        m_dPhi = 360 - m_sPhi;
    }
    generateGeometryInfo();
}

int Geant4Torus::getTypeID()
{
    return 4;
}

void Geant4Torus::generateGeometryInfo()
{
    // 清空之前的 mesh 数据
    meshs.clear();

    // 定义圆柱体的参数
    const int segments = 360; // 圆周分割数
    const float rMin = m_rMin; // 内半径
    const float rMax = m_rMax; // 外半径
    const float tor = m_rTor; // 环半径
    const float sPhi = m_sPhi; // 起始角度
    const float dPhi = m_dPhi; // 跨度角度

    QVector<OpenGLMesh::Vertex> vertices;
    QVector<quint32> indices;

    // 生成圆柱体的顶点
    for (int i = 0; i <= segments; ++i)
    {
        float phi = 2 * M_PI * i / segments; // 纬度角
        if((phi - (sPhi + dPhi) / 180 * M_PI) < 0.001  && (phi - sPhi / 180 * M_PI) > -0.001)
        {
            for (int j = 0; j <= 90; ++j)
            {
                float theta = 2 * M_PI * j / 90;
                float z = rMax * sin(theta);
                float x = tor * cos(phi) + rMax * cos(theta) * cos(phi); // 外半径的 x 坐标
                float y = tor * sin(phi) + rMax * cos(theta) * sin(phi); // 外半径的 z 坐标

                // 添加外层顶点
                vertices.append({ QVector3D(x, y, z), QVector3D(x, y, z).normalized()});

                // 添加内层顶点
                z = rMin * sin(theta);
                x = tor * cos(phi) + rMin * cos(theta) * cos(phi); // 内半径的 x 坐标
                y = tor * sin(phi) + rMin * cos(theta) * sin(phi); // 内半径的 z 坐标

                vertices.append({ QVector3D(x, y, z), QVector3D(x, y, z).normalized()});
            }
        }
    }

    // 生成圆环的索引
    for (int i = 0; i < dPhi; ++i)
    {
        int start = i * 182;
        for (int j = 0; j < 90; ++j)
        {
            // 添加三角形的索引
            indices.append(start + j * 2);
            indices.append(start + j * 2 + 2);
            indices.append(start + j * 2 + 182);
            indices.append(start + j * 2 + 182);
            indices.append(start + j * 2 + 2);
            indices.append(start + j * 2 + 184);

            indices.append(start + j * 2 + 1);
            indices.append(start + j * 2 + 3);
            indices.append(start + j * 2 + 183);
            indices.append(start + j * 2 + 183);
            indices.append(start + j * 2 + 3);
            indices.append(start + j * 2 + 185);
        }
    }

    if(static_cast<int>(dPhi) % 360 != 0)
    {
        int start = dPhi * 182;
        for (int j = 0; j < 90; ++j)
        {
            // 添加三角形的索引
            indices.append(j * 2);
            indices.append(j * 2 + 2);
            indices.append(j * 2 + 1);
            indices.append(j * 2 + 1);
            indices.append(j * 2 + 2);
            indices.append(j * 2 + 3);

            indices.append(start + j * 2);
            indices.append(start + j * 2 + 2);
            indices.append(start + j * 2 + 1);
            indices.append(start + j * 2 + 1);
            indices.append(start + j * 2 + 2);
            indices.append(start + j * 2 + 3);
        }
    }

    // 创建 OpenGLMesh 对象并设置数据
    OpenGLMesh mesh;
    mesh.setData(vertices, indices, {}); // 这里可以添加纹理数据，如果有的话

    // 将生成的 Mesh 添加到 meshs 成员
    meshs.append(mesh);
}
