#include "TAVTdigitizerG.hxx"
#include "TAVTparGeo.hxx"

#include "GlobalPar.hxx"

ClassImp(TAVTdigitizerG);

using namespace std;

Float_t   TAVTdigitizerG::fgkFWTH = 2*TMath::Sqrt(2*TMath::Log(10));


// --------------------------------------------------------------------------------------
TAVTdigitizerG::TAVTdigitizerG(TAVTbaseParGeo* parGeo)
: TAVTbaseDigitizer(parGeo)
{
   fDistriX = new TF1("fDistriX", "gaus");
   fDistriY = new TF1("fDistriY", "gaus");
}

// --------------------------------------------------------------------------------------
TAVTdigitizerG::~TAVTdigitizerG()
{
   delete fDistriX;
   delete fDistriY;
}

// --------------------------------------------------------------------------------------
Bool_t TAVTdigitizerG::MakeCluster(Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/)
{
   // std array
   fMap.clear();
   
   Float_t FWTH = 2*TMath::Sqrt(2*TMath::Log(10));
   
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
   if (regX == 5 && regY == 5) {
      // rounding pb
      x0 += (x0 > 0) ? -1 : +1;
      y0 += (y0 > 0) ? -1 : +1;
   }
   
   Float_t sigmaX = TMath::Sqrt(fPixelsN)/fgkFWTH;
   Float_t sigmaY = TMath::Sqrt(fPixelsN)/fgkFWTH;

   Float_t h  = 10.;
   Float_t sig = fgkFWTH*sigmaX;
   
   if (regX == 5 && regY == 5) {
      fDistriX->SetParameters(h, line0+regY/10., sigmaX);
      fDistriY->SetParameters(h, col0+regX/10.,  sigmaY);
   } else {
      fDistriX->SetParameters(h, line0, sigmaX);
      fDistriY->SetParameters(h, col0,  sigmaY);
   }
   
   Int_t xmin = TMath::Nint(line0-sig*sigmaX);
   Int_t xmax = TMath::Nint(line0+sig*sigmaY);
   
   Int_t ymin = TMath::Nint(col0-sig*sigmaY);
   Int_t ymax = TMath::Nint(col0+sig*sigmaY);
   
   fDistriX->SetRange(xmin, xmax);
   fDistriY->SetRange(ymin, ymax);
   
   for (Int_t x = xmin;  x <= xmax; ++x) {
      
      for (Int_t y = ymin; y <= ymax; ++y) {
         
         Float_t count = fDistriX->Eval(x)*fDistriY->Eval(y);
         if (count > h/TMath::Sqrt(fgkFWTH)) {
            Int_t idx  = GetIndex(y, x);
            if (idx < 0) continue;
            fMap[idx] = count;
         }
      }
   }
   
   return true;
}
