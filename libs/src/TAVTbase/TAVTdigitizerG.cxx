#include "TAVTdigitizerG.hxx"
#include "TAVTparGeo.hxx"

#include "GlobalPar.hxx"

ClassImp(TAVTdigitizerG);

using namespace std;

Float_t TAVTdigitizerG::fgkFWTH     = 2*TMath::Sqrt(2*TMath::Log(10));
Float_t TAVTdigitizerG::fgThreshold = 0.1;


// --------------------------------------------------------------------------------------
TAVTdigitizerG::TAVTdigitizerG(TAVTbaseParGeo* parGeo)
: TAVTbaseDigitizer(parGeo)
{
   SetFunctions();
}

// --------------------------------------------------------------------------------------
TAVTdigitizerG::~TAVTdigitizerG()
{
   delete fFuncClusterDisX;
   delete fFuncClusterDisY;
}

// --------------------------------------------------------------------------------------
Bool_t TAVTdigitizerG::MakeCluster(Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/)
{
   // std array
   fMap.clear();
      
   if (TMath::Abs(y0) > fPitchX*fPixelsNx/2.) return false;
   if (TMath::Abs(x0) > fPitchY*fPixelsNy/2.) return false;
   
   Int_t line0 = GetLine(y0);
   Int_t col0  = GetColumn(x0);

   if(FootDebugLevel(1))
      printf("%d %d %d\n", fPixelsN, line0, col0);
   
   Float_t pitchX  = GetPitchX();
   Float_t pitchY  = GetPitchY();

   // choose the cluster version
   Int_t regX  = GetColRegion(x0);
   Int_t regY  = GetLineRegion(y0);
   
   Float_t sigmaX = fClusterWidth/2.; //TMath::Sqrt(fPixelsN)/fgkFWTH;
   Float_t sigmaY = fClusterWidth/2.; //TMath::Sqrt(fPixelsN)/fgkFWTH;

   if(FootDebugLevel(1))
      printf("%g\n", fClusterWidth);
   
   Float_t height = TMath::Sqrt(fClusterHeight);
   Float_t sigX   = fgkFWTH*sigmaX;
   Float_t sigY   = fgkFWTH*sigmaY;

   if (regX == 1) regX = 0;
   if (regY == 1) regY = 0;

   fFuncClusterDisX->SetParameters(height, line0+regY/10., sigmaX);
   fFuncClusterDisY->SetParameters(height, col0+regX/10.,  sigmaY);
   
   Int_t xmin = TMath::Nint(line0-sigX);
   Int_t xmax = TMath::Nint(line0+sigX);
   
   Int_t ymin = TMath::Nint(col0-sigY);
   Int_t ymax = TMath::Nint(col0+sigY);
   
   fFuncClusterDisX->SetRange(xmin, xmax);
   fFuncClusterDisY->SetRange(ymin, ymax);
   
   for (Int_t x = xmin;  x <= xmax; ++x) {
      
      for (Int_t y = ymin; y <= ymax; ++y) {
         
         Float_t value = fFuncClusterDisX->Eval(x)*fFuncClusterDisY->Eval(y);
         if (value > height*height*fgThreshold) {
            Int_t idx  = GetIndex(x, y);
            if (idx < 0) continue;
            fMap[idx] = value;
         }
      }
   }
   
   return true;
}

// --------------------------------------------------------------------------------------
void TAVTdigitizerG::SetFunctions()
{
   fFuncClusterDisX = new TF1("fFuncClusterDisX", "gaus");
   fFuncClusterDisY = new TF1("fFuncClusterDisY", "gaus");
}
