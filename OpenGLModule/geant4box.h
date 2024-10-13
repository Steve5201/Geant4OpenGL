#ifndef GEANT4BOX_H
#define GEANT4BOX_H

#include "openglobject.h"

class Geant4Box : public OpenGLObject
{
public:
    Geant4Box(float halfwWidth = 1.0, float halfHeight = 1.0, float halfDepth = 1.0);

    // 获取宽度
    float getHalfWidth() const;
    // 设置宽度
    void setHalfWidth(float width);

    // 获取高度
    float getHalfHeight() const;
    // 设置高度
    void setHalfHeight(float height);

    // 获取深度
    float getHalfDepth() const;
    // 设置深度
    void setHalfDepth(float depth);

    virtual int getTypeID() override;

private:
    float m_halfWidth;  // 宽度
    float m_halfHeight; // 高度
    float m_halfDepth;  // 深度

    void generateGeometryInfo();  // 生成几何信息的函数

};

#endif // GEANT4BOX_H
