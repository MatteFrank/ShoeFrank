#include "TAMSDdigitizerG.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGrecoManager.hxx"

using namespace std;

Float_t TAMSDdigitizerG::fgkThres         = 0.1;
Float_t TAMSDdigitizerG::fgkFWTH          = 2*TMath::Sqrt(2*TMath::Log(1./fgkThres));
Bool_t  TAMSDdigitizerG::fgSmearFlag      = true;
Float_t TAMSDdigitizerG::fgDefSmearPos    = 25;    // in micron
Float_t TAMSDdigitizerG::fgkPairCreation  = 3.6e-3; // keV
Float_t TAMSDdigitizerG::fgkFanoFactor    = 0.115;
Float_t TAMSDdigitizerG::fgkNormFactor    = TMath::Sqrt(2*TMath::Pi());


// --------------------------------------------------------------------------------------
TAMSDdigitizerG::TAMSDdigitizerG(TAMSDparGeo* parGeo)
 : TAGbaseDigitizer(),
   fDe0Par(-8.7),
   fDe0ParErr(0.2),
   fRsPar(1.39),
   fRsParErr(0.03),
   fThresPar(503),
   fThresParErr(66),
   fLinPar(1.59e-2),
   fLinParErr(0.07e-2),
   fClusterHeight(0.),
   fLogHeightPar(665),
   fLogHeightParErr(46),
   fCstHeightPar(2655),
   fCstHeightParErr(250),
   fZcstHeightPar(0.935),
   fZgainHeightPar(0.0325),
   fClusterWidth(0.),
   fCstWidthPar(2.2),
   fCstWidthParErr(0.1),
   fTotConversion(0.73),
   fTotExponent(0.6),
   fTotThres(0.5)
{
   SetFunctions();
   
   fPitch   = fpParGeo->GetPitchX()*TAGgeoTrafo::CmToMu();
   fStripsN = fpParGeo->GetStripsN();
   
}

// --------------------------------------------------------------------------------------
TAMSDdigitizerG::~TAMSDdigitizerG()
{
   delete fFuncClusterDis;
}

//------------------------------------------+-----------------------------------
//! fill pixel signal
Bool_t TAMSDdigitizerG::Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin, Double_t zout, Double_t /*time*/, Int_t sensorId, Int_t Z, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{
   x0 *= TAGgeoTrafo::CmToMu();
   Double_t  smear = 0;
   
   Double_t deltaE = edep*TAGgeoTrafo::GevToKev();
   Float_t electronsN = deltaE/fgkPairCreation;
   
   // cluster height
   smear = gRandom->Gaus(0, fCstHeightParErr);
   fFuncClusterHeight->SetParameter(0, fCstHeightPar+smear);
   
   smear = gRandom->Gaus(0, fLogHeightParErr);
   fFuncClusterHeight->SetParameter(1, fLogHeightPar+smear);
   
   smear = 0;
   fFuncClusterHeight->SetParameter(2, fZcstHeightPar+smear);
   
   smear = 0;
   fFuncClusterHeight->SetParameter(3, fZgainHeightPar+smear);
   
   fClusterHeight = fFuncClusterHeight->Eval(deltaE, Z);
   if (fClusterHeight <= 1) fClusterHeight = 1.;
   
   // cluster width
   smear = gRandom->Gaus(0, fCstWidthParErr);
   fFuncClusterWidth->SetParameter(0, fCstWidthPar+smear);
   
   fClusterWidth = TMath::Nint(fFuncClusterWidth->GetRandom());
   
   if (fClusterWidth <= 0) fClusterWidth = 0.8;

   if(FootDebugLevel(1)) {
      printf("\nnext hit:\n");
      printf("eloss %6.1f width %.0f\n", deltaE, fClusterWidth);
   }
   
   if (fgSmearFlag) {
      Float_t dx = gRandom->Gaus(0, fgDefSmearPos);
      x0 += dx;
   }
   
   return MakeCluster(x0, y0, zin, zout, sensorId);
}


// --------------------------------------------------------------------------------------
Bool_t TAMSDdigitizerG::MakeCluster(Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/, Int_t sensorId)
{
   // std array
   fMap.clear();
   
   Int_t view  = fpParGeo->GetSensorPar(sensorId).TypeIdx;
   Float_t pos = 0.;
   
   if (view == 0)
      pos = x0;
   else
      pos = y0;
   
   fMap.clear();
   
   if (TMath::Abs(pos) > fPitch*fStripsN/2.) return false;
   
   Int_t strip = GetStrip(pos);
   
   if(FootDebugLevel(1))
      printf("Seed strip %d\n", strip);
   
   // choose the cluster version
   Int_t reg     = GetRegion(pos);
   Float_t sigma = fClusterWidth;
   if (reg == 1) reg = 0;

   if(FootDebugLevel(1))
      printf("%g\n", fClusterWidth);
   
   Float_t height = fClusterHeight;
   Float_t sig    = fgkFWTH*sigma;
   Int_t xmin     = TMath::Nint(strip-sig);
   Int_t xmax     = TMath::Nint(strip+sig);

   fFuncClusterDis->SetParameters(height, strip+reg/10., sigma);
   fFuncClusterDis->SetRange(xmin, xmax);
   
   for (Int_t x = xmin;  x <= xmax; ++x) {
      Float_t value = fFuncClusterDis->Eval(x);
      Float_t err = TMath::Sqrt(value*fgkFanoFactor);
      value += gRandom->Uniform(err);
      
      if (value > height*fgkThres)
         fMap[x] = GetAdcValue(value);
   }
   
   return true;
}

// --------------------------------------------------------------------------------------
void TAMSDdigitizerG::SetFunctions()
{
   fFuncClusterDis = new TF1("ClusterDis", "gaus");
   
   // compute cluster width/height for a given Edep, x and y
   fFuncClusterHeight = new TF1("ClusterHeight", this, &TAMSDdigitizerG::FuncClusterHeight, 0, 2000, 4, "TAMSDdigitizerG", "FuncClusterHeight");
   fFuncClusterWidth  = new TF1("ClusterWidth",  this, &TAMSDdigitizerG::FuncClusterWidth,  0, 2000, 4, "TAMSDdigitizerG", "FuncClusterWidth");
}

// --------------------------------------------------------------------------------------
Int_t TAMSDdigitizerG::GetAdcValue(Float_t charge)
{
   return int(fFuncToDigital->Eval(charge)+0.5);
}

// --------------------------------------------------------------------------------------
Double_t TAMSDdigitizerG::FuncClusterHeight(Double_t* x, Double_t* par)
{
   Float_t de = x[0];
   Float_t z  = x[1];
   
   Float_t f = (par[0]+par[1]*TMath::Log(de))*(par[2] + par[3]*z);
   
   return f;
}

// --------------------------------------------------------------------------------------
Double_t TAMSDdigitizerG::FuncClusterWidth(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t f = TMath::PoissonI(xx, par[0]);
   
   return f;
}

//_____________________________________________________________________________
Int_t TAMSDdigitizerG::GetStrip(Float_t pos) const
{
   Float_t posmin = -fStripsN*fPitch/2.;
   
   if (pos < posmin || pos > -posmin) {
      if(FootDebugLevel(1))
         Warning("GetStrip()", "Value of position: %f out of range +/- %f\n", pos, posmin);
      return -1;
   }
   
   Int_t strip = floor((pos-posmin)/fPitch);
   
   return strip;
}

//_____________________________________________________________________________
Float_t TAMSDdigitizerG::GetRemainder(Float_t pos) const
{
   Float_t posmin = -fStripsN*fPitch/2.;
   
   if (pos < posmin || pos > -posmin) {
      if(FootDebugLevel(1))
         Warning("GetRemainder()", "Value of position: %f out of range +/- %f\n", pos, posmin);
      return -1;
   }
   
   Float_t line = 1 -((pos-posmin)/fPitch - floor((pos-posmin)/fPitch));
   
   return line;
}

//_____________________________________________________________________________
Int_t TAMSDdigitizerG::GetRegion(Float_t pos) const
{
   Float_t rem = GetRemainder(pos);
   
   if (rem < 0.33)
      return 0;
   else if (rem > 0.33 && rem < 0.66)
      return 5;
   else if (rem > 0.66)
      return 1;
   else
      return -99;
}
