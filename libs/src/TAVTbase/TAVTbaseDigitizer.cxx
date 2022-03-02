/*!
 \file TAVTbaseDigitizer.cxx
 \brief   Implementation of TAVTbaseDigitizer.
 */
#include "TMath.h"
#include "TGeoMatrix.h"
#include "TLegend.h"
#include "TList.h"
#include "TF1.h"
#include "TStyle.h"
#include "TRandom2.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTbaseDigitizer.hxx"

/*!
 \class TAVTbaseDigitizer 
 \brief digitizer for pixel **
 */

Bool_t   TAVTbaseDigitizer::fgSmearFlag     = true;
Float_t  TAVTbaseDigitizer::fgDefSmearPos   = 10.35;  // in micron
Float_t  TAVTbaseDigitizer::fgkPairCreation = 3.6e-3; // keV
Float_t  TAVTbaseDigitizer::fgkFanoFactor   = 0.115;
Float_t  TAVTbaseDigitizer::fgkNormFactor   = TMath::Sqrt(2*TMath::Pi());
Float_t  TAVTbaseDigitizer::fgTotMaxValue   = 100;
Int_t    TAVTbaseDigitizer::fgTotAdcDepth   = 7;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] parGeo geometry parameters
TAVTbaseDigitizer::TAVTbaseDigitizer(TAVTbaseParGeo* parGeo)
 : TAGbaseDigitizer(),
   fpParGeo(parGeo),
   fPixelsN(-1),
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
   fLogWidthPar(0.075),
   fLogWidthParErr(0.008),
   fCstWidthPar(0.81),
   fCstWidthParErr(0.05),
   fZcstWidthPar(0.925),
   fZgainWidthPar(0.0375),
   fTotConversion(0.73),
   fTotExponent(0.6),
   fTotThres(0.5)
{
   SetFunctions();
   fPitchX   = fpParGeo->GetPitchX()*TAGgeoTrafo::CmToMu();
   fPixelsNx = fpParGeo->GetPixelsNx();
   fPitchY   = fpParGeo->GetPitchY()*TAGgeoTrafo::CmToMu();
   fPixelsNy = fpParGeo->GetPixelsNy();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTbaseDigitizer::~TAVTbaseDigitizer()
{
   delete fFuncClusterSize;
   if (fpParGeo->GetType() == 1) {
      delete fFuncClusterHeight;
      delete fFuncClusterWidth;
   }
}

//------------------------------------------+-----------------------------------
//! Fill pixel signal
//!
//! \param[in] edep energy loss
//! \param[in] x0  impact position in X direction
//! \param[in] y0  impact position in Y direction
//! \param[in] zin impact position in z direction
//! \param[in] zout outgoing position in z direction
//! \param[in] time impact time
//! \param[in] sensorId sensor id
//! \param[in] Z atomic charge of the particle
//! \param[in] px0 momentum in X direction
//! \param[in] py0 momentum in Y direction
//! \param[in] pz0 momentum in z direction
Bool_t TAVTbaseDigitizer::Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin, Double_t zout, Double_t /*time*/, Int_t /*sensorId*/, Int_t Z, Double_t /*px0*/, Double_t /*py0*/, Double_t /*pz0*/)
{
   x0 *= TAGgeoTrafo::CmToMu();
   y0 *= TAGgeoTrafo::CmToMu();
   
   Double_t  smear = 0;

   // cluster size
   Double_t deltaE = edep*TAGgeoTrafo::GevToKev();

   smear = gRandom->Gaus(0, fRsParErr);
   fFuncClusterSize->SetParameter(0, fRsPar+smear);

   smear = gRandom->Gaus(0, fThresParErr);
   fFuncClusterSize->SetParameter(1, fThresPar+smear);
   
   smear = gRandom->Gaus(0, fDe0Par);
   fFuncClusterSize->SetParameter(2, fDe0Par+smear);
   
   smear = gRandom->Gaus(0, fLinParErr);
   fFuncClusterSize->SetParameter(3, fLinPar+smear);
   
   fPixelsN = TMath::Nint(fFuncClusterSize->Eval(deltaE));
   if (fPixelsN <= 0) fPixelsN = 1;
   
   if (fpParGeo->GetType() == 1) {
      Float_t electronsN = deltaE/fgkPairCreation;
      
      // cluster height
//      smear = gRandom->Gaus(0, fCstHeightParErr);
//      fFuncClusterHeight->SetParameter(0, fCstHeightPar+smear);
//
//      smear = gRandom->Gaus(0, fLogHeightParErr);
//      fFuncClusterHeight->SetParameter(1, fLogHeightPar+smear);
//
//      smear = 0;
//      fFuncClusterHeight->SetParameter(2, fZcstHeightPar+smear);
//
//      smear = 0;
//      fFuncClusterHeight->SetParameter(3, fZgainHeightPar+smear);
//
//      fClusterHeight = fFuncClusterHeight->Eval(deltaE, Z);
//      if (fClusterHeight <= 1) fClusterHeight = 1.;
      
      // cluster width
      smear = gRandom->Gaus(0, fCstWidthParErr);
      fFuncClusterWidth->SetParameter(0, fCstWidthPar+smear);
      
      smear = gRandom->Gaus(0, fLogWidthParErr);
      fFuncClusterWidth->SetParameter(1, fLogWidthPar+smear);
      
      smear = 0;
      fFuncClusterWidth->SetParameter(2, fZcstWidthPar+smear);

      smear = 0;
      fFuncClusterWidth->SetParameter(3, fZgainWidthPar+smear);

      fClusterWidth = fFuncClusterWidth->Eval(deltaE, Z);
      if (fClusterWidth <= 0) fClusterWidth = 0.8;
      
      // conversion Tot
      smear = 0;
      fFuncTotAnalog->SetParameter(0, fTotConversion+smear);
      fFuncTotDigital->SetParameter(0, fTotConversion+smear);

      // Tot threshold
      fFuncTotAnalog->SetParameter(1, fTotThres+smear);
      fFuncTotDigital->SetParameter(1, fTotThres+smear);

      //Tot exponent
      fFuncTotAnalog->SetParameter(2, fTotExponent+smear);
      fFuncTotDigital->SetParameter(2, fTotExponent+smear);
      
      //Tot Adc depth
      fFuncTotDigital->SetParameter(3, fgTotAdcDepth);
      
      // Tot max value
      fFuncTotDigital->SetParameter(4, fgTotMaxValue);
      
      Double_t value = fFuncTotAnalog->Eval(electronsN);
      fClusterHeight = value/(fClusterWidth*fgkNormFactor);
      if (fClusterHeight <= 1) fClusterHeight = 1.;
      
      if(FootDebugLevel(1))
         printf("edep: %g PixelsN: %d elec %d value %g\n", deltaE, fPixelsN, int(electronsN), value);

   }
   
   if(FootDebugLevel(1)) {
      printf("\nnext hit:\n");
      printf("eloss %6.1f pixels %d\n", deltaE, fPixelsN);
   }
   
   if (fgSmearFlag) {
      Float_t dx = gRandom->Gaus(0, fgDefSmearPos);
      Float_t dy = gRandom->Gaus(0, fgDefSmearPos);
      x0 += dx;
      y0 += dy;
   }
   
   return MakeCluster(x0, y0, zin, zout);
}


// --------------------------------------------------------------------------------------
//! Set functions
void  TAVTbaseDigitizer::SetFunctions()
{
   // compute cluster size for a given Edep, x and y
   fFuncClusterSize   = new TF1("ClusterSize",   this, &TAVTbaseDigitizer::FuncClusterSize,   0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterSize");
   if (fpParGeo->GetType() == 1) {
      fFuncClusterHeight = new TF1("ClusterHeight", this, &TAVTbaseDigitizer::FuncClusterHeight, 0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterHeight");
      fFuncClusterWidth  = new TF1("ClusterWidth",  this, &TAVTbaseDigitizer::FuncClusterWidth,  0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterWidth");
      fFuncTotAnalog   = new TF1("TotAnalog", this, &TAVTbaseDigitizer::FuncTotAnalog, 0, 2000, 3, "TAVTbaseDigitizer", "FuncTotAnalog");
      fFuncTotDigital  = new TF1("TotDigital", this, &TAVTbaseDigitizer::FuncTotDigital, 0, 2000, 5, "TAVTbaseDigitizer", "FuncTotDigital");
   }
}

// --------------------------------------------------------------------------------------
//! Cluster size function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TAVTbaseDigitizer::FuncClusterSize(Double_t* x, Double_t* par)
{
   Float_t xx = x[0]-par[2];
   
   Float_t f = 2*TMath::Pi()*par[0]*TMath::Log(xx/(2*TMath::Pi()*3.6e-3*par[1])) + par[3]*xx;
   
   return f;
}

// --------------------------------------------------------------------------------------
//! Cluster height function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TAVTbaseDigitizer::FuncClusterHeight(Double_t* x, Double_t* par)
{
   Float_t de = x[0];
   Float_t z  = x[1];
   
   Float_t f = (par[0]+par[1]*TMath::Log(de))*(par[2] + par[3]*z);

   return f;
}

// --------------------------------------------------------------------------------------
//! Cluster width function
//!
//! \param[in] x position vector
//! \param[in] par parameters vector
Double_t TAVTbaseDigitizer::FuncClusterWidth(Double_t* x, Double_t* par)
{
   Float_t de = x[0];
   Float_t z  = x[1];

   Float_t f = (par[0]+par[1]*TMath::Log(de))*(par[2] + par[3]*z);
   
   return f;
}

// --------------------------------------------------------------------------------------
//! Time over threshold analog function
//!
//! Models the estimate of a the input charge qin (in ke-)
//!  with the time-over-threshold (ToT) technique:
//!  ToT = conversion-factor * (qin-threshold)^exponent
//!
//! \param[in] qin number of electrons
//! \param[in] par parameters vector
//!
//!  par[0] = conversion factor e- -> arbitrary unit
//!  par[1] = threshold in ke-
//!  par[2] = exponent for the ToT model
Double_t TAVTbaseDigitizer::FuncTotAnalog(Double_t *qin, Double_t *par) {
   
   double width = par[0]*pow(qin[0]-par[1],par[2]);
   
   return width;
}

// --------------------------------------------------------------------------------------
//! Time over threshold digital function
//!
//! Models the estimate of a the input charge qin (in ke-)
//!  with the time-over-threshold (ToT) technique:
//!  ToT = conversion-factor * (qin-threshold)^exponent
//!
//! \param[in] qin number of electrons
//! \param[in] par parameters vector
//!
//!  par[0] = conversion factor e- -> arbitrary unit
//!  par[1] = threshold in ke-
//!  par[2] = exponent for the ToT model
//!  par[3] = number of bits for digitization
//!  par[4] = maximal value for digitization
Double_t TAVTbaseDigitizer::FuncTotDigital(Double_t *qin, Double_t *par) {
  
   Double_t width = FuncTotAnalog(qin, par);
   
   return DigitizeAdc( width, (Int_t)par[3], par[4]);
}

//-----------------------------------------------------
//! Digitize a  value over nbits bits, whith maxValue as the maximum value
//!
//!  note that any value>maxValue returns 0 (no clipping)
//!
//! \param[in] value value
//! \param[in] nbits number of bits in ADC
//! \param[in] maxValue maximum value available
Int_t TAVTbaseDigitizer::DigitizeAdc( Double_t value, Int_t nbits, Double_t maxValue)
{
   Int_t nbDigitalValues = pow(2, nbits);
   Double_t binWidth = maxValue / nbDigitalValues;
   
   if ( value < maxValue ) { return (Int_t)(value / binWidth); }
   else { return 0;}
   
}

//_____________________________________________________________________________
//! Get column number from x position
//!
//! \param[in] x position in X
Int_t TAVTbaseDigitizer::GetColumn(Float_t x) const
{
   Float_t xmin = -fPixelsNx*fPitchX/2.;
   
   if (x < xmin || x > -xmin) {
      if(FootDebugLevel(1))
         Warning("GetColumn()", "Value of X: %f out of range +/- %f\n", x, xmin);
      return -1;
   }
   
   Int_t col = floor((x-xmin)/fPitchX);
   return col;
}

//_____________________________________________________________________________
//! Get line number from y position
//!
//! \param[in] y position in Y
Int_t TAVTbaseDigitizer::GetLine(Float_t y) const
{
   // equivalent to  floor((-y-ymin)/fPitchV)-1
   Float_t ymin = -fPixelsNy*fPitchY/2.;
   
   if (y < ymin || y > -ymin) {
     if(FootDebugLevel(1))
         Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", y, ymin);
      return -1;
   }
   
   Int_t line = floor((y-ymin)/fPitchY);
   return fPixelsNy - line - 1;
}


//-----------------------------------------+---------------------------------
//! Get index from line and column number
//!
//! \param[in] line a given line
//! \param[in] column a given column
Int_t TAVTbaseDigitizer::GetIndex(Int_t line, Int_t column) const
{
   return line*fPixelsNx + column;
}

//_____________________________________________________________________________
//! Get position in X from column number
//!
//! \param[in] column a given column
Float_t TAVTbaseDigitizer::GetPositionU(Int_t column) const
{
   Float_t x = (float(2*column - fPixelsNx + 1) * fPitchX)/2.;
   return  x;
}

//_____________________________________________________________________________
//! Get position in Y from line number
//!
//! \param[in] line a given line
Float_t TAVTbaseDigitizer::GetPositionV(Int_t line) const
{
   Float_t y = -(float(2*line - fPixelsNy + 1) * fPitchY)/2.;
   return  y;
}

//_____________________________________________________________________________
//! Get colummn modulo number from x position
//!
//! \param[in] x position in X
Float_t TAVTbaseDigitizer::GetColRemainder(Float_t x) const
{
   Float_t xmin = -fPixelsNx*fPitchX/2.;
   
   if (x < xmin || x > -xmin) {
     if(FootDebugLevel(1))
         Warning("GetColumn()", "Value of X: %f out of range +/- %f\n", x, xmin);
      return -1;
   }
   
   Float_t col = (x-xmin)/fPitchX - floor((x-xmin)/fPitchX);
   return col;
}

//_____________________________________________________________________________
//! Get line modulo number from y position
//!
//! \param[in] y position in Y
Float_t TAVTbaseDigitizer::GetLineRemainder(Float_t y) const
{
   // equivalent to  floor((-y-ymin)/fPitchV)-1
   Float_t ymin = -fPixelsNy*fPitchY/2.;
   
   if (y < ymin || y > -ymin) {
      if(FootDebugLevel(1))
         Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", y, ymin);
      return -1;
   }
   
   Float_t line = 1 -((y-ymin)/fPitchY - floor((y-ymin)/fPitchY));
   return line;
}

//_____________________________________________________________________________
//! Get colummn region number from x position
//!
//! \param[in] x position in X
//!
//! \return 0 for hit lower corner of pixel
//! \return 5 for hit middle of pixel
//! \return 1 for hit upper corner of pixel
Int_t TAVTbaseDigitizer::GetColRegion(Float_t x) const
{
   Float_t rem = GetColRemainder(x);
   
   if (rem < 0.33) // hit a corner
      return 0;
   else if (rem > 0.33 && rem < 0.66)
      return 5; // hit the middle of the pixel
   else if (rem > 0.66)
      return 1; // hit the upper cormer
   else
      return -99;
}

//_____________________________________________________________________________
//! Get line region number from Y position
//!
//! \param[in] y position in Y
//!
//! \return 0 for hit lower corner of pixel
//! \return 5 for hit middle of pixel
//! \return 1 for hit upper corner of pixel
Int_t TAVTbaseDigitizer::GetLineRegion(Float_t y) const
{
   Float_t rem = GetLineRemainder(y);
   
   if (rem < 0.33)
      return 0;
   else if (rem > 0.33 && rem < 0.66)
      return 5;
   else if (rem > 0.66)
      return 1;
   else
      return -99;
}

//_____________________________________________________________________________
//! Get last shell
//!
//! \param[in] shell shell vector
//! \param[in] maxTurn maximium number of shells
Int_t TAVTbaseDigitizer::GetLastShell(Int_t* shell, Int_t maxTurn) const
{
   Int_t lastTurn = 0;
   
   while (fPixelsN > shell[lastTurn]) {
      lastTurn++;
      if (lastTurn >= maxTurn) {
         lastTurn = maxTurn;
         break;
      }
   }
   
   return lastTurn;
}
