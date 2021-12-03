/*!
 \file
 \version $Id: TAITntuCluster.cxx
 \brief Class for ITR cluster container
 */

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

/*!
 \class TAITntuCluster
 \brief Class for ITR cluster container
 */

ClassImp(TAITntuCluster);

TString TAITntuCluster::fgkBranchName   = "itclus.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAITntuCluster::TAITntuCluster() 
: TAGdata(),
  fGeometry(0x0),
  fListOfClusters(0x0)
{
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
Int_t TAITntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntries();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAITntuCluster::GetListOfClusters(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAITntuCluster::GetListOfClusters(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
	  
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return a cluster
TAITcluster* TAITntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAITcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAITcluster* TAITntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAITcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAITntuCluster::SetupClones()
{
   fGeometry = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();

   if (fListOfClusters) return;
   fListOfClusters    = new TObjArray(fGeometry->GetSensorsN());
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
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
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
	  TClonesArray* list = GetListOfClusters(i);
	  list->Delete();
   }   
}

//______________________________________________________________________________
//  
TAITcluster* TAITntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAITcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster();
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//______________________________________________________________________________
//  
TAITcluster* TAITntuCluster::NewCluster(TAITcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAITcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAITcluster(*clus);
     cluster->SetClusterIdx(clusterArray.GetEntriesFast()-1);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAITntuCluster::ToStream(ostream& os, Option_t* option) const
{
   for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
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

