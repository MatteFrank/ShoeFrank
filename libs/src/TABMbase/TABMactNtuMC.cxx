/*!
  \file
  \version $Id: TABMactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TABMactNtuMC.
*/

#include "TABMactNtuMC.hxx"

/*!
  \class TABMactNtuMC TABMactNtuMC.hxx "TABMactNtuMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/


ClassImp(TABMactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuMC::TABMactNtuMC(const char* name,
			   TAGdataDsc* dscnturaw, 
			   TAGparaDsc* dscbmcon, 
			   TAGparaDsc* dscbmgeo, 
			   EVENT_STRUCT* evStr)
  : TAGaction(name, "TABMactNtuMC - NTuplize ToF raw data"),
    fpNtuMC(dscnturaw),
    fpParCon(dscbmcon),
    fpParGeo(dscbmgeo),
    fpEvtStr(evStr)
{
 if (FootDebugLevel(1))
   cout<<"TABMactNtuMC::default constructor::Creating the Beam Monitor MC tuplizer action"<<endl;
 AddPara(fpParCon, "TABMparCon");
 AddPara(fpParGeo, "TABMparGeo");
 AddDataOut(fpNtuMC, "TABMntuRaw");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuMC::~TABMactNtuMC()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TABMactNtuMC::Action()
{ 
   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   TABMntuRaw* p_nturaw  = (TABMntuRaw*) fpNtuMC->Object();
   TABMparCon* p_bmcon  = (TABMparCon*) fpParCon->Object();
   TABMparGeo* p_bmgeo   = (TABMparGeo*) fpParGeo->Object();
  
  Int_t cell, view, lay, ipoint, tmp_int;
  Int_t tobecharged[fpEvtStr->BMNn]; //-1=discharged hit, otherwise tobecharged=number of the BMNcellid(0-35)
  Double_t rdriftxcell[fpEvtStr->BMNn];
  Int_t nhits=0, nrealhits=0;
   
  TVector3 loc, gmom, mom,  glo;
  p_nturaw->SetupClones();
   
  if (FootDebugLevel(1))
    cout<<"TABMactNtuMC::Processing "<<fpEvtStr->BMNn<<" hits"<<endl;
   
   fMap.clear();
  //loop for double hits and hits with energy less than enxcell_cut:
  for (Int_t i = 0; i < fpEvtStr->BMNn; ++i) {
    if(fpEvtStr->TRcha[fpEvtStr->BMNid[i]-1]!=0 && fpEvtStr->TRtrlen[fpEvtStr->BMNid[i]-1]>0.1){//selection criteria: no neutral particles, at least 0,1 mm
      cell = fpEvtStr->BMNicell[i];
      lay = fpEvtStr->BMNilay[i];
      view = fpEvtStr->BMNiview[i]==-1 ? 1:0;
      tobecharged[i]=p_bmgeo->GetBMNcell(lay, view, cell);
      ++nrealhits;

      glo.SetXYZ(fpEvtStr->BMNxin[i],fpEvtStr->BMNyin[i],fpEvtStr->BMNzin[i]);
      loc = geoTrafo->FromGlobalToBMLocal(glo);
      gmom.SetXYZ(fpEvtStr->BMNpxin[i],fpEvtStr->BMNpyin[i],fpEvtStr->BMNpzin[i]);
      if(gmom.Mag()!=0){
        rdriftxcell[i] = p_bmgeo->FindRdrift(loc, gmom, p_bmgeo->GetWirePos(view, lay,p_bmgeo->GetSenseId(cell)), p_bmgeo->GetWireDir(view),false);
        for(Int_t j=0;j<i;j++){//if there is a double hit in the same cell, it charges the hits if they have rdrift difference more than p_bmcon->GetRdriftCut() 
          if(tobecharged[i]==tobecharged[j] && ((rdriftxcell[i]-rdriftxcell[j]) < p_bmcon->GetRdriftCut()) ){
            tobecharged[ (rdriftxcell[i] >= rdriftxcell[j]) ? i : j  ]=-1;
            --nrealhits;
            break;
          }
        }
      }else{ //if a particle is born without energy, it shouldn't release energy for a hit
        tobecharged[i]=-1;
        --nrealhits;
      }
    }else
      tobecharged[i]=-1;
  }
    
  //set the number of hits
  Int_t hitsrandtot, remainhitsn;    
  gRandom->SetSeed(); 
    
  if(p_bmcon->GetSmearhits()) {     
    if(gRandom->Uniform(0,10)<p_bmcon->GetFakehitsMean())
      hitsrandtot = 12 - (Int_t) fabs(gRandom->Gaus(0, p_bmcon->GetFakehitsSigmaLeft()));
    else
      hitsrandtot = 12 + (Int_t) fabs(gRandom->Gaus(0, p_bmcon->GetFakehitsSigmaRight()));

    //prune the real hits
    Int_t nprunehits = nrealhits*(1.-gRandom->Gaus(p_bmcon->GetMCEffMean(), p_bmcon->GetMCEffSigma()))+0.5;
    if(nprunehits<0)
      nprunehits=0;
    if(nprunehits > nrealhits)
      nprunehits = nrealhits;
    if((nrealhits-nprunehits) > hitsrandtot)
      nprunehits = nrealhits-hitsrandtot;
    remainhitsn=nrealhits-nprunehits;
    while(nprunehits>0){
      for(Int_t k=gRandom->Uniform(0,fpEvtStr->BMNn);k<fpEvtStr->BMNn;k++){
        if(tobecharged[k]!=-1){
          tobecharged[k]=-1;
          nprunehits--;
          break;
        }
      }
    }
    
    //add fake hits
    if(hitsrandtot-remainhitsn > 0)
      CreateFakeHits(hitsrandtot-remainhitsn, nhits);
     
  } // smear number of hit
   
  Double_t realrdrift, time;
  Int_t nLayers = p_bmgeo->GetLayersN();
   
  //charge the hits:
  for (Int_t i = 0; i < fpEvtStr->BMNn; i++) {
    if(tobecharged[i]!=-1) {
      ipoint=fpEvtStr->BMNid[i]-1;
      cell = fpEvtStr->BMNicell[i];
      lay = fpEvtStr->BMNilay[i];
      view = fpEvtStr->BMNiview[i]==-1 ? 1:0;
      realrdrift = rdriftxcell[i];
      
      if(p_bmcon->GetSmearrdrift()>0) //smear the hit distance according to the resolution
        SmearRdrift(p_bmcon->GetSmearrdrift(), rdriftxcell[i], p_bmcon->ResoEval(rdriftxcell[i]));   
      time=p_bmcon->InverseStrel(rdriftxcell[i]);  
      if(FootDebugLevel(4))
        cout<<"TABMactNtuMC::Action::In the charging hits loop: I'm going to charge hit number:"<<i<<"/"<<fpEvtStr->BMNn<<"  tobecharged="<<tobecharged[i]<<"  view="<<view<<"  lay="<<lay<<"  cell="<<cell<<"  rdriftxcell[i]="<<rdriftxcell[i]<<"  realrdrift="<<realrdrift<<"  time="<<time<<"  resolution="<<p_bmcon->ResoEval(rdriftxcell[i])<<endl;
      
      //create hit
       TABMntuHit *mytmp = 0x0;
       pair<int, int> p(nLayers*view+lay, cell);

       if (fMap[p] == 0) {
          mytmp = p_nturaw->NewHit(view, lay, cell,p_bmgeo->GetBMNcell(lay,view,cell), rdriftxcell[i], time, p_bmcon->ResoEval(rdriftxcell[i]));
          fMap[p] = mytmp;
       } else
          mytmp = fMap[p];
       
      mytmp->AddMcTrackIdx(ipoint, i);

      mytmp->SetRealRdrift(realrdrift);
      mytmp->SetIsFake( (fpEvtStr->TRpaid[fpEvtStr->BMNid[i]-1]==0) ? 0 : 1);
      nhits++;
      if (ValidHistogram()){
        fpHisCell->Fill(cell);
        fpHisView->Fill(view);
        fpHisPlane->Fill(lay);
        fpHisRdrift->Fill(rdriftxcell[i]);
        fpHisFakeIndex->Fill(mytmp->GetIsFake());
        fpHisSmearDiff->Fill(realrdrift-rdriftxcell[i]);
      }
    }//end of charging
  }
  
  if(hitsrandtot!= nhits && p_bmcon->GetSmearhits())
    cout<<"TABMactNtuMC::ERROR!!!!!!!!  nhits="<<nhits<<"  hitsrandtot="<<hitsrandtot<<"  remainhitsn"<<remainhitsn<<"  nrealhits="<<nrealhits<<endl;
  
  if (ValidHistogram())
    fpHisHitNum->Fill(nhits);
  fpNtuMC->SetBit(kValid);
  if(FootDebugLevel(2))
    cout<<"TABMactNtuMC::Action():: done without problems!"<<endl;
  
  return kTRUE;
}


void TABMactNtuMC::CreateHistogram(){
  DeleteHistogram();
 
  fpHisCell = new TH1I( "BM_hit_cell", "cell index; index; Counter", 3, 0., 3.);
  AddHistogram(fpHisCell);   
  fpHisView = new TH1I( "BM_hit_view", "view index; index; Counter", 2, 0., 2.);
  AddHistogram(fpHisView);   
  fpHisPlane = new TH1I( "BM_hit_plane", "plane index; index; Counter", 6, 0., 6.);
  AddHistogram(fpHisPlane);   
  fpHisRdrift = new TH1F( "BM_hit_rdrift", "Rdrift; Rdrift [cm]; numevent", 100, 0., 1.);
  AddHistogram(fpHisRdrift);   
  fpHisSmearDiff = new TH1F( "BM_hit_smear", "Real drift distance - smeared drift distance; diff [cm]; numevent", 400, -0.2, 0.2);
  AddHistogram(fpHisSmearDiff);   
  fpHisHitNum=new TH1I( "BM_hit_distribution", "Number of ACCEPTED hits x event; Number of hits; Events", 30, 0, 30);
  AddHistogram(fpHisHitNum);
  fpHisFakeIndex=new TH1I( "BM_hit_fake", "Charged hits fake index; 0=Hit from primaries, 1=Other fluka hits, 2=Random hit not from fluka; Events", 4, 0, 4);
  AddHistogram(fpHisFakeIndex);
   
  SetValidHistogram(kTRUE);
}


void TABMactNtuMC::CreateFakeHits(Int_t nfake, Int_t &nhits)
{
  TABMparCon* p_bmcon = (TABMparCon*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();
  TABMntuRaw* p_nturaw  = (TABMntuRaw*) fpNtuMC->Object();

  Int_t plane, view, cell;
  if(FootDebugLevel(2))
    cout<<"TABMactNtuMC::CreateFakeHits:: I'm going to create "<<nfake<<" number of fake hits"<<endl;
  for(Int_t i=0;i<nfake;i++){
    
    //set the fake view, plane, cell, rdrift values 
    do{
       plane=gRandom->Uniform(0,6);
    }while(plane<0 || plane>5);
    view=(gRandom->Uniform(0,2)>1) ? 1: 0;
    do{
       cell=gRandom->Uniform(0,3);
    }while(cell<0 || cell>2);
    Double_t rdrift=gRandom->Uniform(0.,0.8);

    //charge the fake hits
    if(FootDebugLevel(3))
      cout<<"TABMactNtuMC::CreateFakeHits::I'm going to charge a fake hit view="<<view<<"  plane="<<plane<<"  cell="<<cell<<"  rdrift="<<rdrift<<"  time="<<p_bmcon->InverseStrel(rdrift)<<endl;
    TABMntuHit *mytmp = p_nturaw->NewHit(view, plane, cell, p_bmgeo->GetBMNcell(plane,view,cell), rdrift, p_bmcon->InverseStrel(rdrift), -1.);
    mytmp->SetSigma(p_bmcon->ResoEval(rdrift));
    mytmp->SetRealRdrift(rdrift);  
    mytmp->SetIsFake(2);
    nhits++;  
    if (ValidHistogram()){
      fpHisCell->Fill(cell);
      fpHisView->Fill(view);
      fpHisPlane->Fill(plane);
      fpHisRdrift->Fill(rdrift);
      fpHisFakeIndex->Fill(2);
    }
  }
  
  if(FootDebugLevel(2))
    cout<<"TABMactNtuMC::CreateFakeHits::done"<<endl;
  
  return;
}


void TABMactNtuMC::SmearRdrift(Int_t smear_type, Double_t &tobesmeared, Double_t sigma){
   Double_t smeared;
   
   if(smear_type==0)
      return;
   
   if(smear_type==1){ //gaussian truncated to 1 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>sigma || smeared>0.944  || smeared<0);
   }
   
   if(smear_type==2){ //gaussian truncated to 2 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>2.*sigma || smeared>0.944  || smeared<0);
   }
   
   if(smear_type==3){ //gaussian truncated to 3 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>3.*sigma || smeared>0.944  || smeared<0);
   }
   
   if(smear_type==4) //gaussian not truncated
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(smeared>0.944 || smeared<0);
   
   
   if(smear_type==5) //flat smearing
      smeared=tobesmeared+gRandom->Uniform(-sigma*sqrt(12.)/2.,sigma*sqrt(12.)/2.);
   
   
   if (smeared<0)
      smeared=0.;
   tobesmeared=smeared;
   return;  
} 


