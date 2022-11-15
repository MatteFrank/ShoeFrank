/*!
  \file
  \version $Id: TAGbaseWC.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TAGbaseWC.
*/

#include <string.h>
#include <fstream>
#include <bitset>
#include <numeric>
#include <functional>

#include <algorithm>
#include "TString.h"
#include "TAGbaseWC.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TAGbaseWC TAGbaseWC.hxx "TAGbaseWC.hxx"
*/

using namespace std;

ClassImp(TAGbaseWC);

 Int_t   TAGbaseWC::fgNoiseBinMax     = 10;
 Float_t TAGbaseWC::fgNoiseSigmaLevel = 5.;
 Float_t TAGbaseWC::fgCFDfac          = 0.4;
 Float_t TAGbaseWC::fgCFDdelay        = 15; // bin
 Float_t TAGbaseWC::fgPWfast          = 120; // ns

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGbaseWC::TAGbaseWC()
 : TObject(),
   fChId(-1),
   fAmplitude(0.),
   fCharge(0.),
   fChargeFast(0.),
   fChargeSlow(0.),
   fBaseline(0.),
   fRiseTime(0.),
   fFallTime(0.),
   fTimeLE(0.),
   fTimeCFD(0.),
   fTriggerTypeId(0),
   fIdPhoswich(false),
   fMcTrackCount(0)
{
}

//------------------------------------------+-----------------------------------
TAGbaseWC::TAGbaseWC(TAGwaveCatcher *W, Bool_t pwFlag)
 : TObject(),
   fChId(-1),
   fAmplitude(0.),
   fCharge(0.),
   fChargeFast(0.),
   fChargeSlow(0.),
   fBaseline(0.),
   fRiseTime(0.),
   fFallTime(0.),
   fTimeLE(0.),
   fTimeCFD(0.),
   fTriggerTypeId(0),
   fIdPhoswich(pwFlag),
   fMcTrackCount(0)
{
   ComputeInfo(W);
}

//------------------------------------------+-----------------------------------
TAGbaseWC::~TAGbaseWC()
{

}

//------------------------------------------+-----------------------------------
void TAGbaseWC::SetHit(TAGwaveCatcher *W, Bool_t pwFlag)
{
   Clear();
   fIdPhoswich = pwFlag;
   ComputeInfo(W);
}

//______________________________________________________________________________
//
void TAGbaseWC:: AddMcTrackId(Int_t trackId)
{
   fMcTrackId[fMcTrackCount++] = trackId;
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGbaseWC::Clear(Option_t*)
{
   fChId=-1;
   fAmplitude=0.;
   fCharge=0.;
   fBaseline=0.;
   fRiseTime=0.;
   fFallTime=0.;
   fTimeLE=0.;
   fTimeCFD=0.;
   fTriggerTypeId=0;
   fMcTrackCount=0;
}

//------------------------------------------+-----------------------------------
void TAGbaseWC::ComputeInfo(TAGwaveCatcher* w)
{
   //Id
   fChId = w->GetChannelId();

   // baseline
   Int_t xBinMax = fgNoiseBinMax;

   float* noise = new float[xBinMax];
   memcpy(noise, w->GetVectA(), sizeof(float)*xBinMax);
   
   float a0 = TMath::Mean(xBinMax, noise);
   float sigma = TMath::RMS(xBinMax, noise);
   
   delete[] noise;
   
   Int_t xmin = 0.;
   Int_t xmax = 0.;
   Float_t sigLevel = fgNoiseSigmaLevel;
   Float_t pedestal = 0;

   // look for begining of rising
   for (Int_t i = xBinMax; i <  w->GetSamplesN(); ++i) {
      if (w->GetVectA(i) < a0 - sigLevel*sigma) {
         xmin = i;
         break;
      }
   }

   // search for first minimum
   Float_t peak = 999999;
   Int_t minBin = -1;
   Int_t count = 0; // count the number of index afer the first minimum
   for (Int_t i = xBinMax; i <  w->GetSamplesN(); ++i) {
      if (w->GetVectA(i) < a0 - sigLevel*sigma)
         count++;
      
      if (w->GetVectA(i) < peak) {
         peak = w->GetVectA(i);
         minBin = i;
         count = 0;
      }
      
      if (count > 10) break;
   }
   
   // look for end of falling
   for (Int_t i = minBin; i <  w->GetSamplesN(); ++i) {
      if ( w->GetVectA(i) > a0 - sigLevel*sigma) {
         xmax = i;
         break;
      }
   }

   // Computing
   Float_t t0 = w->GetVectT(xmin);
   float sum = 0.;
   float sumFast = 0.;
   float sumSlow = 0.;

   for (Int_t i = xmin; i < xmax; ++i) {
      sum += w->GetVectA(i);
      
      if (fIdPhoswich) {
         if (w->GetVectT(i) - t0 < fgPWfast)
            sumFast += w->GetVectA(i);
         else
            sumSlow += w->GetVectA(i);
      }
   }
   
   fGates[kTfall] = w->GetVectT(xmax);;

   if (fIdPhoswich)
      fGates[kTfast] = (w->GetVectT(xmin) + fgPWfast);

   fBaseline = a0;
   pedestal = TMath::Abs(xmax-xmin)*a0;
   fCharge = -1.*(sum - pedestal);
   if (fIdPhoswich) {
      fChargeFast = -1.*(sumFast - pedestal);
      fChargeSlow = -1.*(sumSlow - pedestal);
   }
   fAmplitude   = -1.*(peak - a0);
   
   // rise time
   Float_t peak10 = (peak-a0)*0.1;
   Float_t peak90 = (peak-a0)*0.9;
   Float_t t10 = 0;
   Float_t t90 = 0;
   
   Int_t iMin = 0;
   Int_t iMax = 0;

   
   //90%
   for (Int_t i = minBin; i > 0; --i) {
      if ((w->GetVectA(i) - a0) > peak90) {
         t90 = w->GetVectT(i);
         iMax = i;
         break;
      }
   }
   
   // 10%
   for (Int_t i = minBin; i > 0; --i) {
      if ((w->GetVectA(i) - a0) > peak10) {
         t10 = w->GetVectT(i);
         iMin = i;
         break;
      }
   }

   fGates[kT10] = w->GetVectT(iMin);
   fGates[kT90] = w->GetVectT(iMax);

   vector<double> x;
   vector<double> y;
   vector<double> dy;
   
   for (Int_t i = iMin; i < iMax; ++i) {
      x.push_back(w->GetVectT(i));
      y.push_back(w->GetVectA(i));
      dy.push_back(1.);
   }
   
   vector<double> res = GetLinearFit(x,y,dy);

   fTimeLE   = (a0-res[0])/res[1];
   fRiseTime = t90-t10;

   // fall time
   t10 = 0;
   t90 = 0;
   
   //90%
   for (Int_t i = minBin; i <  w->GetSamplesN(); ++i) {
      if ((w->GetVectA(i) - a0) > peak90) {
         t90 = w->GetVectT(i);
         break;
      }
   }
   
   // 10%
   for (Int_t i = minBin; i <  w->GetSamplesN(); ++i) {
      if ((w->GetVectA(i) - a0) > peak10) {
         t10 = w->GetVectT(i);
         break;
      }
   }
   
   fFallTime = t10-t90;
   
   // CFD
   Float_t fac   = fgCFDfac;
   Float_t delay = fgCFDdelay; // in bins
   
   float* delayV = new float[w->GetSamplesN()];
   memset(delayV, 0, sizeof(float)*w->GetSamplesN());
   
   for (Int_t i = 0; i < w->GetSamplesN()-delay; ++i)
      delayV[i] = (w->GetVectA(i)-a0) - fac*(w->GetVectA(i+delay)-a0);
   
   // look for zero crossing
   minBin = TMath::LocMin(w->GetSamplesN(), delayV);

   Int_t imin = 0;
   
   for (Int_t i = minBin; i > 0;  --i) {
      if (delayV[i] >  0) {
         xmin = w->GetVectT(i);
         imin = i;
         break;
      }
   }
   
   // a = (y1-y0)/(x1-x0), b = y0-slope*x0.  ax0+b = 0; t0 = -b/a
   Double_t slope  = (delayV[imin] - delayV[imin+1])/(w->GetVectT()[imin] - w->GetVectT()[imin+1]);
   Double_t offset = delayV[imin]-xmin*slope;
   fTimeCFD = -offset/slope;
   
   delete[] delayV;   
}

//------------------------------------------+-----------------------------------
vector<double> TAGbaseWC::GetLinearFit(const vector<double>& z, const vector<double>& x, const vector<double>& dx)
{
   double zzSum = 0, zxSum = 0, slope, intercept;
   double zMean = 0, xMean = 0, wSum = 0;
   
   for (long i = 0; i < z.size(); i++) {
      double w = 1./(dx[i]*dx[i]);
      zMean += w*z[i];
      xMean += w*x[i];
      wSum += 1./(dx[i]*dx[i]);
   }
   
   zMean /= wSum;
   xMean /= wSum;
   
   for (long i = 0; i < z.size(); i++) {
      double w = 1./(dx[i]*dx[i]);
      zxSum += (x[i] - xMean)*(z[i] - zMean)*w;
      zzSum += (z[i] - zMean)*(z[i] - zMean)*w;
   }
   
   slope = zxSum/zzSum;
   intercept = xMean - slope*zMean;
   
   vector<double> res;
   res.push_back(intercept);
   res.push_back(slope);
   
   return res;
}
