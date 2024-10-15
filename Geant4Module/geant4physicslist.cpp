#include "geant4physicslist.h"
#include "G4SystemOfUnits.hh"
#include "G4EmStandardPhysics.hh"  // 标准电磁物理过程
#include "G4EmLivermorePhysics.hh" // 低能电磁物理过程
#include "G4DecayPhysics.hh"       // 粒子衰变物理过程
#include "G4HadronElasticPhysics.hh"  // 强子弹性散射过程
#include "G4HadronPhysicsQGSP_BIC.hh" // QGSP_BIC 强子模型
#include "G4OpticalPhysics.hh"     // 光学物理过程（如果需要模拟光学效应）
#include "G4StepLimiterPhysics.hh" // 步长限制器（用于精确控制粒子步长）

Geant4PhysicsList::Geant4PhysicsList(bool highE)
{
    // 注册粒子电磁物理过程
    if(highE)
    {
        RegisterPhysics(new G4EmStandardPhysics());
    }
    else
    {
        RegisterPhysics(new G4EmLivermorePhysics());
    }

    // 注册粒子衰变物理过程
    RegisterPhysics(new G4DecayPhysics());

    // 注册强子弹性散射物理过程
    RegisterPhysics(new G4HadronElasticPhysics());

    // 注册强子物理模型，使用 QGSP_BIC 模型
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC());

    // （可选）注册光学物理过程
    RegisterPhysics(new G4OpticalPhysics());

    // （可选）注册步长限制器物理过程
    RegisterPhysics(new G4StepLimiterPhysics());
}

Geant4PhysicsList::~Geant4PhysicsList() {}

void Geant4PhysicsList::SetCuts()
{
    // 设置全局截断值（适用于所有粒子类型），单位为 mm
    SetCutsWithDefault();

    // 设置电子的截断值为 0.01 mm（即大约 1 keV 能量）
    SetCutValue(0.01 * mm, "e-");

    // 设置质子的截断值为 0.1 mm
    SetCutValue(0.1 * mm, "proton");

    // 设置伽马的截断值为 0.1 mm
    SetCutValue(0.01 * mm, "gamma");
}

