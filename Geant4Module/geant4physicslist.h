#ifndef GEANT4PHYSICSLIST_H
#define GEANT4PHYSICSLIST_H

#include "G4VModularPhysicsList.hh"

class Geant4PhysicsList : public G4VModularPhysicsList
{
public:
    Geant4PhysicsList();

    ~Geant4PhysicsList() override;

    void SetCuts() override;

};

#endif // GEANT4PHYSICSLIST_H
