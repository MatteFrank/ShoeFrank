////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

/*!
 \file TAMSDcluster.cxx
 \brief  Implementation of TAMSDcluster
 */

#include "TAMSDcluster.hxx"
#include "TAMSDhit.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TF1.h"

#include "TAGnameManager.hxx"

#include <math.h>

/*!
 \class TAMSDcluster
 \brief simple container class for tracks with the associated clusters
 
 \author Ch. Finck
 */

//! Class Imp
ClassImp(TAMSDcluster) // Description of a cluster

//______________________________________________________________________________
//! Detector
TAMSDcluster::TAMSDcluster()
:  TAGcluster(),
   fPositionF(0.),
   fPosErrorF(0),
   fPositionCorr(0),
   fCurPosition(0,0,0),
   fCog(0),
   fPlaneView(-1),
   fEnergyLoss(0),
   fEnergyLossCorr(0),
   fEtaValue(0),
   fEtaFastValue(0)
{
   // TAMSDcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//! Setup clones
void TAMSDcluster::SetupClones()
{
   fListOfStrips = new TClonesArray("TAMSDhit");
   fListOfStrips->SetOwner(true);
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TAMSDcluster::TAMSDcluster(const TAMSDcluster& cluster)
:  TAGcluster(cluster),
   fPositionF(cluster.fPositionF),
   fPosErrorF(cluster.fPosErrorF),
   fPositionCorr(cluster.fPositionCorr),
   fCog(cluster.fCog),
   fCurPosition(cluster.fCurPosition),
   fPlaneView(cluster.fPlaneView),
   fEnergyLoss(cluster.fEnergyLoss),
   fEnergyLossCorr(cluster.fEnergyLossCorr),
   fEtaValue(cluster.fEtaValue),
   fEtaFastValue(cluster.fEtaFastValue)
{
   fListOfStrips = (TClonesArray*)cluster.fListOfStrips->Clone();
}

//______________________________________________________________________________
//! default destructor
TAMSDcluster::~TAMSDcluster()
{ 
   // TAMSDcluster default destructor
   delete fListOfStrips;
}

//______________________________________________________________________________
//! Add strip
//!
//! \param[in] strip strip to add
void TAMSDcluster::AddStrip(TAMSDhit* strip)
{
   for (Int_t k = 0; k < strip->GetMcTracksN(); ++k) {
      Int_t idx = strip->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &StripArray = *fListOfStrips;
   new(StripArray[StripArray.GetEntriesFast()]) TAMSDhit(*strip);
   
   fElementsN = fListOfStrips->GetEntriesFast();
}

//______________________________________________________________________________
//! Set global cluster positon
//!
//! \param[in] posGlo global position
void TAMSDcluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   
   if (GetPlaneView() == 0)
      fPosError2.SetXYZ(fPosErrorF, 0, 0.01);
   else
      fPosError2.SetXYZ(0, fPosErrorF, 0.01);
}

//______________________________________________________________________________
//! Get cluster strip
//!
//! \param[in] idx index strip
TAMSDhit* TAMSDcluster::GetStrip(Int_t idx)
{
   if (idx >=0 && idx < fListOfStrips->GetEntriesFast())
      return (TAMSDhit*)fListOfStrips->At(idx);
   else
      return 0x0;
}

//______________________________________________________________________________
//! Set cluster positon 1D
//!
//! \param[in] pos position
void TAMSDcluster::SetPositionF(Float_t pos)
{
   fPositionF = pos;
   if (GetPlaneView() == 0)
      fCurPosition.SetXYZ(fPositionF, 0, 0);
   else
      fCurPosition.SetXYZ(0, fPositionF, 0);
}

//______________________________________________________________________________
//! Set cluster positon error 1D
//!
//! \param[in] pos position error
void TAMSDcluster::SetPosErrorF(Float_t pos)
{
   fPosErrorF = pos;
}

//______________________________________________________________________________
//! Set COG position
//!
//! \param[in] pos cog position
void TAMSDcluster::SetCog(Float_t pos)
{
   fCog = pos;
}

//______________________________________________________________________________
//! Set eta
//!
//! \param[in] eta eta value
void TAMSDcluster::SetEta(Float_t eta)
{
   fEtaValue = eta;
}

//______________________________________________________________________________
//! Set plane view
//!
//! \param[in] v view value
void TAMSDcluster::SetPlaneView(Int_t v)
{
   fPlaneView = v;
   fDeviceType = TAGgeoTrafo::GetDeviceType(FootBaseName("TAMSDparGeo")) + fPlaneView;
}

//______________________________________________________________________________
//! Return the distance between this clusters and the pointed cluster regardless of the plane
//!
//! \param[in] aClus a given cluster
Float_t TAMSDcluster::Distance(TAMSDcluster* aClus)
{
   TVector3 clusPosition( aClus->GetPositionG() );
   
   // Now compute the distance beetween the two hits
   clusPosition -= (GetPositionG());
   
   // Insure that z position is 0 for 2D length computation
   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
   
   return clusPosition.Mag();
}

//______________________________________________________________________________
//! Get address (first strip position)
Float_t TAMSDcluster::GetAddress() const
{
   const TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(0);
   return strip->GetPosition();
}


