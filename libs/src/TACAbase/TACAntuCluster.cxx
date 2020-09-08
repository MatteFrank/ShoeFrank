////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TACAcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TACAparGeo.hxx"
#include "TACAntuCluster.hxx"

ClassImp(TACAcluster) // Description of a cluster

//______________________________________________________________________________
//  
TACAcluster::TACAcluster()
:  TAGcluster(),
   fPosition(0., 0., 0.),
   fPosError(0., 0., 0.),
   fPositionG(0., 0., 0.),
   fPosErrorG(0., 0., 0.),
   fListOfHits(0x0),
   fIndexSeed(-1),
   fNumber(-1),
   fCharge(0.),
   fIsValid(kFALSE)
{
   SetupClones();
}

//______________________________________________________________________________
//
void TACAcluster::SetupClones()
{
   fListOfHits = new TClonesArray("TACAntuHit");
   fListOfHits->SetOwner(true);
}

//______________________________________________________________________________
//  
TACAcluster::TACAcluster(const TACAcluster& cluster)
:  TAGcluster(cluster),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fPositionG(cluster.fPositionG),
   fPosErrorG(cluster.fPosErrorG),
   fIndexSeed(cluster.fIndexSeed),
   fNumber(cluster.fNumber),
   fCharge(cluster.fCharge),
   fIsValid(cluster.fIsValid)
{
   // TACAcluster constructor
   if (cluster.fListOfHits)
      fListOfHits = (TClonesArray*)cluster.fListOfHits->Clone();
}

//______________________________________________________________________________
//  
TACAcluster::~TACAcluster()
{ 
   // TACAcluster default destructor
   delete fListOfHits;
}

//______________________________________________________________________________
//  
void TACAcluster::SetPosition(TVector3& pos)
{
   fPosition.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//  
void TACAcluster::SetPosError(TVector3& pos)
{
   fPosError.SetXYZ(pos.X(), pos.Y(), pos.Z());
}

//______________________________________________________________________________
//  
void TACAcluster::SetPositionG(TVector3& posGlo)
{
   fPositionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   fPosErrorG.SetXYZ(fPosError.X(), fPosError.Y(), 0.01);
}

//______________________________________________________________________________
// 
TACAntuHit* TACAcluster::GetHit(Int_t idx)
{ 
   if (idx >=0 && idx < fListOfHits->GetEntries())
	  return (TACAntuHit*)fListOfHits->At(idx);
   else
	  return 0x0;
}

//______________________________________________________________________________
//  
void TACAcluster::ResetHits()
{
   fListOfHits->Delete();
}

//______________________________________________________________________________
//
void TACAcluster::AddHit(TACAntuHit* hit)
{
   for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
      Int_t idx = hit->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &pixelArray = *fListOfHits;
   new(pixelArray[pixelArray.GetEntriesFast()]) TACAntuHit(*hit);
}

//###############################################################################

ClassImp(TACAntuCluster);

TString TACAntuCluster::fgkBranchName   = "caclus.";

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
   return list->GetEntries();
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
TACAcluster* TACAntuCluster::GetCluster(Int_t iCluster)
{
   TClonesArray* list = GetListOfClusters();
   return (TACAcluster*)list->At(iCluster);
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TACAcluster* TACAntuCluster::GetCluster(Int_t iCluster) const
{
   TClonesArray* list = GetListOfClusters();
   return (TACAcluster*)list->At(iCluster);
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
//
TACAcluster* TACAntuCluster::NewCluster()
{
   TClonesArray &clusterArray = *GetListOfClusters();
   TACAcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TACAcluster();
   cluster->SetNumber(clusterArray.GetEntriesFast()-1);
   
   return cluster;
}

//______________________________________________________________________________
//
TACAcluster* TACAntuCluster::NewCluster(TACAcluster* clus)
{
   TClonesArray &clusterArray = *GetListOfClusters();
   TACAcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TACAcluster(*clus);
   cluster->SetNumber(clusterArray.GetEntriesFast()-1);

   return cluster;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TACAntuCluster::ToStream(ostream& os, Option_t* option) const
{
   os << "TACAntuCluster " << GetName()
   << Form("  nClus=%3d", GetClustersN())
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetClustersN(); j++) {
      const TACAcluster*  cluster = GetCluster(j);
         if (cluster)
            os << Form("%4d", j);
      os << endl;
      
   }
}

