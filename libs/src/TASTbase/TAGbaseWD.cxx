/*!
  \file
  \version $Id: TAGbaseWD.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TAGbaseWD.
*/

#include <string.h>
#include <fstream>
#include <bitset>

#include <algorithm>
#include "TString.h"
#include "TAGbaseWD.hxx"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
/*!
  \class TAGbaseWD TAGbaseWD.hxx "TAGbaseWD.hxx"
*/

using namespace std;

ClassImp(TAGbaseWD);

TAGbaseWD::~TAGbaseWD()
{

  
}


TAGbaseWD::TAGbaseWD(TWaveformContainer &W)
{
  // set channel/board id
  chid=W.ChannelId;
  boardid=W.BoardId;

  // do not change the order of these methods
  pedestal=W.ComputePedestal();
  amplitude=W.ComputeAmplitude();
  chg= W.ComputeCharge();
  time= W.ComputeArrivalTime();
  triggertype=W.TrigType;
  clock_time = -9999.;
  if (W.IsAClock())clock_time=W.FindFirstRaisingEdgeTime();
  isclock = W.IsAClock();
  mcid = -999;
  chisquare = W.GetChiSquare();
  
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGbaseWD::TAGbaseWD()
  : time(999999.), chg(0.), chid(0),pedestal(0),
    amplitude(0),boardid(0), isclock(-1),clock_time(-9999.),triggertype(0), mcid(-999), chisquare(-999)
{
}



void TAGbaseWD::SetData(Int_t cha ,Int_t board, Double_t charge,
			Double_t ampl, Double_t apedestal, Double_t atime,
			Int_t iscl,Double_t clock_t,
			Int_t TriggerType) {
  
  time = atime;
  chg  = charge;
  pedestal=apedestal;
  amplitude=ampl;
  chid   = cha;
  boardid=board;
  isclock=iscl;
  clock_time=clock_t;
  triggertype=TriggerType;

  return;

}

Int_t  TAGbaseWD::IsClock()
{
  return isclock;
}



Double_t TAGbaseWD::Clocktime()
{
  if (IsClock())
    {
      return clock_time;
    }
  return -1;
}
