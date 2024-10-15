#include "geant4detector.h"
#include "Randomize.hh"
#include <mutex>
#include <QDebug>

std::mutex dataMutex;

Geant4Detector::Geant4Detector(const G4String &name)
    : G4VSensitiveDetector(name)
{
    resolution = 0.05;
    resKeV = 1;
    collectionName.insert("Geant4HitsCollection");
    logicalVolume = nullptr;
}

Geant4Detector::~Geant4Detector()
{
    // 释放资源
}

void Geant4Detector::clearData()
{
    data.clear();
    trackData.clear();
}

void Geant4Detector::Initialize(G4HCofThisEvent *hce)
{
    // 用户自定义初始化操作
    hitCollection = new G4THitsCollection<Geant4Hit>(SensitiveDetectorName, collectionName[0]);
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(hitCollection);
    hce->AddHitsCollection(hcID, hitCollection);
    trackE.clear();
}

G4bool Geant4Detector::ProcessHits(G4Step *step, G4TouchableHistory *history)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.0)
    {
        return false;
    }

    // 创建新击中并设置信息
    Geant4Hit* hit = new Geant4Hit();
    G4double energy = GaussianSmearing(edep, resolution);
    hit->SetEnergyDep(energy);
    hit->SetPosition(step->GetPreStepPoint()->GetPosition());
    hit->SetTime(step->GetPreStepPoint()->GetGlobalTime());

    int tid = step->GetTrack()->GetTrackID();
    if(trackE.contains(tid))
    {
        trackE[tid] += energy;
    }
    else
    {
        trackE[tid] = energy;
    }

    // 将击中添加到Hits Collection
    hitCollection->insert(hit);
    return true;
}

void Geant4Detector::EndOfEvent(G4HCofThisEvent *hitsCollection)
{
    // 处理事件结束时的所有击中数据
    G4int numHits = hitCollection->entries();
    G4double energyDeposition = 0.0;

    // 遍历所有击中，提取能量沉积
    for (G4int i = 0; i < numHits; ++i)
    {
        Geant4Hit* hit = (*hitCollection)[i];
        energyDeposition += hit->GetEnergyDep();
    }

    std::lock_guard<std::mutex> lock(dataMutex);

    int e = round(energyDeposition / CLHEP::keV * resKeV);
    if(data.size() < e + 1)
    {
        data.resize(e + 1);
    }
    data[e]++;

    foreach (auto tracke, trackE)
    {
        int etk = round(tracke / CLHEP::keV * resKeV);
        if(trackData.size() < etk + 1)
        {
            trackData.resize(etk + 1);
        }
        trackData[etk]++;
    }
    trackE.clear();
}

G4double Geant4Detector::getResolution() const
{
    return resolution;
}

void Geant4Detector::setResolution(G4double newResolution)
{
    resolution = newResolution;
}

QVector<double> Geant4Detector::getData() const
{
    return data;
}

void Geant4Detector::setData(const QVector<double> &newData)
{
    data = newData;
}

QVector<double> Geant4Detector::getTrackData() const
{
    return trackData;
}

void Geant4Detector::setTrackData(const QVector<double> &newTrackData)
{
    trackData = newTrackData;
}

G4LogicalVolume *Geant4Detector::getLogicalVolume() const
{
    return logicalVolume;
}

void Geant4Detector::setLogicalVolume(G4LogicalVolume *newLogicalVolume)
{
    logicalVolume = newLogicalVolume;
}

int Geant4Detector::getResKeV() const
{
    return resKeV;
}

void Geant4Detector::setResKeV(int newResKeV)
{
    resKeV = newResKeV;
}

G4double Geant4Detector::GaussianSmearing(G4double energy, G4double resolution)
{
    G4double sigma = energy * resolution / 2.35;
    G4double smearedEnergy = G4RandGauss::shoot(energy, sigma);
    return smearedEnergy;
}
