#include "EventAction.hh"
#include "Analysis.hh"
#include "pft.hpp"

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
      fScintillator2EdepID(-1), fScintillator0nGammaID(-1),
      fScintillator1nGammaID(-1), fScintillator2nGammaID(-1),
      fScintillatorCollID(-1) {}

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

void EventAction::PrintEventStatistics(G4int i, G4double absoEdep,
                                       G4double nGamma) const {
  // Print event statistics
  G4cout << "   Scintillator " << i << " : total energy: " << std::setw(7)
         << G4BestUnit(absoEdep, "Energy") << ", nGamma: " << nGamma << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event*) {
  fScintillator0EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator0/Edep");
  fScintillator1EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator1/Edep");
  fScintillator2EdepID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator2/Edep");

  fScintillator0nGammaID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator0/nGamma");
  fScintillator1nGammaID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator1/nGamma");
  fScintillator2nGammaID =
      G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator2/nGamma");

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

    fScintillator0nGammaID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator0/nGamma");
    fScintillator1nGammaID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator1/nGamma");
    fScintillator2nGammaID =
        G4SDManager::GetSDMpointer()->GetCollectionID("Scintillator2/nGamma");

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
    // fParticles.Populate(ScintHC);
    Populate(fParticles, ScintHC);
  }

  // Get sum values from hits collections
  auto scint0Edep = GetSum(GetHitsCollection(fScintillator0EdepID, event));
  auto scint1Edep = GetSum(GetHitsCollection(fScintillator1EdepID, event));
  auto scint2Edep = GetSum(GetHitsCollection(fScintillator2EdepID, event));

  auto scint0nGamma = GetSum(GetHitsCollection(fScintillator0nGammaID, event));
  auto scint1nGamma = GetSum(GetHitsCollection(fScintillator1nGammaID, event));
  auto scint2nGamma = GetSum(GetHitsCollection(fScintillator2nGammaID, event));

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, scint0Edep);
  analysisManager->FillH1(1, scint1Edep);
  analysisManager->FillH1(2, scint2Edep);
  analysisManager->AddNtupleRow(0);
  analysisManager->FillNtupleDColumn(0, 4, scint0Edep);
  analysisManager->FillNtupleDColumn(0, 5, scint1Edep);
  analysisManager->FillNtupleDColumn(0, 6, scint2Edep);
  analysisManager->FillNtupleIColumn(0, 7, scint0nGamma);
  analysisManager->FillNtupleIColumn(0, 8, scint1nGamma);
  analysisManager->FillNtupleIColumn(0, 9, scint2nGamma);

  // print per event (modulo n)
  auto eventID     = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    G4cout << "---> End of event: " << eventID << G4endl;
    PrintEventStatistics(0, scint0Edep, scint0nGamma);
    PrintEventStatistics(1, scint1Edep, scint1nGamma);
    PrintEventStatistics(2, scint2Edep, scint2nGamma);
  }
}

void EventAction::Populate(pft::Particles_t& par,
                           const ScintillatorHitsCollection* ScintHC) {
  std::size_t nHits = ScintHC->entries();
  par.Reserve(nHits);
  for (size_t i = 0; i < nHits; i++) {
    par.det_id.push_back(std::atoi((*ScintHC)[i]->GetScintName().c_str()));
    par.parent_id.push_back((*ScintHC)[i]->GetParentId());
    par.trid.push_back((*ScintHC)[i]->GetTrId());
    par.times.push_back((*ScintHC)[i]->GetTime() / ns);
    par.edep.push_back((*ScintHC)[i]->GetEdep() / MeV);
    par.energy.push_back((*ScintHC)[i]->GetE() / MeV);
    par.posX.push_back((*ScintHC)[i]->GetPos().x());
    par.posY.push_back((*ScintHC)[i]->GetPos().y());
    par.posZ.push_back((*ScintHC)[i]->GetPos().z());
    par.theta.push_back((*ScintHC)[i]->GetPos().theta());
    par.phi.push_back((*ScintHC)[i]->GetPos().phi());
    par.trlen.push_back((*ScintHC)[i]->GetTrLen());
  }
}
