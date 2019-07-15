/*!
  \file
  \version $Id: TASTdatRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TASTdatRaw.
*/

#include <string.h>
#include <fstream>
#include <bitset>
using namespace std;
#include <algorithm>
#include "TString.h"
#include "TASTdatRaw.hxx"
#include "TGraph.h"
#include "TF1.h"
/*!
  \class TASTdatRaw TASTdatRaw.hxx "TASTdatRaw.hxx"
  \brief Mapping and Geometry parameters for IR detectors. **
*/

ClassImp(TASTrawHit);

TString TASTdatRaw::fgkBranchName   = "stdat.";

TASTrawHit::TASTrawHit(TWaveformContainer &W)
  : TAGbaseWD(W)
{
}

//______________________________________________________________________________
//
TASTrawHit::~TASTrawHit()
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTrawHit::TASTrawHit()
  : TAGbaseWD()
{
}

TASTrawHit::TASTrawHit(Int_t cha ,Int_t board, Double_t charge,
		       Double_t amplitude, Double_t pedestal,
		       Double_t time,Int_t isclock,Double_t clock_time,
		       Int_t TriggerType ) :
  TAGbaseWD(cha, board,charge,amplitude,pedestal,time,
	    isclock,clock_time,TriggerType)
{
}

//##############################################################################

ClassImp(TASTdatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTdatRaw::TASTdatRaw() :
  nirhit(0), hir(0), m_run_time(0x0)
{
   SetupClones();
}


//------------------------------------------+-----------------------------------
//! Destructor.

TASTdatRaw::~TASTdatRaw() {
  delete hir;
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TASTdatRaw::SetupClones()
{
  if (!hir) hir = new TClonesArray("TASTrawHit");
  return;
}


//------------------------------------------+-----------------------------------
//! Clear event.

void TASTdatRaw::Clear(Option_t*)
{
  TAGdata::Clear();
  nirhit = 0;

  if (hir) hir->Clear();
  return;
}


void TASTdatRaw::NewHit(TWaveformContainer &W)
{
  // get channel/board id
  Int_t cha =W.ChannelId;
  Int_t board =W.BoardId;
  W.SanitizeWaveform(); 
  // do not change the order of these methods
  Double_t pedestal=W.ComputePedestal();
  Double_t amplitude=W.ComputeAmplitude();
  Double_t charge= W.ComputeChargeST();
  Double_t time= W.ComputeArrivalTime();
  Double_t ClockRaisingTime=-1;
  Int_t TriggerType= W.TrigType;

  if (W.IsAClock())
    {
      ClockRaisingTime=W.FindFirstRaisingEdgeTime();
    }
  
  TClonesArray &pixelArray = *hir;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(cha ,board, charge, amplitude, pedestal, time, W.IsAClock(),ClockRaisingTime,TriggerType);
  nirhit++;
  return;
}

void TASTdatRaw::NewHit(int cha, int board, double pedestal, double amplitude, double charge, double time, int TriggerType, bool isclock, double ClockRaisingTime)
{

  TClonesArray &pixelArray = *hir;
  TASTrawHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TASTrawHit(cha ,board, charge, amplitude, pedestal, time, isclock, ClockRaisingTime,TriggerType);
  nirhit++;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TASTdatRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TASTdatRaw " << GetName()
	 << " nirhit"    << nirhit
     << endl;
  return;
}

//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TASTrawHit* TASTdatRaw::Hit(Int_t i)
{
  return (TASTrawHit*) ((*hir)[i]);;
}

//------------------------------------------+-----------------------------------
//! Read-only access \a i 'th hit

const TASTrawHit* TASTdatRaw::Hit(Int_t i) const
{
  return (const TASTrawHit*) ((*hir)[i]);;
}
