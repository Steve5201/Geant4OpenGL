#ifndef GEANT4DETECTOR_H
#define GEANT4DETECTOR_H

#include <QHash>
#include <QVector>
#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4THitsCollection.hh"
#include "geant4hit.h"

class Geant4Detector : public G4VSensitiveDetector
{
public:
    Geant4Detector(const G4String& name);

    ~Geant4Detector() override;

    void clearData();

    // 每次事件开始时调用，初始化探测器的读数容器
    void Initialize(G4HCofThisEvent* hce) override;

    // 探测器响应，当粒子经过探测器时触发
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // 每个事件结束时调用，处理收集到的探测数据
    void EndOfEvent(G4HCofThisEvent* hitsCollection) override;

    G4double getResolution() const;
    void setResolution(G4double newResolution);

    QVector<double> getData() const;
    void setData(const QVector<double> &newData);

    QVector<double> getTrackData() const;
    void setTrackData(const QVector<double> &newTrackData);

    G4LogicalVolume *getLogicalVolume() const;
    void setLogicalVolume(G4LogicalVolume *newLogicalVolume);

    int getResKeV() const;
    void setResKeV(int newResKeV);

private:
    int resKeV;
    G4double resolution;                  // 探测器分辨率
    G4THitsCollection<Geant4Hit>* hitCollection; // 存储击中对象的集合

    QVector<double> data;
    QHash<int, double> trackE;
    QVector<double> trackData;

    G4LogicalVolume *logicalVolume;

    G4double GaussianSmearing(G4double energy, G4double resolution);

};

#endif // GEANT4DETECTOR_H
