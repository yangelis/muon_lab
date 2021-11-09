#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>

TCanvas* canvas   = nullptr;
TTree* input_tree = nullptr;
TGraph* wf_ch0    = nullptr;
TGraph* wf_ch2    = nullptr;

void wav(const char* filename) {
  canvas     = new TCanvas("can", "", 1366, 768);
  canvas->Divide(2, 1);
  canvas->GetPad(1)->SetGrid();
  canvas->GetPad(2)->SetGrid();

  auto ifile = new TFile(filename, "READ");
  ifile->GetObject("wav", input_tree);
  input_tree->SetBranchAddress("waveform0", &wf_ch0);
  input_tree->SetBranchAddress("waveform2", &wf_ch2);
}

void draw(Int_t entry = 0) {

  input_tree->GetEntry(entry);

  canvas->cd(1);
  wf_ch0->SetMarkerStyle(20);
  wf_ch0->SetMarkerSize(0.6);
  wf_ch0->Draw("ap");

  canvas->cd(2);
  wf_ch2->SetMarkerStyle(20);
  wf_ch2->SetMarkerColor(kRed);
  wf_ch2->SetMarkerSize(0.6);
  wf_ch2->Draw("ap");

  canvas->Draw();
  canvas->Update();
}
