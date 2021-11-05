#include "SiPMSD.hh"

#include <G4PhysicalConstants.hh>
#include <G4SDManager.hh>
#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4VProcess.hh>

#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>

SiPMSD::SiPMSD(G4String name, sipmMC* gossip)
    : G4VSensitiveDetector(std::move(name)), sipm(gossip) {
  G4String HCname;
  collectionName.insert(HCname = "SiPMParticleCollection");

  n_event = 0;

  sipm->SetSampling(2.0);
  sipm->SetGate(500);
  sipm->SetPreGate(500);

  write_photons = false; ///default - don't write photon information
  write_gossip  = true;  ///default - don't run gossip

  filename_photons = "output_photons.bin"; ///default path
  filename_gossip  = "output_gossip.bin";  ///default path

  par_name  = "opticalphoton";
  proc_name = "OpAbsorption";
}

SiPMSD::~SiPMSD() {}

void SiPMSD::Initialize(G4HCofThisEvent* HCE) {
  sipmCollection =
      new SiPMHitsCollection(SensitiveDetectorName, collectionName[0]);
  static G4int HCID = -1;
  if (HCID < 0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }
  HCE->AddHitsCollection(HCID, sipmCollection);

  ///write photon information
  if (write_photons) {
    file_photons.open(filename_photons.c_str(),
                      std::ios::out | std::ios::app | std::ios::binary);
    file_photons.write((char*)&n_event, sizeof(int)); ///write event number
  }

  ///write gossip information
  if (write_gossip) {
    file_gossip.open(filename_gossip.c_str(),
                     std::ios::out | std::ios::app | std::ios::binary);
    file_gossip.write((char*)&n_event, sizeof(int)); ///write event number
  }

  photons.clear();
}

G4bool SiPMSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  ///only process optical photons
  if (aStep->GetTrack()->GetDefinition()->GetParticleName() != par_name) {
    return false;
  }

  ///only process photons which are absorpt
  if (aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() !=
      proc_name) {
    return false;
  }

  if (aStep->IsFirstStepInVolume()) {
    ///write photon position and time to hit collection
    SiPMHit* newHit = new SiPMHit();

    const G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();
    const G4double time     = aStep->GetPostStepPoint()->GetGlobalTime();
    const G4double wavelength =
        h_Planck * c_light / aStep->GetTrack()->GetTotalEnergy();

    newHit->SetPos(pos);
    newHit->SetTime(time);
    newHit->SetWavelength(wavelength);

    sipmCollection->insert(newHit);

    ///write photon to list
    photons.AddPhoton(pos.x() / mm, pos.y() / mm, time / ns, wavelength / nm);

    return true;
  }
  return false;
}

void SiPMSD::EndOfEvent(G4HCofThisEvent*) {
  ///write photon information
  if (write_photons) {
    const auto n_photons = photons.size();
    file_photons.write((char*)&n_photons,
                       sizeof(unsigned int)); ///write number of photons

    for (std::size_t i = 0; i < n_photons; i++) {
      const double x  = photons[i].x;
      const double y  = photons[i].y;
      const double t  = photons[i].time;
      const double wl = photons[i].wavelength;

      file_photons.write((char*)&x,
                         sizeof(double)); ///write x position of photon
      file_photons.write((char*)&y,
                         sizeof(double)); ///write y position of photon
      file_photons.write((char*)&t,
                         sizeof(double)); ///write arrival time of photon
      file_photons.write((char*)&wl,
                         sizeof(double)); ///write wavelength of photon
    }

    file_photons.close();
  }

  ///run gossip and write output data
  if (write_gossip && photons.size() > 0) {
    sipm->Generate(photons); ///generate sipm response from photon list
    const double charge = sipm->GetCharge();   ///get output signal charge
    TGraph* waveform    = sipm->GetWaveform(); ///get output waveform

    const double sampling        = sipm->GetSampling();
    const unsigned int n_samples = static_cast<unsigned int>(waveform->GetN());

    file_gossip.write((char*)&charge, sizeof(double));   ///write output charge
    file_gossip.write((char*)&sampling, sizeof(double)); ///write sampling
    file_gossip.write((char*)&n_samples,
                      sizeof(unsigned int)); ///write number of samples

    for (int i = 0; i < waveform->GetN(); i++) ///write amplitudes
    {
      const double amplitude = waveform->GetY()[i];
      file_gossip.write((char*)&amplitude, sizeof(double));
    }

    file_gossip.close();
  }

  n_event++;
}
