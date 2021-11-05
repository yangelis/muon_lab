#include "DetectorConstruction.hh"
#include "SiPM.hh"

// G4 includes
#include <G4Box.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalVolume.hh>
#include <G4MultiFunctionalDetector.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
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
  sipm                    = nullptr;

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
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  G4Material* LYSO = new G4Material("LYSO", 7.4 * g / cm3, 4);

  G4Element* eLu = nistManager->FindOrBuildElement("Lu"); //Lu
  G4Element* eSi = nistManager->FindOrBuildElement("Si"); //Si
  G4Element* eO  = nistManager->FindOrBuildElement("O");  //O
  G4Element* eY  = nistManager->FindOrBuildElement("Y");  //Y

  LYSO->AddElement(eLu, 71 * perCent);
  LYSO->AddElement(eSi, 7 * perCent);
  LYSO->AddElement(eO, 18 * perCent);
  LYSO->AddElement(eY, 4 * perCent);

  // Optical properties of LYSO
  const G4int nLYSO1                = 17;
  G4double FAST_Energy_LYSO[nLYSO1] = {
      1.0 * eV,  1.77 * eV, 2.07 * eV, 2.14 * eV, 2.21 * eV, 2.30 * eV,
      2.38 * eV, 2.48 * eV, 2.58 * eV, 2.70 * eV, 2.82 * eV, 2.88 * eV,
      2.95 * eV, 3.10 * eV, 3.26 * eV, 3.44 * eV, 4.08 * eV};
  G4double FAST_COMPONENT_LYSO[nLYSO1] = {0.00, 0.00, 0.05, 0.10, 0.15, 0.20,
                                          0.40, 0.60, 0.80, 1.40, 2.20, 2.20,
                                          2.00, 0.60, 0.00, 0.00, 0.00};

  const G4int nLYSO2                = 2;
  G4double RIND_Energy_LYSO[nLYSO2] = {1.0 * eV, 6.0 * eV};
  G4double RIND_INDEX_LYSO[nLYSO2]  = {1.82, 1.82};
  G4double ABS_Energy_LYSO[nLYSO2]  = {1.0 * eV, 6.0 * eV};
  G4double ABS_LENGTH_LYSO[nLYSO2]  = {5. * m, 5. * m};

  G4MaterialPropertiesTable* LYSO_mt = new G4MaterialPropertiesTable();
  LYSO_mt->AddProperty("FASTCOMPONENT", FAST_Energy_LYSO, FAST_COMPONENT_LYSO,
                       nLYSO1);
  LYSO_mt->AddProperty("RINDEX", RIND_Energy_LYSO, RIND_INDEX_LYSO, nLYSO2);
  LYSO_mt->AddProperty("ABSLENGTH", ABS_Energy_LYSO, ABS_LENGTH_LYSO, nLYSO2);
  LYSO_mt->AddConstProperty("SCINTILLATIONYIELD", 20. / keV);
  LYSO_mt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  LYSO_mt->AddConstProperty("FASTTIMECONSTANT", 40. * ns);
  LYSO_mt->AddConstProperty("YIELDRATIO", 1.0);
  LYSO_mt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.5 * ns);
  LYSO->SetMaterialPropertiesTable(LYSO_mt);
  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto absorberMaterial = G4Material::GetMaterial("G4_Fe");
  /* auto scintillatorMaterial = */
  /*     G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); */
  auto scintillatorMaterial = G4Material::GetMaterial("LYSO");

  // Envelope parameters
  G4double env_sizeXY = 20 * cm, env_sizeZ = 50 * cm;
  // World
  G4double worldSizeXY = 1.2 * env_sizeXY;
  G4double worldSizeZ  = 1.2 * env_sizeZ;
  // G4double fscintX = 16 * cm;
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
      new G4LogicalVolume(fsolidScintillator, scintillatorMaterial, "scintLV0");
  fphysScintillator0 =
      new G4PVPlacement(0, G4ThreeVector(), flogicScintillator0, "scintPV0",
                        flogicWorld, false, 0, fCheckOverlaps);

  flogicScintillator2 =
      new G4LogicalVolume(fsolidScintillator, scintillatorMaterial, "scintLV2");
  fphysScintillator2 =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 100.), flogicScintillator2,
                        "scintPV2", flogicWorld, false, 0, fCheckOverlaps);

  // scintillator in the middle
  fsolidSmallScintillator =
      new G4Box("smallscint", fscintX / 2, scintY / 6, scintZ);
  flogicScintillator1 = new G4LogicalVolume(fsolidSmallScintillator,
                                            scintillatorMaterial, "scintLV1");
  fphysScintillator1  = new G4PVPlacement(
      0, G4ThreeVector(0., 0., 3. * scintZ / 2.), flogicScintillator1,
      "scintPV1", flogicWorld, false, 0, fCheckOverlaps);

  // sipm stuff
  const G4double sipm_x =
      500. *
      um; ///total sipm thickness (400um coating (fixed in sipm class) + 100um silicon)

  /* sipm = new SiPM(0, G4ThreeVector(0, 0, 0), flogicWorld, false, 0, */
  sipm =
      new SiPM(0, G4ThreeVector(fscintX / 2., 0, 100.), flogicWorld, false, 0,
               false); ///build sipm
  /* sipm->SetDimensions(sipm_x, sipm_y, sipm_z); */
  sipm->SetDimensions(sipm_x, scintY, scintZ);
  /* sipm->SetTranslation( */
  /*     G4ThreeVector(fscintX / 2., 0, 100.)); ///move sipm to end of scintillator */

  // iron plate where the scint0 is positioned
  fsolidAbsorber = new G4Box("absorber", fscintX / 2, scintY / 2, scintZ / 2);

  flogicAbsorber =
      new G4LogicalVolume(fsolidAbsorber, absorberMaterial, "absorbeLV");
  fphysAbsorber =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 100 - scintZ), flogicAbsorber,
                        "absorberPV", flogicWorld, false, 0, fCheckOverlaps);

  // Build scintillator surface (Teflon wrapping)
  G4OpticalSurface* OpSurface = new G4OpticalSurface("scintillatorSurface");
  OpSurface->SetType(dielectric_dielectric);

  OpSurface->SetFinish(polishedfrontpainted);
  OpSurface->SetModel(unified);

  OpSurface->SetSigmaAlpha(1 * deg);

  const G4int nEntries            = 2;
  G4double PhotonEnergy[nEntries] = {1. * eV, 6. * eV};

  G4double RefractiveIndex[nEntries] = {1, 1};
  G4double SpecularLobe[nEntries]    = {1, 1};
  G4double SpecularSpike[nEntries]   = {0, 0};
  G4double Backscatter[nEntries]     = {0, 0};
  G4double Lambertian[nEntries]      = {0, 0};
  G4double Reflectivity[nEntries]    = {0.98, 0.98};

  G4MaterialPropertiesTable* myMPT = new G4MaterialPropertiesTable();

  myMPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, nEntries);
  myMPT->AddProperty("SPECULARLOBECONSTANT", PhotonEnergy, SpecularLobe,
                     nEntries);
  myMPT->AddProperty("SPECULARSPIKECONSTANT", PhotonEnergy, SpecularSpike,
                     nEntries);
  myMPT->AddProperty("BACKSCAERCONSTANT", PhotonEnergy, Backscatter, nEntries);
  myMPT->AddProperty("LAMBERTIAN", PhotonEnergy, Lambertian, nEntries);
  myMPT->AddProperty("REFLECTIVITY", PhotonEnergy, Reflectivity, nEntries);

  OpSurface->SetMaterialPropertiesTable(myMPT);

  new G4LogicalBorderSurface("scintillatorSurface", fphysScintillator0,
                             fphysWorld, OpSurface);
  new G4LogicalBorderSurface("scintillatorSurface", fphysScintillator1,
                             fphysWorld, OpSurface);
  new G4LogicalBorderSurface("scintillatorSurface", fphysScintillator2,
                             fphysWorld, OpSurface);

  auto red    = new G4VisAttributes(G4Colour::Red());
  auto yellow = new G4VisAttributes(G4Colour::Yellow());
  auto cyan   = new G4VisAttributes(G4Colour::Cyan());

  red->SetVisibility(true);
  yellow->SetVisibility(true);
  cyan->SetVisibility(true);

  red->SetForceSolid();
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
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscint0MFDet.Get());
  SetSensitiveDetector(flogicScintillator0, fscint0MFDet.Get());

  primitive = new G4PSEnergyDeposit("Edep");
  if (!fscint1MFDet.Get()) {
    auto scint1Detector = new G4MultiFunctionalDetector("Scintillator1");
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
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscint2MFDet.Get());
  SetSensitiveDetector(flogicScintillator2, fscint2MFDet.Get());

  if (!fscintSD.Get()) {
    auto scintSD = new ScintillatorSD("scintillators");
    fscintSD.Put(scintSD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fscintSD.Get());
  SetSensitiveDetector(flogicScintillator0, fscintSD.Get());
  G4SDManager::GetSDMpointer()->AddNewDetector(fscintSD.Get());
  SetSensitiveDetector(flogicScintillator1, fscintSD.Get());
  G4SDManager::GetSDMpointer()->AddNewDetector(fscintSD.Get());
  SetSensitiveDetector(flogicScintillator2, fscintSD.Get());

  if (!fsipmSD.Get()) {
    auto* gossip = sipm->GetGossip();
    auto sipmSD  = new SiPMSD("sipmSD", gossip);
    fsipmSD.Put(sipmSD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fsipmSD.Get());
  SetSensitiveDetector(sipm->GetLogVol(), fsipmSD.Get());

  primitive = new G4PSEnergyDeposit("Edep");
  if (!fsipm0MFDet.Get()) {
    auto sipmDet = new G4MultiFunctionalDetector("sipm");
    sipmDet->RegisterPrimitive(primitive);
    fsipm0MFDet.Put(sipmDet);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fsipm0MFDet.Get());
  SetSensitiveDetector(sipm->GetLogVol(), fsipm0MFDet.Get());
}

void DetectorConstruction::SetWidth0X(G4double myWidhtX) {
  fscintX = myWidhtX;
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}
