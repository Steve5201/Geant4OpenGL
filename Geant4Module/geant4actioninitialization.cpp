#include "geant4actioninitialization.h"
#include <QDebug>

Geant4ActionInitialization::Geant4ActionInitialization()
{
    runActionMaster = new Geant4UserRunAction();
    pgas = new QVector<Geant4UserPrimaryGeneratorAction*>;
}

void Geant4ActionInitialization::BuildForMaster() const
{
    G4Mutex mutex = G4MUTEX_INITIALIZER;
    G4AutoLock lock(&mutex);
    pgas->clear();
    SetUserAction(runActionMaster);
}

void Geant4ActionInitialization::Build() const
{
    G4Mutex mutex = G4MUTEX_INITIALIZER;
    G4AutoLock lock(&mutex);
    Geant4UserRunAction *runAction = new Geant4UserRunAction();
    Geant4UserEventAction *eventAction = new Geant4UserEventAction();
    Geant4UserTrackingAction *trackingAction = new Geant4UserTrackingAction();
    Geant4UserSteppingAction *steppingAction = new Geant4UserSteppingAction();
    Geant4UserPrimaryGeneratorAction *primaryGeneratorAction = new Geant4UserPrimaryGeneratorAction();
    runAction->setMaster(runActionMaster);
    runAction->setEventAction(eventAction);
    runAction->setTrackingAction(trackingAction);
    runAction->setSteppingAction(steppingAction);
    runAction->setPrimaryGeneratorAction(primaryGeneratorAction);
    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(trackingAction);
    SetUserAction(steppingAction);
    SetUserAction(primaryGeneratorAction);
    pgas->append(primaryGeneratorAction);
}

Geant4UserRunAction *Geant4ActionInitialization::getRunAction() const
{
    return runActionMaster;
}

Geant4UserEventAction *Geant4ActionInitialization::getEventAction() const
{
    return eventAction;
}

Geant4UserTrackingAction *Geant4ActionInitialization::getTrackingAction() const
{
    return trackingAction;
}

Geant4UserSteppingAction *Geant4ActionInitialization::getSteppingAction() const
{
    return steppingAction;
}

Geant4UserPrimaryGeneratorAction *Geant4ActionInitialization::getPrimaryGeneratorAction() const
{
    return primaryGeneratorAction;
}

void Geant4ActionInitialization::setRadius(G4double r)
{
    radius = r;
    foreach (auto pga, *pgas)
    {
        pga->SetRadius(radius);
    }
}

void Geant4ActionInitialization::setThickness(G4double t)
{
    thickness = t;
    foreach (auto pga, *pgas)
    {
        pga->SetThickness(thickness);
    }
}

void Geant4ActionInitialization::setPosition(G4ThreeVector pos)
{
    position = pos;
    foreach (auto pga, *pgas)
    {
        pga->SetPosition(position);
    }
}

void Geant4ActionInitialization::setRotation(G4ThreeVector rot)
{
    rotation = rot;
    foreach (auto pga, *pgas)
    {
        pga->SetRotation(rotation);
    }
}

void Geant4ActionInitialization::setEnergy(G4double newEnergy)
{
    energy = newEnergy;
    foreach (auto pga, *pgas)
    {
        pga->SetEnergy(energy);
    }
}

void Geant4ActionInitialization::setParticleName(const G4String &newParticleName)
{
    particleName = newParticleName;
    foreach (auto pga, *pgas)
    {
        pga->SetParticleType(particleName);
    }
}
