#ifndef GEANT4USERRUNACTION_H
#define GEANT4USERRUNACTION_H

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "geant4usereventaction.h"
#include "geant4usertrackingaction.h"
#include "geant4usersteppingaction.h"
#include "geant4userprimarygeneratoraction.h"

class Geant4UserRunAction : public G4UserRunAction
{
public:
    Geant4UserRunAction();
    ~Geant4UserRunAction() override;

    // 运行开始时调用
    void BeginOfRunAction(const G4Run* run) override;

    // 运行结束时调用
    void EndOfRunAction(const G4Run* run) override;

    Geant4UserEventAction *getEventAction() const;
    void setEventAction(Geant4UserEventAction *newEventAction);

    Geant4UserTrackingAction *getTrackingAction() const;
    void setTrackingAction(Geant4UserTrackingAction *newTrackingAction);

    Geant4UserSteppingAction *getSteppingAction() const;
    void setSteppingAction(Geant4UserSteppingAction *newSteppingAction);

    Geant4UserPrimaryGeneratorAction *getPrimaryGeneratorAction() const;
    void setPrimaryGeneratorAction(Geant4UserPrimaryGeneratorAction *newPrimaryGeneratorAction);

    void clearTraks();
    void addTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> tks);
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> getTraks() const;
    void setTraks(const QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> &newTraks);

    void clearWorkRun();
    void addWorkRun(Geant4UserRunAction *run);
    QVector<Geant4UserRunAction *> getWorkRuns() const;
    void setWorkRuns(const QVector<Geant4UserRunAction *> &newWorkRuns);

    Geant4UserRunAction *getMaster() const;
    void setMaster(Geant4UserRunAction *newMaster);

    int getRunNumber() const;
    void setRunNumber(int newRunNumber);

    int getDrawNumber() const;
    void setDrawNumber(int newDrawNumber);

private:
    int runNumber;
    int drawNumber;
    Geant4UserEventAction *eventAction;
    Geant4UserTrackingAction *trackingAction;
    Geant4UserSteppingAction *steppingAction;
    Geant4UserPrimaryGeneratorAction *primaryGeneratorAction;
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> traks;
    Geant4UserRunAction *master;
    QVector<Geant4UserRunAction*> workRuns;

};

#endif // GEANT4USERRUNACTION_H
