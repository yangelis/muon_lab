#include "SiPMSD.hh"

/* #include <G4PhysicalConstants.hh> */
#include <G4SDManager.hh>
#include <G4Step.hh>
#include <G4VProcess.hh>

SiPMSD::SiPMSD(G4String name) : G4VSensitiveDetector(std::move(name)) {
  G4String HCname;
  collectionName.insert(HCname = "SiPMParticleCollection");

  n_event = 0;

  par_name  = "opticalphoton";
  proc_name = "OpAbsorption";
}

SiPMSD::~SiPMSD() {}

void SiPMSD::Initialize(G4HCofThisEvent* HCE) {
  sipmCollection =
      new SiPMHitsCollection(SensitiveDetectorName, collectionName[0]);
  static G4int HCID = -1;
  if (HCID < 0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }
  HCE->AddHitsCollection(HCID, sipmCollection);
}

G4bool SiPMSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  ///only process optical photons
  if (aStep->GetTrack()->GetDefinition()->GetParticleName() != par_name) {
    return false;
  }

  ///only process photons which are absorpt
  if (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() !=
      proc_name) {
    return false;
  }

  if (aStep->IsFirstStepInVolume()) {
    ///write photon position and time to hit collection
    SiPMHit* newHit = new SiPMHit();

    const G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();
    const G4double time     = aStep->GetPostStepPoint()->GetGlobalTime();
    const G4double wavelength =
        CLHEP::h_Planck * CLHEP::c_light / aStep->GetTrack()->GetTotalEnergy();

    newHit->SetSiPMName(aStep->GetTrack()->GetVolume()->GetName().back());
    newHit->SetPos(pos);
    newHit->SetTime(time);
    newHit->SetWavelength(wavelength);

    sipmCollection->insert(newHit);
    return true;
  }
  return false;
}

void SiPMSD::EndOfEvent(G4HCofThisEvent*) { n_event++; }
