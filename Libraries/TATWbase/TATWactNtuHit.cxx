/*!
  \file TATWactNtuHit.cxx
  \brief   Implementation of TATWactNtuHit.
*/

#include <map>

#include "TError.h"
#include "TMath.h"
#include "TH1F.h"

#include "TAGnameManager.hxx"
#include "TATWactNtuHit.hxx"
#include "TATWcalibrationMap.hxx"

/*!
  \class TATWactNtuHit
  \brief Get TW raw data from WD. **
*/

//! Class Imp
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
    fEvtCnt(0)
{

  AddDataIn(p_datraw, "TATWntuRaw");
  if(p_STnturaw)
    AddDataIn(p_STnturaw, "TASTntuHit");
  else
    Warning("TATWactNtuHit","No ST information present");

  AddDataOut(p_nturaw, "TATWntuHit");

  AddPara(p_pargeom, "TATWparGeo");
  AddPara(p_parmap, "TATWparMap");
  AddPara(p_calmap, "TATWparCal");
  AddPara(p_pargeoG, "TAGparGeo");


  f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   

  // global geo object
  f_pargeo_gl = (TAGparGeo*)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object();

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
   
  for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++) {

    fpHisCharge[ilayer] = new TH1F(Form("twCharge_%s",LayerName[(TLayer)ilayer].data()), Form("twCharge_%s",LayerName[(TLayer)ilayer].data()), 10000, 0., 50.);
    AddHistogram(fpHisCharge[ilayer]);

    fpHisPos[ilayer] = new TH1F(Form("twPos_%s",LayerName[(TLayer)ilayer].data()), Form("twPos_%s",LayerName[(TLayer)ilayer].data()), 800, -20., 20.);
    AddHistogram(fpHisPos[ilayer]);
    
    fpHisRawTof[ilayer] = new TH1F(Form("twRawTof_%s",LayerName[(TLayer)ilayer].data()), Form("twRawTof_%s",LayerName[(TLayer)ilayer].data()), 5000, -50., 50);
    AddHistogram(fpHisRawTof[ilayer]);

    fpHisChargeCentralBar[ilayer] = new TH1F(Form("twChargeCentralBar_%s",LayerName[(TLayer)ilayer].data()), Form("twChargeCentralBar_%s",LayerName[(TLayer)ilayer].data()), 10000, 0., 50.);
    AddHistogram(fpHisChargeCentralBar[ilayer]);
        
    fpHisRawTofCentralBar[ilayer] = new TH1F(Form("twRawTofCentralBar_%s",LayerName[(TLayer)ilayer].data()), Form("twRawTofCentralBar_%s",LayerName[(TLayer)ilayer].data()), 5000, -50., 50);
    AddHistogram(fpHisRawTofCentralBar[ilayer]);

    fpHisRawTofBothCentralBarVsEloss[ilayer] = new TH2F(Form("twRawTofBothCentralBarVsEloss_%s",LayerName[(TLayer)ilayer].data()), Form("twRawTofBothCentralBarVsEloss_%s",LayerName[(TLayer)ilayer].data()), 5000, -50., 50, 490,0,120.);
    AddHistogram(fpHisRawTofBothCentralBarVsEloss[ilayer]);

    fpHisRawTofBothCentralBarVsElossVsSTRiseTime[ilayer] = new TH3F(Form("twRawTofBothCentralBarVsElossVsSTRiseTime_%s",LayerName[(TLayer)ilayer].data()), Form("twRawTofBothCentralBarVsElossVsSTRiseTime_%s",LayerName[(TLayer)ilayer].data()), 75,0.5,2.5,5000,-50., 50, 490,0,120.);
    AddHistogram(fpHisRawTofBothCentralBarVsElossVsSTRiseTime[ilayer]);

    fpHisBarsID[ilayer] = new TH1I(Form("twBarsID_%s",LayerName[(TLayer)ilayer].data()), Form("twBarsID_%s",LayerName[(TLayer)ilayer].data()),20, -0.5, 19.5);
    AddHistogram(fpHisBarsID[ilayer]);
  
    fpHisElossTof_layer[ilayer] = new TH2D(Form("twdE_vs_Tof_%s",LayerName[(TLayer)ilayer].data()),Form("dE_vs_Tof_%s",LayerName[(TLayer)ilayer].data()),3000,0.,30.,480,0.,120.);
    AddHistogram(fpHisElossTof_layer[ilayer]);

    fpHisAmpA[ilayer] = new TH1F(Form("twAmpA_%s",LayerName[(TLayer)ilayer].data()), "TW - AmpA ", 1000,0.,1.);
    AddHistogram(fpHisAmpA[ilayer]);

    fpHisAmpB[ilayer] = new TH1F(Form("twAmpB_%s",LayerName[(TLayer)ilayer].data()), "TW - AmpB ", 1000,0.,1.);
    AddHistogram(fpHisAmpB[ilayer]);
    
    fpHisAmpA_vs_Eloss[ilayer] = new TH2F(Form("twAmpADe_%s",LayerName[(TLayer)ilayer].data()), "TW - AmpA vs Energy Loss ", 480, 0., 120.,1000,0.,1.);
    AddHistogram(fpHisAmpA_vs_Eloss[ilayer]);
    
    fpHisAmpB_vs_Eloss[ilayer] = new TH2F(Form("twAmpBDe_%s",LayerName[(TLayer)ilayer].data()), "TW - AmpB vs Energy Loss", 480, 0., 120.,1000,0.,1.);
    AddHistogram(fpHisAmpB_vs_Eloss[ilayer]);    
    
  }

  fpHisDeltaTimeFrontRear = new TH2F("twDeltaTimeFrontRear","twDeltaTimeFrontRear", 2000, -5., 5.,480,0,120.);
  AddHistogram(fpHisDeltaTimeFrontRear);
  
  for(int iZ=1; iZ < fZbeam+1; iZ++) {
    
    fpHisElossTof_Z.push_back( new TH2D(Form("twdE_vs_Tof_Z%d",iZ),Form("dE_vs_Tof_%d",iZ),5000,0.,50.,480,0.,120.) );

    AddHistogram(fpHisElossTof_Z[iZ-1]);

    fpHisTof_Z.push_back( new TH1D(Form("twTof_Z%d",iZ),Form("hTof_%d",iZ),5000,0.,50.) );

    AddHistogram(fpHisTof_Z[iZ-1]);

    for(int ilayer=0; ilayer<(TWparam)nLayers; ilayer++) {
      
      fpHisEloss_Z[ilayer].push_back( new TH1D(Form("twEloss_Z%d_%s",iZ,LayerName[(TLayer)ilayer].data()),Form("hEloss_%d_%s",iZ,LayerName[(TLayer)ilayer].data()),200,0.,100.) );

      AddHistogram(fpHisEloss_Z[ilayer][iZ-1]);
    }

  }
  
  SetValidHistogram(kTRUE);
   
  return;
}
//------------------------------------------+-----------------------------------
//! Action.
Bool_t TATWactNtuHit::Action()
{
   TATWntuRaw*   p_datraw = (TATWntuRaw*) fpDatRaw->Object();
   TASTntuHit*   p_STnturaw;
   if(fpSTNtuRaw)
      p_STnturaw = (TASTntuHit*)  fpSTNtuRaw->Object();
   TATWntuHit*   p_nturaw = (TATWntuHit*)  fpNtuRaw->Object();
   
   
   
   //////////// Time Trigger info from ST ///////////////
   
   if(FootDebugLevel(1)) {
      cout<<""<<endl;
      Info("Action()","Evt N::%d",fEvtCnt);
      fEvtCnt++;
   }
   
   double STtrigTime(0);
   Int_t SThitN(0.);
   TASThit *stHit;
   Double_t time_st(0.);
   
   if(fpSTNtuRaw) {
      
      STtrigTime = p_STnturaw->GetTriggerTime();
      
      SThitN = p_STnturaw->GetHitsN();  // same of STtrigTime
      
      stHit = (TASThit*)p_STnturaw->GetHit(0);
      time_st = stHit->GetTime();
   }
   
   if(FootDebugLevel(1))
      cout<<"ST hitN::"<<SThitN<<" trigTime::"<<STtrigTime<<" time_ST::"<<time_st<<endl;
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

   Bool_t hitcenter[nLayers]={false,false};
   Double_t rawtof_bothcentral[nLayers]={-99999.,-99999.};
   Double_t energy_bothcentral[nLayers]={-99999.,-99999.};

   // loop over all the bars
   for (auto it=chmap->begin();it!=chmap->end();++it) {
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
	    Int_t ShoeBarId = (BarId)%nBarsPerLayer;

	    // get Charge, Amplitude (pedesta subtracted) and time on side a of the bar
	    Double_t ChargeA  = hita->GetCharge();
	    Double_t AmplitudeA  = hita->GetAmplitude();
	    Double_t TimeA    = hita->GetTime();

	    // get Charge, Amplitude (pedesta subtracted) and time on side b of the bar
	    Double_t ChargeB  = hitb->GetCharge();
	    Double_t AmplitudeB  = hitb->GetAmplitude();
	    Double_t TimeB    = hitb->GetTime();

	    // get raw energy (not calibrated)
	    Double_t rawEnergy = GetRawEnergy(hita,hitb);

	    // get raw tof in ns (not calibrated)
	    Double_t rawToF    = GetRawTime(hita,hitb) - STtrigTime;

            if(FootDebugLevel(1)) {
              if(ChargeA<0 || ChargeB<0 || TimeA<0 || TimeB<0 || AmplitudeA<0 || AmplitudeB<0)
                cout<<"rawEnergy::"<<rawEnergy<<" chA::"<<ChargeA<<"  chB::"<<ChargeB<<" AmpA::"<<AmplitudeA<<"  AmpB::"<<AmplitudeB<<" rawToF::"<<rawToF<<" timeA::"<<TimeA<<" timeB::"<<TimeB<<endl;
            }
                          
	    // get position along the bar from the time difference between the two edges
	    Double_t posAlongBar = GetPosition(hita,hitb,Layer,ShoeBarId);

	    // get the PosId (0-399) of the cross btw horizontal and vertical bars
	    Int_t PosId  = GetBarCrossId(Layer,ShoeBarId,posAlongBar);

	    Int_t TrigType = hita->GetTriggerType();
            
	    // get calibrated energy in MeV
	    Double_t Energy = GetEnergy(rawEnergy,Layer,PosId,BarId);

	    // get Tof calibrated in ns
	    Double_t ToF = GetToF(rawToF,Layer,PosId,BarId);
	    	    
	    
	    if(FootDebugLevel(1)) {
              cout<<LayerName[(TLayer)Layer].data()<<"  barId::"<<BarId<<"  shoeId::"<<ShoeBarId<<"  posId::"<<PosId<<"  pos::"<<posAlongBar<<endl;
              cout<<"Eloss::"<<Energy<<"Eraw::"<<rawEnergy<<" chA::"<<ChargeA<<"  chB::"<<ChargeB<<" AmpA::"<<AmplitudeA<<" AmpB::"<<AmplitudeB<<endl;
	      cout<<"ToF::"<<ToF<<" RawToF"<<rawToF<<" timeA::"<<TimeA<<" timeB::"<<TimeB<<endl;
	    }


	    fCurrentHit = (TATWhit*)p_nturaw->NewHit(Layer,ShoeBarId,Energy,rawToF,posAlongBar,ChargeA,ChargeB,AmplitudeA,AmplitudeB,TimeA,TimeB,TrigType);

	    Int_t Zrec = f_parcal->GetChargeZ(Energy,ToF,Layer);
	    fCurrentHit->SetChargeZ(Zrec);
	    // Set ToF
	    fCurrentHit->SetToF(ToF);
            // Set Valid Hit
	    if(ChargeA<0 || ChargeB<0 || TimeA<0 || TimeB<0 || AmplitudeA<0 || AmplitudeB<0)
	      fCurrentHit->SetValid(false);
	    
	    
	    if(ShoeBarId==CentralBarsID[1] && Energy>2) {  // only for central bar (9-29) studies  //threshold ad minchiam to consider zero suppression
	      rawtof_bothcentral[Layer] = rawToF; 
	      hitcenter[Layer] = true;
	      energy_bothcentral[Layer]= Energy;
	    }


	      
	    if (ValidHistogram() && fCurrentHit->IsValid()) {

              fpHisBarsID[Layer]->Fill(ShoeBarId);

	      fpHisCharge[Layer]->Fill(rawEnergy);
	      fpHisPos[Layer]->Fill(posAlongBar);
	      fpHisRawTof[Layer]->Fill(rawToF);

	      fpHisElossTof_layer[Layer]->Fill(ToF,Energy);
	      
	      if(ShoeBarId>=CentralBarsID[0] && ShoeBarId<=CentralBarsID[2]) {  // only for central bars for trigger purposes
		
                fpHisChargeCentralBar[Layer]->Fill(rawEnergy);
                fpHisRawTofCentralBar[Layer]->Fill(rawToF);

		fpHisAmpA[Layer]->Fill(AmplitudeA);
		fpHisAmpB[Layer]->Fill(AmplitudeB);
		fpHisAmpA_vs_Eloss[Layer]->Fill(Energy,AmplitudeA);
		fpHisAmpB_vs_Eloss[Layer]->Fill(Energy,AmplitudeB);
	      }


	      

	      if(Zrec>0 && Zrec<fZbeam+1) {
		fpHisEloss_Z[Layer][Zrec-1]->Fill(Energy);
		fpHisTof_Z[Zrec-1]->Fill(ToF);
		fpHisElossTof_Z[Zrec-1]->Fill(ToF,Energy);
	      }
	    }
	  }
      } 
    }
    

   if (ValidHistogram() && hitcenter[0] && hitcenter[1]) {
     for(int iL=0;iL<nLayers;iL++){
       fpHisRawTofBothCentralBarVsEloss[iL]->Fill(rawtof_bothcentral[iL],energy_bothcentral[iL]);
       fpHisRawTofBothCentralBarVsElossVsSTRiseTime[iL]->Fill(p_STnturaw->GetRiseTime(), rawtof_bothcentral[iL],energy_bothcentral[iL]);
     }
     fpHisDeltaTimeFrontRear->Fill(rawtof_bothcentral[0] - rawtof_bothcentral[1],energy_bothcentral[0]);
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
  if ( rawenergy<0 || posId<0 || posId>=nBarCross || (layerId!=LayerX && layerId!=LayerY) )
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

  if(FootDebugLevel(4)) {
    Info("GetEnergy()","posId::%d, barId::%d, layId::%d, rawEnergy::%.2f",posId,barId,layerId,rawenergy);
    Info("GetEnergy()","Parameters read from energy calibration map file: p0::%.2f, p1::%.2f",p0,p1);
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

Double_t  TATWactNtuHit::GetToF(Double_t rawToF, Int_t layerId, Int_t posId, Int_t barId)
{

  if ( posId<0 || posId>=nBarCross || (layerId!=LayerX && layerId!=LayerY) )
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
    cout<<"rawTime::"<<rawToF<<" deltaT::"<<deltaT<<" deltaT2::"<<deltaT2<<" TOF::"<<rawToF-deltaT<<endl;
  
  return rawToF - deltaT;

}
//________________________________________________________________

Double_t TATWactNtuHit::GetPosition(TATWrawHit *a, TATWrawHit *b, Int_t layer, Int_t bar)
{

  Double_t BarCspeed = f_parcal->GetBarLightSpeed(layer,bar);
  Double_t PosOffset = f_parcal->GetDeltaTimePosOffset(layer,bar);

  Double_t deltaTimePos = PosOffset + (b->GetTime() - a->GetTime())/2 * BarCspeed;  // local (TW) ref frame
  // A <----------- B

  if(FootDebugLevel(4))
    Info("GetPosition","timeA::%f, timeB::%f, BarCspeed::%f, PosOffset::%f, deltaTimePos::%f",a->GetTime(),b->GetTime(),BarCspeed,PosOffset,deltaTimePos);
  
  return deltaTimePos;
  
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

  } else {
    Error("GetBarCrossId()","Layer %d doesn't exist",layer);
    return -1;
  }

  barCrossId = barY + barX * nBarsPerLayer;  // left top corner is (0,0) --> pos=0, right bottom corner is (19,19) --> pos = 399

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
    
  } else if(layer==LayerY) {
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

  } else {
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
