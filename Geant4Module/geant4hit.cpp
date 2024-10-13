#include "geant4hit.h"

G4ThreadLocal G4Allocator<Geant4Hit>* Geant4HitAllocator = nullptr;


Geant4Hit::Geant4Hit() : energyDep(0.0), time(0.0)
{

}

Geant4Hit::~Geant4Hit()
{

}

void Geant4Hit::SetEnergyDep(G4double edep)
{
    energyDep = edep;
}

G4double Geant4Hit::GetEnergyDep() const
{
    return energyDep;
}

void Geant4Hit::SetPosition(const G4ThreeVector &pos)
{
    position = pos;
}

const G4ThreeVector &Geant4Hit::GetPosition() const
{
    return position;
}

void Geant4Hit::SetTime(G4double t)
{
    time = t;
}

G4double Geant4Hit::GetTime() const
{
    return time;
}
