/*!
  \file
  \version $Id: TATWdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TATWdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TATWdatRaw.hxx"
#include "TGraph.h"
#include "TF1.h"
/*!
  \class TATWdatRaw TATWdatRaw.hxx "TATWdatRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TATWrawHit);

TString TATWdatRaw::fgkBranchName   = "twdat.";

TATWrawHit::TATWrawHit(TWaveformContainer &W)
  : TAGbaseWD(W)
{
}

//______________________________________________________________________________
//
TATWrawHit::~TATWrawHit()
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWrawHit::TATWrawHit()
  : TAGbaseWD()
{
}

TATWrawHit::TATWrawHit(Int_t cha ,Int_t board, Double_t charge,
		       Double_t amplitude, Double_t pedestal,
		       Double_t time,Int_t isclock,Double_t clock_time,
		       Int_t TriggerType ) :
  TAGbaseWD(cha,board,charge,amplitude,pedestal,time,
	    isclock,clock_time,TriggerType)
{
}

//##############################################################################

ClassImp(TATWdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWdatRaw::TATWdatRaw() :
  nirhit(0), hir(0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TATWdatRaw::~TATWdatRaw() {
  delete hir;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TATWdatRaw::SetupClones()
{
  if (!hir) hir = new TClonesArray("TATWrawHit");
  return;
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TATWdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  nirhit = 0;

  if (hir) hir->Clear();
  return;
}


void TATWdatRaw::NewHit(TWaveformContainer &W)
{
  // get channel/board id
  Int_t cha =W.ChannelId;
  Int_t board =W.BoardId;
  W.SanitizeWaveform(); 
  // do not change the order of these methods
  Double_t pedestal=W.ComputePedestal();
  Double_t amplitude=W.ComputeAmplitude();
  Double_t charge= W.ComputeCharge();
  Double_t time= W.ComputeTimeStamp();
  Double_t ClockRaisingTime=-1;
  Int_t TriggerType= W.TrigType;

  if (W.IsAClock())
  {
	  ClockRaisingTime=W.FindFirstRaisingEdgeTime();
  }

  TClonesArray &pixelArray = *hir;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(cha ,board, charge, amplitude, pedestal, time, W.IsAClock(),ClockRaisingTime,TriggerType);
  nirhit++;
  return;
}

void TATWdatRaw::NewHit(int cha, int board, double pedestal, double amplitude, double charge, double time, int TriggerType, bool isclock, double ClockRaisingTime)
{

  TClonesArray &pixelArray = *hir;
  TATWrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWrawHit(cha ,board, charge, amplitude, pedestal, time, isclock, ClockRaisingTime,TriggerType);
  nirhit++;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TATWdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TATWdatRaw " << GetName()
	 << " nirhit"    << nirhit
     << endl;
  return;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWrawHit* TATWdatRaw::Hit(Int_t i)
{
  return (TATWrawHit*) ((*hir)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TATWrawHit* TATWdatRaw::Hit(Int_t i) const
{
  return (const TATWrawHit*) ((*hir)[i]);;
}
