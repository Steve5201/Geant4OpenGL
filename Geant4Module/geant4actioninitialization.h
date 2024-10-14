#ifndef GEANT4ACTIONINITIALIZATION_H
#define GEANT4ACTIONINITIALIZATION_H

#include "G4VUserActionInitialization.hh"
#include "geant4userrunaction.h"

class Geant4ActionInitialization : public G4VUserActionInitialization
{
public:
    Geant4ActionInitialization();
    ~Geant4ActionInitialization() override = default;

    virtual void BuildForMaster() const override;

    virtual void Build() const override;

    Geant4UserRunAction *getRunAction() const;

    // 设置粒子源的半径和厚度
    void setRadius(G4double radius);
    void setThickness(G4double thickness);

    // 设置粒子源的位置、方向和能量
    void setPosition(G4ThreeVector position);
    void setRotation(G4ThreeVector direction);
    void setEnergy(G4double newEnergy);
    void setParticleName(const G4String &newParticleName);

private:
    Geant4UserRunAction *runActionMaster;
    QVector<Geant4UserPrimaryGeneratorAction*> *pgas;

    G4double radius;
    G4double thickness;
    G4ThreeVector position;
    G4ThreeVector rotation;
    G4double energy;
    G4String particleName;

};

#endif // GEANT4ACTIONINITIALIZATION_H
