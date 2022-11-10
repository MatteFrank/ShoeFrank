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

#include "TASTntuHit.hxx"
#include "TATWntuHit.hxx"

#endif


// main
void ProjToF(TString fileNameIn = "Run_400cebr3_1200pla_2GS_24.6MeV.root")
//void ProjToF(TString fileNameIn = "Run_800Cebr3_900Labr_60Co137Cs.root")
{
   //Histo
   Int_t bins = 500;
   TH1F* h = new TH1F("h", "TOF coinc", bins, -5000, 5000);

   // TAGropot
   TAGroot tagr;
   
   // Branch setting
   TASTntuHit *stRaw    = new TASTntuHit();
   TAGdataDsc* dscStRaw = new TAGdataDsc("stRaw", stRaw);

   TATWntuHit *twRaw    = new TATWntuHit();
   TAGdataDsc* dsctwRaw = new TAGdataDsc("twRaw", twRaw);

   TAGactTreeReader* vtActReader = new TAGactTreeReader("vtActEvtReader");

   vtActReader->SetupBranch(dscStRaw, TASTntuHit::GetBranchName());
   vtActReader->SetupBranch(dsctwRaw, TATWntuHit::GetBranchName());
   
   vtActReader->Open(fileNameIn);
   
   tagr.AddRequiredItem(vtActReader);

   Int_t nentries = vtActReader->NEvents();
   
   tagr.BeginEventLoop();
   
   for (Int_t i = 0; i < nentries; ++i) {
      
      if (!tagr.NextEvent() ) break;
      
      TASThit* hitS = stRaw->GetHit();
      if (hitS == 0x0) continue;
      TATWhit* hitW = twRaw->GetHit();
      if (hitW == 0x0) continue;
      
      Float_t timeS = hitS->GetTime();
      Float_t timeW = hitW->GetTime();
      
      Float_t amplS = hitS->GetAmplitude();
    //  if (amplS < 20 || amplS > 40) continue;
      
      Float_t amplW = hitW->GetAmplitude();
     // if (amplW < 41 || amplW > 50) continue;
         
      h->Fill(timeW-timeS);
   }
   
   h->Draw();
   tagr.EndEventLoop();   
}



