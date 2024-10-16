#include "geant4manager.h"
#include <QDebug>
#include <QApplication>
#include "G4NistManager.hh"
#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"
#include "G4TransportationManager.hh"

Geant4Manager::Geant4Manager()
{
    uiExecutive = nullptr;
    visManager = nullptr;
    runManager = nullptr;
    physicsList = nullptr;
    cmdManager = G4UImanager::GetUIpointer();
    actionInitialization = nullptr;
    detectorConstruction = nullptr;

}

Geant4Manager::~Geant4Manager()
{
    delete runManager;
    delete visManager;
}

void Geant4Manager::initialize(int nthread, bool highEM)
{
    if(!runManager)
    {
        runManager = new G4MTRunManager();
    }
    runManager->SetNumberOfThreads(nthread);
    if(!detectorConstruction)
    {
        detectorConstruction = new Geant4DetectorConstruction();
    }
    runManager->SetUserInitialization(detectorConstruction);
    if(!physicsList)
    {
        physicsList = new Geant4PhysicsList(highEM);
    }
    runManager->SetUserInitialization(physicsList);
    if(!actionInitialization)
    {
        actionInitialization = new Geant4ActionInitialization();
    }
    runManager->SetUserInitialization(actionInitialization);
    runManager->Initialize();
    G4NistManager* nistManager = G4NistManager::Instance();
    if(nistManager)
    {
        materialNames.clear();
        std::vector<G4String> materials = nistManager->GetNistMaterialNames();
        for (const auto& materialName : materials)
        {
            materialNames.append(QString::fromStdString(materialName));
            materialNames.sort();
        }
    }
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();
    particleNames.clear();
    particleIterator->reset();
    while ((*particleIterator)())
    {
        G4ParticleDefinition* particle = particleIterator->value();
        particleNames.append(QString::fromStdString(particle->GetParticleName()));
        particleNames.sort();
    }
}

void Geant4Manager::initialize(int argc, char *argv[], int nthread, bool highEM)
{
    if(!uiExecutive)
    {
        uiExecutive = new G4UIExecutive(argc, argv, "Qt");
    }
    if(!visManager)
    {
        visManager = new G4VisExecutive();
    }
    if(runManager)
    {
        delete runManager;
    }
    runManager = new G4MTRunManager();
    runManager->SetNumberOfThreads(nthread);
    if(!detectorConstruction)
    {
        detectorConstruction = new Geant4DetectorConstruction();
    }
    runManager->SetUserInitialization(detectorConstruction);
    if(!physicsList)
    {
        physicsList = new Geant4PhysicsList(highEM);
    }
    runManager->SetUserInitialization(physicsList);
    if(!actionInitialization)
    {
        actionInitialization = new Geant4ActionInitialization();
    }
    runManager->SetUserInitialization(actionInitialization);
    runManager->Initialize();
    visManager->Initialize();
    cmdManager->ApplyCommand("/control/execute init_vis.mac");
    G4NistManager* nistManager = G4NistManager::Instance();
    if(nistManager)
    {
        materialNames.clear();
        std::vector<G4String> materials = nistManager->GetNistMaterialNames();
        for (const auto& materialName : materials)
        {
            materialNames.append(QString::fromStdString(materialName));
            materialNames.sort();
        }
    }
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();
    particleNames.clear();
    particleIterator->reset();
    while ((*particleIterator)())
    {
        G4ParticleDefinition* particle = particleIterator->value();
        particleNames.append(QString::fromStdString(particle->GetParticleName()));
        particleNames.sort();
    }
}

void Geant4Manager::clearGeometry()
{
    // 获取世界体
    G4VPhysicalVolume* worldVolume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();

    if (!worldVolume)
    {
        G4Exception("ClearNonWorldVolumes()", "Error", FatalException, "No world volume found.");
        return;
    }

    // 打开几何体，准备进行清理
    G4GeometryManager::GetInstance()->OpenGeometry();

    // 获取世界逻辑体
    G4LogicalVolume* worldLogical = worldVolume->GetLogicalVolume();

    if (!worldLogical)
    {
        G4GeometryManager::GetInstance()->CloseGeometry();
        qDebug() << "World logical volume not found.";
        G4Exception("ClearNonWorldVolumes()", "Error", FatalException, "World logical volume not found.");
        return;
    }

    // 清除世界体的子物理体
    while (worldLogical->GetNoDaughters() > 0)
    {
        // 删除子物理体
        worldLogical->RemoveDaughter(worldLogical->GetDaughter(0));
    }

    // 关闭几何体
    G4GeometryManager::GetInstance()->CloseGeometry();
}

G4UImanager *Geant4Manager::getCmdManager() const
{
    return cmdManager;
}

G4RunManager *Geant4Manager::getRunManager() const
{
    return runManager;
}

G4UIExecutive *Geant4Manager::getUiExecutive() const
{
    return uiExecutive;
}

G4VisExecutive *Geant4Manager::getVisManager() const
{
    return visManager;
}

Geant4PhysicsList *Geant4Manager::getPhysicsList() const
{
    return physicsList;
}

Geant4DetectorConstruction *Geant4Manager::getDetectorConstruction() const
{
    return detectorConstruction;
}

Geant4ActionInitialization *Geant4Manager::getActionInitialization() const
{
    return actionInitialization;
}

QStringList Geant4Manager::getMaterialNames() const
{
    return materialNames;
}

QStringList Geant4Manager::getParticleNames() const
{
    return particleNames;
}
