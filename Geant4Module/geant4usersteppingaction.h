#ifndef GEANT4USERSTEPPINGACTION_H
#define GEANT4USERSTEPPINGACTION_H

#include <QMap>
#include <QVector>
#include <QVector3D>
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class Geant4UserSteppingAction : public G4UserSteppingAction
{
public:
    Geant4UserSteppingAction();
    ~Geant4UserSteppingAction() override;

    // 每一步时调用
    void UserSteppingAction(const G4Step* step) override;

    void setTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *newTraks);

    bool getNeedRecord() const;
    void setNeedRecord(bool newNeedRecord);

private:
    bool needRecord;
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *traks;

};

#endif // GEANT4USERSTEPPINGACTION_H
