/*!
  \file TAGparaDsc.cxx
  \brief   Implementation of TAGparaDsc.
*/

#include "TClass.h"
#include "TList.h"

#include "TAGroot.hxx"
#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"

/*!
  \class TAGparaDsc 
  \brief Persistent descriptor for parameter object . **
*/

ClassImp(TAGparaDsc);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGparaDsc::TAGparaDsc(const char* name, TAGpara* p_para)
  : TAGnamed(name, 0),
    fpObject(0),
    fpObjectClass(0),
    fpConsumerList(0)
{
  if (!gTAGroot) Fatal("TAGparaDsc()", "TAGroot not instantiated");
  SetBit(kMustCleanup);

  if (name==0 || name[0]==0) SetName(gTAGroot->DefaultParaDscName());

  if (gTAGroot->FindParaDsc(GetName())) {
    Warning("TAGparaDsc()", 
	    "Parameter descriptor with name '%s' already exists",
	    GetName());
  }

  gTAGroot->ListOfParaDsc()->Add(this);

  if (p_para) {
    fpObject = p_para;
    fpObjectClass = p_para->IsA();
    SetTitle(p_para->IsA()->GetName());
  }
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGparaDsc::~TAGparaDsc()
{
  delete fpObject;
}

//------------------------------------------+-----------------------------------
//! Set para object.

void TAGparaDsc::SetObject(TAGpara* p_para)
{
  delete fpObject;
  fpObject = p_para;
  if (p_para == 0) ResetBit(kValid);
  return;
}

//------------------------------------------+-----------------------------------
//! Set consumer action.

void TAGparaDsc::SetConsumer(TAGaction* p_action)
{
  if (!fpConsumerList) fpConsumerList = new TList();
  fpConsumerList->Add(p_action);
  return;
}

//------------------------------------------+-----------------------------------
//! Invalidate para object.

void TAGparaDsc::Clear(Option_t*)
{
  if (fpObject) fpObject->Clear();
  ResetBit(kValid|kFail);
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGparaDsc::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGparaDsc:      " << "'" << GetName() << "'" << endl;
  if (fpObjectClass) {
    os << "  object class:  " << fpObjectClass->GetName() << endl;
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
//! Cleanup dangling object references to TObject \a p_obj .

void TAGparaDsc::RecursiveRemove(TObject* p_obj)
{
  if (fpConsumerList) while (fpConsumerList->Remove(p_obj));
  return;
}

