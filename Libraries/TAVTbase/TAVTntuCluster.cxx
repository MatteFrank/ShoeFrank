/*!
 \file TAVTntuCluster.cxx
  \brief Class for VTX cluster container
 */

#include "TAVTparGeo.hxx"
#include "TAVTntuCluster.hxx"
#include "TAGnameManager.hxx"
#include "TAVTntuTrack.hxx"

/*!
 \class TAVTntuCluster
 \brief Class for VTX cluster container
 */

//! Class Imp
ClassImp(TAVTntuCluster);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuCluster::TAVTntuCluster(Int_t sensorsN)
: TAGdata(),
  fSensorsN(sensorsN),
  fListOfClusters(0x0)
{
   SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTntuCluster::~TAVTntuCluster() 
{
   delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
//!
//! \param[in] iSensor sensor id
Int_t TAVTntuCluster::GetClustersN(Int_t iSensor) const
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
TClonesArray* TAVTntuCluster::GetListOfClusters(Int_t iSensor)
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
TClonesArray* TAVTntuCluster::GetListOfClusters(Int_t iSensor) const
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
TAVTcluster* TAVTntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAVTcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor (const)
//!
//! \param[in] iSensor sensor id
//! \param[in] iCluster cluster index
const TAVTcluster* TAVTntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 && iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAVTcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAVTntuCluster::SetupClones()
{
   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray();
   for (Int_t i = 0; i < fSensorsN; ++i) {
	  TClonesArray* arr = new TClonesArray("TAVTcluster");
	  arr->SetOwner(true);
	  fListOfClusters->AddAt(arr, i);
   }
   fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAVTntuCluster::Clear(Option_t*)
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
TAVTcluster* TAVTntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAVTcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAVTcluster();
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
TAVTcluster* TAVTntuCluster::NewCluster(TAVTcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  && iSensor < fSensorsN) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAVTcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAVTcluster(*clus);
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
void TAVTntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fSensorsN; ++i) {
      os << "TAVTntuCluster " << GetName()
      << Form("  nClus=%3d", GetClustersN(i))
      << endl;
      
      //TODO properly
      //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
      for (Int_t j = 0; j < GetClustersN(i); j++) {
         const TAVTcluster*  cluster = GetCluster(i,j);
         if (cluster)
            os << Form("%4d", j);
         os << endl;
         
      }
   }
}

