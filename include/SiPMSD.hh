#ifndef SIPMSD_H_
#define SIPMSD_H_
#include "SiPMHit.hh"

#include <G4VSensitiveDetector.hh>
#include <fstream>

// gossip includes
#include <PhotonList.h>
#include <sipmMC.h>

class G4HCofThisEvent;
class G4Step;
class TTree;
class TFile;

class SiPMSD : public G4VSensitiveDetector {
public:
  SiPMSD(G4String, sipmMC*);
  ~SiPMSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  void RunGossip(bool enable) { write_gossip = enable; };
  void WritePhotons(bool enable) { write_photons = enable; };
  void SetFilePhotons(const std::string& file) { filename_photons = file; };
  void SetFileGossip(const std::string& file) { filename_gossip = file; };

private:
  SiPMHitsCollection* sipmCollection;

  sipmMC* sipm;
  PhotonList photons;

  std::ofstream file_photons;
  std::ofstream file_gossip;

  unsigned int n_event; ///event number

  bool write_photons; ///enable to save photon output
  bool write_gossip;  ///enable to run gossip

  std::string filename_photons; ///filename for photon output
  std::string filename_gossip;  ///filename for gossip output

  G4String par_name;
  G4String proc_name;
};

#endif // SIPMSD_H_
