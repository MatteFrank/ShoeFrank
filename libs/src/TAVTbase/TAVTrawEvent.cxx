/*!
  \file
  \version $Id: TAVTrawEvent.cxx
  \brief    Representation of ascii raw data event
*/

#include <string.h>

#include "TString.h"

#include "TAVTrawEvent.hxx"

/*!
  \class TAVTrawEvent
  \brief Representation of ascii raw data event
*/

ClassImp(TAVTrawEvent);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTrawEvent::TAVTrawEvent()
: TAGdata(),
  fData(0x0),
  fEventTrigger(0),
  fEventSize(0)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTrawEvent::~TAVTrawEvent()
{
   delete fData;
}

//------------------------------------------+-----------------------------------
//! Add a sub event.
void TAVTrawEvent::AddEvent(Int_t size, const UInt_t data[])
{
   fEventSize = size;
   
   if (fData) 
	  delete fData;
   fData = new UInt_t[size];

   memcpy(fData, data, size*sizeof(UInt_t));
}

//------------------------------------------+-----------------------------------
//! Returns \c true if object needs automatic delete when read from tree.
Bool_t TAVTrawEvent::NeedAutoDelete() const
{
   return kFALSE;
}

///------------------------------------------+-----------------------------------
//! Clear event.
void TAVTrawEvent::Clear(Option_t*)
{
   TAGdata::Clear();
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAVTrawEvent::ToStream(ostream& os, Option_t* option) const
{
   return;
}

