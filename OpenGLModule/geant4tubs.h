#ifndef GEANT4TUBS_H
#define GEANT4TUBS_H

#include "openglobject.h"

class Geant4Tubs : public OpenGLObject
{
public:
    Geant4Tubs(float innerRadius = 0.5, float outerRadius = 1.0, float halfHeight = 1.0, float startAngle = 0, float lenAngle = 360);

    float getInnerRadius() const;
    void setInnerRadius(float innerRadius);

    float getOuterRadius() const;
    void setOuterRadius(float outerRadius);

    float getHalfHeight() const;
    void setHalfHeight(float height);

    float getStartAngle() const;
    void setStartAngle(float startAngle);

    float getLenAngle() const;
    void setLenAngle(float lenAngle);

    virtual int getTypeID() override;

private:
    float m_innerRadius; // 内半径
    float m_outerRadius; // 外半径
    float m_halfHeight;      // 高度
    float m_startAngle;  // 起始角度
    float m_lenAngle;    // 结束角度

    void generateGeometryInfo();
};

#endif // GEANT4TUBS_H
