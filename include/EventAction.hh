#ifndef EVENTACTION_H_
#define EVENTACTION_H_

#include "ScintillatorHit.hh"

#include <G4SystemOfUnits.hh>
#include <G4THitsMap.hh>
#include <G4UserEventAction.hh>
#include <globals.hh>

// TODO(#5): Move particles struct to separate module
struct Particles_t {
  std::vector<G4int> scint_id;
  std::vector<G4int> parent_id;
  std::vector<G4int> trid;
  std::vector<G4double> times;
  std::vector<G4double> edep;
  std::vector<G4double> energy;
  std::vector<G4double> posX;
  std::vector<G4double> posY;
  std::vector<G4double> posZ;
  std::vector<G4double> theta;
  std::vector<G4double> phi;
  std::vector<G4double> trlen;
  // std::vector<int> n_secondaries;

  void Populate(const ScintillatorHitsCollection* ScintHC) {

    size_t nHits = ScintHC->entries();
    for (size_t i = 0; i < nHits; i++) {
      scint_id.push_back(std::atoi((*ScintHC)[i]->GetScintName().c_str()));
      parent_id.push_back((*ScintHC)[i]->GetParentId());
      trid.push_back((*ScintHC)[i]->GetTrId());
      times.push_back((*ScintHC)[i]->GetTime() / ns);
      edep.push_back((*ScintHC)[i]->GetEdep() / MeV);
      energy.push_back((*ScintHC)[i]->GetE() / MeV);
      posX.push_back((*ScintHC)[i]->GetPos().x());
      posY.push_back((*ScintHC)[i]->GetPos().y());
      posZ.push_back((*ScintHC)[i]->GetPos().z());
      theta.push_back((*ScintHC)[i]->GetPos().theta());
      phi.push_back((*ScintHC)[i]->GetPos().phi());
      // n_secondaries.push_back((*ScintHC)[i]->GetSecondaryPhotons());
      trlen.push_back((*ScintHC)[i]->GetTrLen());
    }
  }

  void ClearVecs() {
    scint_id.clear();
    parent_id.clear();
    trid.clear();
    times.clear();
    edep.clear();
    energy.clear();
    posX.clear();
    posY.clear();
    posZ.clear();
    theta.clear();
    phi.clear();
    trlen.clear();
    // n_secondaries.clear();
  }
};

// Event action class
class EventAction : public G4UserEventAction {
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  Particles_t fParticles;

private:
  G4THitsMap<G4double>* GetHitsCollection(G4int hcID,
                                          const G4Event* event) const;
  G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
  void PrintEventStatistics(G4int i, G4double absoEdep) const;

  G4int fScintillator0EdepID;
  G4int fScintillator1EdepID;
  G4int fScintillator2EdepID;
  G4int fScintillatorCollID;
};

#endif // EVENTACTION_H_
