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

/*!
 \file TPGspectrumCalibrator.cxx
 \brief   Implementation of TPGspectrumCalibrator.
 */

#include "TPGspectrumCalibrator.hxx"

#include "TROOT.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TArrayD.h"
#include "TString.h"

#include <iostream>
#include <fstream>

using namespace std;

/** \class TPGspectrumCalibrator
 \brief TPGspectrumCalibrator is a service class in order to calibrate a TH1
 The raw histogram must never be changed
 The calibrated histogram is just a copy of the raw histogram (same binning).
 In case of fixed size of bins : the limits of the axis (xmin and xmax) are changed according to the
 calibration function or parameters.
 
 \author Bertrand Rossé
 */

//! Class Imp
ClassImp(TPGspectrumCalibrator);

//__________________________________________________________
//! default constructor
TPGspectrumCalibrator::TPGspectrumCalibrator()
:	fXRawAxis(0x0),
fYRawAxis(0x0),
fZRawAxis(0x0),
fHistoCal(0x0)
{
   
}

//__________________________________________________________
//! default destructor
TPGspectrumCalibrator::~TPGspectrumCalibrator()
{
   if (fXRawAxis != 0) delete fXRawAxis;
   if (fYRawAxis != 0) delete fYRawAxis;
   if (fZRawAxis != 0) delete fZRawAxis;
   if (fHistoCal != 0) delete fHistoCal;
}

//------------------------------------------+-----------------------------------
//! Calibrated with a function a histogram
//!
//! \param[in] histo a given histogram
//! \param[in] function function to use for fit
//! \param[in] option options
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

//------------------------------------------+-----------------------------------
//!   Calibration with an order 2 polynome (a + bX + c*X*X)
//!
//! \param[in] histo a given histogram
//! \param[in] a 1st parameter
//! \param[in] b 2nd parameter
//! \param[in] c 3rd parameter
//! \param[in] option options
void TPGspectrumCalibrator::Calibrate(const TH1 *histo, const Double_t a, const Double_t b,
                                      const Double_t c, const Option_t *option) 
{
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

//------------------------------------------+-----------------------------------
//! Calibration with an order 1 polynome (a + bX )
//!
//! \param[in] histo a given histogram
//! \param[in] a 1st parameter
//! \param[in] b 2nd parameter
//! \param[in] option options
void TPGspectrumCalibrator::Calibrate(const TH1 *histo, const Double_t a, const Double_t b, const Option_t *option)
{
   Calibrate(histo,a,b,0.0,option);
}

//------------------------------------------+-----------------------------------
//! Calibration a 2D histogram with 2 functions
//!
//! \param[in] histo a given 2D histogram
//! \param[in] functionX calibration function for X-direction
//! \param[in] functionY calibration function for Y-direction
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

//------------------------------------------+-----------------------------------
//!   Calibration with an order 2 polynome (a + bX + c*X*X) for 2D histogram
//!
//! \param[in] histo a given 2D histogram
//! \param[in] ax 1st parameter for X-direction
//! \param[in] bx 2nd parameter for X-direction
//! \param[in] cx 3rd parameter for X-direction
//! \param[in] ay 1st parameter for Y-direction
//! \param[in] by 2nd parameter for Y-direction
//! \param[in] cy 3rd parameter for Y-direction
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

//------------------------------------------+-----------------------------------
//! Calibration a 3D histogram with 3 functions
//!
//! \param[in] histo a given 2D histogram
//! \param[in] functionX calibration function for X-direction
//! \param[in] functionY calibration function for Y-direction
//! \param[in] functionZ calibration function for Z-direction
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

//------------------------------------------+-----------------------------------
//! Calibration a 3D histogram with 2 functions
//!
//! \param[in] histo a given 2D histogram
//! \param[in] function1 calibration function for X/Y/Z-direction
//! \param[in] function2 calibration function for X/Y/Z-direction
//! \param[in] option axis options
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

//------------------------------------------+-----------------------------------
//!   Calibration with an order 2 polynome (a + bX + c*X*X) for 3D histogram
//!
//! \param[in] histo a given 2D histogram
//! \param[in] ax 1st parameter for X-direction
//! \param[in] bx 2nd parameter for X-direction
//! \param[in] cx 3rd parameter for X-direction
//! \param[in] ay 1st parameter for Y-direction
//! \param[in] by 2nd parameter for Y-direction
//! \param[in] cy 3rd parameter for Y-direction
//! \param[in] az 1st parameter for Z-direction
//! \param[in] bz 2nd parameter for Z-direction
//! \param[in] cz 3rd parameter for Z-direction
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

//------------------------------------------+-----------------------------------
//!   Calibration with an order 1 polynome (a + bX) for 3D histogram
//!
//! \param[in] histo a given 2D histogram
//! \param[in] ax 1st parameter for X-direction
//! \param[in] bx 2nd parameter for X-direction
//! \param[in] ay 1st parameter for Y-direction
//! \param[in] by 2nd parameter for Y-direction
//! \param[in] az 1st parameter for Z-direction
//! \param[in] bz 2nd parameter for Z-direction
void TPGspectrumCalibrator::Calibrate(const TH3 *histo, const Double_t ax, const Double_t bx, const Double_t ay,
                                      const Double_t by, const Double_t az, const Double_t bz)
{
   Calibrate(histo,ax,bx,0.0,ay,by,0.0,az,bz,0.0);
}

//------------------------------------------+-----------------------------------
//! Calibration a 3D histogram with 2 functions with 2 order polynom
//!
//! \param[in] histo a given 2D histogram
//! \param[in] a1 1st parameter for X/Y/Z-direction
//! \param[in] b1 2nd parameter for X/Y/Z-direction
//! \param[in] c1 3rd parameter for X/Y/Z-direction
//! \param[in] a2 1st parameter for X/Y/Z-direction
//! \param[in] b2 2nd parameter for X/Y/Z-direction
//! \param[in] c2 3rd parameter for X/Y/Z-direction
//! \param[in] option axis options
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

//------------------------------------------+-----------------------------------
//! Calibration a 3D histogram with 2 functions with 1st order polynom
//!
//! \param[in] histo a given 2D histogram
//! \param[in] a1 1st parameter for X/Y/Z-direction
//! \param[in] b1 2nd parameter for X/Y/Z-direction
//! \param[in] a2 1st parameter for X/Y/Z-direction
//! \param[in] b2 2nd parameter for X/Y/Z-direction
//! \param[in] option axis options
void TPGspectrumCalibrator::Calibrate(const TH3 *histo,  const Double_t a1, const Double_t b1,
                                      const Double_t a2, const Double_t b2, const Option_t *option)
{
   Calibrate(histo,a1,b1,0.0,a2,b2,0.0,option);
}

//__________________________________________________________________________________________________
//! Get histogram
TH1* TPGspectrumCalibrator::GetHistoCal() const
{
   if (!fHistoCal) {
      printf("Info in <TPGspectrumCalibrator::GetHistoCal> fHistoCal not defined, return 0 !!\n");
      return 0;
   }
   return fHistoCal;
}

//__________________________________________________________________________________________________
//! draw the calibrated histogram in the current pad with same range of the Raw Histogram
//! set the range of the calibrated histogram
//!
//! \param[in] opt draw options
void TPGspectrumCalibrator::DrawResult(const Option_t *opt) const
{
   fHistoCal->GetXaxis()->SetRange(fXRawAxis->GetFirst(),fXRawAxis->GetLast());
   fHistoCal->GetYaxis()->SetRange(fYRawAxis->GetFirst(),fYRawAxis->GetLast());
   fHistoCal->GetZaxis()->SetRange(fZRawAxis->GetFirst(),fZRawAxis->GetLast());
   fHistoCal->Draw(opt);
}

//__________________________________________________________________________________________________
//! Set the TPGspectrumCalibrator with a new raw histogram - the old histogram and the 3 axis are deleted
//!
//! \param[in] histo a given histogram
void TPGspectrumCalibrator::Set(const TH1 *histo)
{
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

//------------------------------------------+-----------------------------------
//! Calibrated with a function
//!
//! \param[in] function function to use for fit
//! \param[in] option axis options
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

//------------------------------------------+-----------------------------------
//!   Calibration with an order 2 polynome (a + bX + c*X*X)
//!
//! \param[in] a 1st parameter
//! \param[in] b 2nd parameter
//! \param[in] c 3rd parameter
//! \param[in] option axis options
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

//------------------------------------------+-----------------------------------
//! Calibration with an order 1 polynome (a + bX )
//!
//! \param[in] a 1st parameter
//! \param[in] b 2nd parameter
//! \param[in] option axis options
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
//! Invert
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








