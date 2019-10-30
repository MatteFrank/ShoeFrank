/*!
 \file
 \version $Id: TAVTdigitizerE.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAVTdigitizerE.
 */
#include "TMath.h"
#include "TGeoMatrix.h"
#include "TLegend.h"
#include "TList.h"
#include "TF1.h"
#include "TStyle.h"
#include "TRandom2.h"

#include "TAVTparGeo.hxx"
#include "TAVTdigitizerE.hxx"

/*!
 \class TAVTdigitizerE 
 \brief digitizer for pixel **
 */

ClassImp(TAVTdigitizerE);

using namespace std;

Int_t   TAVTdigitizerE::fgkMaxTurn      = 10;
Float_t TAVTdigitizerE::fgkFactorRad    = 1.71;

//00
Int_t   TAVTdigitizerE::fgkShel00[]     = {4,  12,    16,    32,    44,    52,    76,    88,   112,   120 };
Int_t   TAVTdigitizerE::fgkDeltaRad00[] = {0,   0,     0,     0,     6,     0,     8,     8,    12,    10 };

//55
Int_t   TAVTdigitizerE::fgkShel55[]     = {5,   9,    13,    21,    25,    45,    69,   101,   109,   113 };
Int_t   TAVTdigitizerE::fgkDeltaRad55[] = {0,   0,     0,     0,     0,     0,     0,     4,     8,     7 };

//05
Int_t   TAVTdigitizerE::fgkShel50[]     = {2,   8,    12,    26,    40,    44,    61,    82,    94,   104 };
Int_t   TAVTdigitizerE::fgkDeltaRad50[] = {0,   0,     0,     0,     2,     0,     1,     4,     8,     4 };

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTdigitizerE::TAVTdigitizerE(TAGparaDsc* parGeo)
: TAVTbaseDigitizer(parGeo),
  fDeltaRad(0x0)
{

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTdigitizerE::~TAVTdigitizerE()
{

}

// --------------------------------------------------------------------------------------
Bool_t TAVTdigitizerE::MakeCluster(Double_t x0, Double_t y0, Double_t /*zin*/, Double_t /*zout*/)
{
   // std array
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpParGeo->Object();

   fMap.clear();
   fDistance.clear();
   std::pair<int, float> pairDistance;
   
   if (TMath::Abs(x0) > fPitchX*fPixelsNx/2.) {
      // if ( GlobalPar::GetPar()->Debug() > 0 )   
         cout << "WARNING :: TAVTdigitizerE::MakeCluster  -->  TMath::Abs(x0) > fPitchX*fPixelsNx/2 is false. x0 = " << TMath::Abs(x0) << " fPitchX "<<fPitchX<<" fPixelsNx "<<fPixelsNx<< endl;
      return false;
   }
   if (TMath::Abs(y0) > fPitchY*fPixelsNy/2.) { 
      // if ( GlobalPar::GetPar()->Debug() > 0 )   
         cout << "WARNING :: TAVTdigitizerE::MakeCluster  -->  TMath::Abs(y0) > fPitchY*fPixelsNy/2 is false. y0 = "  << TMath::Abs(y0) << " fPitchY "<<fPitchY<<" fPixelsNy "<<fPixelsNy<< endl;
      return false;
   }
   
   Int_t line0 = GetLine(y0);
   Int_t col0  = GetColumn(x0);
   
   if (fDebugLevel)
      printf("%d %d %d\n", fPixelsN, line0, col0);
   
   // why this? WE already have fPitchX fPitchY...
   Float_t pitchX  = GetPitchX();
   Float_t pitchY  = GetPitchY();
   
   // choose the cluster version
   if (!SetRegion(x0, y0)) {
      cout << "WARNING :: TAVTdigitizerE::MakeCluster  -->  SetRegion false" << endl;
      return false;
   }
      
   
   // define radii
   Int_t rpixels      = fPixelsN;
   Int_t lastTurn     = GetLastShell(fShel, fgkMaxTurn);
   Int_t shellPixels  = fShel[lastTurn];
   rpixels            = shellPixels - rpixels;
   
   Int_t sigma        = Int_t(TMath::Sqrt(shellPixels)/2.+1);
   Float_t radiusLim  = TMath::Sqrt(shellPixels - fDeltaRad[lastTurn])/fgkFactorRad*pitchX;
   
   // fill map
   // x1 - y1
   Int_t col1  = col0  - sigma;
   Int_t line1 = line0 + sigma;
   Float_t x1  = pGeoMap->GetPositionU(col1);
   Float_t y1  = pGeoMap->GetPositionV(line1);
   
   // x2 - y2
   Int_t col2  = col0  + sigma;
   Int_t line2 = line0 - sigma;
   Float_t x2  = pGeoMap->GetPositionU(col2);
   Float_t y2  = pGeoMap->GetPositionV(line2);
   
   if (fDebugLevel) {
      printf("%.1f %.1f %d %d\n", x1, x2, col1, line1);
      printf("%.1f %.1f %d %d\n", y1, y2, col2, line2);
   }
   
   for (Float_t y = y1; y <= y2+pitchY/4.; y+= pitchY) {
      for (Float_t x = x1; x <= x2+pitchX/4.; x+= pitchX) {
         
         Int_t line   = GetLine(y);
         Int_t col    = GetColumn(x);
         Int_t idx    = GetIndex(line, col);
         Float_t A    = (x-x0)/radiusLim;
         Float_t B    = (y-y0)/radiusLim;
         Float_t rad2 =  A*A + B*B;;
         
         if (rad2 < 1.) {
            pairDistance.first  = idx;
            pairDistance.second = TMath::Sqrt(rad2);
            fMap[idx] = 1;
            fDistance.push_back(pairDistance);
         }
      }
   }
   
   // remove pixels until the pixels number is reached
   RemovePixels(rpixels);
   
   return true;}


//_____________________________________________________________________________
Bool_t TAVTdigitizerE::SetRegion(Float_t x0, Float_t y0)
{
   Int_t regX  = GetColRegion(x0);
   Int_t regY  = GetLineRegion(y0);
   
   if (fDebugLevel) {
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
      fShel     = fgkShel55;
      fDeltaRad = fgkDeltaRad55;
      
   } else if ((regX == 0 && regY == 0) || (regX == 1 && regY == 1) || (regX == 1 && regY == 0) || (regX == 0 && regY == 1)) {
      fShel     = fgkShel00;
      fDeltaRad = fgkDeltaRad00;
      
      
   } else if ((regX == 5 && regY == 0) || (regX == 5 && regY == 1)) {
      fShel     = fgkShel50;
      fDeltaRad = fgkDeltaRad50;
      
   } else if ((regX == 0 && regY == 5) || (regX == 1 && regY == 5)) {
      fShel     = fgkShel50;
      fDeltaRad = fgkDeltaRad50;
      
   } else
      return false;
   
   return true;
}

//_____________________________________________________________________________
void TAVTdigitizerE::RemovePixels(Int_t rpixels)
{
   // remove pixels
   if (rpixels != 0) {
      
      std::vector<int> rarray;
      
      for (Int_t i = 0; i < int(rpixels*1.3); ++i) {
         if (i > rpixels-1)
            rarray.push_back(0);
         else
            rarray.push_back(1);
      }
      
      std::random_shuffle(rarray.begin(), rarray.end(), GetRandom);
      
      Int_t r = 0;
      std::sort(fDistance.begin(), fDistance.end(), SortBack);
      
      for (std::vector<std::pair<int, float> >::iterator it=fDistance.begin(); it!=fDistance.end(); ++it) {
         if (rarray[r] == 1) {
            fMap.erase(it->first);
         }
         if (r > rpixels+1)
            break;
         r++;
      }
   }
}


