
//
// Documentation : See doc.html (readme, main.cc, ...)
// Convention    : Root coding convention: TMyClass for classes, fMyVariable for class data members, kMyVariable for constants, gMyVariable for global variables
// Help          : http://www.lcsim.org/software/geant4/doxygen/html/classes.html
//

#include "TAMCparTools.hxx"

#include "TCFOtrackingAction.hxx"
#include "G4TrackingManager.hh"

#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "TVector3.h"

using namespace CLHEP;

map<TString, Int_t> TCFOtrackingAction::fgkVolumeToRegion = {{"World",0}, {"StartCounter",10}, {"BM",20}, {"BeamMonitor",20}, {"Gaz",21}, {"Foil", 22},{"targetPhy",30},{"Vertex",40},{"vtxEpiPhy",40},{"Magnets",50}, {"itEpiPhy",60}, {"msdEpiPhy",70}, {"Tof",80}, {"Calo",90}};

//
//---------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOtrackingAction::TCFOtrackingAction(TCFObaseEventAction* aEventAction)
{
    fEventAction = aEventAction ;
}

//
//---------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TCFOtrackingAction::~TCFOtrackingAction()
{
}

//
//---------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOtrackingAction::PreUserTrackingAction(const G4Track*)
{
}

//
//---------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void TCFOtrackingAction::PostUserTrackingAction(const G4Track* aTrack){

    TString partName = aTrack->GetDefinition()->GetParticleName() ;
    TString detName  = aTrack->GetVolume()->GetName() ;
    TString regName  = aTrack->GetOriginTouchable()->GetVolume()->GetName() ;
    Int_t flukaID    = TAMCparTools::GetFlukaId(partName) ;
    Int_t trackID    = aTrack->GetTrackID() ;
    Int_t parentID   = aTrack->GetParentID() ;
    Int_t charge     = (int)aTrack->GetDefinition()->GetPDGCharge() ;
    Int_t baryon     = aTrack->GetDefinition()->GetBaryonNumber() ;
    Int_t deadId     = fgkVolumeToRegion[detName] ;
    Int_t regId      = fgkVolumeToRegion[regName] ;
    Double_t mass    = aTrack->GetDefinition()->GetPDGMass()/GeV ;
    Double_t tof     = aTrack->GetLocalTime()/s ;
    Double_t time    = aTrack->GetGlobalTime()/s ;
    Double_t length  = aTrack->GetTrackLength()/cm ;

    TVector3 vtxpos ; /// position of the vertex of the track
    vtxpos.SetX(aTrack->GetVertexPosition().getX()/cm);
    vtxpos.SetY(aTrack->GetVertexPosition().getY()/cm);
    vtxpos.SetZ(aTrack->GetVertexPosition().getZ()/cm);
    TVector3 vtxmom ; /// momentum of the track at the vertex
    vtxmom.SetX(aTrack->GetMomentum().getX()/GeV);
    vtxmom.SetY(aTrack->GetMomentum().getY()/GeV);
    vtxmom.SetZ(aTrack->GetMomentum().getZ()/GeV);

    TVector3 finpos(0,0,0); /// final position of the track
    TVector3 finmom(0,0,0); /// final momentum of the track
    if(aTrack->GetTrackStatus()==fStopAndKill){
        finpos.SetX(aTrack->GetPosition().getX()/cm);
        finpos.SetY(aTrack->GetPosition().getY()/cm);
        finpos.SetZ(aTrack->GetPosition().getZ()/cm);
        finmom.SetX(aTrack->GetMomentum().getX()/GeV);
        finmom.SetY(aTrack->GetMomentum().getY()/GeV);
        finmom.SetZ(aTrack->GetMomentum().getZ()/GeV);
    }
    TVector3 xposparent(0,0,0);  /// position of the parent particle - not needed, so initialized to zero
    fEventAction->GetTrackMc()->NewTrack(flukaID,charge,trackID,regId,baryon,deadId,mass,parentID,time,tof,
                                       length,vtxpos,finpos,vtxmom,finmom,xposparent,xposparent,-1);
}





