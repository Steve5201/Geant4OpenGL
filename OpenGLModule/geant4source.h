#ifndef GEANT4SOURCE_H
#define GEANT4SOURCE_H

#include "openglobject.h"

class Geant4Source : public OpenGLObject
{
public:
    Geant4Source(float radius = 1.0 , float width = 0.1, float enegy = 100.0);

    virtual int getTypeID() override;

    float getRadius() const;
    void setRadius(float newRadius);

    float getWidth() const;
    void setWidth(float newWidth);

    float getEnegy() const;
    void setEnegy(float newEnegy);

    QString getType() const;
    void setType(const QString &newType);

private:
    float m_radius;  // 半径
    float m_width;  // 厚度
    float m_enegy; // 能量
    QString type; // 粒子类型

    void generateGeometryInfo();  // 生成几何信息的函数
};

#endif // GEANT4SOURCE_H
