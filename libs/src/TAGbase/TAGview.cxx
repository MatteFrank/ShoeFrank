/*!
  \file TAGview.cxx
  \brief   Implementation of TAGview.
*/

#include "TList.h"
#include "TVirtualPad.h"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGview.hxx"

/*!
  \class TAGview 
  \brief Base class for all views . **
*/

ClassImp(TAGview);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGview::TAGview(TObject* p_obj)
  : fpPad(0),
    fpObservableList(0)
{
  if (p_obj) AddObservable(p_obj);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGview::~TAGview()
{
  delete fpObservableList;
}

//------------------------------------------+-----------------------------------
//Add \a p_obj to the list of observered objects.
void TAGview::AddObservable(TObject* p_obj, const char* baseclass)
{
  if (!p_obj) {
    Error("AddObservable()", "object pointer is nil");
    MakeZombie();
    return;
  }

  if (baseclass) {
    if (baseclass[0] != '@') {
      if (!p_obj->InheritsFrom(baseclass)) {
	Error("AddObservable()", "object type '%s' not based on '%s'",
	      p_obj->GetName(), baseclass);
	MakeZombie();
	return;
      }

    } else {
      TObject* p_objobj = 0;
      if (p_obj->InheritsFrom("TAGdataDsc")) {
	p_objobj = ((TAGdataDsc*)p_obj)->Object();
      } else if (p_obj->InheritsFrom("TAGparaDsc")) {
	p_objobj = ((TAGparaDsc*)p_obj)->Object();
      } else {
	Error("AddObservable()", "object type not TAGdataDsc/TAGparaDsc");
	MakeZombie();
	return;
      }
      if (!p_objobj) {
	Error("AddObservable()", "data or para object pointer is nil");
	MakeZombie();
	return;	
      }
      if (!p_objobj->InheritsFrom(baseclass+1)) {
	Error("AddObservable()", "data/para object type '%s' not based on '%s'",
	      p_objobj->GetName(), baseclass+1);
	MakeZombie();
	return;
      }
    }
  }

  if (!fpObservableList) fpObservableList = new TList();
  p_obj->SetBit(kMustCleanup);
  fpObservableList->Add(p_obj);
  return;
}

//------------------------------------------+-----------------------------------
// Remove \a p_obj from the list of observered objects.
void TAGview::RemoveObservable(TObject* p_obj)
{
  if (fpObservableList) while (fpObservableList->Remove(p_obj));  
  return;
}

//------------------------------------------+-----------------------------------
// Replace \a p_old with \a p_new in the list of observered objects.
void TAGview::ReplaceObservable(TObject* p_old, TObject* p_new,
				const char* baseclass)
{
  if (p_old) RemoveObservable(p_old);
  AddObservable(p_new, baseclass);
  return;
}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAGview::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGview:      " << "'" << GetName() << "'" << endl;
  return;
}

//------------------------------------------+-----------------------------------
// Draw view
void TAGview::Draw(Option_t* option)
{
  if (fpPad) {				    // already drawn on a pad
    if (gPad != fpPad) {
      Error("Draw()", "View drawn on pad %s, ignore request for %s",
	    fpPad->GetName(), gPad->GetName());
    } else {
      fpPad->Modified(kTRUE);
    }
    return;
  }

  AppendPad(option);
  fpPad = gPad;
  SetBit(kMustCleanup);
  SetBit(kCanDelete);			    // will delete view when pad deleted
  return;
}

//------------------------------------------+-----------------------------------
// Cleanup dangling object references to TObject \a p_obj .
void TAGview::RecursiveRemove(TObject* p_obj)
{
  if (fpObservableList) while (fpObservableList->Remove(p_obj)) MakeZombie();
  return;
}

