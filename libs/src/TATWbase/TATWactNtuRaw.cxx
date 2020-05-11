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
			     TAGparaDsc* p_calmap,
			     TAGparaDsc* p_pargeoG)
  : TAGaction(name, "TATWactNtuRaw - Unpack TW raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParGeo(p_pargeom),
    fpParMap(p_parmap),
    fpCalPar(p_calmap),
    fpParGeo_Gl(p_pargeoG),
    fTofPropAlpha(0.066), // velocity^-1 of light propagation in the TW bar (ns/cm)
    fTofErrPropAlpha(2.e-03)
{
  AddDataIn(p_datraw, "TATWdatRaw");
  AddDataOut(p_nturaw, "TATWntuRaw");
  AddPara(p_pargeom, "TATWparGeo");
  AddPara(p_parmap,"TATWparMap");
  AddPara(p_calmap,"TATWparCal");
  AddPara(p_pargeoG, "TAGparGeo");

 
  f_pargeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();

  Z_beam = f_pargeo->GetBeamPar().AtomicNumber;
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
   
  for(int ilayer=0; ilayer<TATWparCal::kLayers; ilayer++) {
    fpHisElossTof_layer[ilayer] = new TH2D(Form("dE_vs_Tof_layer%d",ilayer),Form("dE_vs_Tof_ilayer%d",ilayer),500,0.,50.,480,0.,120.);
    AddHistogram(fpHisElossTof_layer[ilayer]);
  }
  
  for(int iZ=1; iZ < Z_beam; iZ++) {
    
    fpHisElossTof_Z.push_back( new TH2D(Form("dE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),500,0.,50.,480,0.,120.) );

    AddHistogram(fpHisElossTof_Z[iZ-1]);
  }
  
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
  TATWparCal*   p_parcal = (TATWparCal*)    fpCalPar->Object();
  TAGparGeo*    p_pargeo_gl = (TAGparGeo*)  fpParGeo_Gl->Object();

  TAGgeoTrafo* p_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
  p_nturaw->SetupClones();
  CChannelMap *chmap=p_parmap->GetChannelMap();
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
  for (auto it=chmap->begin();it!=chmap->end();++it)
    {
      //
      Int_t boardid=get<0>(it->second);
      int channelA=get<1>(it->second);
      int channelB=get<2>(it->second);
      int BarId=it->first;
      //
      if (PMap.find(boardid)!=PMap.end())
	{
	  TATWrawHit* hita=PMap[boardid][channelA];
	  TATWrawHit* hitb=PMap[boardid][channelB];
	  // if one of the channels was not acquired
	  // not present, do not create the Hit
	  if (hita!=nullptr && hitb!=nullptr )
	    {
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

	      int Layer = (int)chmap->GetBarLayer(BarId);
	      fCurrentHit = (TATWntuHit*)p_nturaw->NewHit(Layer,ShoeBarId, Energy,Time,TimeOth,rawPos,chargeCOM,
							  ChargeA,ChargeB,TimeA,TimeB,TimeAOth,TimeBOth);
	      Int_t Zrec = p_parcal->GetChargeZ(Energy,Time,Layer);
	      fCurrentHit->SetChargeZ(Zrec);
	      
	      if (ValidHistogram()) {
		fpHisDeTot->Fill(Energy);
		fpHisTimeTot->Fill(Time);
		fpHisElossTof_layer[Layer]->Fill(Time,Energy);

		if(Zrec>0 && Zrec<Z_beam+1)
		  fpHisElossTof_Z[Zrec-1]->Fill(Time,Energy);
	      }
	      
	    }
	}
    }
  fpNtuRaw->SetBit(kValid);
  return kTRUE;
  
}

//________________________________________________________________

Double_t TATWactNtuRaw::GetRawEnergy(TATWrawHit*a,TATWrawHit*b)
{

  // if the waveform is strange/corrupted it is likely that the charge is <0
  if (a->GetCharge()<0 || b->GetCharge()<0)
    {
      return -1;
    }
  return TMath::Sqrt(a->GetCharge()*b->GetCharge());

}
//________________________________________________________________

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
//________________________________________________________________

Double_t  TATWactNtuRaw::GetTime(Double_t RawTime, Int_t BarId)
{
  return RawTime;
}
//________________________________________________________________

Double_t  TATWactNtuRaw::GetTimeOth(Double_t RawTimeOth, Int_t BarId)
{
  return RawTimeOth;
}
//________________________________________________________________

Double_t TATWactNtuRaw::GetRawTime(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()+b->GetTime())/2;
}
//________________________________________________________________

Double_t TATWactNtuRaw::GetRawTimeOth(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTimeOth()+b->GetTimeOth())/2;
}
//________________________________________________________________

Double_t TATWactNtuRaw::GetPosition(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()-b->GetTime())/(2*fTofPropAlpha);
}
//________________________________________________________________

Double_t TATWactNtuRaw::GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b)
{
  return TMath::Log(a->GetCharge()/b->GetCharge());
}
//________________________________________________________________








