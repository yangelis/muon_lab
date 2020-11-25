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
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Fe");
  auto scintillatorMaterial =
      G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // Envelope parameters
  G4double env_sizeXY = 20 * cm, env_sizeZ = 50 * cm;
  // World
  G4double worldSizeXY = 1.2 * env_sizeXY;
  G4double worldSizeZ = 1.2 * env_sizeZ;
  G4double scintX = 16 * cm;
  G4double scintY = 11 * cm;
  G4double scintZ = 2 * cm;

  if (!defaultMaterial || !absorberMaterial || !scintillatorMaterial) {
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

  // The two same scintillators, top and bottom one
  auto scintillatorSolid =
      new G4Box("scint", scintX / 2, scintY / 2, scintZ / 2);

  auto scintillatorLV0 =
      new G4LogicalVolume(scintillatorSolid, scintillatorMaterial, "scintLV0");
  new G4PVPlacement(0, G4ThreeVector(), scintillatorLV0, "scintPV0", worldLV,
                    false, 0, fCheckOverlaps);

  auto scintillatorLV2 =
      new G4LogicalVolume(scintillatorSolid, scintillatorMaterial, "scintLV2");
  new G4PVPlacement(0, G4ThreeVector(0., 0., 70.), scintillatorLV2, "scintPV2",
                    worldLV, false, 0, fCheckOverlaps);

  auto scintillatorSmallSolid =
      new G4Box("smallscint", scintX / 2, scintY / 6, scintZ);

  auto scintillatorLV1 = new G4LogicalVolume(scintillatorSmallSolid,
                                             scintillatorMaterial, "scintLV1");
  new G4PVPlacement(0, G4ThreeVector(0., 0., 3. * scintZ / 2.), scintillatorLV1,
                    "scintPV1", worldLV, false, 0, fCheckOverlaps);

  auto red = new G4VisAttributes(G4Colour::Red());
  red->SetVisibility(true);

  auto yellow = new G4VisAttributes(G4Colour::Yellow());
  yellow->SetVisibility(true);

  auto cyan = new G4VisAttributes(G4Colour::Cyan());
  cyan->SetVisibility(true);

  scintillatorLV0->SetVisAttributes(red);
  scintillatorLV1->SetVisAttributes(yellow);
  scintillatorLV2->SetVisAttributes(cyan);

  // Always return the physical World
  return worldPV;
}

void DetectorConstruction::ConstructSDandField() {
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  auto scint0Detector = new G4MultiFunctionalDetector("Scintillator0");
  G4SDManager::GetSDMpointer()->AddNewDetector(scint0Detector);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("Edep");
  scint0Detector->RegisterPrimitive(primitive);
  SetSensitiveDetector("scintLV0", scint0Detector);

  auto scint1Detector = new G4MultiFunctionalDetector("Scintillator1");
  G4SDManager::GetSDMpointer()->AddNewDetector(scint1Detector);
  primitive = new G4PSEnergyDeposit("Edep");
  scint1Detector->RegisterPrimitive(primitive);
  SetSensitiveDetector("scintLV1", scint1Detector);

  auto scint2Detector = new G4MultiFunctionalDetector("Scintillator2");
  G4SDManager::GetSDMpointer()->AddNewDetector(scint2Detector);
  primitive = new G4PSEnergyDeposit("Edep");
  scint2Detector->RegisterPrimitive(primitive);
  SetSensitiveDetector("scintLV2", scint2Detector);
}
