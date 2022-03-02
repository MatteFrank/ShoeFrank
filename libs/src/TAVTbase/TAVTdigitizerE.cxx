/*!
 \file TAVTdigitizerE.cxx
 \brief VTX elipsoidal digitizer
 */

#include "TAVTdigitizerE.hxx"
#include "TAVTparGeo.hxx"

#include <math.h>
#include "TVector2.h"
#include "TRandom.h"
#include "TList.h"

#include "TAGrecoManager.hxx"

/*!
 \class TAVTdigitizerE
 \brief VTX elipsoidal digitizer
 */

using namespace std;

Int_t   TAVTdigitizerE::fgkMaxTurn   = 16;
Int_t   TAVTdigitizerE::fgkMeshWidth = 10; // test radius condition in a square of +/- mesh width

//00
Int_t   TAVTdigitizerE::fgkShel00[20];// {4,  12,  16,   24,   32,   44,   52,   76,   88,  112,   120,  148,  164,  188,  208,  248};

//05
Int_t   TAVTdigitizerE::fgkShel50[20];// {2,   6,   8,   16,   26,   34,   44,   56,   74,   86,   104,  124,  142,  166,  188,  216};

//55
Int_t   TAVTdigitizerE::fgkShel55[20];// {1,   5,   9,   13,   21,   37,   45,   57,   69,   89,   101,  121,  137,  161,  185,  213};

// --------------------------------------------------------------------------------------
//! Default constructor
//!
//! \param[in] parGeo geometry parameters
TAVTdigitizerE::TAVTdigitizerE(TAVTbaseParGeo* parGeo)
: TAVTbaseDigitizer(parGeo)
{
   DefineRadii();
}

// --------------------------------------------------------------------------------------
//! Default destructor
TAVTdigitizerE::~TAVTdigitizerE() 
{   
}

// --------------------------------------------------------------------------------------
//! Make cluster from x0, y0 point
//!
//! \param[in] x0 impact in X direction
//! \param[in] y0 impact in Y direction
Bool_t TAVTdigitizerE::MakeCluster(Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/)
{
   // std array
   fMap.clear();

   if (TMath::Abs(x0) > fPitchX*fPixelsNx/2.) return false;
   if (TMath::Abs(y0) > fPitchY*fPixelsNy/2.) return false;
   
   Int_t line0 = GetLine(y0);
   Int_t col0  = GetColumn(x0);

   if(FootDebugLevel(1))
      printf("%d %d %d\n", fPixelsN, line0, col0);
   
   Float_t pitchX  = GetPitchX();
   Float_t pitchY  = GetPitchY();

   // choose the cluster version
   if (!SetRegion(x0, y0))
      return false;
   
   Int_t regX  = GetColRegion(x0);
   Int_t regY  = GetLineRegion(y0);
   if (regX == 5 && regY == 5) {
      // rounding pb
      x0 += (x0 > 0) ? -1 : +1;
      y0 += (y0 > 0) ? -1 : +1;
   }
   
   // define radii
   Int_t rpixels      = fPixelsN;
   if (fPixelsN <= 0) fPixelsN = 1;
  // printf("pixels %d\n", fPixelsN);
   Int_t lastTurn     = GetLastShell(fShel, fgkMaxTurn);
   Int_t shellPixels  = fShel[lastTurn];
   rpixels            = shellPixels - rpixels;

   if (rpixels != 0)lastTurn--;

   // radius
   Int_t closed  = fShel[lastTurn];
   Int_t valence = fShel[lastTurn+1];
   
   Float_t radiusLim  = (*fMapRadius)[closed];
   Float_t radiusLim1 = (*fMapRadius)[valence];
   Int_t sigma        = (radiusLim1/pitchY)*1.5;
   
   // fill map
   // x1 - y1
   Int_t col1  = col0  - sigma;
   Int_t line1 = line0 + sigma;
   
   Float_t x1  = GetPositionU(col1);
   Float_t y1  = GetPositionV(line1);
   
   // x2 - y2
   Int_t col2  = col0  + sigma;
   Int_t line2 = line0 - sigma;
   
   Float_t x2  = GetPositionU(col2);
   Float_t y2  = GetPositionV(line2);
   
   if(FootDebugLevel(1)) {
      printf("%.1f %.1f %d %d\n", x1, x2, col1, line1);
      printf("%.1f %.1f %d %d\n", y1, y2, col2, line2);
   }
   
   std::vector<int> rarray;
   
   for (Float_t y = y1; y <= y2+pitchY/4.; y+= pitchY) {
      for (Float_t x = x1; x <= x2+pitchX/4.; x+= pitchX) {
         
         Int_t line = GetLine(y);
         Int_t col  = GetColumn(x);
         Int_t idx  = GetIndex(line, col);
         if (idx < 0) continue;
         
         Float_t A    = (x-x0);
         Float_t B    = (y-y0);
         Float_t rad2 =  A*A + B*B;;
         
         // inner shell
         if (rad2 <= radiusLim*radiusLim)
            fMap[idx] = 1;
         
         if (rpixels != 0) {
            // from inner shell to next closed shell
            if (rad2 > radiusLim*radiusLim && rad2 < radiusLim1*radiusLim1) {
               fMap[idx] = 1;
               rarray.push_back(idx);
            }
         }
      }
   }
   
   // remove randomly pixels from outer shell
   Int_t r = 0;
   if (rpixels != 1) {
      
      std::random_shuffle(rarray.begin(), rarray.end(), GetRandom);
      
      for (std::vector<int>::iterator it=rarray.begin(); it!=rarray.end(); ++it) {
         
         if (r > rpixels-1)
            break;
         
         fMap.erase(*it);
         r++;
      }
   }
   
   if(FootDebugLevel(1)) {
      printf("pixels input %d\n", fPixelsN);
      printf("shell close  : %d r %4.2f\n", closed, (*fMapRadius)[closed]);
      printf("shell valence: %d r %4.2f\n", valence, (*fMapRadius)[valence]);
      printf("sigma %d\n", sigma);
      printf("Cluster size %lu\n", fMap.size());
   }

   return true;
}

//_____________________________________________________________________________
//! Set region from x0, y0 point
//!
//! \param[in] x0 impact in X direction
//! \param[in] y0 impact in Y direction
Bool_t TAVTdigitizerE::SetRegion(Double_t x0, Double_t y0)
{
   Int_t regX  = GetColRegion(x0);
   Int_t regY  = GetLineRegion(y0);
   
   if(FootDebugLevel(1)) {
      printf("regX: %d\n", regX);
      printf("regY: %d\n", regY);
   }
   
   Float_t pitchX  = GetPitchX();
   Float_t pitchY  = GetPitchY();
   
   Int_t line0 = GetLine(y0);
   Int_t col0  = GetColumn(x0);
   
   // reset position in one of the 9 remarkable position
   switch (regX) {
      case 0:
         x0 += -(GetColRemainder(x0))*pitchX;
         break;
      case 5:
         x0 += (0.5 - GetColRemainder(x0))*pitchX;
         break;
      case 1:
         x0 += (1 - GetColRemainder(x0))*pitchX;
         col0  = GetColumn(x0); // next colummn
         break;
      default:
         break;
   }
   
   switch (regY) {
      case 0:
         y0 +=  (GetLineRemainder(y0))*pitchY;
         break;
      case 5:
         y0 += -(0.5 - GetLineRemainder(y0))*pitchY;
         break;
      case 1:
         y0 += -(1 - GetLineRemainder(y0))*pitchY;
         line0 = GetLine(y0); // next line
         break;
      default:
         break;
   }
   
   if (regX == 5 && regY == 5) {
      fShel      = fgkShel55;
      fMapRadius = &fMapRadius55;
      // rounding pb
      x0 += (x0 > 0) ? -1 : +1;
      y0 += (y0 > 0) ? -1 : +1;
      
   } else if ((regX == 0 && regY == 0) || (regX == 1 && regY == 1) || (regX == 1 && regY == 0) || (regX == 0 && regY == 1)) {
      fShel      = fgkShel00;
      fMapRadius = &fMapRadius00;

   } else if ((regX == 5 && regY == 0) || (regX == 5 && regY == 1)) {
      fShel      = fgkShel50;
      fMapRadius = &fMapRadius50;
      
   } else if ((regX == 0 && regY == 5) || (regX == 1 && regY == 5)) {
      fShel      = fgkShel50;
      fMapRadius = &fMapRadius50;
      
   } else
      return false;
   
   return true;
}

//_____________________________________________________________________________
//! Define all radiii
void TAVTdigitizerE::DefineRadii()
{
   DefineRadius00();
   DefineRadius50();
   DefineRadius55();
}

//_____________________________________________________________________________
//! Define radius 55
void TAVTdigitizerE::DefineRadius55()
{
   Float_t pitchX = GetPitchX();
   Float_t pitchY = GetPitchY();
   
   Int_t limx = fgkMeshWidth;
   Int_t limy = fgkMeshWidth;
   Int_t turn = 0;
   
   for (Float_t r = 0.7*pitchX; r < 8.6*pitchX; r += 0.5*pitchX) {
      
      Int_t n   = 0;
      
      for (int i = -limx ; i <= limx ; i++) {
         for (int j = -limy ; j <= limy ; j++) {
            if ((i*pitchX*i*pitchX) + (j*pitchY*j*pitchY) < r*r) {
               n++;
            }
         }
      }
      fgkShel55[turn++] = n;
      fMapRadius55[n] = r;
   }
}

//_____________________________________________________________________________
//! Define radius 50
void TAVTdigitizerE::DefineRadius50()
{
   Float_t pitchX = GetPitchX();
   Float_t pitchY = GetPitchY();
   
   Int_t limx = fgkMeshWidth;
   Int_t limy = fgkMeshWidth;
   Int_t turn = 0;
   
   for (Float_t r = 0.75*pitchX; r <= 8.75*pitchX; r += 0.5*pitchX) {
      
      Int_t n   = 0;
      
      for (int i = -limx ; i <= limx ; i++) {
         for (int j = -limy ; j <= limy ; j++) {
            if (((i-0.5)*pitchX*(i-0.5)*pitchX) + ((j)*pitchY*(j)*pitchY) < r*r) {
               n++;
            }
         }
      }
      fgkShel50[turn++] = n;
      fMapRadius50[n] = r;
   }
}

//_____________________________________________________________________________
//! Define radius 00
void TAVTdigitizerE::DefineRadius00()
{
   Float_t pitchX = GetPitchX();
   Float_t pitchY = GetPitchY();
   
   Int_t limx = fgkMeshWidth;
   Int_t limy = fgkMeshWidth;
   Int_t turn = 0;
   
   for (Float_t r = 1.25*pitchX; r <= 9.25*pitchX; r += 0.5*pitchX) {
      
      Int_t n   = 0;
      
      for (int i = -limx ; i <= limx ; i++) {
         for (int j = -limy ; j <= limy ; j++) {
            if (((i-0.5)*pitchX*(i-0.5)*pitchX) + ((j-0.5)*pitchY*(j-0.5)*pitchY) < r*r) {
               n++;
            }
         }
      }
      fgkShel00[turn++] = n;
      fMapRadius00[n] = r;
   }
}


