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

/** \file TPGspectrumPlayer.hxx header file for TPGspectrumPlayer.cxx */

#ifndef TPGspectrumPlayer_h
#define TPGspectrumPlayer_h

#include "TNamed.h"
#include "TParameter.h"
#include "TVector3.h"

#include "TPGpadManager.hxx"
#include "TPGpeakCreator.hxx"

class TSeqCollection;
class TList;
class TCanvas;
class TH1;
class TFormula;
class TContextMenu;


class TPGbasePeak;

class TPGspectrumPlayer : public TNamed
{
protected:
   //! default TPGpeakCreator
   TPGpeakCreator *fCreator; 
   
private:
   TSeqCollection *fPeakList; ///< Current collection of peaks
   TList *fInnerPeakList;     ///< Inner list of peaks
   
private:   
   Int_t fLastX;              ///< last x event position
   Int_t fLastY;              ///< last y event position
   
protected:
   //! Get fLastX/
   Int_t GetLastX()  { return fLastX; }
   //! Get fLastY
   Int_t GetLastY()  { return fLastY; }
   //! Set fLastX/fLastY
   void SetLastXY(Int_t lastX, Int_t lastY) { fLastX = lastX; fLastY = lastY; }
   
   // check if peak is in range of drawn spectrum
   Bool_t IsInRange(TH1*h, TPGbasePeak* peak);
   
protected:
   TContextMenu *fContextMenu;    ///< context menu in canvas
   
protected:
   // default constructor
   TPGspectrumPlayer();
   
public:
   virtual ~TPGspectrumPlayer();
   
   // Connect the Canvas to this to collect events
   /*!
    With no argument (or OxO) connect the current canvas
    */
   Bool_t Connect(TCanvas *c = 0x0); 
   
   // Connect the Canvas to this to collect events
   /*!
    With no argument (or 0x0) disconnect the current canvas
    */  
   Bool_t Disconnect(TCanvas *c = 0x0);   
   
   //! Get formula that gives the with as a function of energy
   virtual const TF1* GetDefaultPeakWidth() const  { return fCreator->GetDefaultPeakFWHM(); }
   
   // to change the definition of the formula for the width
   void SetDefaultPeakFWHM( const char * = "PK_FWHM" );
   
   //! Return the current collection of peaks
   /*!
    \warning DO NOT DELETE IT !
    */ 
   virtual TSeqCollection* GetPeakList() const { return fPeakList; }
   
   // Show the list of peaks on the current pad
   virtual void ShowPeakList() const;
   
   // set collection
   /*!
    Without argument (or NULL) set the inner list as the current collection 
    */ 
   virtual void SetPeakList(TSeqCollection *col = 0x0);
   
   // Add a peak to the current collection (at the end)
   virtual void AddPeak(TPGbasePeak* peak);
   
   // Sort all peaks and rename them
   virtual Bool_t RenamePeak(const Char_t* baseName = "Peak", Bool_t force = false); 
   
   // Fit all peaks using the spectrum in the current pad
   virtual void FitAll(const char* nameFunc = "gaus", Option_t* optFit = "RN", Option_t* optBkg = "lin"); //*MENU*
   
   // Popup AddLink menu
   virtual void PopupFitMenu();
   
   // Find peaks from the histo in the current pad and store them in the current collection
   virtual Int_t FindPeaks(Option_t* opt = "");
   
   //! find peaks in the given histo and store them in the current collection
   virtual Int_t FindPeaks(TH1 *histo, Option_t* opt = "") = 0;
   
   // sort peak list
   virtual void SortPeakList(const Char_t* parName = "Position", Bool_t sortDes = false);
   
   //! Compute the background for that histogram
   virtual TH1 *Background(const TH1 */*histo*/, Option_t* /*opt = ""*/) { return 0x0; }  
   
   // Compute the background for the histogram in the current pad
   virtual TH1 *Background(Option_t* opt = "");
   
   // Apply background substraction for that histogram
   virtual void DoBackground(TH1 *histo, Option_t* opt = "");
   
   //! Apply background substraction for current histogram in pad	
   virtual void DoBackground(Option_t *opt = "") { DoBackground(TPGpadManager::GetHisto(),opt); }
   
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* /*name*/, Double_t /*value*/) { return false; }
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* /*name*/, Int_t /*value*/)    { return false; }	
   virtual Bool_t SetParameter(const char* name, const TObject* value);
   
   // Collect the peaks from the current pad
   /*!
    returns the number of peak collected
    */       
   virtual Int_t CollectPeaks(Option_t* opt = "");
   
   // Print out informations concerning the parameters of that player
   virtual void Print(Option_t* opt = "") const;
   
   // Handle Movement
   void HandleMovement(Int_t eventType, Int_t eventX, Int_t eventY, TObject* select);
   
   ClassDef(TPGspectrumPlayer, 0); ///< Base class for facilities to play with spectra   
};


#endif
