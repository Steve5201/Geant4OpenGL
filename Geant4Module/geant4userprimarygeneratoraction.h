#ifndef GEANT4USERPRIMARYGENERATORACTION_H
#define GEANT4USERPRIMARYGENERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"

class Geant4UserPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    Geant4UserPrimaryGeneratorAction();
    ~Geant4UserPrimaryGeneratorAction() override;

    // 生成初级粒子
    void GeneratePrimaries(G4Event* event) override;

    // 设置粒子源的半径和厚度
    void SetRadius(G4double radius);
    void SetThickness(G4double thickness);

    // 设置粒子源的位置、方向和能量
    void SetPosition(G4ThreeVector position);
    void SetRotation(G4ThreeVector direction);
    void SetEnergy(G4double energy);
    void SetParticleType(const G4String& particleName);

private:
    G4double radius;
    G4double thickness;
    G4ThreeVector position;
    G4ThreeVector rotation;
    G4GeneralParticleSource* fParticleSource; // 粒子源
};

#endif // GEANT4USERPRIMARYGENERATORACTION_H
