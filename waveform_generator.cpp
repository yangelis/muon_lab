#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <sipmMC.h>

int main(int argc, char* argv[]) {

  if (argc != 2) {
    fprintf(stderr, "No filename given!\n");
    exit(1);
  }

  TFile ifile(argv[1], "READ");
  TTree* input_tree = nullptr;
  ifile.GetObject("photons", input_tree);

  std::vector<double>* posX       = nullptr;
  std::vector<double>* posY       = nullptr;
  std::vector<double>* posZ       = nullptr;
  std::vector<double>* time       = nullptr;
  std::vector<double>* wavelength = nullptr;

  input_tree->SetBranchAddress("posX", &posX);
  input_tree->SetBranchAddress("posY", &posY);
  input_tree->SetBranchAddress("posZ", &posZ);
  input_tree->SetBranchAddress("time", &time);
  input_tree->SetBranchAddress("wavelength", &wavelength);

  auto sipm = new sipmMC();

  TFile f("input/SpectralSensitivity.root");
  sipm->SetSpectralSensitivity((TGraph*)f.Get("MPPC_noRef"));
  f.Close();

  TGraph* waveform = nullptr;
  TFile results_file("pepega.root", "RECREATE");
  auto rtree = std::make_shared<TTree>("wav", "tree with waveforms");
  rtree->Branch("waveform", &waveform);

  const Long64_t nEvents = input_tree->GetEntries();

  for (Long64_t iEvent = 0; iEvent < nEvents; ++iEvent) {
    input_tree->GetEntry(iEvent);

    sipm->SetSampling(1.0);
    sipm->SetGate(3000);
    sipm->SetPreGate(500);
    sipm->noiseRMS  = 0.0;
    sipm->PDE       = 0.99;
    sipm->gain      = 30;
    sipm->signalAmp = 20.0;
    sipm->NpixX     = 97;
    sipm->NpixY     = 97;

    PhotonList photons;

    const auto nphotons = posX->size();
    for (std::size_t i = 0; i < nphotons; ++i) {
      photons.AddPhoton((*posZ)[i], (*posY)[i], (*time)[i], (*wavelength)[i]);
    }

    sipm->Generate(photons); ///generate sipm response from photon list
    /* double charge = sipm->GetCharge();   ///get output signal charge */
    waveform = sipm->GetWaveform(); ///get output waveform
    rtree->Fill();

    fprintf(stdout, "\r");
    fprintf(stdout, "Generating Waveform: %lld:%lld", iEvent + 1, nEvents);
    fflush(stdout);
  }

  rtree->Write();
  ifile.Close();

  return 0;
}
