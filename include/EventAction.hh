#ifndef EVENTACTION_H_
#define EVENTACTION_H_

#include "ScintillatorHit.hh"
#include "SiPMHit.hh"
#include "pft.hpp"

#include <G4THitsMap.hh>
#include <G4UserEventAction.hh>

class TTree;

// Event action class
class EventAction : public G4UserEventAction {
public:
  EventAction();
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  pft::Particles_t fParticles;

  std::vector<int> sipmID;        // sipm ID
  std::vector<double> posX;       // photon x position on sipm
  std::vector<double> posY;       // photon y position on sipm
  std::vector<double> posZ;       // photon y position on sipm
  std::vector<double> time;       // photon arrival time on sipm
  std::vector<double> wavelength; // photon wavelength

  int nPhotonHits; ///number of photons on the sipm

  G4int feventID;
  G4int frunID;

private:
  G4THitsMap<G4double>* GetHitsCollection(G4int hcID,
                                          const G4Event* event) const;
  G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
  void PrintEventStatistics(G4int i, G4double absoEdep) const;
  void PrintEventStatistics(const G4String& detName, G4double absoEdep) const;

  void Populate(pft::Particles_t& par,
                const ScintillatorHitsCollection* ScintHC);

  void PopulatePhotons(const SiPMHitsCollection* SipmHC);

  G4int fScintillator0EdepID;
  G4int fScintillator1EdepID;
  G4int fScintillator2EdepID;
  G4int fSipm0EdepID;
  G4int fSipm2EdepID;

  G4int fScintillatorCollID;
  G4int fSiPMCollID;
};

#endif // EVENTACTION_H_
