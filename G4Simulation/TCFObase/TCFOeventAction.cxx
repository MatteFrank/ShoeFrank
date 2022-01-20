/*! \file TCFObaseEventAction.cxx
 \brief Implementation of TCFObaseEventAction.
 */

#include "TCFOeventAction.hxx"

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
#include "TAMCntuPart.hxx"

#include "TCFOrunAction.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TFile.h"

/*! \class TCFOeventAction
 \brief Event action class for FOOT
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] runAction run action
//! \param[in] footGeomConstructor FOOT geometry constructor
TCFOeventAction::TCFOeventAction(TCFOrunAction* runAction, TCGbaseGeometryConstructor* footGeomConstructor)
: TCFObaseEventAction(runAction,footGeomConstructor)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCFOeventAction::~TCFOeventAction()
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! End of event action
//!
//! \param[in] evt a given event
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
//! Collect hits per event
//!
//! \param[in] evt a given event
void TCFOeventAction::Collect(const G4Event* evt)
{
   
   if (fStCollId >= 0)
      GetHitPerPlane(evt, fStCollId);
   
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
//! Get hits per plane
//!
//! \param[in] evt a given event
//! \param[in] idColl hit collection id
void TCFOeventAction::GetHitPerPlane(const G4Event* evt, G4int idColl)
{
   G4HCofThisEvent*  hitCollEv = evt->GetHCofThisEvent();
   TCGmcCollections* hitList   = (TCGmcCollections*)(hitCollEv->GetHC(idColl));
   TAMCevent*            hit   = fRunAction->GetEventMC();
   
   Int_t entries =  hitList->entries();
    fDetName = hitList->GetName();
    
    if (FootMcDebugLevel(1))
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

    if(FootMcDebugLevel(1)) printf("%s \n",fDetName.Data());

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
        if(trackId1 == trackId2 && sensorId1==sensorId2)
          edep += mcHit2->GetEdep();
        else{
            vou = mcHit1->GetPosOut();
            pou = mcHit1->GetMomOut();
            mcHit1->SetEdep(edep);
            mcHit1->SetPosOut(vou);
            mcHit1->SetMomOut(pou);
            mcHit1->SetPosIn(vin);
            mcHit1->SetMomIn(pin);
            FillHits(hit, mcHit1);

            if(FootMcDebugLevel(1)) printf("[%d,%d] \t %.3e MeV \t posInit(%.3e,%.3e,%.3e) \t posOut(%.3e,%.3e,%.3e)\n",trackId1,sensorId1,edep,vin.getX(),vin.getY(),vin.getZ(),vou.getX(),vou.getY(),vou.getZ());
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
            if(FootMcDebugLevel(1)) printf("[%d,%d] \t %.3e MeV \t posInit(%.3e,%.3e,%.3e) \t posOut(%.3e,%.3e,%.3e)\n",trackId2,sensorId2,edep,vin.getX(),vin.getY(),vin.getZ(),vou.getX(),vou.getY(),vou.getZ());
        }
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Fill track info
void TCFOeventAction::FillTrack()
{
    TAMCevent* event = fRunAction->GetEventMC();

    Int_t nTracks = fMcEvent->GetTracksN();
    fMapTrackIdx.clear();
    TVector3 initpos;
    TVector3 initmom;
    TVector3 finalpos;
    TVector3 finalmom;
    Int_t    flukaID;
    Int_t    trackID;
    Int_t    parentID;
    Int_t    nbaryon;
    Int_t    charge;
    Int_t    dead;
    Int_t    region;
    Double_t mass;
    Double_t tof;
    Double_t time;
    Double_t length;

    for(Int_t i = 0 ; i < nTracks; ++i){
        flukaID  = fMcEvent->GetTrack(i)->GetFlukaID();
        trackID  = fMcEvent->GetTrack(i)->GetType();
        parentID = fMcEvent->GetTrack(i)->GetMotherID();
        mass     = fMcEvent->GetTrack(i)->GetMass();
        charge   = fMcEvent->GetTrack(i)->GetCharge();
        nbaryon  = fMcEvent->GetTrack(i)->GetBaryon();
        tof      = fMcEvent->GetTrack(i)->GetTof();
        time     = fMcEvent->GetTrack(i)->GetTime();
        length   = fMcEvent->GetTrack(i)->GetTrkLength();
        initpos  = fMcEvent->GetTrack(i)->GetInitPos();
        initmom  = fMcEvent->GetTrack(i)->GetInitP();
        finalpos = fMcEvent->GetTrack(i)->GetFinalPos();
        finalmom = fMcEvent->GetTrack(i)->GetFinalP();
        dead     = fMcEvent->GetTrack(i)->GetDead() ;
        region   = fMcEvent->GetTrack(i)->GetRegion();

        fMapTrackIdx[trackID] = i;
        parentID = fMapTrackIdx[parentID]+1; // index is stored not id, compliant with Fluka
      
        event->AddPart(parentID,trackID,charge,region,nbaryon,dead,flukaID,
                                    initpos,finalpos,initmom,finalmom,mass,time,tof,length);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Fill hits
//!
//! \param[in] event MC event
//! \param[in] mcHit G4 MC hit
void TCFOeventAction::FillHits(TAMCevent* event, TCGmcHit* mcHit)
{
    G4ThreeVector vin = mcHit->GetPosIn()*TAGgeoTrafo::MmToCm();
    G4ThreeVector vou = mcHit->GetPosOut()*TAGgeoTrafo::MmToCm();
    G4ThreeVector pin = mcHit->GetMomIn()*TAGgeoTrafo::MmToCm();
    G4ThreeVector pou = mcHit->GetMomOut()*TAGgeoTrafo::MmToCm();

    Int_t    sensorId = mcHit->GetSensorId();
    Int_t    trackId  = mcHit->GetTrackId();
    Double_t edep     = mcHit->GetEdep()*TAGgeoTrafo::MevToGev();
    Double_t time     = mcHit->GetGlobalTime()*TAGgeoTrafo::NsToSec();
    Double_t al       = 0;
    Int_t    trackIdx = fMapTrackIdx[trackId]+1; // to be compliant with Fluka
  
    event->AddEvent(fEventNumber);

    if (fStCollId >= 0 && fDetName==TCSTgeometryConstructor::GetSDname()){
        event->AddSTC(trackIdx, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fBmCollId >= 0  && fDetName==TCBMgeometryConstructor::GetSDname()) {
        TVector3 pos(vin[0],vin[1],vin[2]);
        TVector3 posL = fFootGeomConstructor->GetGeoTrafo()->FromGlobalToBMLocal(pos);
        Int_t layer = (int)sensorId/2;
        Int_t view  = sensorId%2;
        Int_t cell  = fFootGeomConstructor->GetParGeoBm()->GetCell(pos,layer,view);
        view = -view;
        event->AddBMN(trackIdx,layer, view, cell, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]),
                    TVector3(pin[0],pin[1],pin[2]), TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fVtxCollId >= 0 && fDetName==TCVTgeometryConstructor::GetSDname()) {
        Int_t layer = sensorId;
        event->AddVTX(trackIdx, layer,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fItCollId >= 0 && fDetName==TCITgeometryConstructor::GetSDname()) {
        Int_t layer = sensorId;
        event->AddITR(trackIdx, layer, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }

    if (fMsdCollId >= 0 && fDetName==TCMSDgeometryConstructor::GetSDname()) {
        Int_t layer  = sensorId;
        event->AddMSD(trackIdx, layer, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }

    if (fTwCollId >= 0 && fDetName==TCTWgeometryConstructor::GetSDname()) {
        Int_t barId = sensorId % TATWparGeo::GetLayerOffset();
        Int_t view  = sensorId /  TATWparGeo::GetLayerOffset();
        event->AddTW(trackIdx, barId, view,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                   TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fCaCollId >= 0 && fDetName==TCCAgeometryConstructor::GetSDname()) {
        Int_t crystalId = sensorId;
        event->AddCAL(trackIdx, crystalId,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]),edep, al, time, trackId);
    }

   static Int_t crossIdOld = 0;
   if (TAGrecoManager::GetPar()->IsRegionMc()) {
      TAMCpart* part = fMcEvent->GetTrack(trackIdx-1);
      Int_t crossId  = part->GetRegion();
      TVector3 pos   = part->GetInitPos();
      TVector3 mom   = part->GetInitP();
      Double_t mass  = part->GetMass();
      Int_t charge   = part->GetCharge();
      Double_t tof   = part->GetTime();

      event->AddCROSS(trackIdx, crossId, crossIdOld, pos, mom, mass, charge, tof);
      crossIdOld = crossId;
   }
}



