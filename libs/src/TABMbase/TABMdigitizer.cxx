//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#include "TABMdigitizer.hxx"

// --------------------------------------------------------------------------------------
TABMdigitizer::TABMdigitizer(TABMntuRaw* pNtuRaw, TABMparCon* parCon)
 : TAGobject(),
   fpNtuRaw(pNtuRaw),
   fpParCon(parCon),
   fTimeMinDiff(50)
{
  fpEffDist=new TF1("EffVsRdrift",this,&TABMdigitizer::EffFunc, 0., 0.78,3,"TABMdigitizer","EffFunc");
  fpEffDist->SetParameters(1.00072,-0.000162505,10.9375);
  fpParCon->ResetStrelFunc();
}

// --------------------------------------------------------------------------------------
TABMdigitizer::~TABMdigitizer()
{
  delete fpEffDist;
}

//___________________________________________________________________________________________
Double_t TABMdigitizer::EffFunc(Double_t* x, Double_t* par){
  Double_t xx=x[0];
  return (xx>0.77) ? 0.2 : par[0]+par[1]*exp(xx*par[2]);
}

//___________________________________________________________________________________________
// Bool_t TABMdigitizer::AddHitInMap(Int_t cellid, Double_t inrdrift, Int_t inhitindex,Int_t inipoint)
Bool_t TABMdigitizer::Process(Double_t rdrift, Int_t cellid, Int_t plane, Int_t view, Int_t cell, Int_t inhitindex,Int_t inipoint)
{
  if(fpParCon->GetSmearHits())
    if(gRandom->Uniform(0,1)>fpEffDist->Eval(rdrift))
      return false;

  TABMntuHit *mytmp = 0x0;
  if(fpParCon->GetSmearRDrift()>0)
    rdrift=SmearRdrift(rdrift,fpParCon->ResoEval(rdrift));

  std::multimap<Int_t, TABMntuHit*>::iterator pos=fMap.find(cellid);
  if(pos==fMap.end()){//new hit in a new cellid
    mytmp=fpNtuRaw->NewHit(cellid, plane, view, cell, rdrift, fpParCon->GetTimeFromRDrift(rdrift), fpParCon->ResoEval(rdrift));
    mytmp->SetIsFake((inipoint==0) ? 0 : (inipoint>0 ? 1 : 2));
    mytmp->AddMcTrackIdx(inipoint, inhitindex);
    fMap.insert(std::pair<Int_t, TABMntuHit*>(cellid,mytmp));
    return true;
  }else{//multihit in the same cellid
    Int_t currhittime=fpParCon->GetTimeFromRDrift(rdrift);
    std::pair <std::multimap<Int_t, TABMntuHit*>::iterator, std::multimap<Int_t, TABMntuHit*>::iterator> rangeite;
    rangeite=fMap.equal_range(pos->first);
    for(auto d=rangeite.first;d!=rangeite.second;){//loop on all the hit with the same cellid
      Double_t distance=d->second->GetTdrift()-currhittime;
      if(fabs(distance)<fTimeMinDiff){
        if( distance>0){//the previously saved hit have to be substituted by the present hit
          d->second->Clear();
          d->second->SetIdCell(cellid);
          d->second->SetCell(cell);
          d->second->SetView(view);
          d->second->SetPlane(plane);
          d->second->SetRdrift(rdrift);
          d->second->SetTdrift(fpParCon->GetTimeFromRDrift(rdrift));
          d->second->SetIsFake((inipoint==0) ? 0 : (inipoint>0 ? 1 : 2));
          d->second->SetSigma(fpParCon->ResoEval(rdrift));
          d->second->AddMcTrackIdx(inipoint, inhitindex);
          return false;
        }else
          return false;// the present hit should not be charged
      }else
        ++d;
    }
    mytmp=fpNtuRaw->NewHit(cellid, plane,view, cell, rdrift, fpParCon->GetTimeFromRDrift(rdrift), fpParCon->ResoEval(rdrift));
    mytmp->SetIsFake((inipoint==0) ? 0 : (inipoint>0 ? 1 : 2));
    mytmp->AddMcTrackIdx(inipoint, inhitindex);
    fMap.insert(std::pair<Int_t, TABMntuHit*>(cellid,mytmp));
    return true;
  }

return false;
}


//___________________________________________________________________________________________
Double_t TABMdigitizer::SmearRdrift(const Double_t tobesmeared, const Double_t sigma){
   Double_t smeared;

   if(fpParCon->GetSmearRDrift()==1){ //gaussian truncated to 1 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>sigma || smeared>0.944  || smeared<0);
   }

   if(fpParCon->GetSmearRDrift()==2){ //gaussian truncated to 2 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>2.*sigma || smeared>0.944  || smeared<0);
   }

   if(fpParCon->GetSmearRDrift()==3){ //gaussian truncated to 3 sigma
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(fabs(smeared-tobesmeared)>3.*sigma || smeared>0.944  || smeared<0);
   }

   if(fpParCon->GetSmearRDrift()==4) //gaussian not truncated
      do{smeared=gRandom->Gaus(tobesmeared,sigma);}while(smeared>0.944 || smeared<0);


   if(fpParCon->GetSmearRDrift()==5) //flat smearing
      smeared=tobesmeared+gRandom->Uniform(-sigma*sqrt(12.)/2.,sigma*sqrt(12.)/2.);

   if(smeared<0)
     return 0;
   if(smeared>0.94)
     return 0.94;
   return  smeared;
}
