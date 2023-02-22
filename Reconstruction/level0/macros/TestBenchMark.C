// Macro to plot tracked cluster size with and w/o target for FIRST data
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TString.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TLegend.h>

#endif

const Float_t sigma = 0.68;
std::map<TString, std::vector<TString> > mapName = {{"VT", {"vtClusPixelTot"}}, {"MSD", {"msClusStripTot"}}};

// main
void TestBenchMark(Bool_t rawData = true)
{
   TString nameRef;
   TString name;
   
   if (rawData) {
      nameRef = "./data/runGSI2021_4306_RefPlots_1kEvts.root";
      name = "runGSI2021_4306_Plots_1kEvts.root";
   } else {
      nameRef = "./data/runGSI2021_MC_400_RefPlots_1kEvts.root";
      name = "runGSI2021_MC_400_Plots_1kEvts.root";
   }
   
   TFile *fRefPlots = new TFile(nameRef.Data());
   TFile *fPlots = new TFile(name.Data());

   
   for (auto const& it : mapName) {
      
      auto vec = it.second;
      for (auto const& itv : vec) {
         
         TString nameHist = Form("%s/%s", it.first.Data(), itv.Data());
         TH1F* hPixTotRef = (TH1F*)fRefPlots->Get(nameHist.Data());
         TH1F* hPixTot = (TH1F*)fPlots->Get(nameHist.Data());
         
         printf("PValue for detector %s:\n", it.first.Data());
         
         Float_t PValue =  hPixTotRef->Chi2Test(hPixTot,"UU P");
         
         if (PValue < sigma) printf("Cluster distribution changes for detector %s\n", it.first.Data());
      }
   }
}


