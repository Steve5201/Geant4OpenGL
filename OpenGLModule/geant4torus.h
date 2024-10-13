#ifndef GEANT4TORUS_H
#define GEANT4TORUS_H

#include "openglobject.h"

class Geant4Torus : public OpenGLObject
{
public:
    // 构造函数参考 G4Torus
    Geant4Torus(float rMin = 0.3, float rMax = 0.5, float rTor = 1.0, float sPhi = 0.0, float dPhi = 360.0);

    // Getter 和 Setter 函数
    float getRMin() const;
    void setRMin(float rMin);

    float getRMax() const;
    void setRMax(float rMax);

    float getRTor() const;
    void setRTor(float rTor);

    float getSPhi() const;
    void setSPhi(float sPhi);

    float getDPhi() const;
    void setDPhi(float dPhi);

    virtual int getTypeID() override;

private:
    float m_rMin;        // 内半径
    float m_rMax;        // 外半径
    float m_rTor;        // 管的半径
    float m_sPhi;        // 起始角度
    float m_dPhi;        // 角度跨度

    void generateGeometryInfo();  // 生成几何信息的函数
};

#endif // GEANT4TORUS_H
