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

#include "TRandom3.h"

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
  Info("Action()"," Creating the Beam Monitor MC tuplizer action\n");
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
  vector<Int_t> hitxcell(fpEvtStr->BMNn, 99); 
  vector<bool> tobecharged(fpEvtStr->BMNn, true);
  vector<Double_t> rdriftxcell(fpEvtStr->BMNn, 99.);
  Int_t nhits=0;
   
  TVector3 loc, gmom, mom, A0, Wvers, glo;;
  p_nturaw->SetupClones();//se non c'è l'array di h, lo crea
   
   if (fDebugLevel > 0)
      Info("Action()","Processing n :: %2d hits \n",fpEvtStr->BMNn);
   
   if(p_bmcon->GetCalibro()!=0){
      if(p_bmcon->GetCalibro()==1) {
         p_bmgeo->SetWireAlignment();
      }
   }
  //loop for double hits and hits with energy less than enxcell_cut:
  for (Int_t i = 0; i < fpEvtStr->BMNn; i++) {
     
    if(fpEvtStr->BMNde[i] < p_bmcon->GetEnxcellcut() && fpEvtStr->TRpaid[fpEvtStr->BMNid[i]-1]!=0)
      tobecharged[i]=false;
     
    if(tobecharged[i]){
      cell = fpEvtStr->BMNicell[i];
      lay = fpEvtStr->BMNilay[i];
      view = fpEvtStr->BMNiview[i]==-1 ? 1:0;
//       printf("cell %d layer %d view %d\n", cell, lay, view);

      hitxcell[i]=p_bmgeo->GetBMNcell(lay, view, cell);
      glo.SetXYZ(fpEvtStr->BMNxin[i],fpEvtStr->BMNyin[i],fpEvtStr->BMNzin[i]);
       
      loc = geoTrafo->FromGlobalToBMLocal(glo);
      gmom.SetXYZ(fpEvtStr->BMNpxin[i],fpEvtStr->BMNpyin[i],fpEvtStr->BMNpzin[i]);
       
      A0.SetXYZ(p_bmgeo->GetWireX(p_bmgeo->GetSenseId(cell),lay,view),    //sarebbe più elegante mettere questa roba in FindRdrift,
                p_bmgeo->GetWireY(p_bmgeo->GetSenseId(cell),lay,view),    //ma in FindRdrift dovrei caricare p_bmgeo, e forse non conviene
                p_bmgeo->GetWireZ(p_bmgeo->GetSenseId(cell),lay,view));
       
      Wvers.SetXYZ(p_bmgeo->GetWireCX(p_bmgeo->GetSenseId(cell),lay,view),
                   p_bmgeo->GetWireCY(p_bmgeo->GetSenseId(cell),lay,view),
                   p_bmgeo->GetWireCZ(p_bmgeo->GetSenseId(cell),lay,view));
       
      Wvers.SetMag(1.);
      rdriftxcell.at(i) = FindRdrift(loc, gmom, A0, Wvers);
      
      if(rdriftxcell.at(i)==99) //FindRdrift return 99 if a particle is born without energy, so it shouldn't release energy for a hit.
        tobecharged[i]=false;
       
      //if there is a double hit in the same cell, it charges the hits if they have rdrift difference more than p_bmcon->GetRdriftCut()
      for(Int_t j=0;j<i;j++){ 
        if((rdriftxcell.at(i)-rdriftxcell.at(j)) < p_bmcon->GetRdriftCut() && rdriftxcell.at(i) >= rdriftxcell.at(j) && hitxcell[i]==hitxcell[j])
          tobecharged[i]=false;
         
        if((rdriftxcell.at(j)-rdriftxcell.at(i)) < p_bmcon->GetRdriftCut() && rdriftxcell.at(j) > rdriftxcell.at(i) && hitxcell[i]==hitxcell[j])
          tobecharged[j]=false;
        }
      }
    }
    
   if(p_bmcon->GetCalibro()!=0){
      if(p_bmcon->GetCalibro()==1) {
         p_bmgeo->SetWireAlignment(false);
      }
   }
          
  //set the number of hits
  Int_t hitsrandtot;    
  gRandom->SetSeed(0);
  Int_t remainhitsn, nrealhits;
  
  if(p_bmcon->GetSmearhits()){
    nrealhits=0;
    for(Int_t i=0;i<tobecharged.size();i++)
      if(tobecharged[i])
        nrealhits++;
     
    //prune the real hits
    Int_t nprunehits = nrealhits*(1.-gRandom->Gaus(p_bmcon->GetMCEffMean(), p_bmcon->GetMCEffSigma()))+0.5;
    if(nprunehits<0)
      nprunehits=0;

    //provv
     Int_t tmp_int=gRandom->Uniform(0,10);//check if this number is ok
     if(tmp_int<p_bmcon->GetFakehitsMean())
        hitsrandtot = 12 - (Int_t) fabs(gRandom->Gaus(0, p_bmcon->GetFakehitsSigmaLeft()));
     else
        hitsrandtot = 12 + (Int_t) fabs(gRandom->Gaus(0, p_bmcon->GetFakehitsSigmaRight()));

    if(nprunehits > nrealhits)
      nprunehits = nrealhits;
    else if((nrealhits-nprunehits) > hitsrandtot)
      nprunehits = nrealhits-hitsrandtot;
     
    remainhitsn=nrealhits-nprunehits;
     
    while(nprunehits>0){
      tmp_int=gRandom->Uniform(0,nrealhits);
      if(tmp_int<nrealhits)  
        if(tobecharged[tmp_int]==true){
          tobecharged[tmp_int]=false;
          nprunehits--;
        }
    }
    
    //add fake hits
    if(hitsrandtot-remainhitsn > 0)
      CreateFakeHits(hitsrandtot-remainhitsn, nhits);
     
  } // smear hit
   
  Double_t realrdrift;
   
  //charge the hits:
  for (Int_t i = 0; i < fpEvtStr->BMNn; i++) {
          
     
    if(tobecharged[i]) {
      ipoint=fpEvtStr->BMNid[i]-1;
      cell = fpEvtStr->BMNicell[i];
      lay = fpEvtStr->BMNilay[i];
      view = fpEvtStr->BMNiview[i]==-1 ? 1:0;
       
      //shift the t0 and change the strelations:
      realrdrift = rdriftxcell.at(i);
       
    if(p_bmcon->GetBMdebug()>=3)
      cout<<"In the charging hits loop: I'm going to charge hit number:"<<i<<"/"<<fpEvtStr->BMNn<<"  tobecharged="<<tobecharged[i]<<"  view="<<view<<"  lay="<<lay<<"  cell="<<cell<<"  rdriftxcell.at(i)="<<rdriftxcell.at(i)<<"  time="<<p_bmcon->InverseStrel(rdriftxcell.at(i))<<endl;
      
      //create hit
      TABMntuHit *mytmp = p_nturaw->NewHit(fpEvtStr->BMNid[i],	view, lay, cell, rdriftxcell.at(i), p_bmcon->InverseStrel(rdriftxcell.at(i)), 0.);
       mytmp->AddMcTrackId(ipoint, i);
       
      if(p_bmcon->ResoEval(rdriftxcell.at(i))>0)
        mytmp->SetSigma(p_bmcon->ResoEval(rdriftxcell.at(i)));
      else{  
        cout<<"WARNING: error from config resoEval! sigma on rdrift is zero!!! going to set error=0.015; rdrift="<<rdriftxcell.at(i)<<endl;
        mytmp->SetSigma(p_bmcon->GetRdrift_err());
      }
       
      mytmp->SetRealRdrift(realrdrift);
       
      if(p_bmcon->GetSmearrdrift()>0)
        mytmp->SmearRdrift(p_bmcon->GetSmearrdrift());   //smearing
       
      if(fpEvtStr->TRpaid[fpEvtStr->BMNid[i]-1]!=0)
        mytmp->SetIsFake(1);
      else
        mytmp->SetIsFake(0);
       
      nhits++;
    }//end of tobecharded if
  }
  
  if(hitsrandtot!= nhits && p_bmcon->GetSmearhits())
    cout<<"TABMactNtuMC::ERROR!!!!!!!!  nhits="<<nhits<<"  hitsrandtot="<<hitsrandtot<<"  remainhitsn"<<remainhitsn<<"  nrealhits"<<nrealhits<<endl;
  
  fpNtuMC->SetBit(kValid);
  return kTRUE;
}


void TABMactNtuMC::CreateHistogram(){
   
   DeleteHistogram();
   

   SetValidHistogram(kTRUE);
}


void TABMactNtuMC::CreateFakeHits(Int_t nfake, Int_t &nhits)
{
  TABMparCon* p_bmcon = (TABMparCon*) fpParCon->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();
  TABMntuRaw* p_nturaw  = (TABMntuRaw*) fpNtuMC->Object();

  Int_t plane, view, cell;
  for(Int_t i=0;i<nfake;i++){
     
    do{
       plane=gRandom->Uniform(0,6);
    }while(plane<0 || plane>5);
     
     
    view=(gRandom->Uniform(0,2)>1) ? 1: -1;
    do{
       cell=gRandom->Uniform(0,3);
    }while(cell<0 || cell>2);
     
    Double_t rdrift=gRandom->Uniform(0.,0.9);

    //charge the fake hits
    TABMntuHit *mytmp = p_nturaw->NewHit(
                    -100,	view, plane, cell,        
                    rdrift, p_bmcon->InverseStrel(rdrift), -1.);     //tdrift has no meaning for MC (now)

    if(p_bmcon->ResoEval(rdrift)>0)
      mytmp->SetSigma(p_bmcon->ResoEval(rdrift));
    else
      mytmp->SetSigma(p_bmcon->GetRdrift_err());
    mytmp->SetRealRdrift(rdrift);  
    mytmp->SetIsFake(2);
    nhits++;  
  }
  return;
}


Double_t TABMactNtuMC::FindRdrift(TVector3 pos, TVector3 dir, TVector3 A0, TVector3 Wvers) {

  Double_t tp = 0., tf= 0., rdrift; 
  TVector3 D0, R0, Pvers;

  //~ if (dir.Mag()!=0.)
    //~ dir.SetMag(1.);
  //~ else{
  if(dir.Mag()==0.){
    cout<<"WARNING: FindRdrift: momentum is 0 and the hit shouldn't be charged because this hit is from a fragmentated particle with zero momentum"<<endl;
    return 99;//fake value
    }
    
  R0.SetXYZ(pos.X(),pos.Y(),pos.Z());//set position
  Pvers=dir;
  Pvers.SetMag(1.);
  
  D0 = R0 - A0;//distance between position of reference point of current wire and current particle position

  Double_t prosca = Pvers*Wvers ;//scalar product of directions
  Double_t D0W = D0*Wvers;//distance projected on wire
  Double_t D0P = D0*Pvers;//distance projected on particle direction

  if(prosca!= 1.) {//if the don't fly parallel
    tp = (D0W*prosca - D0P)/(1.-prosca*prosca);
    tf = (-D0P*prosca + D0W)/(1.-prosca*prosca);
    rdrift = sqrt( abs(D0.Mag2() + tp*tp + tf*tf + 2.*tp*D0P -2.*tf*D0W -2.*prosca*tf*tp ));
    } 
  else  //if they go parallel
    rdrift = sqrt(abs( D0.Mag2() - D0W*D0W)); 

  if(rdrift<0){
    cout<<"WARNING!!!!! SOMETHING IS WRONG, YOU HAVE A NEGATIVE RDRIFT!!!!!!!!!  look at TABMactNtuMC::FindRdrift   rdrift="<<rdrift<<endl;
    rdrift=0;
    cout<<"rdrift="<<rdrift<<endl;
    cout<<"pos=("<<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<")  dir=("<<dir.X()<<","<<dir.Y()<<","<<dir.Z()<<")"<<endl;
    cout<<"A0=("<<A0.X()<<","<<A0.Y()<<","<<A0.Z()<<")  Wvers=("<<Wvers.X()<<","<<Wvers.Y()<<","<<Wvers.Z()<<")"<<endl;
    }
    
    //~ cout<<"rdrift="<<rdrift<<endl;
    //~ cout<<"pos=("<<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<")  dir=("<<dir.X()<<","<<dir.Y()<<","<<dir.Z()<<")"<<endl;
    //~ cout<<"A0=("<<A0.X()<<","<<A0.Y()<<","<<A0.Z()<<")  Wvers=("<<Wvers.X()<<","<<Wvers.Y()<<","<<Wvers.Z()<<")"<<endl;  
    
  return rdrift;
}


