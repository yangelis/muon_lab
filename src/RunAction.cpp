#include "RunAction.hh"
#include "Analysis.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>

RunAction::RunAction(EventAction *eventAction,
                     DetectorConstruction *detConstruction,
                     PrimaryGeneratorAction *primaryGenAction)
    : G4UserRunAction(), fEventAction(eventAction),
      fDetConstruction(detConstruction),
      fPrimaryGeneratorAction(primaryGenAction) {
  // print event number after each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // create the analysis manager
  // the analysis method is choosed from myAnalysis.hh
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->SetVerboseLevel(0);
  analysisManager->SetNtupleMerging(true);

  // create histograms
  analysisManager->CreateH1("Eabs", "Edep in absorber", 100, 0., 5. * GeV);

  // create ntuples
  // analysisManager->CreateNtuple("Gaps", "Electron Ntuple");
  // analysisManager->CreateNtupleDColumn("eDep",
  // fEventAction->fElectrons.edep); analysisManager->FinishNtuple();
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }


void RunAction::BeginOfRunAction(const G4Run *) {
  auto analysisManager = G4AnalysisManager::Instance();

  G4String fileName = "output_file";
  analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run *aRun) {
  auto analysisManager = G4AnalysisManager::Instance();
  G4int n_run = aRun->GetRunID();
  G4cout << "INFO: run : " << n_run << G4endl;

  // save and close the file
  analysisManager->Write();
  analysisManager->CloseFile();
}