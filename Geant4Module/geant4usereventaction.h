#ifndef GEANT4USEREVENTACTION_H
#define GEANT4USEREVENTACTION_H

#include <QMap>
#include <QVector>
#include <QVector3D>
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "geant4usertrackingaction.h"
#include "geant4usersteppingaction.h"

class Geant4UserEventAction : public G4UserEventAction
{
public:
    Geant4UserEventAction();
    ~Geant4UserEventAction() override;

    // 在事件开始时调用
    virtual void BeginOfEventAction(const G4Event* event) override;

    // 在事件结束时调用
    virtual void EndOfEventAction(const G4Event* event) override;

    Geant4UserTrackingAction *getTrackingAction() const;
    void setTrackingAction(Geant4UserTrackingAction *newTrackingAction);

    Geant4UserSteppingAction *getSteppingAction() const;
    void setSteppingAction(Geant4UserSteppingAction *newSteppingAction);

    int getRunNumber() const;
    void setRunNumber(int newRunNumber);

    int getDrawNumber() const;
    void setDrawNumber(int newDrawNumber);

private:
    int runNumber;
    int drawNumber;
    Geant4UserTrackingAction *trackingAction;
    Geant4UserSteppingAction *steppingAction;

};

#endif // GEANT4USEREVENTACTION_H
