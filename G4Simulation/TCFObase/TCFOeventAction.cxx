
#include "TCFOeventAction.hxx"

//include hit class
//Endo of event:
//collection hits: all hits of this collections contains the information about what happened in the sensitive volume (epitaxial layer)
//each hit represents a step and in the epi layer there may have been many
//than the position to send to CmosDigitizer is the PosIn of the first hits and the pos out of the last hits
//the energy deposited is the sum of the energy deposited in each step
//All positon in microns and all energy in eV


#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"

#include "TCFOgeometryConstructor.hxx"

#include "TCSTgeometryConstructor.hxx"
#include "TCBMgeometryConstructor.hxx"
#include "TCVTgeometryConstructor.hxx"
#include "TCITgeometryConstructor.hxx"
#include "TCMSDgeometryConstructor.hxx"
#include "TCTWgeometryConstructor.hxx"
#include "TCCAgeometryConstructor.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"

#include "TAMCevent.hxx"
#include "TCGmcHit.hxx"
#include "TAMCntuEve.hxx"

#include "TCFOrunAction.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TFile.h"

//---------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOeventAction::TCFOeventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* footGeomConstructor)
: TCFObaseEventAction(runAction,footGeomConstructor)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOeventAction::~TCFOeventAction()
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOeventAction::EndOfEventAction(const G4Event* evt)
{
    // fill track
    FillTrack();

    // digitize evt
    Collect(evt);

    //At the end of each EVENT
    fMapTrackIdx.clear();
    FillAndClear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOeventAction::Collect(const G4Event* evt)
{
   
   if (fIrCollId >= 0)
      GetHitPerPlane(evt, fIrCollId);
   
    if (fBmCollId >= 0)
      GetHitPerPlane(evt, fBmCollId);
   
    if (fVtxCollId >= 0)
      GetHitPerPlane(evt, fVtxCollId);

   if (fItCollId >= 0)
      GetHitPerPlane(evt, fItCollId);
   
   if (fMsdCollId >= 0)
      GetHitPerPlane(evt, fMsdCollId);
   
   if (fTwCollId >= 0)
      GetHitPerPlane(evt, fTwCollId);
   
   if (fCaCollId >= 0)
      GetHitPerPlane(evt, fCaCollId);
   
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOeventAction::GetHitPerPlane(const G4Event* evt, G4int idColl)
{
   G4HCofThisEvent*  hitCollEv = evt->GetHCofThisEvent();
   TCGmcCollections* hitList   = (TCGmcCollections*)(hitCollEv->GetHC(idColl));
   TAMCevent*            hit   = fRunAction->GetEventMC();
   
   Int_t entries =  hitList->entries();
    fDetName = hitList->GetName();
    
   if (fDebugLevel)
      printf("IdColl %d entries %d\n", idColl, entries);

    Double_t edep = 0.;
    G4ThreeVector vou(0.,0.,0.);
    G4ThreeVector pou(0.,0.,0.);
    G4ThreeVector vin(0.,0.,0.);
    G4ThreeVector pin(0.,0.,0.);
    Int_t trackId1 = -100 ;
    Int_t trackId2 = -200 ;
    Int_t sensorId1 = -100 ;
    Int_t sensorId2 = -200 ;

    if(fDebugLevel) printf("%s \n",fDetName.Data());

    /// If only 1 hit in the detector (e.g. in SC)
    if(entries==1){
        TCGmcHit* mcHit = (*hitList)[0];
        FillHits(hit, mcHit);
    }
    /// If several hits in detector
    for (Int_t i = 1; i < entries; ++i) {
        TCGmcHit* mcHit1 = (*hitList)[i-1];
        TCGmcHit* mcHit2 = (*hitList)[i];
        trackId1 = mcHit1->GetTrackId();
        trackId2 = mcHit2->GetTrackId();
        sensorId1 = mcHit1->GetSensorId();
        sensorId2 = mcHit2->GetSensorId();
        if(i==1){
            edep = mcHit1->GetEdep();
            vin = mcHit1->GetPosIn();
            pin = mcHit1->GetMomIn();
        }
        if(trackId1 == trackId2 && sensorId1==sensorId2) edep = edep + mcHit2->GetEdep();
        else{
            vou = mcHit1->GetPosOut();
            pou = mcHit1->GetMomOut();
            mcHit1->SetEdep(edep);
            mcHit1->SetPosOut(vou);
            mcHit1->SetMomOut(pou);
            mcHit1->SetPosIn(vin);
            mcHit1->SetMomIn(pin);
            FillHits(hit, mcHit1);

            if(fDebugLevel) printf("[%d,%d] \t %.3e MeV \t posInit(%.3e,%.3e,%.3e) \t posOut(%.3e,%.3e,%.3e)\n",trackId1,sensorId1,edep,vin.getX(),vin.getY(),vin.getZ(),vou.getX(),vou.getY(),vou.getZ());
            edep = mcHit2->GetEdep();
            vin = mcHit2->GetPosIn();
            pin = mcHit2->GetMomIn();
        }
        /// For the last hit
        if(i==entries-1){
            vou = mcHit2->GetPosOut();
            pou = mcHit2->GetMomOut();
            mcHit2->SetEdep(edep);
            mcHit2->SetPosOut(vou);
            mcHit2->SetMomOut(pou);
            mcHit2->SetPosIn(vin);
            mcHit2->SetMomIn(pin);
            FillHits(hit, mcHit2);
            if(fDebugLevel) printf("[%d,%d] \t %.3e MeV \t posInit(%.3e,%.3e,%.3e) \t posOut(%.3e,%.3e,%.3e)\n",trackId2,sensorId2,edep,vin.getX(),vin.getY(),vin.getZ(),vou.getX(),vou.getY(),vou.getZ());
        }
    }

}

////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOeventAction::FillTrack()
{
    TAMCevent* hit = fRunAction->GetEventMC();

    Int_t nTracks = fMcTrack->GetTracksN();

    TVector3 initpos ;
    TVector3 initmom ;
    TVector3 finalpos ;
    TVector3 finalmom ;
    Int_t flukaID ;
    Int_t trackID ;
    Int_t parentID ;
    Int_t nbaryon ;
    Int_t charge ;
    Int_t dead ;
    Int_t region ;
    Double_t mass ;
    Double_t tof ;
    Double_t time ;
    Double_t length ;

    for(Int_t i=0 ; i<nTracks ; ++i){
        flukaID = fMcTrack->GetTrack(i)->GetFlukaID();
        trackID = fMcTrack->GetTrack(i)->GetType();
        parentID = fMcTrack->GetTrack(i)->GetMotherID();
        mass = fMcTrack->GetTrack(i)->GetMass();
        charge = fMcTrack->GetTrack(i)->GetCharge();
        nbaryon = fMcTrack->GetTrack(i)->GetBaryon();
        tof = fMcTrack->GetTrack(i)->GetTof();
        time = fMcTrack->GetTrack(i)->GetTime();
        length = fMcTrack->GetTrack(i)->GetTrkLength();
        initpos = fMcTrack->GetTrack(i)->GetInitPos();
        initmom = fMcTrack->GetTrack(i)->GetInitP();
        finalpos = fMcTrack->GetTrack(i)->GetFinalPos();
        finalmom = fMcTrack->GetTrack(i)->GetFinalP();
        dead = fMcTrack->GetTrack(i)->GetDead() ;
        region = fMcTrack->GetTrack(i)->GetRegion() ;
        fMapTrackIdx[trackID] = i;
        hit->AddPart(parentID,trackID,charge,region,nbaryon,dead,flukaID,
                                    initpos,finalpos,initmom,finalmom,mass,time,tof,length);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOeventAction::FillHits(TAMCevent* hit, TCGmcHit* mcHit)
{
    G4ThreeVector vin = mcHit->GetPosIn()*TAGgeoTrafo::MmToCm();
    G4ThreeVector vou = mcHit->GetPosOut()*TAGgeoTrafo::MmToCm();
    G4ThreeVector pin = mcHit->GetMomIn()*TAGgeoTrafo::MmToCm();
    G4ThreeVector pou = mcHit->GetMomOut()*TAGgeoTrafo::MmToCm();

    Int_t    sensorId = mcHit->GetSensorId();
    Int_t    trackId  = mcHit->GetTrackId();
    Double_t edep     = mcHit->GetEdep()*TAGgeoTrafo::MevToGev();
    Double_t time     = mcHit->GetGlobalTime()*TAGgeoTrafo::NsToSec();
//    Double_t time     = mcHit->GetLocalTime()*TAGgeoTrafo::NsToSec();
    Double_t al       = 0;
    Int_t    trackIdx = fMapTrackIdx[trackId];
   
    hit->SetEvent(fEventNumber);

    if (fIrCollId >= 0 && fDetName==TCSTgeometryConstructor::GetSDname()){
        hit->AddSTC(trackId, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackIdx);
    }

    if (fBmCollId >= 0  && fDetName==TCBMgeometryConstructor::GetSDname()) {
        Int_t layer ;
        Int_t view = -2;
        TVector3 pos(vin[0],vin[1],vin[2]);
        layer = (int)sensorId/2;
        view = -sensorId%2;
        Int_t cell = fFootGeomConstructor->GetParGeoBm()->GetCell(pos,sensorId,view);
        hit->AddBMN(trackIdx,layer,TMath::Abs(view), cell, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fVtxCollId >= 0 && fDetName==TCVTgeometryConstructor::GetSDname()) {
        Int_t layer = sensorId;
        hit->AddVTX(trackIdx, layer,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fItCollId >= 0 && fDetName==TCITgeometryConstructor::GetSDname()) {
        Int_t layer = sensorId;
        hit->AddITR(trackIdx, layer, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }

    if (fMsdCollId >= 0 && fDetName==TCMSDgeometryConstructor::GetSDname()) {
        Int_t layer  = sensorId;
        hit->AddMSD(trackIdx, layer, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }

    if (fTwCollId >= 0 && fDetName==TCTWgeometryConstructor::GetSDname()) {
        Int_t barId  = sensorId % TATWparGeo::GetLayerOffset();
        Int_t view  = sensorId /  TATWparGeo::GetLayerOffset();
        hit->AddTW(trackIdx, barId, view,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fCaCollId >= 0 && fDetName==TCCAgeometryConstructor::GetSDname()) {
        Int_t crystalId  = sensorId;
        hit->AddCAL(trackIdx, crystalId,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }


}



