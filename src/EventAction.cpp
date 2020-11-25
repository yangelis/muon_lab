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

EventAction::EventAction() : G4UserEventAction(), fScintillatorEdepID(-1) {}

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

void EventAction::PrintEventStatistics(G4double absoEdep) const {
  // Print event statistics
  G4cout << "   Absorber: total energy: " << std::setw(7)
         << G4BestUnit(absoEdep, "Energy") << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event*) {
  fScintillatorEdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator/Edep");
}

void EventAction::EndOfEventAction(const G4Event* event) {
  // Get hist collections IDs
  if (fScintillatorEdepID < 0) {
    fScintillatorEdepID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Absorber/Edep");
  }

  // Get sum values from hits collections

  auto scintEdep = GetSum(GetHitsCollection(fScintillatorEdepID, event));

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // // fill histograms
  analysisManager->FillH1(0, scintEdep);
  // analysisManager->AddNtupleRow(0);

  // print per event (modulo n)
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    G4cout << "---> End of event: " << eventID << G4endl;
    PrintEventStatistics(scintEdep);
  }
}
