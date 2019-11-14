////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAIRcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAITparGeo.hxx"
#include "TAIRntuCluster.hxx"


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
//! return number of clusters
Int_t TAIRntuCluster::GetClustersN(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray*list = GetListOfClusters(iSensor);
	  return list->GetEntries();   
   } else 
	  return -1;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAIRntuCluster::GetListOfClusters(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAIRntuCluster::GetListOfClusters(Int_t iSensor) const
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
	  return list;
	  
   } else 
	  return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return a cluster
TAIRcluster* TAIRntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAIRcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAIRcluster* TAIRntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
   if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
	  TClonesArray* list = GetListOfClusters(iSensor);
	  return (TAIRcluster*)list->At(iCluster);
   } else
	  return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAIRntuCluster::SetupClones()
{
   fGeometry = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetItDefParaName(), "TAITparGeo")->Object();

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

//______________________________________________________________________________
//  
TAIRcluster* TAIRntuCluster::NewCluster(Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAIRcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAIRcluster();
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

//______________________________________________________________________________
//  
TAIRcluster* TAIRntuCluster::NewCluster(TAIRcluster* clus, Int_t iSensor)
{
   if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
	  TClonesArray &clusterArray = *GetListOfClusters(iSensor);
	  TAIRcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAIRcluster(*clus);
	  return cluster;
   } else {
	  cout << Form("Wrong sensor number %d\n", iSensor);
	  return 0x0;
   }   
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
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

