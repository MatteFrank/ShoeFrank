/*!
  \file
  \version $Id: TABMntuTrack.cxx,v 1.9 2003/07/07 18:42:33 mueller Exp $
  \brief   Implementation of TABMntuTrack.
*/

#include "TABMntuTrack.hxx"


//########################################   TABMntuTrack   ######################################

/*!
  \class TABMntuTrack TABMntuTrack.hxx "TABMntuTrack.hxx"
  \brief PC track description - container (Qad level). **
*/

ClassImp(TABMntuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuTrack::TABMntuTrack()
 : TAGdata(),
   fListOfTracks(0),
   fPrunedTrack(0),
   fStatus(-1000),
   fPrunedStatus(-1)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuTrack::~TABMntuTrack()
{
  if(fListOfTracks)
    fListOfTracks->Delete();
  if(fPrunedTrack)
    fPrunedTrack->Delete();
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TABMntuTrack::SetupClones()
{
  if (!fListOfTracks)
    fListOfTracks = new TClonesArray("TABMtrack");
  if (!fPrunedTrack)
    fPrunedTrack = new TClonesArray("TABMtrack");
  return;
}

//------------------------------------------+-----------------------------------
//! Clear.

void TABMntuTrack::Clear(Option_t*)
{
  TAGdata::Clear();
  fStatus=-1000;
  fPrunedStatus=-1;
  if(fPrunedTrack)
    fPrunedTrack->Delete();
  if(fListOfTracks)
    fListOfTracks->Delete();

   return;
}

TABMtrack* TABMntuTrack::NewTrack(TABMtrack trk)
{
   TClonesArray &trackArray = *fListOfTracks;
   TABMtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TABMtrack(trk);
   return track;
}

TABMtrack* TABMntuTrack::NewPrunedTrack(TABMtrack trk, Int_t view)
{
  fPrunedStatus=view;
  TClonesArray &trackArray = *fPrunedTrack;
  TABMtrack* track = new(trackArray[trackArray.GetEntriesFast()]) TABMtrack(trk);
  return track;
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMntuTrack::ToStream(ostream& os, Option_t* option) const
{
  //~ os << "TABMntuTrack" << Form("  ntrk=%3d", ntrk) << endl;

  //~ if (ntrk == 0) return;
  //~ os << "ind nh "
     //~ << "    x0     ux   y0     uy"<< endl;

  //~ for (Int_t i = 0; i < ntrk; i++) {
    //~ const TABMtrack* p_trk = GetTrack(i);
    //~ os << Form("%3d", i)
       //~ << Form(" %2d", p_trk->nwire);
    //~ os << Form(" %5.0f %6.3f", p_trk->x0, p_trk->ux)
       //~ << Form(" %4.0f %6.3f", p_trk->y0, p_trk->uy);

    //~ os << endl;
  //~ }

  return;
}
