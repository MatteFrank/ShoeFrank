/*!
 \file
 \version $Id: TAVTbaseDigitizer.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTbaseDigitizer.
 */
#include "TMath.h"
#include "TGeoMatrix.h"
#include "TLegend.h"
#include "TList.h"
#include "TF1.h"
#include "TStyle.h"
#include "TRandom2.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTbaseDigitizer.hxx"

/*!
 \class TAVTbaseDigitizer 
 \brief digitizer for pixel **
 */

Bool_t   TAVTbaseDigitizer::fgSmearFlag       = true;
Float_t  TAVTbaseDigitizer::fgDefSmearPos     =  10.35;    // in micron

//------------------------------------------+-----------------------------------
//! Default constructor.
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
   fZgainWidthPar(0.0375)
{
   SetFunctions();
   fPitchX   = fpParGeo->GetPitchX()*TAGgeoTrafo::CmToMu();
   fPixelsNx = fpParGeo->GetNPixelX();
   fPitchY   = fpParGeo->GetPitchY()*TAGgeoTrafo::CmToMu();
   fPixelsNy = fpParGeo->GetNPixelY();
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
//! fill pixel signal
Bool_t TAVTbaseDigitizer::Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin, Double_t zout, Double_t /*time*/, Int_t /*sensorId*/, Int_t Z)
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

   if(FootDebugLevel(1))
      printf("edep: %g PixelsN: %d\n", deltaE, fPixelsN);
   
   if (fpParGeo->GetType() == 1) {
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
      
      smear = gRandom->Gaus(0, fLogWidthParErr);
      fFuncClusterWidth->SetParameter(1, fLogWidthPar+smear);
      
      smear = 0;
      fFuncClusterWidth->SetParameter(2, fZcstWidthPar+smear);

      smear = 0;
      fFuncClusterWidth->SetParameter(3, fZgainWidthPar+smear);

      fClusterWidth = fFuncClusterWidth->Eval(deltaE, Z);
      if (fClusterWidth <= 0) fClusterWidth = 0.8;
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
void  TAVTbaseDigitizer::SetFunctions()
{
   // compute cluster size for a given Edep, x and y
   fFuncClusterSize   = new TF1("ClusterSize",   this, &TAVTbaseDigitizer::FuncClusterSize,   0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterSize");
   if (fpParGeo->GetType() == 1) {
      fFuncClusterHeight = new TF1("ClusterHeight", this, &TAVTbaseDigitizer::FuncClusterHeight, 0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterHeight");
      fFuncClusterWidth  = new TF1("ClusterWidth",  this, &TAVTbaseDigitizer::FuncClusterWidth,  0, 2000, 4, "TAVTbaseDigitizer", "FuncClusterWidth");
   }
}

// --------------------------------------------------------------------------------------
Double_t TAVTbaseDigitizer::FuncClusterSize(Double_t* x, Double_t* par)
{
   Float_t xx = x[0]-par[2];
   
   Float_t f = 2*TMath::Pi()*par[0]*TMath::Log(xx/(2*TMath::Pi()*3.6e-3*par[1])) + par[3]*xx;
   
   return f;
}

// --------------------------------------------------------------------------------------
Double_t TAVTbaseDigitizer::FuncClusterHeight(Double_t* x, Double_t* par)
{
   Float_t de = x[0];
   Float_t z  = x[1];
   
   Float_t f = (par[0]+par[1]*TMath::Log(de))*(par[2] + par[3]*z);

   return f;
}

// --------------------------------------------------------------------------------------
Double_t TAVTbaseDigitizer::FuncClusterWidth(Double_t* x, Double_t* par)
{
   Float_t de = x[0];
   Float_t z  = x[1];

   Float_t f = (par[0]+par[1]*TMath::Log(de))*(par[2] + par[3]*z);
   
   return f;
}

//_____________________________________________________________________________
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
Int_t TAVTbaseDigitizer::GetIndex(Int_t line, Int_t column) const
{
   return line*fPixelsNx + column;
}

//_____________________________________________________________________________
Float_t TAVTbaseDigitizer::GetPositionU(Int_t column) const
{
   Float_t x = (float(2*column - fPixelsNx + 1) * fPitchX)/2.;
   return  x;
}

//_____________________________________________________________________________
Float_t TAVTbaseDigitizer::GetPositionV(Int_t line) const
{
   Float_t y = -(float(2*line - fPixelsNy + 1) * fPitchY)/2.;
   return  y;
}


//_____________________________________________________________________________
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
