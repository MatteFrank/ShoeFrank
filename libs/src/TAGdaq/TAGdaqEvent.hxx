#ifndef _TAGdaqEvent_HXX
#define _TAGdaqEvent_HXX

/*!
 \file TAGdaqEvent.hxx
 \brief  Interface with DAQ fragment
*/

#include <map>
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
   Int_t                  GetFragmentsN()         const  { return (int)fMapOfFragments.size();   }
   // Get Fragment
   const BaseFragment*    GetFragment(string type, Int_t idx=0);

   // Get fragment size
   size_t GetFragmentSize(string type);

   // Add fragment
   void                   AddFragment(const BaseFragment* frag);
   // Clear
   virtual void           Clear(Option_t* opt="");
   // To stream
   virtual void           ToStream(ostream& os = cout, Option_t* option = "") const;
   
private:
   InfoEvent*                        fInfoEvent;       ///< info event
   TrgEvent*                         fTrgEvent;        ///< trigger event
   map<string, vector<const BaseFragment*>>  fMapOfFragments;  ///< map of fragments

   ClassDef(TAGdaqEvent,0)
};

#endif
