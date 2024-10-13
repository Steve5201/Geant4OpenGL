#include "geant4usertrackingaction.h"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include <QDebug>

Geant4UserTrackingAction::Geant4UserTrackingAction()
{
    traks = nullptr;
}

Geant4UserTrackingAction::~Geant4UserTrackingAction() {}

void Geant4UserTrackingAction::PreUserTrackingAction(const G4Track *track)
{
    if(traks)
    {
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

void Geant4UserTrackingAction::PostUserTrackingAction(const G4Track *track)
{
    if(traks)
    {
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

void Geant4UserTrackingAction::setTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *newTraks)
{
    traks = newTraks;
}
