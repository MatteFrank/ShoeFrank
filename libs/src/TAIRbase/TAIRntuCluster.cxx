/*!
 \file  TAIRntuCluster.cxx
 \brief Simple container class for clusters
 */

#include "TAITparGeo.hxx"
#include "TAIRntuCluster.hxx"

/*!
 \class TAIRntuCluster
 \brief  Simple container class for clusters
 */

//! Class Imp
ClassImp(TAIRntuCluster);

TString TAIRntuCluster::fgkBranchName   = "irclus.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAIRntuCluster::TAIRntuCluster()
: TAGdata(),
  fGeometry(0x0),
  fListOfClusters(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRntuCluster::~TAIRntuCluster()
{
   delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters for a given sensor
//!
//! \param[in] iSensor a given sensor
Int_t TAIRntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntriesFast();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
//! return list of cluster
//!
//! \param[in] iSensor a given sensor
TClonesArray* TAIRntuCluster::GetListOfClusters(Int_t iSensor)
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
//! \param[in] iSensor a given sensor
TClonesArray* TAIRntuCluster::GetListOfClusters(Int_t iSensor) const
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
//! \param[in] iSensor a given sensor
//! \param[in] iCluster cluster index
TAIRcluster* TAIRntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAIRcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a cluster (const)
//!
//! \param[in] iSensor a given sensor
//! \param[in] iCluster cluster index
const TAIRcluster* TAIRntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAIRcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAIRntuCluster::SetupClones()
{
   fGeometry = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();

   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray(fGeometry->GetSensorsN());
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* arr = new TClonesArray("TAIRcluster");
	  arr->SetOwner(true);
	  fListOfClusters->AddAt(arr, i);
   }
   fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAIRntuCluster::Clear(Option_t*)
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* list = GetListOfClusters(i);
	  list->Delete();
   }   
}

//------------------------------------------+-----------------------------------
//! Create a new cluster
//!
//! \param[in] iSensor a given sensor
TAIRcluster* TAIRntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAIRcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAIRcluster();
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//------------------------------------------+-----------------------------------
//! Create a new cluster from a given cluster
//!
//! \param[in] iSensor a given sensor
//! \param[in] clus cluster to copy
TAIRcluster* TAIRntuCluster::NewCluster(TAIRcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAIRcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAIRcluster(*clus);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TAIRntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
   os << "TAIRntuCluster " << GetName()
   << Form("  nClus=%3d", GetClustersN(i))
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetClustersN(i); j++) {
	  const TAIRcluster*  cluster = GetCluster(i,j);
	  if (cluster)
		 os << Form("%4d", j);
	  os << endl;
	  
   }
   }
}

