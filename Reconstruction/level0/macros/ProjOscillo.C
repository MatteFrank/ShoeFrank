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

#include "TAPLntuRaw.hxx"

#endif


// main
void ProjOscillo(Int_t evt = 6, TString fileNameIn = "Run_cible3mm_coinc_2.root")
{
   //Histo
   Int_t bins = 1024;
   TString hName = Form("hOsc_%d", evt);
   TH1F* h = new TH1F(hName.Data(), "oscillo", bins, 0, bins);

   // TAGropot
   TAGroot tagr;
   
   // Branch setting
   TAPLntuRaw *stRaw    = new TAPLntuRaw();
   TAGdataDsc* dscStRaw = new TAGdataDsc("stRaw", stRaw);
   
   TAGactTreeReader* vtActReader = new TAGactTreeReader("vtActEvtReader");
   vtActReader->SetupBranch(dscStRaw, TAPLntuRaw::GetBranchName());
   vtActReader->Open(fileNameIn);
   
   tagr.AddRequiredItem(vtActReader);

   Int_t nentries = vtActReader->NEvents();
   
   tagr.BeginEventLoop();
   
   for (Int_t i = 0; i < nentries; ++i) {
      
      if (!tagr.NextEvent() ) break;
      
      if (i == evt) {
         Int_t samples = stRaw->GetSamplesN();
         TASTrawHit* hit = stRaw->GetHit();
         if (hit) {
            for (Int_t s = 0; s < samples; ++s)
               h->SetBinContent(s+1, hit->GetVectA(s));
         }
         printf("sample %d for evt %d\n", samples, evt);
         h->Draw();
         break;
      }
   }
   tagr.EndEventLoop();
   
}



