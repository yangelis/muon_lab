#include "PrimaryGeneratorAction.hh"

#include <G4Box.hh>
#include <G4Exception.hh>
#include <G4GeneralParticleSource.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4ios.hh>

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr)
{
  G4int nParticles = 1;
  fParticleGun     = new G4GeneralParticleSource();

  // default particle kinematic
  G4String particleName;
  auto particleDefinition =
      G4ParticleTable::GetParticleTable()->FindParticle(particleName = "mu-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetNumberOfParticles(nParticles);
  // TODO(#4): Energy should come from the energy distribution of cosmic ray muons
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // this function is called at the begining of each event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.

  G4double worldZHalfLenght = 0.0;
  auto* worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

  // Check th world volume shape
  G4Box* worldBox = nullptr;
  if (worldLV) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }

  if (worldBox) {
    worldZHalfLenght = worldBox->GetZHalfLength();
  } else {
    G4ExceptionDescription msg;
    msg << "World volume is not a box shape" << G4endl;
    msg << "Geometry has changed" << G4endl;
    msg << "The gun will be placed in the center";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002",
                JustWarning, msg);
  }


  // set gun's position
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -worldZHalfLenght));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
