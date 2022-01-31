/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christian Finck        *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)iphc.cnrs.fr                       *
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
 \file TPGpadManager.cxx
 \brief Implementation of TPGpadManager
 */


#include "TPGpadManager.hxx"

#include "TF1.h"
#include "TPolyLine.h"
#include "TList.h"
#include "TCanvas.h"
#include "TPolyLine.h"

//
TPGpadManager::EActionMode TPGpadManager::gActionMode = kGPad;

/*!
 \class TPGpadManager
 \brief Pad manager
 */

//! Class Imp
ClassImp(TPGpadManager)

//______________________________________________________________________________
//! Standard constructor
TPGpadManager::TPGpadManager()
{
}

//______________________________________________________________________________
//! Standard Destructor
TPGpadManager::~TPGpadManager()
{
}

//------------------------------------------+-----------------------------------
//! Retrieve histogram from pad
//!
//! \param[in] pad a given pad
//! \param[in] opt not used
TH1* TPGpadManager::GetHisto(TVirtualPad *pad, Option_t * /*opt*/ )
{
   // histograms to be returned
   TH1* h = 0x0;
   
   // to get the pad on which the creator is added
   TVirtualPad *locpad = 0x0;
   if ( pad == 0x0 ) 
      if ( TVirtualPad::Pad() == 0x0 )
         return h;
      else 
         locpad = TVirtualPad::Pad();
      else 
         locpad = pad;
   
   TObject* obj = 0x0;
   TIter next(locpad->GetListOfPrimitives());
   while ( (obj = next()) ) { // iterator skips empty slots
      if ( obj->InheritsFrom("TH1") ) {
         h = dynamic_cast<TH1*> ( obj ); 
         break;
      }
   }
   return h;  
}

//------------------------------------------+-----------------------------------
//! Compute difference from histogram and fit fiunction (residuals) in a given range
//!
//! \param[in] h a given histogram
//! \param[in] f a given fit function
//! \param[in] xmin lower limit of range
//! \param[in] xmax upper limit of range
TPolyLine *TPGpadManager::ShowDiff(const TH1 *h, const TF1 *f, Double_t xmin, Double_t xmax)
{
   TPolyLine *result = new TPolyLine();
   
   // 
   Double_t altitude = 0.9*h->GetMaximum();
   
   for (Int_t i = h->GetXaxis()->FindFixBin(xmin); i < h->GetXaxis()->FindFixBin(xmax); i++) {
      //
      Double_t yy, xx, yytmp;
      //
      xx    = h->GetBinLowEdge(i);
      yy    = h->GetBinContent(i) - f->Eval(h->GetBinLowEdge(i)) ;
      yytmp = h->GetBinContent(i) - f->Eval(h->GetBinCenter(i));
      if ( TMath::Abs( yytmp ) < TMath::Abs( yy ) ) {
         xx = h->GetBinCenter(i);
         yy = yytmp;
      }
      yytmp = h->GetBinContent(i) - f->Eval(h->GetBinLowEdge(i)+h->GetBinWidth(i));
      if ( TMath::Abs( yytmp ) < TMath::Abs( yy ) ){
         xx = h->GetBinLowEdge(i)+h->GetBinWidth(i);
         yy = yytmp;	
      }
      //
      yytmp = altitude + yy;
      result->SetNextPoint(xx,yytmp);
   }
   if ( TVirtualPad::Pad() ) 
      result->Draw();
   
   return result;
}




