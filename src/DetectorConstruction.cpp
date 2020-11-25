#include "DetectorConstruction.hh"

// G4 includes
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4MultiFunctionalDetector.hh>
#include <G4NistManager.hh>
#include <G4PSEnergyDeposit.hh>
#include <G4PSNofSecondary.hh>
#include <G4PVPlacement.hh>
#include <G4PhysicalConstants.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4SDParticleFilter.hh>
#include <G4SystemOfUnits.hh>
#include <G4VPrimitiveScorer.hh>
#include <G4VisAttributes.hh>

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(), fCheckOverlaps(true) {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  // Define the materials
  DefineMaterials();

  // Define volumes and return the physical volume
  return DefineVolumes();
}

void DetectorConstruction::DefineMaterials() {
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Fe");

  // Envelope parameters
  G4double env_sizeXY = 20 * cm, env_sizeZ = 50 * cm;
  // World
  G4double worldSizeXY = 1.2 * env_sizeXY;
  G4double worldSizeZ = 1.2 * env_sizeZ;
  G4double scintX = 16 * cm;
  G4double scintY = 11 * cm;
  G4double scintZ = 2 * cm;

  if (!defaultMaterial || !absorberMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()", "MyCode0001",
                FatalException, msg);
  }

  //
  // World
  //
  auto worldS =
      new G4Box("World",                                           // its name
                worldSizeXY / 2, worldSizeXY / 2, worldSizeZ / 2); // its size

  auto worldLV = new G4LogicalVolume(worldS,          // its solid
                                     defaultMaterial, // its material
                                     "World");        // its name

  auto worldPV = new G4PVPlacement(0,               // no rotation
                                   G4ThreeVector(), // at (0,0,0)
                                   worldLV,         // its logical volume
                                   "World",         // its name
                                   0,               // its mother  volume
                                   false,           // no boolean operation
                                   0,               // copy number
                                   fCheckOverlaps); // checking overlaps

  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto scintillatorSolid =
      new G4Box("scint", scintX / 2, scintY / 2, scintZ / 2);

  auto scintillatorLV =
      new G4LogicalVolume(scintillatorSolid, absorberMaterial, "scintLV");
  auto scintillatorPV =
      new G4PVPlacement(0, G4ThreeVector(), scintillatorLV, "scintPV", worldLV,
                        false, 0, fCheckOverlaps);

  //
  // Always return the physical World
  //
  return worldPV;
}

void DetectorConstruction::ConstructSDandField() {
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  auto absDetector = new G4MultiFunctionalDetector("Scintillator");
  G4SDManager::GetSDMpointer()->AddNewDetector(absDetector);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("Edep");
  absDetector->RegisterPrimitive(primitive);

  SetSensitiveDetector("scintLV", absDetector);
}
