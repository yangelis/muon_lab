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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction() : G4UserEventAction(), fAbsoEdepHCID(-1) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4THitsMap<G4double> *
EventAction::GetHitsCollection(G4int hcID, const G4Event *event) const {
  auto hitsCollection = static_cast<G4THitsMap<G4double> *>(
      event->GetHCofThisEvent()->GetHC(hcID));

  if (!hitsCollection) {
    G4ExceptionDescription msg;
    msg << "Cannot access hc ID" << hcID;
    G4Exception("EventAction::GetHitsCollection()", "MyCode0003",
                FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4double EventAction::GetSum(G4THitsMap<G4double> *hitsMap) const {
  G4double sumValue = 0.;
  for (auto it : *hitsMap->GetMap()) {
    sumValue += *(it.second);
  }
  return sumValue;
}

void EventAction::PrintEventStatistics(G4double absoEdep) const {
  // Print event statistics
  //
  G4cout << "   Absorber: total energy: " << std::setw(7)
         << G4BestUnit(absoEdep, "Energy") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::BeginOfEventAction(const G4Event *) {
  // fAbsoEdepHCID =
  //     G4SDManager::GetSDMpointer()->GetCollectionID("Absorber/Edep");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *event) {
  // Get hist collections IDs
  // if (fAbsoEdepHCID < 0) {
  //   fAbsoEdepHCID =
  //       G4SDManager::GetSDMpointer()->GetCollectionID("Absorber/Edep");
  // }

  // Get sum values from hits collections

  auto absoEdep = GetSum(GetHitsCollection(fAbsoEdepHCID, event));

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // // fill histograms
  // analysisManager->FillH1(0, nGamma);
  analysisManager->AddNtupleRow(0);

  // print per event (modulo n)
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    G4cout << "---> End of event: " << eventID << G4endl;
    PrintEventStatistics(absoEdep);
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
