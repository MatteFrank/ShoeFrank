////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"


ClassImp(TAMSDntuCluster);

TString TAMSDntuCluster::fgkBranchName   = "msdclus.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDntuCluster::TAMSDntuCluster() 
: TAGdata(),
  fGeometry(0x0),
  fListOfClusters(0x0)
{
  SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuCluster::~TAMSDntuCluster() 
{
  delete fListOfClusters;
}

//------------------------------------------+-----------------------------------
//! return number of clusters
Int_t TAMSDntuCluster::GetClustersN(Int_t iSensor) const
{
  if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
    TClonesArray*list = GetListOfClusters(iSensor);
    return list->GetEntries();   
  } else 
    return -1;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAMSDntuCluster::GetListOfClusters(Int_t iSensor)
{
  if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
    TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
    return list;
  } else 
    return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return number of clusters
TClonesArray* TAMSDntuCluster::GetListOfClusters(Int_t iSensor) const
{
  if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
    TClonesArray* list = (TClonesArray*)fListOfClusters->At(iSensor);
    return list;
    
  } else 
    return 0x0;   
}

//------------------------------------------+-----------------------------------
//! return a cluster
TAMSDcluster* TAMSDntuCluster::GetCluster(Int_t iSensor, Int_t iCluster)
{
  if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
    TClonesArray* list = GetListOfClusters(iSensor);
    return (TAMSDcluster*)list->At(iCluster);
  } else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
const TAMSDcluster* TAMSDntuCluster::GetCluster(Int_t iSensor, Int_t iCluster) const
{
  if (iCluster >=0 || iCluster < GetClustersN(iSensor)) {
    TClonesArray* list = GetListOfClusters(iSensor);
    return (TAMSDcluster*)list->At(iCluster);
  } else
    return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones.
void TAMSDntuCluster::SetupClones()
{
  fGeometry = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
  
  if (fListOfClusters) return;
  fListOfClusters    = new TObjArray();
  for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
    TClonesArray* arr = new TClonesArray("TAMSDcluster");
    arr->SetOwner(true);
    fListOfClusters->AddAt(arr, i);
  }
  fListOfClusters->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuCluster::Clear(Option_t*)
{
  for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
    TClonesArray* list = GetListOfClusters(i);
    list->Delete();
  }   
}

//______________________________________________________________________________
//  
TAMSDcluster* TAMSDntuCluster::NewCluster(Int_t iSensor)
{
  if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
    TClonesArray &clusterArray = *GetListOfClusters(iSensor);
    TAMSDcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAMSDcluster();
    return cluster;
  } else {
    cout << Form("Wrong sensor number %d\n", iSensor);
    return 0x0;
  }   
}

//______________________________________________________________________________
//  
TAMSDcluster* TAMSDntuCluster::NewCluster(TAMSDcluster* clus, Int_t iSensor)
{
  if (iSensor >= 0  || iSensor < fGeometry->GetSensorsN()) {
    TClonesArray &clusterArray = *GetListOfClusters(iSensor);
    TAMSDcluster* cluster = new(clusterArray[clusterArray.GetEntriesFast()]) TAMSDcluster(*clus);
    return cluster;
  } else {
    cout << Form("Wrong sensor number %d\n", iSensor);
    return 0x0;
  }   
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMSDntuCluster::ToStream(ostream& os, Option_t* option) const
{
  for (Int_t i = 0; i < fGeometry->GetSensorsN(); ++i) {
    os << "TAMSDntuCluster " << GetName()
       << Form("  nClus=%3d", GetClustersN(i))
       << endl;
    
    //TODO properly
    //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
    for (Int_t j = 0; j < GetClustersN(i); j++) {
      const TAMSDcluster*  cluster = GetCluster(i,j);
      if (cluster)
	os << Form("%4d", j);
      os << endl;
      
    }
  }
}

