/*!
  \file
  \version $Id: TAGaction.cxx,v 1.16 2003/06/22 10:48:07 mueller Exp $
  \brief   Implementation of TAGaction.
*/

#include "TDirectory.h"
#include "TH1.h"

#include "TAGroot.hxx"
#include "TAGaction.hxx"

/*!
  \class TAGaction TAGaction.hxx "TAGaction.hxx"
  \brief Base class for actions. **
*/

ClassImp(TAGaction);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGaction::TAGaction(const char* name, const char* title)
  : TAGnamed(name, title),
    fpDataOutList(0),
    fpDataInList(0),
    fpParaList(0),
    fpHistList(0),
    fbHistValid(kFALSE),
    fbIsOpenFile(kFALSE),
    debugLevel(0)
{
  if (!gTAGroot) Fatal("TAGaction()", "TAGroot not instantiated");
  SetBit(kMustCleanup);

  if (name==0 || name[0]==0) SetName(gTAGroot->DefaultActionName());

  if (gTAGroot->FindAction(GetName())) {
    Warning("TAGaction()", "Action with name '%s' already exists", GetName());
  }

  gTAGroot->ListOfAction()->Add(this);

}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGaction::~TAGaction()
{
  delete fpDataOutList;
  delete fpDataInList;
  delete fpParaList;
  delete fpHistList;
}

//------------------------------------------+-----------------------------------
//! Invalidate action object.

void TAGaction::Clear(Option_t*)
{
  ResetBit(kValid|kEof|kFail);
  return;
}

//------------------------------------------+-----------------------------------
//! Process action.

Bool_t TAGaction::Process()
{
  if (Valid()) return kTRUE;
  if (IsZombie()) {
    Error("Process()", "Action %s is Zombie because dependency was deleted",
	  GetName());
    return kFALSE;
  }
  
  Bool_t b_ok = CheckDependencies();
  if (b_ok) b_ok = Action();
  if (b_ok) SetBit(kValid);

  return b_ok;
}

//------------------------------------------+-----------------------------------
//! Execute action.

Bool_t TAGaction::Action()
{
  return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Create histograms.

void TAGaction::CreateHistogram()
{
  return;
}

//------------------------------------------+-----------------------------------
//! Setup directory's histograms.
void TAGaction::SetHistogramDir(TDirectory* dir)
{
   if (fpHistList) {
	  for (TObjLink* lnk = fpHistList->FirstLink(); lnk; lnk=lnk->Next()) {
		 TH1* h = (TH1*)lnk->GetObject();
		  h->SetDirectory(dir);
     }
     fbIsOpenFile = true;
   }
   
   if (!dir->IsWritable()) fbIsOpenFile = false;
}

//------------------------------------------+-----------------------------------
//! Delete all histograms.

void TAGaction::DeleteHistogram()
{
  if (fpHistList) {
    TObjLink* lnk;
    while ( (lnk = fpHistList->FirstLink()) ) {
      TObject* p = fpHistList->Remove(lnk);
      delete p;
    }
  }
  SetValidHistogram(kFALSE);
  return;
}

//------------------------------------------+-----------------------------------
//! Clear histograms.

void TAGaction::ClearHistogram()
{
  if (fpHistList) {
    for (TObjLink* lnk = fpHistList->FirstLink(); lnk; lnk=lnk->Next()) {
      lnk->GetObject()->Clear();
    }
  }
  return;
}

//------------------------------------------+-----------------------------------
//! Call the Write() method for all histograms.
void TAGaction::WriteHistogram()
{
  if (!ValidHistogram()) return;
  if (!fbIsOpenFile) return;

  TDirectory* cd_pwd = gDirectory;
  if (fpHistList) {
    for (TObjLink* lnk = fpHistList->FirstLink(); lnk; lnk=lnk->Next()) {
	   TH1* h = (TH1*)lnk->GetObject();
	   gDirectory = h->GetDirectory();
      lnk->GetObject()->Write();
    }
  }
  gDirectory = cd_pwd;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAGaction::ToStream(ostream& os, Option_t* option) const
{
  /*
  os << "TAGaction:       " << IsA()->GetName() << " '" << GetName() 
     << "'" << endl;
  */
  if (fpDataOutList) {
    const char* pref = "  data output:   ";
    for (TObjLink* lnk = fpDataOutList->FirstLink(); lnk; lnk=lnk->Next()) {
      TObject* p = lnk->GetObject();
      os << pref << "'" << p->GetName() << "'" << endl;
      pref = "                 ";
    }
  }

  if (fpDataInList) {
    const char* pref = "  data input:    ";
    for (TObjLink* lnk = fpDataInList->FirstLink(); lnk; lnk=lnk->Next()) {
      TObject* p = lnk->GetObject();
      os << pref << "'" << p->GetName() << "'" << endl;
      pref = "                 ";
    }
  }

  if (fpParaList) {
    const char* pref = "  parameter:     ";
    for (TObjLink* lnk = fpParaList->FirstLink(); lnk; lnk=lnk->Next()) {
      TObject* p = lnk->GetObject();
      os << pref << "'" << p->GetName() << "'" << endl;
      pref = "                 ";
    }
  }
  return;
}

//------------------------------------------+-----------------------------------
//! Check dangling object references to TObject \a p_obj . -> Zombie

void TAGaction::RecursiveRemove(TObject* p_obj)
{
  Bool_t b_zombie = kFALSE;

  if (fpDataOutList) {
    while (fpDataOutList->Remove(p_obj)) b_zombie = kTRUE;
  }
  if (fpDataInList) {
    while (fpDataInList->Remove(p_obj)) b_zombie = kTRUE;
  }
  if (fpParaList) {
    while (fpParaList->Remove(p_obj)) b_zombie = kTRUE;
  }
  if (fpHistList) {
    while (fpHistList->Remove(p_obj)) SetValidHistogram(kFALSE);
  }

  if (b_zombie) MakeZombie();

  return;
}

//------------------------------------------+-----------------------------------
//! Add output data descriptor.

void TAGaction::AddDataOut(TAGdataDsc* p_data, const char* baseclass)
{
  if (!p_data) {
    Error("AddDataOut()", "descriptor object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_data->Object()) {
    Error("AddDataOut()", "data object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_data->Object()->InheritsFrom(baseclass)) {
    Error("AddDataOut()", "data object type '%s' not based on '%s'", 
	  p_data->Object()->GetName(), baseclass);
    MakeZombie();
    return;
  }
  
  if (!fpDataOutList) fpDataOutList = new TList();
  fpDataOutList->Add(p_data);

  if (p_data->Producer()) {
    Error("AddDataOut()", "data object '%s' already produced by action '%s'",
	  p_data->GetName(), p_data->Producer()->GetName());
    MakeZombie();
    return;
  }

  p_data->SetProducer(this);
  return;
}

//------------------------------------------+-----------------------------------
//! Add input data descriptor.

void TAGaction::AddDataIn(TAGdataDsc* p_data, const char* baseclass)
{
  if (!p_data) {
    Error("AddDataIn()", "descriptor object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_data->Object()) {
    Error("AddDataIn()", "data object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_data->Object()->InheritsFrom(baseclass)) {
    Error("AddDataIn()", "data object type '%s' not based on '%s'", 
	  p_data->Object()->GetName(), baseclass);
    MakeZombie();
    return;
  }
  
  if (!fpDataInList) fpDataInList = new TList();
  fpDataInList->Add(p_data);
  p_data->SetConsumer(this);
  return;
}

//------------------------------------------+-----------------------------------
//! Add parameter descriptor.

void TAGaction::AddPara(TAGparaDsc* p_para, const char* baseclass)
{
  if (!p_para) {
    Error("AddPara()", "descriptor object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_para->Object()) {
    Error("AddPara()", "para object pointer is nil");
    MakeZombie();
    return;
  }
  if (!p_para->Object()->InheritsFrom(baseclass)) {
    Error("AddPara()", "para object type '%s' not based on '%s'", 
	  p_para->Object()->GetName(), baseclass);
    MakeZombie();
    return;
  }
  
  if (!fpParaList) fpParaList = new TList();
  fpParaList->Add(p_para);
  p_para->SetConsumer(this);
  return;
}

//------------------------------------------+-----------------------------------
//! Add histogram.

void TAGaction::AddHistogram(TH1* p_hist)
{
  if (!p_hist) {
    Error("AddPara()", "histgram pointer is nil");
    MakeZombie();
    return;
  }
  
  if (!fpHistList) {
    fpHistList = new TList();
    fpHistList->SetOwner(kTRUE);
  }

  fpHistList->Add(p_hist);
  p_hist->SetBit(kMustCleanup);
  return;
}

//------------------------------------------+-----------------------------------
//! Check all dependdencies.

Bool_t TAGaction::CheckDependencies()
{
  Bool_t b_allvalid = kTRUE;
  
  if (IsZombie()) return kFALSE;

  if (fpDataInList) {
    Bool_t b_noeof  = kTRUE;
    Bool_t b_nofail = kTRUE;

    for (TObjLink* lnk = fpDataInList->FirstLink(); lnk; lnk=lnk->Next()) {
      TAGdataDsc* p = (TAGdataDsc*) lnk->GetObject();
      if (!p->Valid()) p->Generate();
      if (!p->Valid()) {
        if (p->Eof())  b_noeof  = kFALSE;
        if (p->Fail()) b_nofail = kFALSE;
        b_allvalid = kFALSE;
      }
    }

    if (!b_noeof)  SetBit(kEof);
    if (!b_nofail) SetBit(kFail);

  }
  return b_allvalid;
}

//------------------------------------------+-----------------------------------
//! Set a status bit in all output data descriptors.

void TAGaction::SetBitAllDataOut(UInt_t i_bits)
{
  if (fpDataOutList) {
    for (TObjLink* lnk = fpDataOutList->FirstLink(); lnk; lnk=lnk->Next()) {
      lnk->GetObject()->SetBit(i_bits);
    }
  }
  return;
}

