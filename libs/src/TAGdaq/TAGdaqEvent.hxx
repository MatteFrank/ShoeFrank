#ifndef _TAGdaqEvent_HXX
#define _TAGdaqEvent_HXX

/*!
 \file TAGdaqEvent.hxx
 \brief  Interface with DAQ fragment
*/

#include <vector>
#include <string>
using namespace std;

#include "TString.h"

#include "DAQFileHeader.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"
#include "BaseFragment.hh"

#include "TAGdata.hxx"

class TAGdaqEvent : public TAGdata {
public:
   TAGdaqEvent();
   virtual              ~TAGdaqEvent();

   //! return event info
   InfoEvent*             GetInfoEvent()          const  { return fInfoEvent;                    }
   //! return trigger event
   TrgEvent*              GetTrgEvent()           const  { return fTrgEvent;                     }
   //! Set event info
   void                   SetInfoEvent(InfoEvent* info)  {  fInfoEvent = info;                   }
   //! Set trigger event
   void                   SetTrgEvent(TrgEvent* trg)     {  fTrgEvent = trg;                     }
   
   //! return number of fragments
   Int_t                  GetFragmentsN()         const  { return (int)fListOfFragments.size();  }
   //! return fragment
   const BaseFragment*    GetFragment(Int_t idx)         { return fListOfFragments[idx];         }
   //! return class type
   const Char_t*          GetClassType(Int_t idx) const  { return fListOfClassTypes[idx].data(); }
   //! Add fragment
   void                   AddFragment(const BaseFragment* frag);
   //! Clear
   virtual void           Clear(Option_t* opt="");
   //! To stream
   virtual void           ToStream(ostream& os = cout, Option_t* option = "") const;
   
private:
   InfoEvent*                   fInfoEvent;        ///< info event
   TrgEvent*                    fTrgEvent;         ///< trigger event
   vector<const BaseFragment*>  fListOfFragments;  ///< list of fragments
   vector<string>               fListOfClassTypes; ///< list of class types
   
   ClassDef(TAGdaqEvent,0)
};

#endif
