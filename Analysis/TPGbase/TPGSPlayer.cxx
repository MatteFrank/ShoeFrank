/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christian Finck        *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)ires.in2p3.fr                      *
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
 \file TPGSPlayer.cxx
 \brief   Implementation of TPGSPlayer.
 */

#include <Riostream.h>
#include <vector>

#include "TPGpeak1D.hxx"
#include "TPGSPlayer.hxx"

#include "TCanvas.h"
#include "TFormula.h"
#include "TH1.h"
#include "TH2.h"
#include "KeySymbols.h"
#include "TList.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TSpline.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector2.h"
#include "TVirtualPad.h"

/** \class TPGSPlayer
 \brief class to work on spectra
 
 Below an example of fitting in a 152Eu source spectrum:
 \code
 TCanvas* c1 = new TCanvas("c1","titre",10,10,700,500);
 c1->Draw();
 TH1F* h = new TH1F();
 h->SetAxisRange(2100, 2400);
 h->Draw(); // draw the spectrum of 152Eu in the given range.
 
 TPGSPlayer* s = new TPGSPlayer();
 // place the peaks using 'a' key
 // or use FindPeaks() method: press 's' whenever the line pointed a peak you want to add
 s->FitAll(); // fits the peaks with gaussian shape with a linear background
 \endcode
 
 The FitAll(..) method fits all the positionned peaks in the spectrum. Peaks separated less than 2*fwhm
 (default wise fwhm = 6) are fitting with the CombinedFit(...) method.
 
 \code
 // You can also defined background by pressing key 'b' to set each point of the background spline
 s->Background() // fit the background point with a TSpline
 s->DoBackground // fit the background point with a TSpline & removed background from current histogramm
 \endcode
 
 */

//! Class Imp
ClassImp(TPGSPlayer)

//__________________________________________________________
//! Default Constructor
TPGSPlayer::TPGSPlayer()
: TPGspectrumPlayer()
{
}

//__________________________________________________________
//! default destructor
TPGSPlayer::~TPGSPlayer()
{
}

//__________________________________________________________
//! Set paramater
//!
//! \param[in] name parameter name
//! \param[in] value a given value
Bool_t TPGSPlayer::SetParameter(const char* name, Double_t value)
{
   // set parameter
   Bool_t ok = TPGspectrumPlayer::SetParameter(name,value);
   if ( !ok ) {
      // set search parameter
      // 	if (strncmp(name, "FWHM", strlen(name)) == 0)
      // 	fDefaultFwhm = TParameter<Double_t>(name, value);   
   }
   if ( !ok ) {
      Error("SetParameter()",  "Unkown parameter: %s", name);
   }
   return ok;
}

//__________________________________________________________
//! Find peaks in a given histogram
//!
//! \param[in] histo a given histogram
//! \param[in] o search options
Int_t TPGSPlayer::FindPeaks(TH1 *histo, Option_t* o)
{
   // placed peak with a moving on line
   
   Int_t xBinMin  = histo->GetXaxis()->GetFirst();
   Int_t xBinMax  = histo->GetXaxis()->GetLast();
   Double_t xMin  = histo->GetXaxis()->GetBinCenter(xBinMin);
   Double_t xMax  = histo->GetXaxis()->GetBinCenter(xBinMax);
   Double_t xMin0 = xMin;
   Double_t xMax0 = xMax;
   Double_t yMax  = histo->GetMaximum()*1.05;
   Double_t yMin  = histo->GetMinimum()*0.95; 
   
   Double_t range  = xMax - xMin; 
   Double_t width  = GetDefaultPeakWidth()->Eval(xMin)*15;   
   Int_t nSpec     = int(range/width + 0.5);
   Int_t xBin      = 0;
   
   Int_t result    = 0;
   Int_t eventType = 0;; 
   
   // if Contains +, add to the list 
   TString opt = o;
   if ( !opt.Contains("+") ) 
      GetPeakList()->Delete();
   
   Option_t* optPeak;
   if ( opt.Contains("bg") ) 
      optPeak = "bg";
   else 
      optPeak = "";
   
   // activated search for 1D spectrum
   if ( histo->GetDimension() == 1 ) {
      // starts search once button1 is pressed
      do {
         gSystem->ProcessEvents();
         eventType = TVirtualPad::Pad()->GetEvent();
      } while(eventType != kButton1Down);
      
      // loop over ranges
      for (Int_t k = 0; k < nSpec; ++k) {
         xMax = xMin+width;
         histo->SetAxisRange(xMin, xMax);
         xMin = xMax;
         TLine line;
         xBinMin = histo->GetXaxis()->GetFirst();
         xBinMax = histo->GetXaxis()->GetLast();
         yMax    = histo->GetMaximum();
         yMin    = histo->GetMinimum(); 
         
         // loop over bins of the given range
         for (Int_t i = xBinMin; i < xBinMax; ++i) {
            
            xBin = int(histo->GetXaxis()->GetBinCenter(i) + 0.5);
            
            // set line
            line.SetX1(xBin); line.SetY1(yMin);
            line.SetX2(xBin); line.SetY2(yMax);
            line.Draw();
            TVirtualPad::Pad()->Update();
            
            // keep position of line.
            gSystem->ProcessEvents();
            // press q to proceed next range
            if (TVirtualPad::Pad()->GetEvent() == kKeyPress) {
               Int_t keysym = TVirtualPad::Pad()->GetEventY();
               if ( (EKeySym)keysym == kKey_s) { 
                  fCreator->CreatePeak(histo->FindBin(xBin), optPeak);
                  TVirtualPad::Pad()->Update();
               }
            }        
            
            Int_t kk;
            do {
               gSystem->ProcessEvents();
               eventType = TVirtualPad::Pad()->GetEvent();
               kk = TVirtualPad::Pad()->GetEventY();
            } while((EKeySym)kk == kKey_s);
            
            gSystem->Sleep(100);
         }
      }
      histo->SetAxisRange(xMin0, xMax0);
      TVirtualPad::Pad()->Update();
   }
   
   if ( histo->GetDimension() == 2) {
      Error("FindPeaks()", "2D search peak not implemented yet");
      return 0;
   }
   
   CollectPeaks();
   result = GetPeakList()->GetEntries();
   
   return result;
}

//__________________________________________________________
//! Get background histogram
//!
//! \param[in] histo a given histogram
//! \param[in] opt background options not used
TH1* TPGSPlayer::Background(const TH1* histo, Option_t* /*opt*/)
{
   // generates bkg for polymarker (for the moment)
   TH1* bkg        = 0x0;
   TPolyLine* line = 0x0;
   
   TList* list = TVirtualPad::Pad()->GetListOfPrimitives();
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TObject* o       = list->At(i);
      const char* name = o->ClassName();
      TString tmp(name);
      
      // check polyline in pad
      if ( tmp.CompareTo("TPolyLine") == 0 )
         line = static_cast<TPolyLine*>(o);
      
      // check bkg spectrum in pad
      if ( o->InheritsFrom("TH1")) {
         bkg = static_cast<TH1*>(o);
         tmp = bkg->GetName();
         if (tmp.Contains("Bkg"))
            return bkg;
         else
            bkg = 0x0;
      }
   }
   
   if (!line && !bkg) {
      Error("Background()", "No background line/spectrum defined");
      return bkg;
   }
   
   Double_t* x = line->GetX();
   Double_t* y = line->GetY();
   Int_t     n = line->Size();
   
   bkg = (TH1*)histo->Clone();
   Int_t xBinMin  = bkg->FindBin(x[0]);
   Int_t xBinMax  = bkg->FindBin(x[n-1]);
   bkg->Reset("ICE");
   
   TSpline3 spline("Bkg", x, y, n);
   
   // building backgroud spectrum
   for (Int_t i = xBinMin; i < xBinMax; ++i) {
      Double_t xBin = histo->GetXaxis()->GetBinCenter(i);
      bkg->SetBinContent(i, spline.Eval(xBin));
   }
   
   return bkg;
}


