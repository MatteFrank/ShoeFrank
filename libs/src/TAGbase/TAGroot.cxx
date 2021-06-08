/*!
  \file
  \version $Id: TAGroot.cxx,v 1.13 2003/07/08 18:55:38 mueller Exp $
  \brief   Implementation of TAGroot.
*/

#include <string.h>

#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TClass.h"
#include "TSysEvtHandler.h"
#include "TMonitor.h"
#include "TString.h"
#include "TList.h"

#include "TAGroot.hxx"
#include "TAGactionFile.hxx"
#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

/*!
  \class TAGroot TAGroot.hxx "TAGroot.hxx"
  \brief TAG top level object description. **
*/

//------------------------------------------+-----------------------------------

class TAGrootInterruptHandler : public TSignalHandler {
  public:
                    TAGrootInterruptHandler();
    Bool_t          Notify();
};

//------------------------------------------+-----------------------------------
TAGrootInterruptHandler::TAGrootInterruptHandler() 
  : TSignalHandler(kSigInterrupt, kFALSE)
{}

//------------------------------------------+-----------------------------------
Bool_t TAGrootInterruptHandler::Notify()
{
  gTAGroot->AbortEventLoop();
  return kTRUE;
}

//##############################################################################

ClassImp(TAGroot);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGroot::TAGroot()
{
  if (gTAGroot) {
     Error("TAGroot()", "only one instance of TAGroot allowed");
     return;
  }
  gTAGroot = this;
  
  fpActionList          = new TList();
  fpDataDscList         = new TList();
  fpParaDscList         = new TList();
  fpRequiredActionList  = new TList();
  fpRequiredDataDscList = new TList();

  fiDefActionSeqNum  = 0;
  fiDefDataDscSeqNum = 0;
  fiDefParaDscSeqNum = 0;

  fpActionList->SetOwner(kTRUE);
  fpDataDscList->SetOwner(kTRUE);
  fpParaDscList->SetOwner(kTRUE);

  gROOT->GetListOfCleanups()->Add(fpActionList);
  gROOT->GetListOfCleanups()->Add(fpDataDscList);
  gROOT->GetListOfCleanups()->Add(fpParaDscList);
  gROOT->GetListOfCleanups()->Add(fpRequiredActionList);
  gROOT->GetListOfCleanups()->Add(fpRequiredDataDscList);

  fpTAGrootInterruptHandler = new TAGrootInterruptHandler();
  fbDefaultHandlerRemoved   = kFALSE;
  fbTAGrootHandlerAdded     = kFALSE;
  fbAbortEventLoop          = kFALSE;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGroot::~TAGroot()
{
  gROOT->GetListOfCleanups()->Remove(fpRequiredActionList);
  delete fpRequiredActionList;

  gROOT->GetListOfCleanups()->Remove(fpRequiredDataDscList);
  delete fpRequiredDataDscList;

  gROOT->GetListOfCleanups()->Remove(fpParaDscList);
  delete fpParaDscList;

  gROOT->GetListOfCleanups()->Remove(fpDataDscList);
  delete fpDataDscList;

  gROOT->GetListOfCleanups()->Remove(fpActionList);
  delete fpActionList;

  gTAGroot = 0;
}

/*------------------------------------------+---------------------------------*/
//! Add a \c TAGaction or \c TAGdataDsc to the list of required items

void TAGroot::AddRequiredItem(TAGnamed* p_item)
{
  if (p_item->InheritsFrom("TAGaction")) {
    if (fpRequiredActionList->IndexOf(p_item) < 0)
      fpRequiredActionList->Add(p_item);

  } else if (p_item->InheritsFrom("TAGdataDsc")) {
    if (fpRequiredDataDscList->IndexOf(p_item))
      fpRequiredDataDscList->Add(p_item);

  } else {
    Error("AddRequiredItem()",
	  "Object of type %s not based on TAGaction or TAGdataDsc",
	  p_item->IsA()->GetName());
  }
  return;
}

/*------------------------------------------+---------------------------------*/
//! Add a \c TAGaction or \c TAGdataDsc named \a name to list of required items

void TAGroot::AddRequiredItem(const char* name)
{
  TAGaction*  p_act = FindAction(name);
  TAGdataDsc* p_dsc = FindDataDsc(name);

  if (p_act && !p_dsc) {
    AddRequiredItem(p_act);
  } else if (!p_act && p_dsc) {
    AddRequiredItem(p_dsc);
  } else if (p_act && p_dsc) {
    Warning("AddRequiredItem()", "name '%s' ambiguous (action and data available)", name);
  } else {
    Warning("AddRequiredItem()", "no object named '%s' found", name);
  }

  return;
}

/*------------------------------------------+---------------------------------*/
//! Clear list of required items

void TAGroot::ClearRequiredList()
{
  fpRequiredActionList->Clear();
  fpRequiredDataDscList->Clear();
  return;
}

/*------------------------------------------+---------------------------------*/
//! Setup event loop.

void TAGroot::BeginEventLoop() {

  if (gApplication->GetSignalHandler() && !fbDefaultHandlerRemoved) {
    gApplication->GetSignalHandler()->Remove();
    fbDefaultHandlerRemoved = kTRUE;
  }
  if (!fbTAGrootHandlerAdded) {
    fpTAGrootInterruptHandler->Add();
    fbTAGrootHandlerAdded = kTRUE;
  }

  fbAbortEventLoop = kFALSE;
  return;
}

/*------------------------------------------+---------------------------------*/
//! end event loop.

void TAGroot::EndEventLoop()
{
  if (fbTAGrootHandlerAdded) {
    fpTAGrootInterruptHandler->Remove();
    fbTAGrootHandlerAdded = kFALSE;
  }

  if (gApplication->GetSignalHandler() && fbDefaultHandlerRemoved) {
    gApplication->GetSignalHandler()->Add();
    fbDefaultHandlerRemoved = kFALSE;
  }

   for (TObjLink* lnk = fpRequiredActionList->FirstLink(); lnk; lnk=lnk->Next()) {
      TAGaction* p = (TAGaction*) lnk->GetObject();
      p->WriteHistogram();
   }

  return;
}

/*------------------------------------------+---------------------------------*/
//! abort event loop.

void TAGroot::AbortEventLoop()
{
  fbAbortEventLoop = kTRUE;
  return;
}

/*------------------------------------------+---------------------------------*/
//! Process required items of next event

Bool_t TAGroot::NextEvent(long nEv)
{
  Bool_t b_ok = kFALSE;
  if (fbAbortEventLoop) {
    Warning("NextEvent()","Aborting event loop");
    return kFALSE;
  }
  
  gSystem->ProcessEvents();
  TAGactionFile::IncrementTrigger();
  ClearAllAction();
  ClearAllData();

  if(nEv = -999)
    fEventId.SetEventNumber(1+fEventId.EventNumber()); // increment event number
  else
    fEventId.SetEventNumber( nEv );
  
  //break segmentation all'ultimo giro del for
  for (TObjLink* lnk = fpRequiredActionList->FirstLink(); 
       lnk; lnk=lnk->Next()) {    
    TAGaction* p = (TAGaction*) lnk->GetObject();
    if (!p->Process()) return kFALSE;
    b_ok = kTRUE;
  }
  
  for (TObjLink* lnk = fpRequiredDataDscList->FirstLink(); 
       lnk; lnk=lnk->Next()) {
    TAGdataDsc* p = (TAGdataDsc*) lnk->GetObject();
    if (!p->Generate()) return kFALSE;
    b_ok = kTRUE;
  }

  if (!b_ok) Error("NextEvent()", "No required Items registered");

  return b_ok;
}

/*------------------------------------------+---------------------------------*/
//! Clear all action objects

void TAGroot::ClearAllAction()
{
  for (TObjLink* lnk = fpActionList->FirstLink(); lnk; lnk=lnk->Next()) {
    lnk->GetObject()->Clear();
  }
  return;
}

/*------------------------------------------+---------------------------------*/
//! Wait for \a i_msec msec.

void TAGroot::Wait(Int_t i_msec)
{
  TMonitor monitor;
  monitor.Select(i_msec);
  return;
}

/*------------------------------------------+---------------------------------*/
//! Clear all data objects

void TAGroot::ClearAllData()
{
  for (TObjLink* lnk = fpDataDscList->FirstLink(); lnk; lnk=lnk->Next()) {
    lnk->GetObject()->Clear();
  }
  return;
}

/*------------------------------------------+---------------------------------*/
//! Clear all parameter objects

void TAGroot::ClearAllPara()
{
  for (TObjLink* lnk = fpParaDscList->FirstLink(); lnk; lnk=lnk->Next()) {
    lnk->GetObject()->Clear();
  }
  return;
}

/*------------------------------------------+---------------------------------*/
//! Returns next default action name.

const char* TAGroot::DefaultActionName()
{
  return Form("A%03d", fiDefActionSeqNum++);
}

/*------------------------------------------+---------------------------------*/
//! Returns next default data descriptor name.

const char* TAGroot::DefaultDataDscName()
{
  return Form("D%03d", fiDefDataDscSeqNum++);
}

/*------------------------------------------+---------------------------------*/
//! Returns next default data descriptor name.

const char* TAGroot::DefaultParaDscName()
{
  return Form("P%03d", fiDefParaDscSeqNum++);
}

/*------------------------------------------+---------------------------------*/
//! Return pointer to action with name \a name and type \a type (or 0).
/*!
  Scans the list of actions for an action with the name \a name. 
  If \a type is non-nil and non-empty, the type of the action object is 
  checked against \a type. If no match is found a 0 is returned.
 */

TAGaction* TAGroot::FindAction(const char* name, const char* type) const
{
  TAGaction* p = (TAGaction*) fpActionList->FindObject(name);
  if (p && type && type[0]) {
    if (strcmp(p->ClassName(), type) != 0) p = 0;
  }
  return p;
}

/*------------------------------------------+---------------------------------*/
//! Return pointer to data descriptor with name \a name (or 0).
/*!
  Scans the list of data descriptors for a discriptor with the name
  \a name. If \a type is non-nil and non-empty, the type of the data
  object is checked against \a type. If no match is found a 0 is returned.
 */

TAGdataDsc* TAGroot::FindDataDsc(const char* name, const char* type) const
{
  TAGdataDsc* p = (TAGdataDsc*) fpDataDscList->FindObject(name);
  if (p && type && type[0]) {
    if (strcmp(p->Object()->ClassName(), type) != 0) p = 0;
  }
  return p;
}

/*------------------------------------------+---------------------------------*/
//! Return pointer to data descriptor with name \a name (or 0).
/*!
  Scans the list of parameter descriptors for a discriptor with the name
  \a name. If \a type is non-nil and non-empty, the type of the parameter
  object is checked against \a type. If no match is found a 0 is returned.
 */

TAGparaDsc* TAGroot::FindParaDsc(const char* name, const char* type) const
{
  TAGparaDsc* p = (TAGparaDsc*) fpParaDscList->FindObject(name);
  if (p && type && type[0]) {
    if (strcmp(p->Object()->ClassName(), type) != 0) p = 0;
  }
  return p;
}

/*------------------------------------------+---------------------------------*/
//! Set campaign and run number from TAGrunInfo \a info .

void TAGroot::SetRunInfo(const TAGrunInfo& info)
{
  fRunInfo = info;
}

/*------------------------------------------+---------------------------------*/
//! Set campaign, run , and event number from TAGeventId \a info .

void TAGroot::SetEventId(const TAGeventId& info)
{
  fEventId= info;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGroot::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGroot: " << endl;
  os << "Known Actions:" << endl;
  os << "  name              type" << endl;
  for (TObjLink* lnk = fpActionList->FirstLink(); lnk; lnk=lnk->Next()) {
    TAGaction* p = (TAGaction*) lnk->GetObject();
    os << "  " << Form("%-16s", p->GetName())
       << "  " << Form("%-24s", p->IsA()->GetName()) << endl;
  }
  
  os << "Known ParaDsc's:" << endl;
  os << "  name              type" << endl;
  for (TObjLink* lnk = fpParaDscList->FirstLink(); lnk; lnk=lnk->Next()) {
    TAGparaDsc* p = (TAGparaDsc*) lnk->GetObject();
    os << "  " << Form("%-16s", p->GetName())
       << "  " << Form("%-24s", p->Object()->IsA()->GetName()) << endl;
  }

  os << "Known DataDsc's:" << endl;
  os << "  name              type                      produced by" << endl;
  for (TObjLink* lnk = fpDataDscList->FirstLink(); lnk; lnk=lnk->Next()) {
    TAGdataDsc* p = (TAGdataDsc*) lnk->GetObject();
    os << "  " << Form("%-16s", p->GetName())
       << "  " << Form("%-24s", p->Object()->IsA()->GetName());
    if (p->Producer()) os << "  " << p->Producer()->GetName();
    os << endl;
  }

  os << "Required Actions:" << endl;
  for (TObjLink* lnk = fpRequiredActionList->FirstLink(); 
       lnk; lnk=lnk->Next()) {
    TAGaction* p = (TAGaction*) lnk->GetObject();
    os << "  " << Form("%-16s", p->GetName())
       << "  " << Form("%-24s", p->IsA()->GetName()) << endl;
  }
  
  os << "Required DataDsc's:" << endl;
  for (TObjLink* lnk = fpRequiredDataDscList->FirstLink(); 
       lnk; lnk=lnk->Next()) {
    TAGdataDsc* p = (TAGdataDsc*) lnk->GetObject();
    os << "  " << Form("%-16s", p->GetName())
       << "  " << Form("%-24s", p->Object()->IsA()->GetName()) << endl;
  }

  return;
}

/*------------------------------------------+---------------------------------*/

TAGroot* gTAGroot=0;
