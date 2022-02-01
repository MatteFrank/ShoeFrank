#ifndef TPGpadManager_h
#define TPGpadManager_h

/*!
 \file TPGpadManager.hxx
 \brief   Declaration of TPGpadManager.
 */

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
