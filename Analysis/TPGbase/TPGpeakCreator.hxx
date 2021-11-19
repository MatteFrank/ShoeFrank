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

/** \file TPGpeakCreator.h header file for TPGpeakCreator.cpp */

#ifndef TPGpeakCreator_h
#define TPGpeakCreator_h

#include "TPGpeak1D.hxx"

#include "TF1.h"

class TSeqCollection;
class TList;
class TCanvas;
class TH1;
class TFormula;
class TVirtualPad;


/*! TPGpeakCreator 
 
 */
class TPGpeakCreator : public TObject
{  	
private:
   //! list of ceator linked to a canvas
   static TList  fgPeakCreatorList;
   //! default function 
   static TF1   *fgDefaultFWHM;
   
private:  
   //! Default parameters (graphics) for 1D peaks
   TPGpeak1D *fDefaultPeak1D;
   //! if true, PeakCollector is collected
   Bool_t fIsCollecting;
   //! if true, new peaks are created with gate option
   Bool_t fIsGate;
   
private:
   //! formula for default FWHM
   TF1 *fDefaultPeakFWHM; 
   
private:
   //! canvas to which this is connected.
   TCanvas *fCanvas;
   
private:   	
   Int_t fLastX;              // last x event position
   Int_t fLastY;              // last y event position
   
   //! set/get fLastX/fLastY
   virtual Int_t GetLastX() const  { return fLastX; }
   virtual Int_t GetLastY() const  { return fLastY; }
   virtual void SetLastXY(Int_t lastX, Int_t lastY) { fLastX = lastX; fLastY = lastY; }
   
protected:
   //! It adds a point to the polyline at position x,y in the current pad
   /*!
    It adds a point to the a polyline which belongs to the current pad.	 
    */
   virtual void AddPointToPolyline(Double_t x, Double_t y, TVirtualPad *pad = 0x0);   
   //! Get Pad
   /*! 
    if null return gPad, otherwise return 
    */
   static TVirtualPad *PadforAction(TVirtualPad *pad = 0x0);
   
private:
   TPGpeakCreator(TCanvas *);
   
public:
   // change the global formula 
   // static void SetGlobalPeakFWHM(const TF1 *);
   // to get the global formula 
   static const TF1 *GetGlobalPeakFWHM();
   
public:
   TPGpeakCreator();
   virtual ~TPGpeakCreator();
   
   //! if true, peak are collected on key board actions (type h for help)
   void SetGateMode(Bool_t ngate = true); //*TOGGLE* *GETTER=IsGate
   Bool_t IsGate() const { return fDefaultPeak1D->GetDrawAs() == TPGpeak1D::kGate; }
   
   //! if true, peak are collected on key board actions (type h for help)
   void Collect(Bool_t do_collect = true); //*TOGGLE* *GETTER=IsCollect
   Bool_t IsCollect()  { return fIsCollecting; }
   
   //! access to the peak used to create other peaks (through the Clone method)
   TPGbasePeak *DefaultPeak() {return fDefaultPeak1D;}
   
   //! to get the current global FWHM definition
   //const TF1 *GetPeakFWHM();
   
   //! to get the definition of the peak with as a function of the energy
   const TF1 *GetDefaultPeakFWHM() { return fDefaultPeakFWHM; }
   
   // to change the definition of the formula for the width
   /*!
    it looks for a function with the given name in the list of functions and if found,
    it creates a clone that replaces the current definition
    */
   void SetDefaultPeakFWHM( const char * = "PK_FWHM" ) ; //*MENU* 
   
   const TCanvas *GetCanvas() const
   { return fCanvas; }
   
   //! add a peak creator to the given pad. Current pad if argument is 0x0
   static TPGpeakCreator *AddTPGpeakCreator(TVirtualPad *pad = 0x0);
   
   //! Check whether or not a TPGpeakCreator is in this canvas
   static TPGpeakCreator *IsTPGpeakCreator(TVirtualPad *pad = 0x0);
   
   //! It creates a peak at position x for the 1D spectra
   /*!
    options :
    - bg : creates a peak and its associated backgroud
    - apt : (at peak position) it takes exactly the position x given by in the arguments
    */
   virtual TPGbasePeak* CreatePeak(const TH1* h, Double_t x, Option_t *opt = "");
   
   //! It creates a peak at position x,y for the spectrum extraced from pad or gPad
   /*!
    for options, see CreatePeak(const TH1* h, Double_t x, Option_t *opt = "")
    */
   virtual TPGbasePeak* CreatePeak(Double_t x, Option_t *opt = "", TVirtualPad *pad = 0x0);
   
   //! It creates a peak at position x,y for the spectrum
   // virtual TH1* CreateBackground(const TH1* h, TPolyLine *pline = 0x0);
   
   //! Delete, redifined to avoid appearence in Menu
   virtual void Delete(Option_t *opt = "") 
   { TObject::Delete(opt); }
   
   //! Clear: delete all peaks in canvas/pad depending on TPGpadManager
   virtual void Clear(Option_t* = "");
   
   //! Handle Movement
   void HandleMovement(Int_t eventType, Int_t eventX, Int_t eventY, TObject* select);
   
   //! Handle modification of the range of the pad
   void HandleRefresh();
   
   ClassDef(TPGpeakCreator, 0); // To creat peak/gates in a Pad  
};

#endif
