/*!
  \file
  \version $Id: TATWactNtuHit.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TATWactNtuHit.
*/

#include <map>

#include "TError.h"
#include "TMath.h"
#include "TH1F.h"

#include "TATWactNtuHit.hxx"
#include "TATWcalibrationMap.hxx"

/*!
  \class TATWactNtuHit TATWactNtuHit.hxx "TATWactNtuHit.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TATWactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuHit::TATWactNtuHit(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw,
			     TAGdataDsc* p_STnturaw,
			     TAGparaDsc* p_pargeom,
			     TAGparaDsc* p_parmap,
			     TAGparaDsc* p_calmap,
			     TAGparaDsc* p_pargeoG)
  : TAGaction(name, "TATWactNtuHit - Unpack TW raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpSTNtuRaw(p_STnturaw),
    fpParGeo(p_pargeom),
    fpParMap(p_parmap),
    fpCalPar(p_calmap),
    fpParGeo_Gl(p_pargeoG),
    f_geoTrafo(nullptr),
    //GSI:
    // fTofPropAlpha(0.066), // velocity^-1 of light propagation in the TW bar (ns/cm)
    // fTofErrPropAlpha(2.e-03),
    //Morrocchi:
    fTofPropAlpha(67.43e-03), // velocity^-1 of light propagation in the TW bar (ns/cm)
    fTofErrPropAlpha(0.09e-03),  // ns/cm
    fEvtCnt(0)
{

  AddDataIn(p_datraw, "TATWdatRaw");
  AddDataIn(p_STnturaw, "TASTntuRaw");
  AddDataOut(p_nturaw, "TATWntuHit");

  AddPara(p_pargeom, "TATWparGeo");
  AddPara(p_parmap, "TATWparMap");
  AddPara(p_calmap, "TATWparCal");
  AddPara(p_pargeoG, "TAGparGeo");


  f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   

  // global geo object
  f_pargeo_gl = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();

  fZbeam = f_pargeo_gl->GetBeamPar().AtomicNumber;
 
  // TW geo cal and map object
  f_pargeo = (TATWparGeo*) fpParGeo->Object();
  f_parmap = (TATWparMap*) fpParMap->Object();
  f_parcal = (TATWparCal*) fpCalPar->Object();

  fpHisElossTof_Z.clear();

  for(int ilayer=0; ilayer<nLayers; ilayer++)
    fpHisEloss_Z[ilayer].clear();
    // fpHisEloss_Z_try[ilayer].clear();

  // fpHisEloss_Z.clear();
  fpHisTof_Z.clear();


  f_debug = GetDebugLevel();


}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuHit::~TATWactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuHit::CreateHistogram()
{

  DeleteHistogram();
   
  fpHisDeTot = new TH1F("twDeTot", "TW - Total Energy Loss", 480, 0., 120.);
  AddHistogram(fpHisDeTot);
   
  fpHisTimeTot = new TH1F("twTimeTot", "TW - Total Time Of Flight", 5000, 0., 50);
  AddHistogram(fpHisTimeTot);
   
  for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++) {
    fpHisElossTof_layer[ilayer] = new TH2D(Form("dE_vs_Tof_layer%d",ilayer),Form("dE_vs_Tof_ilayer%d",ilayer),500,0.,50.,480,0.,120.);
    AddHistogram(fpHisElossTof_layer[ilayer]);
  }
  
  for(int iZ=1; iZ < fZbeam+1; iZ++) {
    
    fpHisElossTof_Z.push_back( new TH2D(Form("dE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),5000,0.,50.,480,0.,120.) );

    AddHistogram(fpHisElossTof_Z[iZ-1]);

    fpHisTof_Z.push_back( new TH1D(Form("hTof_Z%d",iZ),Form("hTof_%d",iZ),5000,0.,50.) );

    AddHistogram(fpHisTof_Z[iZ-1]);

    for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++) {
      
      fpHisEloss_Z[ilayer].push_back( new TH1D(Form("hEloss_Z%d_lay%d",iZ,ilayer),Form("hEloss_%d_lay%d",iZ,ilayer),200,0.,100.) );

      AddHistogram(fpHisEloss_Z[ilayer][iZ-1]);
    }

  }
  
  SetValidHistogram(kTRUE);
   
  return;
}
//------------------------------------------+-----------------------------------
//! Action.

Bool_t TATWactNtuHit::Action() {

  TATWdatRaw*   p_datraw = (TATWdatRaw*) fpDatRaw->Object();
  TASTntuRaw*   p_STnturaw = (TASTntuRaw*)  fpSTNtuRaw->Object();
  TATWntuHit*   p_nturaw = (TATWntuHit*)  fpNtuRaw->Object();

  //////////// Time Trigger info from ST ///////////////
  
  if(FootDebugLevel(1)) {
    cout<<""<<endl;
    Info("Action()","Evt N::%d",fEvtCnt);
    fEvtCnt++;
  }

  double STtrigTime = p_STnturaw->GetTriggerTime();
  double STtrigTimeOth = p_STnturaw->GetTriggerTimeOth();

  Int_t SThitN = p_STnturaw->GetHitsN();  // same of STtrigTime

  TASTntuHit *stHit = (TASTntuHit*)p_STnturaw->GetHit(0);
  Double_t time_st = stHit->GetTime();

  if(FootDebugLevel(1))
    cout<<"ST hitN::"<<SThitN<<" trigTime::"<<STtrigTime<<"  trigTimeOth::"<<STtrigTimeOth<<" time_ST::"<<time_st<<endl;

  /////////////////////////////////////////////////////
  
  p_nturaw->SetupClones();
  TATWchannelMap *chmap=f_parmap->GetChannelMap();
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

  // create map to store hits for calibration purpose
  map< Int_t,vector<TATWhit*> > hitMap;
  hitMap.clear();

  // loop over all the bars
  for (auto it=chmap->begin();it!=chmap->end();++it)
    {
      //
      Int_t BarId = it->first;
      
      Int_t boardid  = get<0>(it->second);
      Int_t channelA = get<1>(it->second);
      Int_t channelB = get<2>(it->second);
      
      Int_t Layer = (Int_t)chmap->GetBarLayer(BarId);

      //      
      if (PMap.find(boardid)!=PMap.end()) {

	if(FootDebugLevel(1))
	  Info("LoadChannelMap()","BAR_ID, %d LAYER_ID %d - BOARD ID %d, Channel A %d, Channel B %d",BarId,Layer,boardid,channelA,channelB);
	
	TATWrawHit* hita = PMap[boardid][channelA];
	TATWrawHit* hitb = PMap[boardid][channelB];
	
	// if one of the channels was not acquired
	// not present, do not create the Hit
	if (hita!=nullptr && hitb!=nullptr )
	  {

	    // this to be consistent with the the bar id of TATWdetector.geo [barId=0-19]
	    Int_t ShoeBarId = (BarId)%nSlatsPerLayer;

	    // get Charge, Amplitude (pedesta subtracted) and time on side a of the bar
	    Double_t ChargeA  = hita->GetCharge();
	    Double_t AmplitudeA  = hita->GetAmplitude();
	    Double_t TimeA    = hita->GetTime();
	    Double_t TimeAOth = hita->GetTimeOth();

	    // get Charge, Amplitude (pedesta subtracted) and time on side b of the bar
	    Double_t ChargeB  = hitb->GetCharge();
	    Double_t AmplitudeB  = hitb->GetAmplitude();
	    Double_t TimeB    = hitb->GetTime();
	    Double_t TimeBOth = hitb->GetTimeOth();

	    // get raw energy
	    Double_t rawEnergy = GetRawEnergy(hita,hitb);

	    // get raw time in ns
	    Double_t rawTime    = GetRawTime(hita,hitb) - STtrigTime;
	    Double_t rawTimeOth = GetRawTimeOth(hita,hitb) - STtrigTimeOth;

	    Double_t chargeCOM  = GetChargeCenterofMass(hita,hitb);

	    // get position from the TOF between the two channels
	    Double_t posAlongBar = GetPosition(hita,hitb);
	    // get the PosId (0-399) of the cross btw horizontal and vertical bars
	    Int_t    PosId  = GetBarCrossId(Layer,ShoeBarId,posAlongBar);

	    Double_t btrain =  (hitb->GetTime() - hita->GetTime())/(2*fTofPropAlpha);  // local (TW) ref frame
	    Double_t atrain =  (hita->GetTime() - hitb->GetTime())/(2*fTofPropAlpha); 

	    if(FootDebugLevel(1)) {
	      cout<<"ta::"<<hita->GetTime()<<"   tb::"<<hitb->GetTime()<<"  alpha::"<<fTofPropAlpha<<endl;
	      cout<<"a::"<<atrain<<"  b::"<<btrain<<endl;
	      cout<<"Eraw::"<<rawEnergy<<" posId::"<<PosId<<" layer::"<<Layer<<endl;
	    }
	    // get calibrated energy in MeV
	    Double_t Energy = GetEnergy(rawEnergy,Layer,PosId,BarId);
	    // cout<<"qA::"<<ChargeA<<" qB::"<<ChargeB<<" Qtot::"<<rawEnergy<<"  Energy::"<<Energy<<" posId::"<<PosId<<" layer::"<<Layer<<endl;

	    // get time calibrated time in ns
	    Double_t Time    = GetTime(rawTime,Layer,PosId,BarId);
	    Double_t TimeOth = GetTimeOth(rawTimeOth,Layer,PosId,BarId);

	    if(FootDebugLevel(1)) {
	      if(posAlongBar<-22 || posAlongBar>22) {
		cout<<"layer::"<<Layer<<"  barId::"<<BarId<<"  shoeId::"<<ShoeBarId<<"  posId::"<<PosId<<"  pos::"<<posAlongBar<<endl;
		cout<<"eloss::"<<Energy<<" chA::"<<ChargeA<<"  chB::"<<ChargeB<<" Time::"<<Time<<" timeA::"<<TimeA<<" timeB::"<<TimeB<<endl;
		getchar();
	      }
	    }


	    fCurrentHit = (TATWhit*)p_nturaw->NewHit(Layer,ShoeBarId,Energy,Time,TimeOth,posAlongBar,chargeCOM,ChargeA,ChargeB,AmplitudeA, AmplitudeB,TimeA,TimeB,TimeAOth,TimeBOth);

	    Int_t Zrec = f_parcal->GetChargeZ(Energy,Time,Layer);
	    fCurrentHit->SetChargeZ(Zrec);
	    // ToF set as Time
	    fCurrentHit->SetToF(Time);

	    // cout<<"Time::"<<Time<<"  Tof::"<<fCurrentHit->GetToF()<<endl;

	    if (ValidHistogram()) {
	      fpHisDeTot->Fill(Energy);
	      fpHisTimeTot->Fill(Time);
	      fpHisElossTof_layer[Layer]->Fill(Time,Energy);
	      
	      if(Zrec>0 && Zrec<fZbeam+1) {
		// for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++)
		fpHisEloss_Z[Layer][Zrec-1]->Fill(Energy);
		// if(Layer==1)
		//   fpHisEloss_Z[Zrec-1]->Fill(Energy);
		fpHisTof_Z[Zrec-1]->Fill(Time);
		fpHisElossTof_Z[Zrec-1]->Fill(Time,Energy);
	      }
	    }
	  }
      } 
    } 
    
  
  fpNtuRaw->SetBit(kValid);

  return kTRUE;
  
}

//________________________________________________________________

Double_t TATWactNtuHit::GetRawEnergy(TATWrawHit*a,TATWrawHit*b)
{

  // if the waveform is strange/corrupted it is likely that the charge is <0
  if (a->GetCharge()<0 || b->GetCharge()<0)
    {
      return -1;
    }

  return TMath::Sqrt(a->GetCharge()*b->GetCharge());

}
//________________________________________________________________

Double_t TATWactNtuHit::GetEnergy(Double_t rawenergy, Int_t layerId, Int_t posId, Int_t barId)
{
  if ( rawenergy<0 || posId<0 || posId>=nSlatCross || (layerId!=LayerX && layerId!=LayerY) )
    {
      return -1;
    }

  Double_t Ecal;  // final calibrated energy
  Double_t p0 ,p1; // Birks' parameters

  if( f_parcal->IsBarCalibration() ) {

    p0 = f_parcal->getCalibrationMap()->GetBarElossParameter(barId,layerId,0);
    p1 = f_parcal->getCalibrationMap()->GetBarElossParameter(barId,layerId,1);
    
  } else if ( f_parcal->IsPosCalibration() ) {

    p0 = f_parcal->getCalibrationMap()->GetPosElossParameter(posId,layerId,0);
    p1 = f_parcal->getCalibrationMap()->GetPosElossParameter(posId,layerId,1);

  }
  else {
    Error("GetEnergy()","No energy calibration has been uploaded...returned raw energy");
    return rawenergy;
  }

  // correct using the Birk's Law
  Ecal = rawenergy / (p0 - rawenergy * p1);
  
  if( f_parcal->IsPosCalibration() && f_parcal->IsElossTuningON() ) {

    Double_t A = f_parcal->getCalibrationMap()->GetBarElossTuningParameter(barId,layerId,0);
    Double_t B = f_parcal->getCalibrationMap()->GetBarElossTuningParameter(barId,layerId,1);
    Double_t C = f_parcal->getCalibrationMap()->GetBarElossTuningParameter(barId,layerId,2);

    // cout<<"A::"<<A<<" B::"<<B<<" C::"<<C<<" Ecal::"<<Ecal<<endl;
    
    Ecal = (-B+sqrt(B*B-4*A*(C-Ecal)))/(2*A);
    
    // cout<<"A::"<<A<<" B::"<<B<<" C::"<<C<<" Ecal::"<<Ecal<<endl;
  }
  

  return Ecal;
  
}

//________________________________________________________________

Double_t TATWactNtuHit::GetChargeCenterofMass(TATWrawHit*a,TATWrawHit*b)
{

  if (a->GetCharge()<0 || b->GetCharge()<0)
    {
      return -1;
    }
  
  return TMath::Log(a->GetCharge()/b->GetCharge());
}

//________________________________________________________________

Double_t TATWactNtuHit::GetRawTime(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()+b->GetTime())/2;
}

//________________________________________________________________

Double_t TATWactNtuHit::GetRawTimeOth(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTimeOth()+b->GetTimeOth())/2;
}

//________________________________________________________________

Double_t  TATWactNtuHit::GetTime(Double_t RawTime, Int_t layerId, Int_t posId, Int_t barId)
{

  if ( posId<0 || posId>=nSlatCross || (layerId!=LayerX && layerId!=LayerY) )
    {
      return -1;
    }

  //delta Time to be applied to Raw time to get the calibrated value
  Double_t deltaT, deltaT2;

  if( f_parcal->IsBarCalibration() ) {

    deltaT  = f_parcal->getCalibrationMap()->GetBarTofParameter(barId,layerId,0);
    deltaT2 = f_parcal->getCalibrationMap()->GetBarTofParameter(barId,layerId,1);
    
  } else if ( f_parcal->IsPosCalibration() ) {

    deltaT  = f_parcal->getCalibrationMap()->GetPosTofParameter(posId,layerId,0);
    deltaT2 = f_parcal->getCalibrationMap()->GetPosTofParameter(posId,layerId,1);

  }
  
  if(FootDebugLevel(1))
    cout<<"rawTime::"<<RawTime<<" deltaT::"<<deltaT<<" deltaT2::"<<deltaT2<<" TOF::"<<RawTime-deltaT<<endl;
  
  return RawTime - deltaT;

}
//________________________________________________________________

Double_t  TATWactNtuHit::GetTimeOth(Double_t RawTimeOth, Int_t layerId, Int_t posId, Int_t barId)
{

    if ( posId<0 || posId>=nSlatCross || (layerId!=LayerX && layerId!=LayerY) )
    {
      return -1;
    }

  TATWparCal*   p_calmap = (TATWparCal*)    fpCalPar->Object();

  //delta Time to be applied to Raw time to get the calibrated value
  Double_t deltaT, deltaT2;

  if( f_parcal->IsBarCalibration() ) {

    deltaT2 = f_parcal->getCalibrationMap()->GetBarTofParameter(barId,layerId,0);
    deltaT  = f_parcal->getCalibrationMap()->GetBarTofParameter(barId,layerId,1);
    
  } else if ( f_parcal->IsPosCalibration() ) {

    deltaT2 = f_parcal->getCalibrationMap()->GetPosTofParameter(posId,layerId,0);
    deltaT  = f_parcal->getCalibrationMap()->GetPosTofParameter(posId,layerId,1);

  }

  if(FootDebugLevel(1))
    cout<<"rawTimeOth::"<<RawTimeOth<<" deltaT::"<<deltaT<<" deltaT2::"<<deltaT2<<" TOF::"<<(RawTimeOth-deltaT)<<endl;

  return (RawTimeOth - deltaT);
  
}

//________________________________________________________________

Double_t TATWactNtuHit::GetPosition(TATWrawHit*a,TATWrawHit*b)
{
  return (b->GetTime() - a->GetTime())/(2*fTofPropAlpha);  // local (TW) ref frame
  // A <----------- B
}

//________________________________________________________________

Int_t TATWactNtuHit::GetBarCrossId(Int_t layer, Int_t barId, Double_t rawPos)
{
  
  Int_t barX(-1), barY(-1);  // id of horizontal and vertical bars
  Int_t barCrossId = -1;     // id of the cross btw vertical and horizontal bars

  // get the bar perpendicular to the position along the bar rawPos
  Int_t perp_BarId = GetPerpBarId(layer,barId,rawPos);

  if(perp_BarId==-1)
    return -1;
  
  if(layer==LayerX) {

    barX = barId;
    barY = perp_BarId;

  } else if(layer==LayerY) {

    barX = perp_BarId;
    barY = barId;

  }
  else {
    Error("GetBarCrossId()","Layer %d doesn't exist",layer);
    return -1;
  }

  barCrossId = barY + barX * nSlatsPerLayer;  // left top corner is (0,0) --> pos=0, right bottom corner is (19,19) --> pos = 399

  if(FootDebugLevel(1))
    Info("GetBarCrossId()","%s, bar::%d, rawPos::%f, perp_BarId::%d, barCrossId::%d\n",LayerName[(TLayer)layer].data(),barId,rawPos,perp_BarId,barCrossId);
  

  return barCrossId;
  
}


//_____________________________________________________________________________

Int_t TATWactNtuHit::GetPerpBarId(Int_t layer, Int_t barId, Double_t rawPos)
{

  Double_t xloc(-99.), yloc(-99.);
  Double_t zloc = f_pargeo->GetLayerPosition(layer)[2];

  Int_t  perp_BarId = -1; //barId of the perpendicular bar corresponding to rawPos

  if(layer==LayerX) {
    xloc = rawPos;
    yloc = f_pargeo->GetBarPosition(layer,barId)[1];
    double x_glb  = xloc;
    double y_glb  = yloc;
    perp_BarId = f_pargeo->GetBarId(LayerY,x_glb,y_glb);  

    if(FootDebugLevel(1)) {
      cout<<"front:"<<"  barId::"<<barId<<endl;
      TVector3 vlocTW(xloc,yloc,zloc);
      cout<<"LOC::"<<vlocTW.x()<<"  "<<vlocTW.y()<<"  "<<vlocTW.z()<<endl;
      TVector3 vglbTW = f_geoTrafo->FromTWLocalToGlobal(vlocTW);
      cout<<"GLB::"<<vglbTW.x()<<"  "<<vglbTW.y()<<"  "<<vglbTW.z()<<endl;
      TVector3 vloc = f_geoTrafo->FromGlobalToTWLocal(vglbTW);
      cout<<"LOC::"<<vloc.x()<<"  "<<vloc.y()<<"  "<<vloc.z()<<endl;
      cout<<""<<endl;
    }
    
  }
  else if(layer==LayerY) {
    xloc = f_pargeo->GetBarPosition(layer,barId)[0];	      
    yloc = rawPos;
    perp_BarId = f_pargeo->GetBarId(LayerX,xloc,yloc);

    if(FootDebugLevel(1)) {
      cout<<"rear:"<<"  barId::"<<barId<<endl;
      TVector3 vlocTW(xloc,yloc,zloc);
      cout<<"LOC::"<<vlocTW.x()<<"  "<<vlocTW.y()<<"  "<<vlocTW.z()<<endl;
      TVector3 vglbTW = f_geoTrafo->FromTWLocalToGlobal(vlocTW);
      cout<<"GLB::"<<vglbTW.x()<<"  "<<vglbTW.y()<<"  "<<vglbTW.z()<<endl;
      TVector3 vloc = f_geoTrafo->FromGlobalToTWLocal(vglbTW);
      cout<<"LOC::"<<vloc.x()<<"  "<<vloc.y()<<"  "<<vloc.z()<<endl;
      cout<<""<<endl;
    }

  }
  else {
    Error("GetPerBarId()","Layer %d doesn't exist",layer);
    return -1;
  }

  if(perp_BarId==-1)
    return -1;
  
  perp_BarId -= TATWparGeo::GetLayerOffset()*(1-layer);

  if(FootDebugLevel(1))
    Info("GetPerpBarId()","%s, bar::%d, x::%f, y::%f, z::%f, perp_BarId::%d\n",LayerName[(TLayer)layer].data(),barId,xloc,yloc,zloc,perp_BarId);

  return perp_BarId;

}
