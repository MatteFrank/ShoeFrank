/*!
  \file TAMCntuPart.cxx
  \brief   Implementation of TAMCntuPart & TAMCpart.
*/

#include <bitset>
using namespace std;

#include "TString.h"

#include "TAMCntuPart.hxx"

/*!
  \class TAMCpart
  \brief MC Particle. **
*/

//! Class Imp
ClassImp(TAMCpart);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMCpart::TAMCpart()
 : TAGobject(),
   fFlukaId(-1),
   fCharge(0.),
   fType(-1),
   fRegion(-1),
   fBaryon(-1),
   fDead(-1),
   fMass(0.),
   fMotherId(-1),
   fTime(0.),
   fTof(0.),
   fTrkLength(0.),
   fInitPos(0,0,0),
   fFinalPos(0,0,0),
   fInitMom(0,0,0),
   fFinalMom(0,0,0)
{
}

//------------------------------------------+-----------------------------------
//! Construct with data
//!
//! \param[in] i_id sensor id
//! \param[in] i_chg atomic number
//! \param[in] i_type type number
//! \param[in] i_reg region number
//! \param[in] i_bar baryon number
//! \param[in] i_dead dead flag
//! \param[in] i_mass mass of particle
//! \param[in] i_moth mother index
//! \param[in] i_time time of particle
//! \param[in] i_tof time of flight of particle
//! \param[in] i_trlen track length
//! \param[in] i_ipos initial position of particle
//! \param[in] i_fpos final position of particle
//! \param[in] i_ip initial momentum of particle
//! \param[in] i_fp final momentum of particle
TAMCpart::TAMCpart(Int_t i_id, Int_t i_chg, Int_t i_type,
			       Int_t i_reg, Int_t i_bar, Int_t i_dead,
			       Double_t i_mass,  Int_t i_moth, 
			       Double_t i_time,
			       Double_t i_tof, Double_t i_trlen,
			       TVector3 i_ipos, TVector3 i_fpos, 
			       TVector3 i_ip,TVector3 i_fp)
 : TAGobject(),
   fFlukaId(i_id),
   fCharge(i_chg),
   fType(i_type),
   fRegion(i_reg),
   fBaryon(i_bar),
   fDead(i_dead),
   fMass(i_mass),
   fMotherId(i_moth),
   fTime(i_time),
   fTof(i_tof),
   fTrkLength(i_trlen)
{
  fInitPos.SetXYZ(i_ipos.X(),i_ipos.Y(),i_ipos.Z());
  fFinalPos.SetXYZ(i_fpos.X(),i_fpos.Y(),i_fpos.Z());
  fInitMom.SetXYZ(i_ip.X(),i_ip.Y(),i_ip.Z());
  fFinalMom.SetXYZ(i_fp.X(),i_fp.Y(),i_fp.Z());
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCpart::~TAMCpart()
{
}

void TAMCpart::Clear(Option_t* /*opt*/)
{
  delete this;
}

//##############################################################################

/*!
  \class TAMCntuPart
  \brief Particle  container. **
*/

//! Class Imp
ClassImp(TAMCntuPart);

      TString TAMCntuPart::fgkBranchName   = "mctrack.";
const TString TAMCntuPart::fgkDefDataName  = "eveMc";


//------------------------------------------+-----------------------------------
//! Default constructor.
TAMCntuPart::TAMCntuPart()
 : TAGdata(),
   fListOfTracks(0x0)
{
    SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCntuPart::~TAMCntuPart()
{
  delete fListOfTracks;
}

//------------------------------------------+-----------------------------------
//! New track
//!
//! \param[in] i_id sensor id
//! \param[in] i_chg atomic number
//! \param[in] i_type type number
//! \param[in] i_reg region number
//! \param[in] i_bar baryon number
//! \param[in] i_dead dead flag
//! \param[in] i_mass mass of particle
//! \param[in] i_moth mother index
//! \param[in] i_time time of particle
//! \param[in] i_tof time of flight of particle
//! \param[in] i_trlen track length
//! \param[in] i_ipos initial position of particle
//! \param[in] i_fpos final position of particle
//! \param[in] i_ip initial momentum of particle
//! \param[in] i_fp final momentum of particle
TAMCpart* TAMCntuPart::NewTrack(Int_t i_id, Int_t i_chg, Int_t i_type,
                                   Int_t i_reg, Int_t i_bar, Int_t i_dead,
                                   Double_t i_mass, Int_t i_moth,
                                   Double_t i_time,
                                   Double_t i_tof, Double_t i_trlen,
                                   TVector3 i_ipos, TVector3 i_fpos,
                                   TVector3 i_ip,TVector3 i_fp) {

    TClonesArray &trackCollection = *fListOfTracks;
    TAMCpart* track = new( trackCollection[trackCollection.GetEntriesFast()] ) TAMCpart(
                                            i_id,i_chg,i_type,i_reg,i_bar,i_dead,i_mass,i_moth,
                                            i_time,i_tof,i_trlen,i_ipos,i_fpos,i_ip,i_fp);
    return track;

};

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMCntuPart::SetupClones()   {
  if (!fListOfTracks) fListOfTracks = new TClonesArray("TAMCpart", 500);
}

//------------------------------------------+-----------------------------------
//! Clear event.
//!
//! \param[in] opt option for clearing (not used)
void TAMCntuPart::Clear(Option_t*)
{
   fListOfTracks->Clear("C");
}

//------------------------------------------+-----------------------------------
//! return number of tracks
Int_t TAMCntuPart::GetTracksN() const
{
   return fListOfTracks->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! Access to i 'th track
//!
//! \param[in] i track index
TAMCpart* TAMCntuPart::GetTrack(Int_t i)
{
  if(i >= 0 && i < GetTracksN())
   return (TAMCpart*) ((*fListOfTracks)[i]);
  else 
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! Read-only access to i 'th track (const)
//!
//! \param[in] i track index
const TAMCpart* TAMCntuPart::GetTrack(Int_t i) const
{
  if(i >= 0 && i < GetTracksN())
    return (const TAMCpart*) ((*fListOfTracks)[i]);
  else 
    return 0x0;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMCntuPart::ToStream(ostream& os, Option_t* option) const
{
  // os << "TAMCntuPart" 
  //    // << Form("  ntrack=%3d", ntrack) 
  //    << endl;

  // // if (ntrack == 0) return;
  
  // os << "ind slat stat  adct  adcb  tdct  tdcb" << endl;
  
  // // for (Int_t i = 0; i < ntrack; i++) {
  // //   const TAMCpart* p_track = Hit(i);
  // //   os << Form("%3d %4lf", i, p_track->mass)
  // //      << endl;
  // // }

  // return;
}

