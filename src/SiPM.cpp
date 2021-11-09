#include "SiPM.hh"

#include <G4Box.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>
#include <globals.hh>

#include <TFile.h>
#include <TGraph.h>

SiPM::SiPM(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
           G4LogicalVolume* pMother, G4bool pMany, G4int pCopyNo,
           G4bool pSurfChk = false, const G4String& LVname = "sipm_log",
           const G4String& PVname = "SiPM")
    : log_name(LVname), phys_name(PVname), fsipmSD(0) {
  ///initialize
  BuildSi();
  BuildSiO2();

  sipmMessenger = new SiPMMessenger(this);

  ///default geometry values
  G4double sizeX = 1. * CLHEP::mm;
  G4double sizeY = 1. * CLHEP::mm;
  G4double sizeZ = 1 * CLHEP::mm;
  coatingWidth =
      0.4 * CLHEP::mm; ///the coating width if hardcoded at the moment...

  ///sipm
  sipm_box  = new G4Box("sipm_box", sizeX / 2., sizeY / 2., sizeZ / 2.);
  sipm_log  = new G4LogicalVolume(sipm_box, Si, log_name, 0, 0, 0);
  sipm_phys = new G4PVPlacement(pRot, tlate, sipm_log, phys_name, pMother,
                                pMany, pCopyNo, pSurfChk);

  ///coating
  G4ThreeVector tlate2 = G4ThreeVector(0, 0, -coatingWidth / 2. + sizeZ / 2.);
  coating_box =
      new G4Box("coating_box", sizeX / 2., sizeY / 2., coatingWidth / 2.);
  coating_log  = new G4LogicalVolume(coating_box, SiO2, "coating_log", 0, 0, 0);
  coating_phys = new G4PVPlacement(pRot, tlate2, coating_log, "SiPMCoating",
                                   sipm_log, pMany, pCopyNo, pSurfChk);
}

SiPM::~SiPM() {
  delete sipm_box;
  delete sipm_log;
  delete sipm_phys;
  delete coating_box;
  delete coating_log;
  delete coating_phys;
  delete sipmMessenger;
}

void SiPM::SetTranslation(const G4ThreeVector& translate) {
  sipm_phys->SetTranslation(translate);
}

void SiPM::SetDimensions(G4double sipmX, G4double sipmY, G4double sipmZ) {
  sipm_box->SetXHalfLength(sipmX / 2.);
  sipm_box->SetYHalfLength(sipmY / 2.);
  sipm_box->SetZHalfLength(sipmZ / 2.);

  coating_box->SetXHalfLength(sipmX / 2.);
  coating_box->SetYHalfLength(sipmY / 2.);

  G4ThreeVector tlate2 =
      G4ThreeVector(0, 0, -coatingWidth / 2. + sipm_box->GetZHalfLength());
  coating_phys->SetTranslation(tlate2);
}

void SiPM::SetDimensions(const G4ThreeVector& dimensions) {
  sipm_box->SetXHalfLength(dimensions.x() / 2.);
  sipm_box->SetYHalfLength(dimensions.y() / 2.);
  sipm_box->SetZHalfLength(dimensions.z() / 2.);

  coating_box->SetXHalfLength(dimensions.x() / 2.);
  coating_box->SetYHalfLength(dimensions.y() / 2.);

  G4ThreeVector tlate2 =
      G4ThreeVector(0, 0, -coatingWidth / 2. + sipm_box->GetZHalfLength());
  coating_phys->SetTranslation(tlate2);
}

G4ThreeVector SiPM::GetDimensions() {
  G4double sizeX = sipm_box->GetXHalfLength() * 2.;
  G4double sizeY = sipm_box->GetYHalfLength() * 2.;
  G4double sizeZ = sipm_box->GetZHalfLength() * 2.;

  return G4ThreeVector(sizeX, sizeY, sizeZ);
}

void SiPM::BuildSi() {
  Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  G4cout << Si << G4endl;

  TFile f("input/SiData.root");

  G4double c = 299792458 * CLHEP::m / CLHEP::s;
  G4double h = 4.13566733e-15 * CLHEP::eV * CLHEP::s;

  TGraph* gn = (TGraph*)f.Get("nSi");
  if (gn == 0) {
    G4cerr << "WARNING: No data for nSi found!" << G4endl;
    return;
  }

  TGraph* gAbs = (TGraph*)f.Get("AbsSi");
  if (gAbs == 0) {
    G4cerr << "WARNING: No data for AbsSi found!" << G4endl;
    return;
  }

  const int nEntries   = gn->GetN();
  const int AbsEntries = gAbs->GetN();

  G4double wavelength;
  G4double* PhotonEnergy    = new G4double[nEntries];
  G4double* RefractiveIndex = new G4double[nEntries];

  G4double* PhotonEnergyAbs  = new G4double[AbsEntries];
  G4double* AbsorptionLength = new G4double[AbsEntries];

  for (int i = 0; i < nEntries; i++) {
    gn->GetPoint(nEntries - 1 - i, wavelength, RefractiveIndex[i]);
    PhotonEnergy[i] = h * c / (wavelength * CLHEP::nm);
  }
  for (int i = 0; i < AbsEntries; i++) {
    gAbs->GetPoint(nEntries - 1 - i, wavelength, AbsorptionLength[i]);
    PhotonEnergyAbs[i] = h * c / (wavelength * CLHEP::nm);
  }

  G4MaterialPropertiesTable* Si_MPT = new G4MaterialPropertiesTable();
  Si_MPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, nEntries);
  Si_MPT->AddProperty("ABSLENGTH", PhotonEnergyAbs, AbsorptionLength,
                      AbsEntries);

  Si->SetMaterialPropertiesTable(Si_MPT);

  f.Close();

  delete[] PhotonEnergy;
  delete[] RefractiveIndex;
  delete[] PhotonEnergyAbs;
  delete[] AbsorptionLength;
}

void SiPM::BuildSiO2() {
  SiO2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

  TFile f("input/SiData.root");
  constexpr G4double c = 299792458 * CLHEP::m / CLHEP::s;
  constexpr G4double h = 4.13566733e-15 * CLHEP::eV * CLHEP::s;

  TGraph* gn = (TGraph*)f.Get("nSiO2");
  if (gn == 0) {
    G4cerr << "WARNING: No data for nSiO2 found!" << G4endl;
    return;
  }

  const int nEntries = gn->GetN();

  G4double wavelength;
  G4double* PhotonEnergy    = new G4double[nEntries];
  G4double* RefractiveIndex = new G4double[nEntries];
  for (int i = 0; i < nEntries; i++) {
    gn->GetPoint(nEntries - 1 - i, wavelength, RefractiveIndex[i]);
    PhotonEnergy[i] = h * c / (wavelength * CLHEP::nm);
  }
  G4MaterialPropertiesTable* Si02_MPT = new G4MaterialPropertiesTable();

  Si02_MPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex, nEntries);
  SiO2->SetMaterialPropertiesTable(Si02_MPT);

  f.Close();

  delete[] PhotonEnergy;
  delete[] RefractiveIndex;
}
