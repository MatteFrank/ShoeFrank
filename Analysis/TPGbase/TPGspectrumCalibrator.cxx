/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christophe Theisen     *
 *   stezow(AT)ipnl.in2p3.fr, christophe.theisen(AT)cea.fr                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "TPGspectrumCalibrator.hxx"

#include "TROOT.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TArrayD.h"
#include "TString.h"

#include <iostream>
#include <fstream>

using namespace std;

// ROOT dictionnary
ClassImp(TPGspectrumCalibrator);

//__________________________________________________________________________________________________
TPGspectrumCalibrator::TPGspectrumCalibrator()
:	fXRawAxis(0x0),
fYRawAxis(0x0),
fZRawAxis(0x0),
fHistoCal(0x0)
{
   
}

//__________________________________________________________________________________________________
TPGspectrumCalibrator::~TPGspectrumCalibrator() //destructor
{
   if (fXRawAxis != 0) delete fXRawAxis;
   if (fYRawAxis != 0) delete fYRawAxis;
   if (fZRawAxis != 0) delete fZRawAxis;
   if (fHistoCal != 0) delete fHistoCal;
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH1 *histo, const TF1 *function, const Option_t *option)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   
   if (!function) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> function NOT defined !!\n");
      return;
   }
   
   TString opt = option;
   opt.ToLower();
   if ( (!opt.Contains("x")) && (!opt.Contains("y")) && (!opt.Contains("z")) ) opt = "x";
   if (histo->GetDimension() == 1) opt = "x";
   if ( (histo->GetDimension() == 2) && (!opt.Contains("x")) && (!opt.Contains("y")) ) opt = "x";
   
   //Set the calibrator
   Set(histo);
   
   if (opt.Contains("x")) {
      CalibOneAxis(function,"x");
   }
   if (opt.Contains("y")) {
      CalibOneAxis(function,"y");
   }
   if (opt.Contains("z")) {
      CalibOneAxis(function,"z");
   }
   
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH1 *histo, const Double_t a, const Double_t b,
                                      const Double_t c, const Option_t *option) 
{
   //Calibration with an order 2 polynome (a + bX + c*X*X)
   if ((b==0.0) && (c==0.0)) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> Invalid parameters !!\n");
      return;
   }
   
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   
   TString opt = option;
   opt.ToLower();
   if ( (!opt.Contains("x")) && (!opt.Contains("y")) && (!opt.Contains("z")) ) opt = "x";
   if (histo->GetDimension() == 1) opt = "x";
   if ( (histo->GetDimension() == 2) && (!opt.Contains("x")) && (!opt.Contains("y")) ) opt = "x";
   
   //Set the calibrator
   Set(histo);
   
   if (opt.Contains("x")) {
      CalibOneAxis(a,b,c,"x");
   }
   if (opt.Contains("y")) {
      CalibOneAxis(a,b,c,"y");
   }
   if (opt.Contains("z")) {
      CalibOneAxis(a,b,c,"z");
   }
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH1 *histo, const Double_t a, const Double_t b, const Option_t *option)
{
   //Calibration with an order 1 polynome (a + bX )
   Calibrate(histo,a,b,0.0,option);
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH2 *histo, const TF1 *functionX, const TF1 *functionY)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   if (!functionX) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> functionX NOT defined !!\n");
      return;
   }
   if (!functionY) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> functionY NOT defined !!\n");
      return;
   }
   
   //Set the calibrator
   Set(histo);
   
   CalibOneAxis(functionX,"x");
   CalibOneAxis(functionY,"y");
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH2 *histo, const Double_t ax, const Double_t bx, const Double_t cx,
                                      const Double_t ay, const Double_t by, const Double_t cy)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   
   //Set the calibrator
   Set(histo);
   
   CalibOneAxis(ax,bx,cx,"x");
   CalibOneAxis(ay,by,cy,"y");
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH2 *histo, const Double_t ax, const Double_t bx, const Double_t ay, const Double_t by)
{
   Calibrate(histo,ax,bx,0.0,ay,by,0.0);
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo, const TF1 *functionX, const TF1 *functionY, const TF1 *functionZ)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   if (!functionX) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> functionX NOT defined !!\n");
      return;
   }
   if (!functionY) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> functionY NOT defined !!\n");
      return;
   }
   if (!functionZ) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> functionZ NOT defined !!\n");
      return;
   }
   
   //Set the calibrator
   Set(histo);
   
   CalibOneAxis(functionX,"x");
   CalibOneAxis(functionY,"y");
   CalibOneAxis(functionZ,"z");
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo, const TF1 *function1, const TF1 *function2, const Option_t *option)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   if (!function1) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> function1 NOT defined !!\n");
      return;
   }
   if (!function2) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> function2 NOT defined !!\n");
      return;
   }
   
   TString opt = option;
   opt.ToLower();
   if ( (!opt.Contains("xy")) && (!opt.Contains("xz")) && (!opt.Contains("yz")) ) opt = "xy";
   
   //Set the calibrator
   Set(histo);
   
   if (!opt.CompareTo("xy")) {
      CalibOneAxis(function1,"x");
      CalibOneAxis(function2,"y");
   }
   if (!opt.CompareTo("xz")) {
      CalibOneAxis(function1,"x");
      CalibOneAxis(function2,"z");
   }
   if (!opt.CompareTo("yz")) {
      CalibOneAxis(function1,"y");
      CalibOneAxis(function2,"z");
   }
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo, const Double_t ax, const Double_t bx, const Double_t cx,
                                      const Double_t ay, const Double_t by, const Double_t cy, const Double_t az, 
                                      const Double_t bz, const Double_t cz)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   
   //Set the calibrator
   Set(histo);
   
   CalibOneAxis(ax,bx,cx,"x");
   CalibOneAxis(ay,by,cy,"y");
   CalibOneAxis(az,bz,cz,"z");
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo, const Double_t ax, const Double_t bx, const Double_t ay,
                                      const Double_t by, const Double_t az, const Double_t bz)
{
   Calibrate(histo,ax,bx,0.0,ay,by,0.0,az,bz,0.0);
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo,  const Double_t a1, const Double_t b1, const Double_t c1,
                                      const Double_t a2, const Double_t b2, const Double_t c2, const Option_t *option)
{
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Calibrate> histo NOT defined !!\n");
      return;
   }
   
   TString opt = option;
   opt.ToLower();
   if ( (!opt.Contains("xy")) && (!opt.Contains("xz")) && (!opt.Contains("yz")) ) opt = "xy";
   
   //Set the calibrator
   Set(histo);
   
   if (!opt.CompareTo("xy")) {
      CalibOneAxis(a1,b1,c1,"x");
      CalibOneAxis(a2,b2,c2,"y");
   }
   if (!opt.CompareTo("xz")) {
      CalibOneAxis(a1,b1,c1,"x");
      CalibOneAxis(a2,b2,c2,"z");
   }
   if (!opt.CompareTo("yz")) {
      CalibOneAxis(a1,b1,c1,"y");
      CalibOneAxis(a2,b2,c2,"z");
   }
   
   DrawResult();
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Calibrate(const TH3 *histo,  const Double_t a1, const Double_t b1,
                                      const Double_t a2, const Double_t b2, const Option_t *option)
{
   Calibrate(histo,a1,b1,0.0,a2,b2,0.0,option);
}

//__________________________________________________________________________________________________
TH1* TPGspectrumCalibrator::GetHistoCal() const {
   if (!fHistoCal) {
      printf("Info in <TPGspectrumCalibrator::GetHistoCal> fHistoCal not defined, return 0 !!\n");
      return 0;
   }
   return fHistoCal;
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::DrawResult(const Option_t *opt) const
{   // draw the calibrated histogram in the current pad with same range of the Raw Histogram
   // set the range of the calibrated histogram
   fHistoCal->GetXaxis()->SetRange(fXRawAxis->GetFirst(),fXRawAxis->GetLast());
   fHistoCal->GetYaxis()->SetRange(fYRawAxis->GetFirst(),fYRawAxis->GetLast());
   fHistoCal->GetZaxis()->SetRange(fZRawAxis->GetFirst(),fZRawAxis->GetLast());
   fHistoCal->Draw(opt);
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Set(const TH1 *histo)
{ // Set the TPGspectrumCalibrator with a new raw histogram - the old histogram and the 3 axis are deleted
   
   if (!histo) {
      printf("Info in <TPGspectrumCalibrator::Set> histo NOT defined !!\n");
      return;
   }
   
   // deleting old calibrated histogram and old axis
   if (fHistoCal != 0) {delete fHistoCal; fHistoCal=0;}
   if (fXRawAxis != 0) {delete fXRawAxis; fXRawAxis=0;}
   if (fYRawAxis != 0) {delete fYRawAxis; fYRawAxis=0;}
   if (fZRawAxis != 0) {delete fZRawAxis; fZRawAxis=0;}
   
   //Clone the histogram and the axis
   fHistoCal = (TH1 *)histo->Clone("HistoCal");
   fXRawAxis = (TAxis *)histo->GetXaxis()->Clone("XAxisCal");
   fYRawAxis = (TAxis *)histo->GetYaxis()->Clone("YAxisCal");
   fZRawAxis = (TAxis *)histo->GetZaxis()->Clone("ZAxisCal");
   fHistoCal->SetTitle("Calibrated Histogram");
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::CalibOneAxis(const TF1 *function, const Option_t *option)
{
   if (!function) {
      printf("Info in <TPGspectrumCalibrator::CalibOneAxis> function NOT defined !!\n");
      return;
   }
   
   TString opt = option;
   opt.ToLower();
   
   Double_t xmin = 0.0;
   Double_t xmax = 0.0;
   
   if (!opt.CompareTo("x")) {
      xmin = function->Eval(fXRawAxis->GetXmin());
      xmax = function->Eval(fXRawAxis->GetXmax());
      if (!fXRawAxis->IsVariableBinSize()) { //fixed
         if (xmin < xmax) {
            fHistoCal->GetXaxis()->SetLimits(xmin,xmax);
         } else {
            Invert(); 
            fHistoCal->GetXaxis()->SetLimits(xmax,xmin);
         }
      } else { // variable bin size
         Double_t *xbins = new Double_t[fXRawAxis->GetNbins()+1];
         const TArrayD *array = fXRawAxis->GetXbins();
         xmin = function->Eval(fXRawAxis->GetXmin());
         if (xmin < xmax) {
            for (Int_t i = 0 ; i < fXRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[i]);
            }
         } else {
            Invert();
            for (Int_t i=0 ; i<fXRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[fXRawAxis->GetNbins()-i]);
            }
         }
         fHistoCal->GetXaxis()->Set(fXRawAxis->GetNbins(),xbins);
         delete [] xbins;
      }
   }
   
   if (!opt.CompareTo("y")) {
      xmin = function->Eval(fYRawAxis->GetXmin());
      xmax = function->Eval(fYRawAxis->GetXmax());
      if (!fYRawAxis->IsVariableBinSize()) { //fixed
         if (xmin < xmax) {
            fHistoCal->GetYaxis()->SetLimits(xmin,xmax);
         } else {
            Invert(); 
            fHistoCal->GetYaxis()->SetLimits(xmax,xmin);
         }
      } else { // variable bin size
         Double_t *xbins = new Double_t[fYRawAxis->GetNbins()+1];
         const TArrayD *array = fYRawAxis->GetXbins();
         if (xmin < xmax) {
            for (Int_t i = 0 ; i < fYRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[i]);
            }
         } else {
            Invert();
            for (Int_t i = 0 ; i < fYRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[fYRawAxis->GetNbins()-i]);
            }
         }
         fHistoCal->GetYaxis()->Set(fYRawAxis->GetNbins(),xbins);
         delete [] xbins;
      }
   }
   
   if (!opt.CompareTo("z")) {
      xmin = function->Eval(fZRawAxis->GetXmin());
      xmax = function->Eval(fZRawAxis->GetXmax());
      if (!fZRawAxis->IsVariableBinSize()) { //fixed
         if (xmin < xmax) {
            fHistoCal->GetZaxis()->SetLimits(xmin,xmax);
         } else {
            Invert(); 
            fHistoCal->GetZaxis()->SetLimits(xmax,xmin);
         }
      } else { // variable bin size
         Double_t *xbins = new Double_t[fZRawAxis->GetNbins()+1];
         const TArrayD *array = fZRawAxis->GetXbins();
         if (xmin < xmax) {
            for (Int_t i=0 ; i<fZRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[i]);
            }
         } else {
            Invert();
            for (Int_t i = 0 ; i < fZRawAxis->GetNbins()+1; i++) {
               xbins[i] = function->Eval((*array)[fZRawAxis->GetNbins()-i]);
            }
         }
         fHistoCal->GetZaxis()->Set(fZRawAxis->GetNbins(),xbins);
         delete [] xbins;
      }
   }
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::CalibOneAxis(const Double_t a, const Double_t b, const Double_t c, const Option_t *option)
{
   if ((b==0.0) && (c==0.0)) {
      printf("Info in <TPGspectrumCalibrator::CalibOneAxis> invalid parameters !!\n");
      return;
   }
   
   TF1 *function = new TF1("ftemp","pol2(0)");
   function->SetParameter(0,a);
   function->SetParameter(1,b);
   function->SetParameter(2,c);
   
   CalibOneAxis(function,option);
   
   if (function != 0) delete function;
   
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::CalibOneAxis(const Double_t a, const Double_t b, const Option_t *option)
{
   if ( b==0.0 ) {
      printf("Info in <TPGspectrumCalibrator::CalibOneAxis> invalid parameters !!\n");
      return;
   }
   
   TF1 *function = new TF1("ftemp","pol1(0)");
   function->SetParameter(0,a);
   function->SetParameter(1,b);
   
   CalibOneAxis(function,option);
   
   if (function != 0) delete function;
   
}

//__________________________________________________________________________________________________
void TPGspectrumCalibrator::Invert()
{
   if (fHistoCal == 0) {
      printf("Info in <TPGspectrumCalibrator::Invert> fHistoCal not defined !!\n");
      return;
   }
   TH1 *HistoCalTemp = (TH1 *)fHistoCal->Clone("HistoCalTemp");
   if (HistoCalTemp == 0) {
      printf("Info in <TPGspectrumCalibrator::Invert> HistoCalTemp not defined !!\n");
      return;
   }
   if (fHistoCal->GetDimension() > 1) {
      printf("Info in <TPGspectrumCalibrator::Invert> Method not already defined for Histogram Dimension >= 2 !!\n"); 
      return;
   }
   
   fHistoCal->Reset();
   for (Int_t i=0 ; i <= fHistoCal->GetNbinsX()+1 ; i++) {
      fHistoCal->SetBinContent(i,HistoCalTemp->GetBinContent(fHistoCal->GetNbinsX()+1-i));
   }
   if (HistoCalTemp != 0) delete HistoCalTemp;
}








