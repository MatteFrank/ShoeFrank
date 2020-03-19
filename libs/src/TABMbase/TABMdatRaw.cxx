/*!
  \file
  \version $Id: TABMdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TABMdatRaw.
*/

//using namespace std;

#include "TABMdatRaw.hxx"


/*!
  \class TABMdatRaw TABMdatRaw.hxx "TABMdatRaw.hxx"
  \brief Mapping and Geometry parameters for Tof wall. **
*/


ClassImp(TABMdatRaw);

TString TABMdatRaw::fgkBranchName   = "bmdat.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMdatRaw::TABMdatRaw() {
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMdatRaw::~TABMdatRaw() {}

//------------------------------------------+-----------------------------------
//! Setup mapping data for a single slat.

void TABMdatRaw::SetHitData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
{

  Int_t i_ind = (Int_t) fHitList.size();
  fHitList.push_back(TABMrawHit());
 
  fHitList[i_ind].SetData(id, lay,view,cell,time);
  
  return;
}

/*------------------------------------------+---------------------------------*/
//! Set statistics counters.

void TABMdatRaw::AddDischarged()
{
  fiNDrop++;
  return;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();

  fHitList.clear();
  fiNDrop=0;
  fTrigTime=-1000;
  return;
}

/*------------------------------------------+---------------------------------*/
//! helper to format one converter value

static void print_value(ostream& os, Int_t i_val)
{
  char c_ran = (i_val & 0x1000) ? 'h' : 'l';
  os << Form("%4d", i_val & 0x0fff) << " " << c_ran;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TABMdatRaw " << GetName()
     << Form("  nhit=%3d", NHit())
     << Form("  ndrop=%3d", NDrop())
     << endl;
  
  os << "slat stat    adct    adcb    tdct    tdcb" << endl;
  for (Int_t i = 0; i < NHit(); i++) {
    const TABMrawHit& hit = Hit(i);
    os << Form("%4d", hit.GetCell());
    os << "  "; print_value(os, hit.GetPlane());
    os << endl;
  }
  return;
}
