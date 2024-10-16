#ifndef GEANT4MANAGER_H
#define GEANT4MANAGER_H

#include <QStringList>
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "geant4physicslist.h"
#include "geant4detectorconstruction.h"
#include "geant4actioninitialization.h"

class Geant4Manager
{
public:
    Geant4Manager();
    ~Geant4Manager();

    void initialize(int nthread = 1, bool highEM = false);
    void initialize(int argc, char *argv[], int nthread = 1, bool highEM = false);
    void clearGeometry();

    G4UImanager *getCmdManager() const;

    G4RunManager *getRunManager() const;

    G4UIExecutive *getUiExecutive() const;

    G4VisExecutive *getVisManager() const;

    Geant4PhysicsList *getPhysicsList() const;

    Geant4DetectorConstruction *getDetectorConstruction() const;

    Geant4ActionInitialization *getActionInitialization() const;

    QStringList getMaterialNames() const;

    QStringList getParticleNames() const;

private:
    G4UImanager *cmdManager;
    G4RunManager *runManager;
    G4UIExecutive *uiExecutive;
    G4VisExecutive *visManager;
    Geant4PhysicsList *physicsList;
    Geant4DetectorConstruction *detectorConstruction;
    Geant4ActionInitialization *actionInitialization;
    QStringList materialNames;
    QStringList particleNames;

};

#endif // GEANT4MANAGER_H
