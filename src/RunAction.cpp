#include "RunAction.hh"
#include "Analysis.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include <G4Run.hh>
#include <G4RunManager.hh>
/* #include <G4SystemOfUnits.hh> */
#include <G4UnitsTable.hh>

// ROOT includes
#include <TFile.h>
#include <TTree.h>

RunAction::RunAction(EventAction* eventAction,
                     DetectorConstruction* detConstruction,
                     PrimaryGeneratorAction* primaryGenAction)
    : G4UserRunAction(), fEventAction(eventAction),
      fDetConstruction(detConstruction),
      fPrimaryGeneratorAction(primaryGenAction) {
  // print event number after each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // create the analysis manager
  // the analysis method is choosed from myAnalysis.hh
  auto analysisManager = G4AnalysisManager::Instance();

  if (analysisManager->GetType() == "Root") {
    analysisManager->SetVerboseLevel(0);
    analysisManager->SetNtupleMerging(true);
    analysisManager->SetFileName("output_file");
  }

  // create histograms
  analysisManager->CreateH1("Edep0", "Edep in scintillator0", 100, 0.,
                            50. * CLHEP::MeV);
  analysisManager->CreateH1("Edep1", "Edep in scintillator1", 100, 0.,
                            50. * CLHEP::MeV);
  analysisManager->CreateH1("Edep2", "Edep in scintillator2", 100, 0.,
                            50. * CLHEP::MeV);
  analysisManager->CreateH1("Edep3", "Edep in sipm", 100, 0., 50. * CLHEP::MeV);

  // create ntuples
  analysisManager->CreateNtuple("Scintillator", "scintillator measurements");
  analysisManager->CreateNtupleIColumn("scintID",
                                       fEventAction->fParticles.det_id);
  analysisManager->CreateNtupleDColumn("eDep", fEventAction->fParticles.edep);
  analysisManager->CreateNtupleDColumn("posX", fEventAction->fParticles.posX);
  analysisManager->CreateNtupleDColumn("posY", fEventAction->fParticles.posY);
  analysisManager->FinishNtuple();

  // create tree for gossip
  analysisManager->CreateNtuple("photons", "sipm photons");
  analysisManager->CreateNtupleDColumn("posX", fEventAction->posX);
  analysisManager->CreateNtupleDColumn("posY", fEventAction->posY);
  analysisManager->CreateNtupleDColumn("posZ", fEventAction->posZ);
  analysisManager->CreateNtupleDColumn("time", fEventAction->time);
  analysisManager->CreateNtupleDColumn("wavelength", fEventAction->wavelength);
  analysisManager->CreateNtupleDColumn("timesteps", fEventAction->timesteps);
  analysisManager->CreateNtupleDColumn("amplitude", fEventAction->amplitude);
  analysisManager->FinishNtuple();
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }

void RunAction::BeginOfRunAction(const G4Run*) {
  auto analysisManager = G4AnalysisManager::Instance();

  // The default name is given in the constructor
  // extra naming functionality exist through the
  // macro /analysis/setFileName filename
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* aRun) {
  auto analysisManager = G4AnalysisManager::Instance();
  G4int n_run          = aRun->GetRunID();
  G4cout << "INFO: run : " << n_run << G4endl;

  // save and close the file
  analysisManager->Write();
  analysisManager->CloseFile();
}
