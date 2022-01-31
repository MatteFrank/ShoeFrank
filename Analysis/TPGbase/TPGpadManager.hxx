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

/** \file TPGpadManager.hxx header file for TPGpadManager.cpp */

#ifndef TPGpadManager_h
#define TPGpadManager_h

#include <list>

#include "Rtypes.h"

#include "TVirtualPad.h"
#include "TList.h"
#include "TH1.h"

class TF1;
class TPolyLine;

class TPGpadManager
{  
public:
   /*!
    \enum EActionMode
    \brief  Action mode
    - kGap : Action applied to current pad
    - kAllPad : to all pads of the current canvas
    - kNotGpa : to all pads except the current one
    - kCanvas : to the canvas
    */
   enum EActionMode {kGPad, kAllPad, kNotGPad, kCanvas};
   
   static EActionMode gActionMode; ///< Global to be applied on Canvas that are not Gw::Canvas or dont't have an individual mode
   
public:
   TPGpadManager() ;
   virtual ~TPGpadManager() ;
   
   // look for an histogram into the pad
   static TH1 * GetHisto(TVirtualPad *pad = 0x0 , Option_t *opt = "");

   //! to get the pad on which the creator is added
   template <typename Data_T> static void Collect(TList &list, TVirtualPad *pad = 0x0)
   {
      TVirtualPad *locpad = 0x0;
      if ( pad == 0x0 ) 
         if ( TVirtualPad::Pad() == 0x0 )
            return ;
         else 
            locpad = TVirtualPad::Pad();
         else 
            locpad = pad;
      
      TObject* obj = 0x0; TIter next(locpad->GetListOfPrimitives());
      while ( (obj = next()) ) { // 
         if ( obj->InheritsFrom(Data_T::Class_Name()) ) 
            list.Add(obj);
      }		
   }
   
   //! to get the pad on which the creator is added
   template <typename Data_T> static void Collect(std::vector<Data_T *> &array, TVirtualPad *pad = 0x0)
   {
      TVirtualPad *locpad = 0x0;
      if ( pad == 0x0 ) 
         if ( TVirtualPad::Pad() == 0x0 )
            return ;
         else 
            locpad = TVirtualPad::Pad();
         else 
            locpad = pad;
      
      TObject* obj = 0x0; TIter next(locpad->GetListOfPrimitives());
      while ( (obj = next()) ) { // 
         if ( obj->InheritsFrom(Data_T::Class_Name()) ) 
            array.push_back((Data_T *)obj);
      }		
   }
   
   // show a polyline calculated as the difference between an histogram and a function (used for instance for fits)
   static TPolyLine *ShowDiff(const TH1 *h, const TF1 *f, Double_t xmin, Double_t xmax);
   
   ClassDef(TPGpadManager, 0); ///< to manage ROOT pads and canvases
};


#endif
