#include "EventAction.hh"
#include "Analysis.hh"
#include "pft.hpp"

#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4THitsCollection.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>

// ROOT includes
#include <TTree.h>

EventAction::EventAction()
    : G4UserEventAction(), fScintillator0EdepID(-1), fScintillator1EdepID(-1),
      fScintillator2EdepID(-1), fSipmEdepID(-1), fScintillatorCollID(-1),
      fSiPMCollID(-1) {}

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

void EventAction::PrintEventStatistics(const G4String& detName,
                                       G4double absoEdep) const {
  // Print event statistics
  G4cout << detName << " : total energy: " << std::setw(7)
         << G4BestUnit(absoEdep, "Energy") << G4endl;
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
  fSipmEdepID = G4SDManager::GetSDMpointer()->GetCollectionID("sipm/Edep");

  fScintillatorCollID =
      G4SDManager::GetSDMpointer()->GetCollectionID("ScintParticleCollection");
  fParticles.ClearVecs();

  fSiPMCollID =
      G4SDManager::GetSDMpointer()->GetCollectionID("SiPMParticleCollection");
  posX.clear();
  posY.clear();
  time.clear();
  wavelength.clear();
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
    fSipmEdepID = G4SDManager::GetSDMpointer()->GetCollectionID("sipm/Edep");

    fScintillatorCollID = G4SDManager::GetSDMpointer()->GetCollectionID(
        "ScintParticleCollection");
    fSiPMCollID =
        G4SDManager::GetSDMpointer()->GetCollectionID("SiPMParticleCollection");
  }

  ScintillatorHitsCollection* ScintHC = nullptr;
  SiPMHitsCollection* SipmHC          = nullptr;
  if (event->GetHCofThisEvent()) {
    ScintHC = static_cast<ScintillatorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(fScintillatorCollID));
    SipmHC = static_cast<SiPMHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(fSiPMCollID));
  }

  // This is were we get the data from the HitCollection
  if (ScintHC) {
    // Get number of entries
    G4cout << "We got a HitCollection with nHits: " << ScintHC->entries()
           << G4endl;
    // fParticles.Populate(ScintHC);
    Populate(fParticles, ScintHC);
  }

  if (SipmHC) {
    // Get number of entries
    G4cout << "We got a Sipm HitCollection with nHits: " << SipmHC->entries()
           << G4endl;
    PopulatePhotons(SipmHC);
  }

  // Get sum values from hits collections
  auto scint0Edep = GetSum(GetHitsCollection(fScintillator0EdepID, event));
  auto scint1Edep = GetSum(GetHitsCollection(fScintillator1EdepID, event));
  auto scint2Edep = GetSum(GetHitsCollection(fScintillator2EdepID, event));
  auto sipmEdep   = GetSum(GetHitsCollection(fSipmEdepID, event));

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, scint0Edep);
  analysisManager->FillH1(1, scint1Edep);
  analysisManager->FillH1(2, scint2Edep);
  analysisManager->FillH1(3, sipmEdep);
  analysisManager->AddNtupleRow(0);
  analysisManager->AddNtupleRow(1);

  // print per event (modulo n)
  feventID         = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (feventID % printModulo == 0)) {
    G4cout << "---> End of event: " << feventID << G4endl;
    PrintEventStatistics("Scintillator0", scint0Edep);
    PrintEventStatistics("Scintillator1", scint1Edep);
    PrintEventStatistics("Scintillator2", scint2Edep);
    PrintEventStatistics("SiPM         ", sipmEdep);
  }
}

void EventAction::Populate(pft::Particles_t& par,
                           const ScintillatorHitsCollection* ScintHC) {
  size_t nHits = ScintHC->entries();
  par.Reserve(nHits);
  for (size_t i = 0; i < nHits; i++) {
    par.det_id.push_back(std::atoi((*ScintHC)[i]->GetScintName().c_str()));
    par.parent_id.push_back((*ScintHC)[i]->GetParentId());
    par.trid.push_back((*ScintHC)[i]->GetTrId());
    par.times.push_back((*ScintHC)[i]->GetTime() / CLHEP::ns);
    par.edep.push_back((*ScintHC)[i]->GetEdep() / CLHEP::MeV);
    par.energy.push_back((*ScintHC)[i]->GetE() / CLHEP::MeV);
    par.posX.push_back((*ScintHC)[i]->GetPos().x());
    par.posY.push_back((*ScintHC)[i]->GetPos().y());
    par.posZ.push_back((*ScintHC)[i]->GetPos().z());
    par.theta.push_back((*ScintHC)[i]->GetPos().theta());
    par.phi.push_back((*ScintHC)[i]->GetPos().phi());
    par.trlen.push_back((*ScintHC)[i]->GetTrLen());
  }
}

void EventAction::PopulatePhotons(const SiPMHitsCollection* SipmHC) {

  const std::size_t nHits = SipmHC->entries();
  nPhotonHits             = static_cast<int>(nHits);

  posX.reserve(nHits);
  posY.reserve(nHits);
  posZ.reserve(nHits);
  time.reserve(nHits);
  wavelength.reserve(nHits);

  for (std::size_t i = 0; i < nHits; i++) {
    posX.push_back((*SipmHC)[i]->GetPos().x());
    posY.push_back((*SipmHC)[i]->GetPos().y());
    posZ.push_back((*SipmHC)[i]->GetPos().z());
    time.push_back((*SipmHC)[i]->GetTime() / CLHEP::ns);
    wavelength.push_back((*SipmHC)[i]->GetWavelength());
  }
}
