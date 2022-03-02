/*!
  \file TAGdataDsc.cxx
  \brief   Implementation of TAGdataDsc.
*/

#include "TClass.h"
#include "TList.h"

#include "TAGroot.hxx"
#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

/*!
  \class TAGdataDsc 
  \brief Persistent descriptor for data object . **
*/

//! Class Imp
ClassImp(TAGdataDsc);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_data data to descript
TAGdataDsc::TAGdataDsc(const char* name, TAGdata* p_data)
  : TAGnamed(name, 0),
    fpObject(0),
    fpObjectClass(0),
    fpProducer(0),
    fpConsumerList(0)
{
  if (!gTAGroot) Fatal("TAGdataDsc()", "TAGroot not instantiated");
  SetBit(kMustCleanup);

  if (name==0 || name[0]==0) SetName(gTAGroot->DefaultDataDscName());

  if (gTAGroot->FindDataDsc(GetName())) {
    Warning("TAGdataDsc()", "Data descriptor with name '%s' already exists",
	    GetName());
  }

  gTAGroot->ListOfDataDsc()->Add(this);

  if (p_data) {
    fpObject = p_data;
    fpObjectClass = p_data->IsA();
    SetTitle(p_data->IsA()->GetName());
  }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGdataDsc::~TAGdataDsc()
{
  delete fpObject;
}

//------------------------------------------+-----------------------------------
//! Set data object.
//!
//! \param[in] p_data data to descript
void TAGdataDsc::SetObject(TAGdata* p_data)
{
  delete fpObject;
  fpObject = p_data;
  if (p_data == 0) ResetBit(kValid);
  return;
}

//------------------------------------------+-----------------------------------
//! Set producer action.
//!
//! \param[in] p_action action
void TAGdataDsc::SetProducer(TAGaction* p_action)
{
  fpProducer = p_action;
  return;
}

//------------------------------------------+-----------------------------------
//! Set consumer action.
//!
//! \param[in] p_action action
void TAGdataDsc::SetConsumer(TAGaction* p_action)
{
  if (!fpConsumerList) fpConsumerList = new TList();
  fpConsumerList->Add(p_action);
  return;
}

//------------------------------------------+-----------------------------------
//! Generate data object and return a pointer to it.
TAGdata* TAGdataDsc::GenerateObject()
{
  if (!Valid()) Generate();
  return Object();
}

//------------------------------------------+-----------------------------------
//! Clear action object.
//!
//! \param[in] opt clear options
void TAGdataDsc::Clear(Option_t*)
{
  if (fpObject) fpObject->Clear();
  ResetBit(kValid|kEof|kFail);
  return;
}

//------------------------------------------+-----------------------------------
//! Generate data object.
Bool_t TAGdataDsc::Generate()
{
  if (Valid()) return kTRUE;

  if (fpProducer) {
    fpProducer->Process();
  } else {
    Error("Generate()", "no action registered for '%s'", GetName());
    SetBit(kFail);
  }  

  return TestBit(kValid);
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option printout option
void TAGdataDsc::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGdataDsc:      " << "'" << GetName() << "'" << endl;
  if (fpObjectClass) {
    os << "  object class:  " << fpObjectClass->GetName() << endl;
  }
  if (fpProducer) {
    os << "  producer:      " << fpProducer->IsA()->GetName()
       << " '" << fpProducer->GetName() << "'" << endl;
  }
  if (fpConsumerList) {
    const char* pref = "  consumer:      ";
    for (TObjLink* lnk = fpConsumerList->FirstLink(); lnk; lnk=lnk->Next()) {
      TObject* p = lnk->GetObject();
      os << pref << p->IsA()->GetName() << " '" << p->GetName() << "'" << endl;
      pref = "                 ";
    }
  }
  return;
}

//------------------------------------------+-----------------------------------
// Cleanup dangling object references to TObject p_obj .
//!
//! \param[in] p_obj object to be removed
void TAGdataDsc::RecursiveRemove(TObject* p_obj)
{
  if (fpProducer == p_obj) fpProducer = 0;
  if (fpConsumerList) while (fpConsumerList->Remove(p_obj));
  return;
}

