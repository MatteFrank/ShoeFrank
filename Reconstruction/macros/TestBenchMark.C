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
const Float_t gTolerance = 0.1;

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
         TH1* histoRef = (TH1*)fRefPlots->Get(nameHist.Data());
         TH1* histo    = (TH1*)fPlots->Get(nameHist.Data());
         
         Float_t PValue = 0;
         cout << left << setw(55)<< Form("Detector %-4s: PValue for %s: ", it.first.Data(), itv.Data());
         if (!nameHist.Contains("TrackClus")) { // only for filled bins number > 2
            PValue   = histoRef->Chi2TestX(histo, chi2, ndf, igood, "UU");
         } else {
            Float_t mean = histo->GetMean();
            Float_t meanRef = histoRef->GetMean();
            if (TMath::Abs(mean - meanRef) < gTolerance)
               PValue = 1.;
         }
         printf("%.3f\n", PValue);
         
         if (PValue < sigma) {
            printf("%s changes for detector %s\n", histoRef->GetTitle(), it.first.Data());
            printf("PatternError\n");
         }
      }
   }
}
