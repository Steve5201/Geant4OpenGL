#ifndef GEANT4SPHERE_H
#define GEANT4SPHERE_H

#include "openglobject.h"

class Geant4Sphere : public OpenGLObject
{
public:
    Geant4Sphere(float innerRadius = 0.5, float outerRadius = 1.0, float startPhi = 0.0, float lenPhi = 360, float startTheta = 0.0, float lenTheta = 180);

    float getInnerRadius() const;
    void setInnerRadius(float innerRadius);

    float getOuterRadius() const;
    void setOuterRadius(float outerRadius);

    float getStartTheta() const;
    void setStartTheta(float startTheta);

    float getLenTheta() const;
    void setLenTheta(float endTheta);

    float getStartPhi() const;
    void setStartPhi(float startPhi);

    float getLenPhi() const;
    void setLenPhi(float endPhi);

    virtual int getTypeID() override;

private:
    float m_innerRadius; // 内半径
    float m_outerRadius; // 外半径
    float m_startTheta;  // 起始极角
    float m_lenTheta;    // 结束极角
    float m_startPhi;    // 起始方位角
    float m_lenPhi;      // 结束方位角

    void generateGeometryInfo();
};

#endif // GEANT4SPHERE_H
