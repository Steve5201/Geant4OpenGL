#include "geant4userrunaction.h"
#include <QDebug>

Geant4UserRunAction::Geant4UserRunAction()
{
    master = nullptr;
    eventAction = nullptr;
    trackingAction = nullptr;
    steppingAction = nullptr;
    primaryGeneratorAction = nullptr;
}

Geant4UserRunAction::~Geant4UserRunAction() {}

void Geant4UserRunAction::BeginOfRunAction(const G4Run *run)
{
    G4Mutex mutex = G4MUTEX_INITIALIZER;
    G4AutoLock lock(&mutex);
    if(isMaster)
    {
        clearTraks();
        clearWorkRun();
    }
    else
    {
        clearTraks();
        if(trackingAction)
        {
            trackingAction->setTraks(&traks);
        }
        if(steppingAction)
        {
            steppingAction->setTraks(&traks);
        }
    }
}

void Geant4UserRunAction::EndOfRunAction(const G4Run *run)
{
    G4Mutex mutex = G4MUTEX_INITIALIZER;
    G4AutoLock lock(&mutex);
    if(isMaster)
    {
        foreach (auto run, workRuns)
        {
            addTraks(run->getTraks());
        }
    }
    else
    {
        if(master)
        {
            master->addWorkRun(this);
        }
    }
}

Geant4UserEventAction *Geant4UserRunAction::getEventAction() const
{
    return eventAction;
}

void Geant4UserRunAction::setEventAction(Geant4UserEventAction *newEventAction)
{
    eventAction = newEventAction;
}

Geant4UserTrackingAction *Geant4UserRunAction::getTrackingAction() const
{
    return trackingAction;
}

void Geant4UserRunAction::setTrackingAction(Geant4UserTrackingAction *newTrackingAction)
{
    trackingAction = newTrackingAction;
}

Geant4UserSteppingAction *Geant4UserRunAction::getSteppingAction() const
{
    return steppingAction;
}

void Geant4UserRunAction::setSteppingAction(Geant4UserSteppingAction *newSteppingAction)
{
    steppingAction = newSteppingAction;
}

Geant4UserPrimaryGeneratorAction *Geant4UserRunAction::getPrimaryGeneratorAction() const
{
    return primaryGeneratorAction;
}

void Geant4UserRunAction::setPrimaryGeneratorAction(Geant4UserPrimaryGeneratorAction *newPrimaryGeneratorAction)
{
    primaryGeneratorAction = newPrimaryGeneratorAction;
}

void Geant4UserRunAction::clearTraks()
{
    traks.clear();
}

void Geant4UserRunAction::addTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D> > > > tks)
{
    foreach (auto k, tks.keys())
    {
        foreach (auto k2, tks[k].keys())
        {
            foreach (auto k3, tks[k][k2].keys())
            {
                traks[k][k2][k3].append(tks[k][k2][k3]);
            }
        }
    }
}

QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> Geant4UserRunAction::getTraks() const
{
    return traks;
}

void Geant4UserRunAction::setTraks(const QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> &newTraks)
{
    traks = newTraks;
}

void Geant4UserRunAction::clearWorkRun()
{
    workRuns.clear();
}

void Geant4UserRunAction::addWorkRun(Geant4UserRunAction *run)
{
    workRuns.append(run);
}

QVector<Geant4UserRunAction *> Geant4UserRunAction::getWorkRuns() const
{
    return workRuns;
}

void Geant4UserRunAction::setWorkRuns(const QVector<Geant4UserRunAction *> &newWorkRuns)
{
    workRuns = newWorkRuns;
}

Geant4UserRunAction *Geant4UserRunAction::getMaster() const
{
    return master;
}

void Geant4UserRunAction::setMaster(Geant4UserRunAction *newMaster)
{
    master = newMaster;
}
