#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>

auto* canvas = new TCanvas("can", "", 1366, 768);

void wav(const char* filename, Int_t entry = 0) {
  canvas->GetPad(0)->SetGrid();

  auto ifile        = new TFile(filename, "READ");
  TTree* input_tree = nullptr;
  ifile->GetObject("wav", input_tree);
  TGraph* wf_graph = nullptr;
  input_tree->SetBranchAddress("waveform", &wf_graph);

  input_tree->GetEntry(entry);

  wf_graph->SetMarkerStyle(20);
  wf_graph->SetMarkerSize(0.6);
  wf_graph->Draw("ap");

  /* canvas->Draw(); */
  canvas->Update();
}
