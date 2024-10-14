#include "geant4usereventaction.h"
#include "Randomize.hh"
#include <QDebug>

Geant4UserEventAction::Geant4UserEventAction()
{
    runNumber = 100;
    drawNumber = 1000;
    steppingAction = nullptr;
    trackingAction = nullptr;
}

Geant4UserEventAction::~Geant4UserEventAction()
{

}

void Geant4UserEventAction::BeginOfEventAction(const G4Event *event)
{
    bool record = G4UniformRand() < drawNumber * 1.0 / runNumber;
    if(steppingAction)
    {
        steppingAction->setNeedRecord(record);
    }
    if(trackingAction)
    {
        trackingAction->setNeedRecord(record);
    }
}

void Geant4UserEventAction::EndOfEventAction(const G4Event *event)
{

}

Geant4UserTrackingAction *Geant4UserEventAction::getTrackingAction() const
{
    return trackingAction;
}

void Geant4UserEventAction::setTrackingAction(Geant4UserTrackingAction *newTrackingAction)
{
    trackingAction = newTrackingAction;
}

Geant4UserSteppingAction *Geant4UserEventAction::getSteppingAction() const
{
    return steppingAction;
}

void Geant4UserEventAction::setSteppingAction(Geant4UserSteppingAction *newSteppingAction)
{
    steppingAction = newSteppingAction;
}

int Geant4UserEventAction::getRunNumber() const
{
    return runNumber;
}

void Geant4UserEventAction::setRunNumber(int newRunNumber)
{
    runNumber = newRunNumber;
}

int Geant4UserEventAction::getDrawNumber() const
{
    return drawNumber;
}

void Geant4UserEventAction::setDrawNumber(int newDrawNumber)
{
    drawNumber = newDrawNumber;
}
