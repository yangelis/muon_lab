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

  std::vector<int>* sipmID        = nullptr;
  std::vector<double>* posX       = nullptr;
  std::vector<double>* posY       = nullptr;
  std::vector<double>* posZ       = nullptr;
  std::vector<double>* time       = nullptr;
  std::vector<double>* wavelength = nullptr;

  input_tree->SetBranchAddress("sipmID", &sipmID);
  input_tree->SetBranchAddress("posX", &posX);
  input_tree->SetBranchAddress("posY", &posY);
  input_tree->SetBranchAddress("posZ", &posZ);
  input_tree->SetBranchAddress("time", &time);
  input_tree->SetBranchAddress("wavelength", &wavelength);

  auto sipm0 = new sipmMC();
  auto sipm2 = new sipmMC();

  TFile f("input/SpectralSensitivity.root");
  sipm0->SetSpectralSensitivity((TGraph*)f.Get("MPPC_noRef"));
  sipm2->SetSpectralSensitivity((TGraph*)f.Get("MPPC_noRef"));
  f.Close();
  sipm0->SetSampling(1.0);
  sipm0->SetGate(10000);
  sipm0->SetPreGate(500);
  sipm0->noiseRMS  = 0.0;
  sipm0->PDE       = 0.99;
  sipm0->gain      = 30;
  sipm0->signalAmp = 20.0;
  sipm0->NpixX     = 97;
  sipm0->NpixY     = 97;

  sipm2->SetSampling(1.0);
  sipm2->SetGate(10000);
  sipm2->SetPreGate(500);
  sipm2->noiseRMS  = 0.0;
  sipm2->PDE       = 0.99;
  sipm2->gain      = 30;
  sipm2->signalAmp = 20.0;
  sipm2->NpixX     = 97;
  sipm2->NpixY     = 97;

  TGraph* waveform_ch0 = nullptr;
  TGraph* waveform_ch2 = nullptr;
  TFile results_file("pepega.root", "RECREATE");
  auto rtree = std::make_shared<TTree>("wav", "tree with waveforms");
  rtree->Branch("waveform0", &waveform_ch0);
  rtree->Branch("waveform2", &waveform_ch2);

  const Long64_t nEvents = input_tree->GetEntries();

  for (Long64_t iEvent = 0; iEvent < nEvents; ++iEvent) {
    input_tree->GetEntry(iEvent);

    PhotonList photons_ch0;
    PhotonList photons_ch2;

    const auto nphotons = posX->size();
    for (std::size_t i = 0; i < nphotons; ++i) {
      if ((*sipmID)[i] == 0) {
        photons_ch0.AddPhoton((*posZ)[i], (*posY)[i], (*time)[i],
                              (*wavelength)[i]);
      } else {
        photons_ch2.AddPhoton((*posZ)[i], (*posY)[i], (*time)[i],
                              (*wavelength)[i]);
      }
    }

    sipm0->Generate(photons_ch0); ///generate sipm response from photon list
    /* double charge = sipm->GetCharge();   ///get output signal charge */
    waveform_ch0 = sipm0->GetWaveform(); ///get output waveform
    sipm2->Generate(photons_ch2); ///generate sipm response from photon list
    waveform_ch2 = sipm2->GetWaveform(); ///get output waveform
    rtree->Fill();

    fprintf(stdout, "\r");
    fprintf(stdout, "Generating Waveform: %lld:%lld", iEvent + 1, nEvents);
    fflush(stdout);
  }

  rtree->Write();
  ifile.Close();

  return 0;
}
