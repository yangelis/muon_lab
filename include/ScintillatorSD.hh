#ifndef SCINTILLATORSD_H_
#define SCINTILLATORSD_H_

#include "ScintillatorHit.hh"
#include <G4VSensitiveDetector.hh> // Template class for SD

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class ScintillatorSD : public G4VSensitiveDetector {
public:
  ScintillatorSD(G4String name);
  virtual ~ScintillatorSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();

private:
  ScintillatorHitsCollection* fScintHitCollection;
};

#endif // SCINTILLATORSD_H_
