#include "ActionInitialization.hh"
#include "Analysis.hh"
#include "DetectorConstruction.hh"

#ifdef G4MULTITHREADED
#include <G4MTRunManager.hh>
#else
#include <G4RunManager.hh>
#endif

#include <G4EmStandardPhysics_option4.hh>
#include <QGSP_BERT.hh>

#include <G4OpticalPhysics.hh>
#include <G4ScoringManager.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>

#include <Randomize.hh>

namespace {
void PrintUsage() {
  G4cerr << " How to use the program: " << G4endl;
  G4cerr << " bm_electron [-m macro] [-u UIsession] " << G4endl;
}
} // namespace

int main(int argc, char* argv[]) {
  if (argc > 7) {
    PrintUsage();
    return 1;
  }

  G4String macro;
  G4String session;
  for (G4int i = 1; i < argc; i = i + 2) {
    if (G4String(argv[i]) == "-m") {
      macro = argv[i + 1];
    } else if (G4String(argv[i]) == "-u") {
      session = argv[i + 1];
    } else {
      PrintUsage();
      return 1;
    }
  }

  // Detect interactive mode
  G4UIExecutive* ui = nullptr;
  if (!macro.size()) {
    ui = new G4UIExecutive(argc, argv, session);
  }

#ifdef G4MULTITHREADED
  // in MT Mode the random engine uses the same seeds
  auto runManager = new G4MTRunManager;
#else
  // Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(100);
  auto runManager = new G4RunManager;
#endif
  // Activate command-based scorer
  G4ScoringManager::GetScoringManager();

  // Mandatory Initialization classes

  // Detector construction
  auto detConstruction = new DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  // Physics list

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  physicsList->SetVerboseLevel(0);
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  opticalPhysics->Configure(kCerenkov, false);
  opticalPhysics->SetCerenkovStackPhotons(true);
  opticalPhysics->Configure(kScintillation, true);

  physicsList->RegisterPhysics(opticalPhysics);
  physicsList->DumpList();
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  auto actionInit = new ActionInitialization(detConstruction);
  runManager->SetUserInitialization(actionInit);
  // runManager->Initialize();

  // Initialize visualization
  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  // Pointer to User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session

  if (macro.size()) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macro);
  } else {
    // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }
  // Job Termination

  delete visManager;
  delete runManager;
}
