/*!
  \file
  \version $Id: TATWactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TATWactNtuRaw.
*/



#include "TATWparMap.hxx"
#include "TATWactNtuRaw.hxx"
#include "TMath.h"
#include "TH1F.h"
#include <map>
#include "TATWparCal.hxx"
#include "CCalibrationMap.hxx"

/*!
  \class TATWactNtuRaw TATWactNtuRaw.hxx "TATWactNtuRaw.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TATWactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuRaw::TATWactNtuRaw(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw,
			     TAGparaDsc* p_pargeom,
				 TAGparaDsc* p_parmap,
				 TAGparaDsc* p_calmap)
  : TAGaction(name, "TATWactNtuRaw - Unpack TW raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParGeo(p_pargeom),
    fpParMap(p_parmap),
    fpCalPar(p_calmap),
    fTofPropAlpha(0.280/2.), // velocity of the difference need to divide by 2 (ps/cm)
   fTofErrPropAlpha(2.5)
{
  AddDataIn(p_datraw, "TATWdatRaw");
  AddDataOut(p_nturaw, "TATWntuRaw");
  AddPara(p_pargeom, "TATWparGeo");
  AddPara(p_parmap,"TATWparMap");
  AddPara(p_calmap,"TATWparCal");


  m_debug = GetDebugLevel();
  
}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuRaw::~TATWactNtuRaw()
{}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuRaw::CreateHistogram()
{
   DeleteHistogram();
   
   fpHisDeTot = new TH1F("twDeTot", "TW - Total Energy Loss", 500, 0., 300.);
   AddHistogram(fpHisDeTot);
   
   fpHisTimeTot = new TH1F("twTimeTot", "TW - Total Time Of Flight", 500, 0., 300);
   AddHistogram(fpHisTimeTot);
   
   SetValidHistogram(kTRUE);
   
   return;
}
//------------------------------------------+-----------------------------------
//! Action.

Bool_t TATWactNtuRaw::Action() {

   TATWdatRaw*   p_datraw = (TATWdatRaw*) fpDatRaw->Object();
   TATWntuRaw*   p_nturaw = (TATWntuRaw*)  fpNtuRaw->Object();
   TATWparGeo*   p_pargeo = (TATWparGeo*)  fpParGeo->Object();
   TATWparMap*   p_parmap = (TATWparMap*)  fpParMap->Object();

   p_nturaw->SetupClones();
   CChannelMap *c=p_parmap->getChannelMap();
   int nhit = p_datraw->GetHitsN();

   int ch_num, bo_num;
   map<int, vector<TATWrawHit*> > PMap;
   // loop over the hits to populate a map with key boardid and value std::vector of TATWrawHit  pointer
   for(int ih = 0; ih< nhit; ih++){
       TATWrawHit *aHi = p_datraw->GetHit(ih);
       ch_num = aHi->GetChID();
       bo_num = aHi->GetBoardId();
       aHi->SetTime(aHi->GetTime());
       aHi->SetTimeOth(aHi->GetTimeOth());
       if (PMap.find(aHi->GetBoardId())==PMap.end()){
	 PMap[aHi->GetBoardId()].resize(NUMBEROFCHANNELS);
	 for (int ch=0;ch<NUMBEROFCHANNELS;++ch){
	   PMap[aHi->GetBoardId()][ch]=nullptr;
	 }
       }
       PMap[aHi->GetBoardId()][aHi->GetChID()]=aHi;
   }
   

   // loop over all the bars
   for (auto it=c->begin();it!=c->end();++it){
       //
       Int_t boardid=get<0>(it->second);
       int channelA=get<1>(it->second);
       int channelB=get<2>(it->second);
       int BarId=it->first;
       //
       if (PMap.find(boardid)!=PMap.end()){
	   TATWrawHit* hita=PMap[boardid][channelA];
	   TATWrawHit* hitb=PMap[boardid][channelB];
	   // if one of the channels was not acquired
	   // not present, do not create the Hit
	   if (hita!=nullptr && hitb!=nullptr ){
	     // get raw energy
	     Double_t RawEnergy=GetRawEnergy(hita,hitb);
	     // calibrated the energy to MeV
	     Double_t Energy=GetEnergy(RawEnergy,BarId);
	     //get raw time in ns
	     Double_t RawTime=GetRawTime(hita,hitb);
	     Double_t RawTimeOth=GetRawTimeOth(hita,hitb);
	     // get calibrated time in ns
	     Double_t Time=GetTime(RawTime,BarId);
	     Double_t TimeOth=GetTimeOth(RawTimeOth,BarId);
	     // get position from the TOF between the two channels
	     Double_t rawPos=GetPosition(hita,hitb);
	     Double_t chargeCOM=GetChargeCenterofMass(hita,hitb);
	     //
	     Int_t NumberOfBarsPerLayer=NUMBEROFBARS/NUMBEROFLAYERS;
	     // this to be consistent with the the bar id of  TATWdetector.map
	     Int_t ShoeBarId=(BarId)%NumberOfBarsPerLayer;
	     // get Charge and time on side a of the bar
	     Double_t ChargeA=hita->GetCharge();
	     Double_t TimeA=hita->GetTime();
	     Double_t TimeAOth=hita->GetTimeOth();
	     // get Charge and time on side b of the bar
	     Double_t ChargeB=hitb->GetCharge();
	     Double_t TimeB=hitb->GetTime();
	     Double_t TimeBOth=hitb->GetTimeOth();
	     p_nturaw->NewHit((int)c->GetBarLayer(BarId),ShoeBarId, Energy,Time,TimeOth,rawPos,chargeCOM,
			      ChargeA,ChargeB,TimeA,TimeB,TimeAOth,TimeBOth);
	     fpHisDeTot->Fill(Energy);
	     fpHisTimeTot->Fill(Time);
	     }
       }
   }
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}


Double_t TATWactNtuRaw::GetRawEnergy(TATWrawHit*a,TATWrawHit*b)
{
	// if the waveform is strange/corrupted it is likely that the charge is <0
	if (a->GetCharge()<0|| b->GetCharge()<0)
	{
		return -1;
	}
	return TMath::Sqrt(a->GetCharge()*b->GetCharge());
}
Double_t TATWactNtuRaw::GetEnergy(Double_t RawEnergy,Int_t BarId)
{
	if (RawEnergy<0)
	{
		return -1;
	}
	TATWparCal*   p_calmap = (TATWparCal*)    fpCalPar->Object();
	Double_t p0=p_calmap->getCalibrationMap()->GetBarParameter(BarId,0);
	Double_t p1=p_calmap->getCalibrationMap()->GetBarParameter(BarId,1);
	// correct using the Birk's Law
	return RawEnergy/(p0-RawEnergy*p1);
}

Double_t  TATWactNtuRaw::GetTime(Double_t RawTime, Int_t BarId)
{
  return RawTime;
}

Double_t  TATWactNtuRaw::GetTimeOth(Double_t RawTimeOth, Int_t BarId)
{
  return RawTimeOth;
}


Double_t TATWactNtuRaw::GetRawTime(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()+b->GetTime())/2;
}

Double_t TATWactNtuRaw::GetRawTimeOth(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTimeOth()+b->GetTimeOth())/2;
}

Double_t TATWactNtuRaw::GetPosition(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()-b->GetTime())/fTofPropAlpha;
}

Double_t TATWactNtuRaw::GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b)
{
  return TMath::Log(a->GetCharge()/b->GetCharge());
}









