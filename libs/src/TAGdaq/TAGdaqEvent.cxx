/*!
  \file TAGdaqEvent.cxx
  \brief   Implementation of TAGdaqEvent.
*/

#include "TAGdaqEvent.hxx"

/*!
  \class TAGdaqEvent 
  \brief Representation of raw data event. **
*/

//! Class Imp
ClassImp(TAGdaqEvent);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGdaqEvent::TAGdaqEvent()
 : TAGdata(),
   fInfoEvent(0x0),
   fTrgEvent(0x0)
{
   fMapOfFragments.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGdaqEvent::~TAGdaqEvent()
{
}

//------------------------------------------+-----------------------------------
//! Add fragment
//!
//! \param[in] frag add fragment to list
void TAGdaqEvent::AddFragment(const BaseFragment* frag)
{
   string type = frag->classType();
   fMapOfFragments[type].push_back(frag);
}

//------------------------------------------+-----------------------------------
//! Get fragment
//!
//! \param[in] type  frgament type
//! \param[in] idx index in the multi-fragement vector
const BaseFragment* TAGdaqEvent::GetFragment(string type, Int_t idx)
{
   auto itr = fMapOfFragments.find(type);
   if (itr == fMapOfFragments.end()) {
      Error("GetFragment()", "Wrong type for fragment");
      return 0x0;
   }
   
   return fMapOfFragments[type].at(idx);
}

//------------------------------------------+-----------------------------------
//! Get fragment size
//!
//! \param[in] type  frgament type
size_t TAGdaqEvent::GetFragmentSize(string type)
{
   auto itr = fMapOfFragments.find(type);
   if (itr == fMapOfFragments.end()) {
      Error("GetFragment()", "Wrong type for fragment");
      return 0x0;
   }
   
   return fMapOfFragments[type].size();
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGdaqEvent::Clear(Option_t*)
{
   TAGdata::Clear();
   fMapOfFragments.clear();
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAGdaqEvent::ToStream(ostream& /*os*/, Option_t* /*option*/) const
{
}

