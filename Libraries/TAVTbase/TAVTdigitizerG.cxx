/*!
 \file TAVTdigitizerG.cxx
  \brief VTX gaussian digitizer
 */

#include "TAVTdigitizerG.hxx"
#include "TAVTparGeo.hxx"

#include "TAGrecoManager.hxx"

/*!
 \class TAVTdigitizerG
 \brief VTX gaussian digitizer
 */

using namespace std;

Float_t TAVTdigitizerG::fgkThres    = 0.1;
Float_t TAVTdigitizerG::fgkFWTH     = 2*TMath::Sqrt(2*TMath::Log(1./fgkThres));

// --------------------------------------------------------------------------------------
//! Default constructor
TAVTdigitizerG::TAVTdigitizerG(TAVTbaseParGeo* parGeo)
: TAVTbaseDigitizer(parGeo)
{
   SetFunctions();
}

// --------------------------------------------------------------------------------------
//! Default destructor
TAVTdigitizerG::~TAVTdigitizerG()
{
   delete fFuncClusterDisX;
   delete fFuncClusterDisY;
}

// --------------------------------------------------------------------------------------
//! Make cluster from x0, y0 point
//!
//! \param[in] x0 impact in X direction
//! \param[in] y0 impact in Y direction
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
   
   Float_t sigmaX = fClusterWidth/TMath::Sqrt(6);// taking into account that M18 the pitch /= 2 compared to M28
   Float_t sigmaY = fClusterWidth/TMath::Sqrt(6);

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
         Float_t err = TMath::Sqrt(value*fgkFanoFactor);
         value += gRandom->Uniform(err);
         
         if (value > height*height*fgkThres) { 
            Int_t idx  = GetIndex(x, y);
            if (idx < 0) continue;
            fMap[idx] = GetAdcValue(value);
         }
      }
   }
   
   return true;
}

// --------------------------------------------------------------------------------------
//! Set functions
void TAVTdigitizerG::SetFunctions()
{
   fFuncClusterDisX = new TF1("ClusterDisX", "gaus");
   fFuncClusterDisY = new TF1("ClusterDisY", "gaus");
}

// --------------------------------------------------------------------------------------
//! Convert chare into ADC number
//!
//! \param[in] charge charge value
Int_t TAVTdigitizerG::GetAdcValue(Float_t charge)
{
   return int(fFuncTotDigital->Eval(charge)+0.5);
}
