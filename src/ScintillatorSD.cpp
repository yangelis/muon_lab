#include "ScintillatorSD.hh"
#include "Analysis.hh"

#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

ScintillatorSD::ScintillatorSD(G4String name)
    : G4VSensitiveDetector(name), fScintHitCollection(nullptr) {
  G4String HCname;
  collectionName.insert(HCname = "ScintParticleCollection");
}

ScintillatorSD::~ScintillatorSD() {}

void ScintillatorSD::Initialize(G4HCofThisEvent* HCE) {
  fScintHitCollection =
      new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);

  static G4int HCID = -1;
  if (HCID < 0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }

  HCE->AddHitsCollection(HCID, fScintHitCollection);
}

G4bool ScintillatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  G4Track* theTrack = aStep->GetTrack();
  auto particleName = theTrack->GetParticleDefinition()->GetParticleName();

  // TODO(#3): Save only electrons and their first step
  if (particleName == "e-" && aStep->IsFirstStepInVolume()) {
    G4double edep = aStep->GetTotalEnergyDeposit();
    if (!edep)
      return false;
    G4int ParentID = theTrack->GetParentID();
    G4int TrackID = theTrack->GetTrackID();
    G4double TrackLength = theTrack->GetTrackLength();
    auto newHit = new ScintillatorHit;
    newHit->SetScintName(theTrack->GetVolume()->GetName().back());
    newHit->SetParticleName(particleName);
    newHit->SetParentId(ParentID);
    newHit->SetTrId(TrackID);
    newHit->SetEdep(edep);
    newHit->SetE(theTrack->GetTotalEnergy());
    newHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
    // Global time start from the start of the run
    newHit->SetTime(aStep->GetPostStepPoint()->GetGlobalTime());
    // Local time start from particle creation
    // newHit->SetTime(aStep->GetPostStepPoint()->GetLocalTime());
    newHit->SetTrLen(TrackLength + aStep->GetStepLength());
    newHit->SetLocalTime(aStep->GetPostStepPoint()->GetGlobalTime());

    fScintHitCollection->insert(newHit);

    return true;
  }
  return false;
}

void ScintillatorSD::EndOfEvent(G4HCofThisEvent*) {}

void ScintillatorSD::clear() {}

void ScintillatorSD::DrawAll() {}

void ScintillatorSD::PrintAll() {}
