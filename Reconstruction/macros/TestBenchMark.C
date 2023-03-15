// Macro to compare plots with Chi2 test
// Ch. Finck


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TFile.h>
#include <TMath.h>
#include <TString.h>
#include <TROOT.h>

#endif

const Float_t sigma = 0.68;
std::map<TString, std::vector<TString> > mapNameRaw = {
                                                      {"BM",   {"bmHitDistribution", "bmTrackTotNumber", "bmTrackNtotHitsxTrack"}},
                                                      {"VT",   {"vtClusPixelTot", "vtTrackEvt", "vtTrackClus"}},
                                                      {"MSD",  {"msClusStripTot", "msTrackEvt", "msTrackClus"}},
                                                      {"TW",   {"twPointMap"}},
                                                      {"FOOT", {"glbTrackEvt", "glbTrackClus"}}
                                                      };

std::map<TString, std::vector<TString> > mapNameMC = {
                                                     {"BM",   {"bmHitDistribution", "bmTrackTotNumber", "bmTrackNtotHitsxTrack"}},
                                                     {"VT",   {"vtClusPixelTot", "vtTrackEvt", "vtTrackClus"}},
                                                     {"MSD",  {"msClusStripTot", "msTrackEvt", "msTrackClus"}},
                                                     {"TW",   {"twPointMap", "twdE_vs_Tof_LayerX_MCrec", "twdE_vs_Tof_LayerY_MCrec", "twZID_MCtrue_LayerX", "twZID_MCtrue_LayerY"}} ,
                                                     {"FOOT", {"glbTrackEvt", "glbTrackClus"}}
                                                     };

// main
void TestBenchMark(Bool_t rawData = true)
{
   TString nameRef;
   TString name;
   
   if (rawData) {
      nameRef = "./data/runGSI2021_4306_RefPlots_1kEvts.root";
      name    = "runGSI2021_4306_Plots_1kEvts.root";
   } else {
      nameRef = "./data/runGSI2021_MC_400_RefPlots_1kEvts.root";
      name    = "runGSI2021_MC_400_Plots_1kEvts.root";
   }
   
   TFile *fRefPlots = new TFile(nameRef.Data());
   TFile *fPlots    = new TFile(name.Data());
   
   std::map<TString, std::vector<TString> > mapName;
   
   if (rawData)
      mapName = mapNameRaw;
   else
      mapName = mapNameMC;
   
   Double_t chi2;
   Int_t    ndf;
   Int_t    igood;
   
   for (auto const& it : mapName) {
      
      auto vec = it.second;
      for (auto const& itv : vec) {
         
         TString nameHist = Form("%s/%s", it.first.Data(), itv.Data());
         TH1F* hPixTotRef = (TH1F*)fRefPlots->Get(nameHist.Data());
         TH1F* hPixTot    = (TH1F*)fPlots->Get(nameHist.Data());
         
         cout << left << setw(55)<< Form("Detector %-4s: PValue for %s: ", it.first.Data(), itv.Data());
         Float_t PValue   = hPixTotRef->Chi2TestX(hPixTot, chi2, ndf, igood, "UU");
         printf("%.3f\n", PValue);
         
         if (PValue < sigma && ndf != 0) printf("%s changes for detector %s\n", hPixTotRef->GetTitle(), it.first.Data());
      }
   }
}
