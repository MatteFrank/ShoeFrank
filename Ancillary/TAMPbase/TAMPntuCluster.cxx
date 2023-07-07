/*!
 \file TAMPntuCluster.cxx
 \brief Class for ITR cluster container
 */

#include "TAMPparGeo.hxx"
#include "TAMPntuCluster.hxx"

#include "TAGnameManager.hxx"

/*!
 \class TAMPntuCluster
 \brief Class for ITR cluster container
 */

//! Class Imp
ClassImp(TAMPntuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMPntuCluster::TAMPntuCluster()
: TAGdata(),
  fGeometry(0x0),
  fListOfClusters(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPntuCluster::~TAMPntuCluster()
{
   delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
//!
//! \param[in] iSensor sensor id
Int_t TAMPntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntriesFast();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
//! return list of clusters
//!
//! \param[in] iSensor sensor id
TClonesArray* TAMPntuCluster::GetListOfClusters(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return list of clusters (const)
//!
//! \param[in] iSensor sensor id
TClonesArray* TAMPntuCluster::GetListOfClusters(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
	  
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return a cluster
//!
//! \param[in] iSensor sensor id
//! \param[in] iCluster cluster index
TAMPcluster* TAMPntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAMPcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor (const)
//!
//! \param[in] iSensor sensor id
//! \param[in] iCluster cluster index
const TAMPcluster* TAMPntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAMPcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMPntuCluster::SetupClones()
{
   fGeometry = (TAMPparGeo*) gTAGroot->FindParaDsc(FootParaDscName("TAMPparGeo"), "TAMPparGeo")->Object();

   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray(fGeometry->GetSensorsN());
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* arr = new TClonesArray("TAMPcluster");
	  arr->SetOwner(true);
	  fListOfClusters->AddAt(arr, i);
   }
   fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMPntuCluster::Clear(Option_t*)
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* list = GetListOfClusters(i);
	  list->Delete();
   }   
}

//______________________________________________________________________________
//! Create new cluster for a given sensor
//!
//! \param[in] iSensor sensor id
TAMPcluster* TAMPntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAMPcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAMPcluster();
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  Error("NewCluster()", "Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//______________________________________________________________________________
//! Create a new cluster for a given sensor from an existing one
//!
//! \param[in] clus a given cluster
//! \param[in] iSensor sensor id
TAMPcluster* TAMPntuCluster::NewCluster(TAMPcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAMPcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAMPcluster(*clus);
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  Error("NewCluster()", "Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAMPntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
   os << "TAMPntuCluster " << GetName()
   << Form("  nClus=%3d", GetClustersN(i))
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetClustersN(i); j++) {
	  const TAMPcluster*  cluster = GetCluster(i,j);
	  if (cluster)
		 os << Form("%4d", j);
	  os << endl;
	  
   }
   }
}

