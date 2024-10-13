#ifndef GEANT4USEREVENTACTION_H
#define GEANT4USEREVENTACTION_H

#include <QMap>
#include <QVector>
#include <QVector3D>
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

class Geant4UserEventAction : public G4UserEventAction
{
public:
    Geant4UserEventAction();
    ~Geant4UserEventAction() override;

    // 在事件开始时调用
    virtual void BeginOfEventAction(const G4Event* event) override;

    // 在事件结束时调用
    virtual void EndOfEventAction(const G4Event* event) override;

private:

};

#endif // GEANT4USEREVENTACTION_H
