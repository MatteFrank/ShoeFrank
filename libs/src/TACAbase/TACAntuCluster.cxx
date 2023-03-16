

/*!
 \file TACAntuCluster.cxx
 \brief Implementation TACAntuCluster
 */

#include "TACAparGeo.hxx"
#include "TACAntuCluster.hxx"

/*!
 \class TACAcluster
 \brief cluster for CAL detectors.
 */

//! Class Imp
ClassImp(TACAcluster) // Description of a cluster

//______________________________________________________________________________
//! Constructor
TACAcluster::TACAcluster()
:  TAGcluster(),
   fPosition(0., 0., 0.),
   fPosError(0., 0., 0.),
   fPositionG(0., 0., 0.),
   fPosErrorG(0., 0., 0.),
   fListOfHits(0x0),
   fIndexSeed(-1),
   fEnergy(0.)
{
   SetupClones();
}

//______________________________________________________________________________
//! Setup clones
void TACAcluster::SetupClones()
{
   fListOfHits = new TClonesArray("TACAhit");
   fListOfHits->SetOwner(true);
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] cluster cluster to copy
TACAcluster::TACAcluster(const TACAcluster& cluster)
:  TAGcluster(cluster),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fPositionG(cluster.fPositionG),
   fPosErrorG(cluster.fPosErrorG),
   fIndexSeed(cluster.fIndexSeed),
   fEnergy(cluster.fEnergy)
{
   // TACAcluster constructor
   if (cluster.fListOfHits)
      fListOfHits = (TClonesArray*)cluster.fListOfHits->Clone();
}

//______________________________________________________________________________
//! Destrictor
TACAcluster::~TACAcluster()
{ 
   // TACAcluster default destructor
   delete fListOfHits;
}

//______________________________________________________________________________
//! Set cluster positon
//!
//! \param[in] pos position
void TACAcluster::SetPosition(TVector3& pos)
{
   fPosition.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//! Set cluster positon error
//!
//! \param[in] pos position error
void TACAcluster::SetPosError(TVector3& pos)
{
   fPosError.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//! Set global cluster positon
//!
//! \param[in] posGlo global position
void TACAcluster::SetPositionG(TVector3& posGlo)
{
   fPositionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosErrorG.SetXYZ(fPosError.X(), fPosError.Y(), 0.01);
}

//______________________________________________________________________________
//! Get cluster hit
//!
//! \param[in] idx index hit
TACAhit* TACAcluster::GetHit(Int_t idx)
{ 
   if (idx >= 0 && idx < fListOfHits->GetEntriesFast())
	  return (TACAhit*)fListOfHits->At(idx);
   else
	  return 0x0;
}

//______________________________________________________________________________
//! Delete hits in container
void TACAcluster::ResetHits()
{
   fListOfHits->Delete();
}

//______________________________________________________________________________
//! Add hit
//!
//! \param[in] hit hit to add
void TACAcluster::AddHit(TACAhit* hit)
{
   for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
      Int_t idx = hit->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pixelArray = *fListOfHits;
   new(pixelArray[pixelArray.GetEntriesFast()]) TACAhit(*hit);
}

//###############################################################################

//! Class Imp
ClassImp(TACAntuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TACAntuCluster::TACAntuCluster()
: TAGdata(),
  fListOfClusters(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAntuCluster::~TACAntuCluster()
{
   delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
Int_t TACAntuCluster::GetClustersN() const
{
   TClonesArray*list = GetListOfClusters();
   return list->GetEntriesFast();
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TACAntuCluster::GetListOfClusters()
{
   TClonesArray* list = (TClonesArray*)fListOfClusters;
   return list;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TACAntuCluster::GetListOfClusters() const
{
   TClonesArray* list = (TClonesArray*)fListOfClusters;
   return list;
}

//------------------------------------------+-----------------------------------
//! return a cluster
//!
//! \param[in] iCluster cluster index
TACAcluster* TACAntuCluster::GetCluster(Int_t iCluster)
{
   if(iCluster >= 0 && iCluster < GetClustersN())
   {
      TClonesArray* list = GetListOfClusters();
      return (TACAcluster*)list->At(iCluster);
   }
   else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a cluster const
//!
//! \param[in] iCluster cluster index
const TACAcluster* TACAntuCluster::GetCluster(Int_t iCluster) const
{
   if(iCluster >= 0 && iCluster < GetClustersN())
   {
      TClonesArray* list = GetListOfClusters();
      return (TACAcluster*)list->At(iCluster);
   }
   else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TACAntuCluster::SetupClones()
{
   if (fListOfClusters == 0x0)
      fListOfClusters = new TClonesArray("TACAcluster");
   
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TACAntuCluster::Clear(Option_t*)
{
   fListOfClusters->Delete();
}

//______________________________________________________________________________
//! New cluster (default constructor)
TACAcluster* TACAntuCluster::NewCluster()
{
   TClonesArray &clusterArray = *GetListOfClusters();
   TACAcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TACAcluster();
   cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
  
   return cluster;
}

//______________________________________________________________________________
//! New cluster (copy constructor)
//!
//! \param[in] clus cluster to copy
TACAcluster* TACAntuCluster::NewCluster(TACAcluster* clus)
{
   TClonesArray &clusterArray = *GetListOfClusters();
   TACAcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TACAcluster(*clus);
   cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);

   return cluster;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TACAntuCluster::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuCluster " << GetName()
      << Form("  nClus=%3d", GetClustersN())
      << endl;
   
   for (Int_t j = 0; j < GetClustersN(); j++) {
      const TACAcluster*  cluster = GetCluster(j);
      if (cluster)
         os << Form("  %4d  (%5.2f, %5.2f, %5.2f) E=%5.2f", 
                       j, cluster->GetPosition().X(), 
                       cluster->GetPosition().Y(), cluster->GetPosition().Z(),
                       cluster->GetEnergy() );
      os << endl;
   }
}

