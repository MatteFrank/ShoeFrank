/*!
 \file TAITntuCluster.cxx
 \brief Class for ITR cluster container
 */

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

#include "TAGnameManager.hxx"

/*!
 \class TAITntuCluster
 \brief Class for ITR cluster container
 */

//! Class Imp
ClassImp(TAITntuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITntuCluster::TAITntuCluster(Int_t sensorsN)
 : TAGdata(),
   fSensorsN(sensorsN),
   fListOfClusters(0x0)
{
   if (sensorsN == 0) {
      Warning("TAITntuCluster()", "Size of hit array not set, set to %d\n", TAITparGeo::GetDefSensorsN());
      fSensorsN = TAITparGeo::GetDefSensorsN();
   }
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITntuCluster::~TAITntuCluster() 
{
   delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
//!
//! \param[in] iSensor sensor id
Int_t TAITntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntriesFast();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
//! return list of clusters
//!
//! \param[in] iSensor sensor id
TClonesArray* TAITntuCluster::GetListOfClusters(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return list of clusters (const)
//!
//! \param[in] iSensor sensor id
TClonesArray* TAITntuCluster::GetListOfClusters(Int_t iSensor) const
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
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
TAITcluster* TAITntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAITcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor (const)
//!
//! \param[in] iSensor sensor id
//! \param[in] iCluster cluster index
const TAITcluster* TAITntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAITcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAITntuCluster::SetupClones()
{
   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray(fSensorsN);
   for (Int_t i = 0; i < fSensorsN; ++i) {
	  TClonesArray* arr = new TClonesArray("TAITcluster");
	  arr->SetOwner(true);
	  fListOfClusters->AddAt(arr, i);
   }
   fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAITntuCluster::Clear(Option_t*)
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
	  TClonesArray* list = GetListOfClusters(i);
	  list->Delete();
   }   
}

//______________________________________________________________________________
//! Create new cluster for a given sensor
//!
//! \param[in] iSensor sensor id
TAITcluster* TAITntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAITcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster();
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
TAITcluster* TAITntuCluster::NewCluster(TAITcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAITcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster(*clus);
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
void TAITntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
   os << "TAITntuCluster " << GetName()
   << Form("  nClus=%3d", GetClustersN(i))
   << endl;
   
   //TODO properly
   //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
   for (Int_t j = 0; j < GetClustersN(i); j++) {
	  const TAITcluster*  cluster = GetCluster(i,j);
	  if (cluster)
		 os << Form("%4d", j);
	  os << endl;
	  
   }
   }
}

