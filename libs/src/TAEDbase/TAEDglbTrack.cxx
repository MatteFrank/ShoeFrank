#include <Riostream.h>

#include "TEveTrackPropagator.h"
#include "TEveRGBAPalette.h"
#include "TColor.h"

#include "TADIeveTrackPropagator.hxx"

#include "TAEDglbTrack.hxx"

//
ClassImp(TAEDglbTrack)

//__________________________________________________________
TAEDglbTrack::TAEDglbTrack(const Char_t* name, TADIeveTrackPropagator* prop)
 : TEveTrackList(name, prop),
   fPalette(new TEveRGBAPalette()),
   fNofTracks(0),
   fMaxMomentum(0.)
{
   // Set propagator to Runge-Kutta
   fPropagator->SetStepper(TEveTrackPropagator::kRungeKutta);
}

//__________________________________________________________
TAEDglbTrack::~TAEDglbTrack()
{
  // default destructor
   delete fPalette;
}

//__________________________________________________________
void TAEDglbTrack::AddTrack(TVector3& vertex, TVector3& moment, Int_t charge)
{
   TEveRecTrackD *recTrack = new TEveRecTrackD();
   recTrack->fV.Set(vertex[0], vertex[1], vertex[2]);
   recTrack->fP.Set(moment[0], moment[1], moment[2]);
   recTrack->fSign = charge;
   
   Float_t mag = moment.Mag();
   
   const UChar_t* pix = fPalette->ColorFromValue(mag);
   Int_t          ci  = TColor::GetColor(pix[0], pix[1], pix[2]);
   
   fCurrentTrack = new TEveTrack(recTrack, fPropagator);
   fCurrentTrack->SetLineColor(ci);
   
   fNofTracks++;
   fCurrentTrack->SetRnrPoints(kTRUE);

   AddElement(fCurrentTrack);
}

//__________________________________________________________
void TAEDglbTrack::AddTrackMarker(TVector3& point, TVector3& mom)
{
   fCurrentTrack->AddPathMark(TEvePathMarkD(TEvePathMarkD::kReference,
                                            TEveVectorD(point[0], point[1], point[2]),
                                            TEveVectorD(mom[0], mom[1], mom[2])));
}
                              
//__________________________________________________________
void TAEDglbTrack::ResetTracks()
{
   RemoveElements();
   fNofTracks = 0;
}

//__________________________________________________________
void TAEDglbTrack::MakeGlbTracks()
{
   MakeTracks();
}

//__________________________________________________________
void TAEDglbTrack::SetMaxMomentum(Float_t m)
{
   fPalette->SetMax(Int_t(m+0.5));
   fMaxMomentum = Int_t(m+0.5);
}
