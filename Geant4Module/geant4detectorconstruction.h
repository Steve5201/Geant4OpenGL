#ifndef GEANT4DETECTORCONSTRUCTION_H
#define GEANT4DETECTORCONSTRUCTION_H

#include <QHash>
#include <QMap>
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "geant4detector.h"

class Geant4DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    Geant4DetectorConstruction();
    ~Geant4DetectorConstruction() override;

    // 构建探测器几何结构
    G4VPhysicalVolume* Construct() override;

    virtual void ConstructSDandField() override;

    G4LogicalVolume *getWorldLogicalVolume() const;

    G4VPhysicalVolume *getWorldPhysicalVolume() const;

    void setWorldPhysicalVolume(G4VPhysicalVolume *newWorldPhysicalVolume);

    QMap<std::string, QVector<double>> getDetectorMap();
    void setDetectorMap(const QMap<std::string, QVector<double>> &newDetectorMap);

    QHash<G4LogicalVolume *, std::string> getDetectorNames() const;
    void setDetectorNames(const QHash<G4LogicalVolume *, std::string> &newDetectorNames);

    bool getMerged() const;
    void setMerged(bool newMerged);

    bool getUseEventData() const;
    void setUseEventData(bool newUseEventData);

    QMap<std::string, QVector<double> > getDetectorTrackDataMap() const;
    void setDetectorTrackDataMap(const QMap<std::string, QVector<double> > &newDetectorTrackDataMap);

    int getResKeV() const;
    void setResKeV(int newResKeV);

private:
    int resKeV;
    bool merged;
    bool useEventData;
    G4LogicalVolume* worldLogicalVolume;
    G4VPhysicalVolume* worldPhysicalVolume;
    QMap<std::string, QVector<double>> detectorDataMap;
    QMap<std::string, QVector<double>> detectorTrackDataMap;
    QMap<int,QMap<std::string, Geant4Detector*>> worksDetectorMap;
    QHash<G4LogicalVolume*, std::string> detectorNames;

};

#endif // GEANT4DETECTORCONSTRUCTION_H
