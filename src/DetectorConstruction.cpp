#include "DetectorConstruction.hh"

// G4 includes
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4MultiFunctionalDetector.hh>
#include <G4NistManager.hh>
#include <G4OpticalPhoton.hh>
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
    : G4VUserDetectorConstruction(), fCheckOverlaps(true) {
  fsolidWorld = nullptr;
  flogicWorld = nullptr;
  fphysWorld  = nullptr;

  fsolidScintillator      = nullptr;
  flogicScintillator0     = nullptr;
  fphysScintillator0      = nullptr;
  flogicScintillator2     = nullptr;
  fphysScintillator2      = nullptr;
  fsolidSmallScintillator = nullptr;
  flogicScintillator1     = nullptr;
  fphysScintillator1      = nullptr;
  fsolidAbsorber          = nullptr;
  flogicAbsorber          = nullptr;
  fphysAbsorber           = nullptr;

  fDetectorMessenger = new DetectorMessenger(this);
  fscintX            = 16 * cm;
}

DetectorConstruction::~DetectorConstruction() { delete fDetectorMessenger; }

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
  fScintMaterial =
      nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  constexpr std::size_t N           = 4;
  std::array<double, N> wls_Energy  = {2.00 * eV, 2.87 * eV, 2.90 * eV,
                                      3.47 * eV};
  std::array<double, N> rIndex      = {1.58, 1.58, 1.58, 1.58};
  std::array<double, N> absorption1 = {2. * cm, 2. * cm, 2. * cm, 2. * cm};
  std::array<double, N> scintilFast = {0.00, 0.00, 1.00, 1.00};

  auto* fMPTPscint = new G4MaterialPropertiesTable();
  fMPTPscint->AddProperty("RINDEX", wls_Energy.data(), rIndex.data(), N);
  fMPTPscint->AddProperty("ABSLENGTH", wls_Energy.data(), absorption1.data(),
                          N);
  fMPTPscint->AddProperty("FASTCOMPONENT", wls_Energy.data(),
                          scintilFast.data(), N);
  fMPTPscint->AddConstProperty("SCINTILLATIONYIELD", 10. / keV);
  fMPTPscint->AddConstProperty("RESOLUTIONSCALE", 1.0);
  fMPTPscint->AddConstProperty("FASTTIMECONSTANT", 10. * ns);
  fScintMaterial->SetMaterialPropertiesTable(fMPTPscint);

  // Set the Birks Constant for the Polystyrene scintillator

  fScintMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Fe");

  // Envelope parameters
  G4double env_sizeXY = 20 * cm, env_sizeZ = 50 * cm;
  // World
  G4double worldSizeXY = 1.2 * env_sizeXY;
  G4double worldSizeZ  = 1.2 * env_sizeZ;
  // G4double fscintX = 16 * cm;
  G4double scintY = 11 * cm;
  G4double scintZ = 2 * cm;

  if (!defaultMaterial || !absorberMaterial || !fScintMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()", "MyCode0001",
                FatalException, msg);
  }

  //
  // World
  //
  fsolidWorld =
      new G4Box("World",                                           // its name
                worldSizeXY / 2, worldSizeXY / 2, worldSizeZ / 2); // its size

  flogicWorld = new G4LogicalVolume(fsolidWorld,     // its solid
                                    defaultMaterial, // its material
                                    "World");        // its name

  fphysWorld = new G4PVPlacement(0,               // no rotation
                                 G4ThreeVector(), // at (0,0,0)
                                 flogicWorld,     // its logical volume
                                 "World",         // its name
                                 0,               // its mother  volume
                                 false,           // no boolean operation
                                 0,               // copy number
                                 fCheckOverlaps); // checking overlaps

  flogicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  // The two same scintillators, the top and bottom one
  fsolidScintillator = new G4Box("scint", fscintX / 2, scintY / 2, scintZ / 2);

  flogicScintillator0 =
      new G4LogicalVolume(fsolidScintillator, fScintMaterial, "scintLV0");
  fphysScintillator0 =
      new G4PVPlacement(nullptr, G4ThreeVector(), flogicScintillator0,
                        "scintPV0", flogicWorld, false, 0, fCheckOverlaps);

  flogicScintillator2 =
      new G4LogicalVolume(fsolidScintillator, fScintMaterial, "scintLV2");
  fphysScintillator2 = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 100.),
                                         flogicScintillator2, "scintPV2",
                                         flogicWorld, false, 0, fCheckOverlaps);

  // scintillator in the middle
  fsolidSmallScintillator =
      new G4Box("smallscint", fscintX / 2, scintY / 6, scintZ);
  flogicScintillator1 =
      new G4LogicalVolume(fsolidSmallScintillator, fScintMaterial, "scintLV1");
  fphysScintillator1 = new G4PVPlacement(
      nullptr, G4ThreeVector(0., 0., 3. * scintZ / 2.), flogicScintillator1,
      "scintPV1", flogicWorld, false, 0, fCheckOverlaps);

  // iron plate where the scint0 is positioned
  fsolidAbsorber = new G4Box("absorber", fscintX / 2, scintY / 2, scintZ / 2);

  flogicAbsorber =
      new G4LogicalVolume(fsolidAbsorber, absorberMaterial, "absorbeLV");
  fphysAbsorber =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 100 - scintZ), flogicAbsorber,
                        "absorberPV", flogicWorld, false, 0, fCheckOverlaps);

  auto red    = new G4VisAttributes(G4Colour::Red());
  auto yellow = new G4VisAttributes(G4Colour::Yellow());
  auto cyan   = new G4VisAttributes(G4Colour::Cyan());

  red->SetVisibility(true);
  yellow->SetVisibility(true);
  cyan->SetVisibility(true);

  // red->SetForceSolid();
  // yellow->SetForceSolid();
  // cyan->SetForceSolid();

  flogicScintillator0->SetVisAttributes(cyan);
  flogicScintillator1->SetVisAttributes(yellow);
  flogicScintillator2->SetVisAttributes(cyan);
  flogicAbsorber->SetVisAttributes(red);

  // Always return the physical World
  return fphysWorld;
}

void DetectorConstruction::ConstructSDandField() {
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  G4VPrimitiveScorer* primitive;
  primitive = new G4PSEnergyDeposit("Edep");
  if (!fscint0MFDet.Get()) {
    auto scint0Detector = new G4MultiFunctionalDetector("Scintillator0");
    scint0Detector->RegisterPrimitive(primitive);
    fscint0MFDet.Put(scint0Detector);

    primitive         = new G4PSNofSecondary("nGamma");
    auto* gammaFilter = new G4SDParticleFilter(
        "gammaFilter",
        G4OpticalPhoton::OpticalPhotonDefinition()->GetParticleName());
    primitive->SetFilter(gammaFilter);
    scint0Detector->RegisterPrimitive(primitive);
    fscint0MFDet.Put(scint0Detector);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscint0MFDet.Get());
  SetSensitiveDetector(flogicScintillator0, fscint0MFDet.Get());

  primitive = new G4PSEnergyDeposit("Edep");
  if (!fscint1MFDet.Get()) {
    auto scint1Detector = new G4MultiFunctionalDetector("Scintillator1");
    scint1Detector->RegisterPrimitive(primitive);
    fscint1MFDet.Put(scint1Detector);

    primitive         = new G4PSNofSecondary("nGamma");
    auto* gammaFilter = new G4SDParticleFilter(
        "gammaFilter",
        G4OpticalPhoton::OpticalPhotonDefinition()->GetParticleName());
    primitive->SetFilter(gammaFilter);
    scint1Detector->RegisterPrimitive(primitive);
    fscint1MFDet.Put(scint1Detector);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscint1MFDet.Get());
  SetSensitiveDetector(flogicScintillator1, fscint1MFDet.Get());

  primitive = new G4PSEnergyDeposit("Edep");
  if (!fscint2MFDet.Get()) {
    auto scint2Detector = new G4MultiFunctionalDetector("Scintillator2");
    scint2Detector->RegisterPrimitive(primitive);
    fscint2MFDet.Put(scint2Detector);

    primitive         = new G4PSNofSecondary("nGamma");
    auto* gammaFilter = new G4SDParticleFilter(
        "gammaFilter",
        G4OpticalPhoton::OpticalPhotonDefinition()->GetParticleName());
    primitive->SetFilter(gammaFilter);
    scint2Detector->RegisterPrimitive(primitive);
    fscint2MFDet.Put(scint2Detector);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscint2MFDet.Get());
  SetSensitiveDetector(flogicScintillator2, fscint2MFDet.Get());

  if (!fscintSD.Get()) {
    auto scintSD = new ScintillatorSD("scintillators");
    fscintSD.Put(scintSD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscintSD.Get());
  SetSensitiveDetector(flogicScintillator0, fscintSD.Get());
  SetSensitiveDetector(flogicScintillator1, fscintSD.Get());
  SetSensitiveDetector(flogicScintillator2, fscintSD.Get());

}

void DetectorConstruction::SetWidth0X(G4double myWidhtX) {
  fscintX = myWidhtX;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
