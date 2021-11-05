#ifndef RUNACTION_H_
#define RUNACTION_H_

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

#include <G4UserRunAction.hh>
#include <globals.hh>

class G4Run;
class TTree;
class TFile;

// Run action class
class RunAction : public G4UserRunAction {
public:
  RunAction(EventAction* eventAction, DetectorConstruction* detConstruction,
            PrimaryGeneratorAction* primaryGenAction);
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  EventAction* fEventAction;
  DetectorConstruction* fDetConstruction;
  PrimaryGeneratorAction* fPrimaryGeneratorAction;

  TFile* photon_file;
  TTree* tree_photons;
};

#endif // RUNACTION_H_
