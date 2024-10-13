#include "geant4usersteppingaction.h"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include <QDebug>

Geant4UserSteppingAction::Geant4UserSteppingAction()
{
    traks = nullptr;
}

Geant4UserSteppingAction::~Geant4UserSteppingAction() {}

void Geant4UserSteppingAction::UserSteppingAction(const G4Step *step)
{
    if(traks)
    {
        const G4Track *track = step->GetTrack();
        std::string name = track->GetDefinition()->GetParticleName();
        G4ThreeVector position = track->GetPosition();
        int tid = track->GetTrackID();
        const G4Event* event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
        int eid = 0;
        if(event)
        {
            eid = event->GetEventID();
        }

        QVector3D pos(position.x() / cm, position.y() / cm, position.z() / cm);
        (*traks)[name][eid][tid].append(pos);
    }
}

void Geant4UserSteppingAction::setTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *newTraks)
{
    traks = newTraks;
}
