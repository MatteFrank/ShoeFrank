/*!
 \file TAVTntuCluster.cxx
  \brief Class for VTX cluster container
 */

#include "TAVTparGeo.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuTrack.hxx"

/*!
 \class TAVTntuCluster
 \brief Class for VTX cluster container
 */

ClassImp(TAVTntuCluster);

TString TAVTntuCluster::fgkBranchName   = "vtclus.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuCluster::TAVTntuCluster() 
: TAGdata(),
  fGeometry(0x0),
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
// return number of clusters
Int_t TAVTntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntries();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
// return number of clusters
TClonesArray* TAVTntuCluster::GetListOfClusters(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
// return number of clusters
TClonesArray* TAVTntuCluster::GetListOfClusters(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
	  
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
// return a cluster
TAVTcluster* TAVTntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAVTcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
// return a pixel for a given sensor
const TAVTcluster* TAVTntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAVTcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
// Setup clones.
void TAVTntuCluster::SetupClones()
{
   fGeometry = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();

   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray();
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* arr = new TClonesArray("TAVTcluster");
	  arr->SetOwner(true);
	  fListOfClusters->AddAt(arr, i);
   }
   fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
// Clear event.
void TAVTntuCluster::Clear(Option_t*)
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* list = GetListOfClusters(i);
	  list->Delete();
   }   
}

//______________________________________________________________________________
//  
TAVTcluster* TAVTntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAVTcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAVTcluster();
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//______________________________________________________________________________
//  
TAVTcluster* TAVTntuCluster::NewCluster(TAVTcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAVTcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAVTcluster(*clus);
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

/*------------------------------------------+---------------------------------*/
// ostream insertion.
void TAVTntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
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

