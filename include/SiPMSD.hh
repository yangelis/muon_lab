#ifndef SIPMSD_H_
#define SIPMSD_H_

#include "SiPMHit.hh"

#include <G4VSensitiveDetector.hh>

class G4HCofThisEvent;
class G4Step;

class SiPMSD : public G4VSensitiveDetector {
public:
  SiPMSD(G4String);
  ~SiPMSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

private:
  SiPMHitsCollection* sipmCollection;

  unsigned int n_event;

  G4String par_name;
  G4String proc_name;
};

#endif // SIPMSD_H_
