#include "geant4detectorconstruction.h"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include <QDebug>

Geant4DetectorConstruction::Geant4DetectorConstruction()
{
    merged = false;
    // 获取材料管理器
    G4NistManager* nist = G4NistManager::Instance();

    // 定义世界体积
    G4double world_sizeXYZ = 100 * m;
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

    G4Box* solidWorld = new G4Box("World", world_sizeXYZ / 2, world_sizeXYZ / 2, world_sizeXYZ / 2);

    worldLogicalVolume = new G4LogicalVolume(solidWorld, world_mat, "World");

    worldPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(), worldLogicalVolume, "World", 0, false, 0, false);
}

Geant4DetectorConstruction::~Geant4DetectorConstruction() {}

G4VPhysicalVolume *Geant4DetectorConstruction::Construct()
{
    return worldPhysicalVolume;
}

void Geant4DetectorConstruction::ConstructSDandField()
{
    G4Mutex mutex = G4MUTEX_INITIALIZER;
    G4AutoLock lock(&mutex);
    if(G4Threading::IsMasterThread())
    {
        merged = false;
        worksDetectorMap.clear();
        detectorDataMap.clear();
    }
    auto it = detectorNames.begin();
    while (it != detectorNames.end())
    {
        Geant4Detector *detector = nullptr;
        auto *sensitiveDetector = G4SDManager::GetSDMpointer()->FindSensitiveDetector(it.value());
        if(!sensitiveDetector)
        {
            detector = new Geant4Detector(it.value());
            G4SDManager::GetSDMpointer()->AddNewDetector(detector);
        }
        else
        {
            detector = dynamic_cast<Geant4Detector*>(sensitiveDetector);
            if(detector)
            {
                detector->clearData();
            }
        }
        SetSensitiveDetector(it.key(), detector);
        if(!G4Threading::IsMasterThread())
        {
            worksDetectorMap[G4Threading::G4GetThreadId()].insert(it.value(), detector);
        }
        it++;
    }
}

G4LogicalVolume *Geant4DetectorConstruction::getWorldLogicalVolume() const
{
    return worldLogicalVolume;
}

G4VPhysicalVolume *Geant4DetectorConstruction::getWorldPhysicalVolume() const
{
    return worldPhysicalVolume;
}

void Geant4DetectorConstruction::setWorldPhysicalVolume(G4VPhysicalVolume *newWorldPhysicalVolume)
{
    worldPhysicalVolume = newWorldPhysicalVolume;
}

QMap<std::string, QVector<double>> Geant4DetectorConstruction::getDetectorMap()
{
    if(!merged)
    {
        foreach (auto map, worksDetectorMap)
        {
            foreach (auto key, map.keys())
            {
                QVector<double> &data = detectorDataMap[key];
                QVector<double> mdata = map[key]->getData();
                for (int i = 0; i < mdata.size(); ++i)
                {
                    if(data.size() < i + 1)
                    {
                        data.resize(i + 1);
                    }
                    data[i] += mdata[i];
                }
            }
        }
        merged = true;
    }
    return detectorDataMap;
}

void Geant4DetectorConstruction::setDetectorMap(const QMap<std::string, QVector<double>> &newDetectorMap)
{
    detectorDataMap = newDetectorMap;
}

QHash<G4LogicalVolume *, std::string> Geant4DetectorConstruction::getDetectorNames() const
{
    return detectorNames;
}

void Geant4DetectorConstruction::setDetectorNames(const QHash<G4LogicalVolume *, std::string> &newDetectorNames)
{
    detectorNames = newDetectorNames;
}

bool Geant4DetectorConstruction::getMerged() const
{
    return merged;
}

void Geant4DetectorConstruction::setMerged(bool newMerged)
{
    merged = newMerged;
}
