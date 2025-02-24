/*! \file TCLINMbaseEventAction.cxx
 \brief Implementation of TCLINMbaseEventAction.
 */

#include "TCLINMeventAction.hxx"

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

#include "TCLINMgeometryConstructor.hxx"

#include "TCPLgeometryConstructor.hxx"
#include "TCCEgeometryConstructor.hxx"
#include "TCPWgeometryConstructor.hxx"

#include "TAPLparGeo.hxx"
#include "TACEparGeo.hxx"
#include "TAPWparGeo.hxx"

#include "TAMCevent.hxx"
#include "TCGmcHit.hxx"
#include "TAMCntuPart.hxx"

#include "TCLINMrunAction.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TFile.h"

/*! \class TCLINMeventAction
 \brief Event action class for FOOT
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Constructor
//!
//! \param[in] runAction run action
//! \param[in] footGeomConstructor FOOT geometry constructor
TCLINMeventAction::TCLINMeventAction(TCLINMrunAction* runAction, TCGbaseGeometryConstructor* footGeomConstructor)
: TCLINMbaseEventAction(runAction,footGeomConstructor)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Destructor
TCLINMeventAction::~TCLINMeventAction()
{
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! End of event action
//!
//! \param[in] evt a given event
void TCLINMeventAction::EndOfEventAction(const G4Event* evt)
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
void TCLINMeventAction::Collect(const G4Event* evt)
{
   if (fPlCollId >= 0)
      GetHitPerPlane(evt, fPlCollId);
   
   if (fCeCollId >= 0)
      GetHitPerPlane(evt, fCeCollId);
   
   if (fPwCollId >= 0)
      GetHitPerPlane(evt, fPwCollId);
   
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//! Get hits per plane
//!
//! \param[in] evt a given event
//! \param[in] idColl hit collection id
void TCLINMeventAction::GetHitPerPlane(const G4Event* evt, G4int idColl)
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
void TCLINMeventAction::FillTrack()
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
void TCLINMeventAction::FillHits(TAMCevent* event, TCGmcHit* mcHit)
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

    if (fPlCollId >= 0 && fDetName==TCPLgeometryConstructor::GetSDname()){
        event->AddSTC(trackIdx, TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                    TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fCeCollId >= 0 && fDetName==TCCEgeometryConstructor::GetSDname()) {
       Int_t barId = sensorId;
       Int_t view  = 0;
        event->AddTW(trackIdx, barId, view,TVector3(vin[0],vin[1],vin[2]), TVector3(vou[0],vou[1],vou[2]), TVector3(pin[0],pin[1],pin[2]),
                   TVector3(pou[0],pou[1],pou[2]), edep, al, time, trackId);
    }

    if (fPwCollId >= 0 && fDetName==TCPWgeometryConstructor::GetSDname()) {
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



