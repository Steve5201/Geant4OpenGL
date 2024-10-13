#include "geant4userprimarygeneratoraction.h"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

Geant4UserPrimaryGeneratorAction::Geant4UserPrimaryGeneratorAction()
{
    fParticleSource = new G4GeneralParticleSource();
    SetRadius(1);
    SetThickness(0.3);
    SetPosition(G4ThreeVector(0,0,0));
    SetRotation(G4ThreeVector(0,0,0));
    SetEnergy(100.0);
    SetParticleType("gamma");
}

Geant4UserPrimaryGeneratorAction::~Geant4UserPrimaryGeneratorAction()
{
    delete fParticleSource;
}

void Geant4UserPrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(-rotation.x() * deg);
    rot->rotateY(-rotation.y() * deg);
    rot->rotateZ(-rotation.z() * deg);
    G4ThreeVector dr = rot->inverse() * G4ThreeVector(0, 0, 1);
    G4double z = thickness * cm * (G4UniformRand() - 0.5);
    G4double angle = CLHEP::twopi * G4UniformRand();
    G4double x = radius * cm * std::cos(angle) * G4UniformRand();
    G4double y = radius * cm * std::sin(angle) * G4UniformRand();
    G4ThreeVector pos(x, y, z);
    pos = rot->inverse() * pos;
    fParticleSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos + position);
    fParticleSource->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(dr);
    fParticleSource->GeneratePrimaryVertex(event);
}

void Geant4UserPrimaryGeneratorAction::SetRadius(G4double r)
{
    radius = r;
}

void Geant4UserPrimaryGeneratorAction::SetThickness(G4double t)
{
    thickness = t;
}

void Geant4UserPrimaryGeneratorAction::SetPosition(G4ThreeVector pos)
{
    position = pos;
}

void Geant4UserPrimaryGeneratorAction::SetRotation(G4ThreeVector rot)
{
    rotation = rot;
}

void Geant4UserPrimaryGeneratorAction::SetEnergy(G4double energy)
{
    fParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy * keV);
}

void Geant4UserPrimaryGeneratorAction::SetParticleType(const G4String &particleName)
{
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
    if (particle)
    {
        fParticleSource->GetCurrentSource()->SetParticleDefinition(particle);
    }
}
