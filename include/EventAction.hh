#ifndef EVENTACTION_H_
#define EVENTACTION_H_

#include <G4SystemOfUnits.hh>
#include <G4THitsMap.hh>
#include <G4UserEventAction.hh>
#include <globals.hh>

// Event action class
class EventAction : public G4UserEventAction {
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  G4THitsMap<G4double>* GetHitsCollection(G4int hcID,
                                          const G4Event* event) const;
  G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
  void PrintEventStatistics(G4int i, G4double absoEdep) const;

  G4int fScintillator0EdepID;
  G4int fScintillator1EdepID;
  G4int fScintillator2EdepID;
};

#endif // EVENTACTION_H_
