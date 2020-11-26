#include "EventAction.hh"
#include "Analysis.hh"

#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4THitsCollection.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>

EventAction::EventAction()
    : G4UserEventAction(), fScintillator0EdepID(-1), fScintillator1EdepID(-1),
      fScintillator2EdepID(-1), fScintillatorCollID(-1) {}

EventAction::~EventAction() {}

G4THitsMap<G4double>*
EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const {
  auto hitsCollection = static_cast<G4THitsMap<G4double>*>(
      event->GetHCofThisEvent()->GetHC(hcID));

  if (!hitsCollection) {
    G4ExceptionDescription msg;
    msg << "Cannot access hc ID" << hcID;
    G4Exception("EventAction::GetHitsCollection()", "MyCode0003",
                FatalException, msg);
  }

  return hitsCollection;
}

G4double EventAction::GetSum(G4THitsMap<G4double>* hitsMap) const {
  G4double sumValue = 0.;
  for (auto it : *hitsMap->GetMap()) {
    sumValue += *(it.second);
  }
  return sumValue;
}

void EventAction::PrintEventStatistics(G4int i, G4double absoEdep) const {
  // Print event statistics
  G4cout << "   Scintillator " << i << " : total energy: " << std::setw(7)
         << G4BestUnit(absoEdep, "Energy") << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event*) {
  fScintillator0EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator0/Edep");
  fScintillator1EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator1/Edep");
  fScintillator2EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator2/Edep");

  fScintillatorCollID =
      G4SDManager::GetSDMpointer()->GetCollectionID("ScintParticleCollection");
  fParticles.ClearVecs();
}

void EventAction::EndOfEventAction(const G4Event* event) {
  // Get hist collections IDs
  if (fScintillator1EdepID < 0) {
    fScintillator0EdepID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator0/Edep");
    fScintillator1EdepID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator1/Edep");
    fScintillator2EdepID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator2/Edep");
    fScintillatorCollID = G4SDManager::GetSDMpointer()->GetCollectionID(
        "ScintParticleCollection");
  }

  ScintillatorHitsCollection* ScintHC = nullptr;
  if (event->GetHCofThisEvent()) {
    ScintHC = static_cast<ScintillatorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(fScintillatorCollID));
  }

  // This is were we get the data from the HitCollection
  if (ScintHC) {
    // Get number of entries
    G4cout << "We got a HitCollection with nHits: " << ScintHC->entries()
           << G4endl;
    fParticles.Populate(ScintHC);
  }

  // Get sum values from hits collections
  auto scint0Edep = GetSum(GetHitsCollection(fScintillator0EdepID, event));
  auto scint1Edep = GetSum(GetHitsCollection(fScintillator1EdepID, event));
  auto scint2Edep = GetSum(GetHitsCollection(fScintillator2EdepID, event));

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // // fill histograms
  analysisManager->FillH1(0, scint0Edep);
  analysisManager->FillH1(1, scint1Edep);
  analysisManager->FillH1(2, scint2Edep);
  analysisManager->AddNtupleRow(0);

  // print per event (modulo n)
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    G4cout << "---> End of event: " << eventID << G4endl;
    PrintEventStatistics(0, scint0Edep);
    PrintEventStatistics(1, scint1Edep);
    PrintEventStatistics(2, scint2Edep);
  }
}
