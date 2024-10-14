#ifndef GEANT4USERTRACKINGACTION_H
#define GEANT4USERTRACKINGACTION_H

#include <QMap>
#include <QVector>
#include <QVector3D>
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"

class Geant4UserTrackingAction : public G4UserTrackingAction
{
public:
    Geant4UserTrackingAction();
    ~Geant4UserTrackingAction() override;

    // 粒子跟踪开始时调用
    void PreUserTrackingAction(const G4Track* track) override;

    // 粒子跟踪结束时调用
    void PostUserTrackingAction(const G4Track* track) override;

    void setTraks(QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *newTraks);

    bool getNeedRecord() const;
    void setNeedRecord(bool newNeedRecord);

private:
    bool needRecord;
    QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> *traks;

};

#endif // GEANT4USERTRACKINGACTION_H
