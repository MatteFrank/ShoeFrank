// Macro to fit LaBr3 spectra
// Ch. Finck, sept 08.


#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <Getline.h>
#include "TMath.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TVirtualPad.h"

#include "TAGactTreeReader.hxx"
#include "TAGactTreeWriter.hxx"

#include "TAPLntuHit.hxx"
#include "TACEntuHit.hxx"

#endif


// main
void ProjSpec(TString fileNameIn = "Run_400cebr3_1200pla_2GS_24.6MeV.root", Bool_t twflag = true)
{
   //Histo
   Int_t bins = 1024;
   TH1F* hSt = new TH1F("hSt", "ST - Amplitude", bins, 0, 1000);
   TH1F* hTw = new TH1F("hTw", "TW - Amplitude", bins, 0, 1000);

   // TAGropot
   TAGroot tagr;
   
   // Branch setting
   TAPLntuHit *stRaw    = new TAPLntuHit();
   TAGdataDsc* dscStRaw = new TAGdataDsc("stRaw", stRaw);

   TACEntuHit *twRaw    = new TACEntuHit();
   TAGdataDsc* dsctwRaw = new TAGdataDsc("twRaw", twRaw);

   TAGactTreeReader* vtActReader = new TAGactTreeReader("vtActEvtReader");

   vtActReader->SetupBranch(dscStRaw, TAPLntuHit::GetBranchName());
   if (twflag)
      vtActReader->SetupBranch(dsctwRaw, TACEntuHit::GetBranchName());
   
   vtActReader->Open(fileNameIn);
   
   tagr.AddRequiredItem(vtActReader);

   Int_t nentries = vtActReader->NEvents();
   
   tagr.BeginEventLoop();
   
   for (Int_t i = 0; i < nentries; ++i) {
      
      if (!tagr.NextEvent() ) break;
      
      TAPLhit* hit = stRaw->GetHit();
      Float_t amp = hit->GetAmplitude();
      hSt->Fill(amp);
      
      
      if (twflag) {
         TACEhit* hit = twRaw->GetHit();
         Float_t amp = hit->GetAmplitude();
         hTw->Fill(amp);
      }
   }
   
   TCanvas* c1 = new TCanvas();
   
   if (twflag) {
      c1->Divide(2,1);
      c1->cd(1);
      hSt->Draw();
      c1->cd(2);
      hTw->Draw();
   } else {
      hSt->Draw();
   }
      
   tagr.EndEventLoop();
   
}



