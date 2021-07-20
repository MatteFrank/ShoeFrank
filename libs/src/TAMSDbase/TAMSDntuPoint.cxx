#include "TAMSDntuPoint.hxx"

#include "TString.h"
#include "TClonesArray.h"
#include "TAMSDparGeo.hxx"
#include "TAMSDntuPoint.hxx"


ClassImp(TAMSDpoint) // Description of Single Detector TAMSDpoint


//______________________________________________________________________________
//  default constructor
TAMSDpoint::TAMSDpoint()
: TAGcluster(),
   fStation(0),
   fColClus(0),
   fRowClus(0),
   fChargeZ(0),
   fChargeZProba(0.)
{
}

//______________________________________________________________________________
//  build a point
TAMSDpoint::TAMSDpoint( int layer, double x, double y, TVector3 position )
 : TAGcluster(),
   fStation(layer),
   fChargeZ(0),
   fChargeZProba(0.)
{
   fPosition = position;
}

//______________________________________________________________________________
//  build a point
TAMSDpoint::TAMSDpoint(Int_t layer, Double_t x, Double_t dx, TAMSDcluster* clusX, Double_t y, Double_t dy, TAMSDcluster* clusY)
 : TAGcluster(),
   fStation(layer),
   fColClus(new TAMSDcluster(*clusX)),
   fRowClus(new TAMSDcluster(*clusY)),
   fChargeZ(0),
   fChargeZProba(0.)
{
   fPosition.SetXYZ(x,y,0);
   fPosError.SetXYZ(dx,dy,0);
   
   Bool_t common = false;
   for (Int_t j = 0; j < fColClus->GetMcTracksN(); ++j) {
      Int_t idr = fColClus->GetMcTrackIdx(j);
      for (Int_t k = 0; k < fRowClus->GetMcTracksN(); ++k) {
         Int_t idc = fRowClus->GetMcTrackIdx(k);
         if (idr == idc) {
            AddMcTrackIdx(idr);
            common = true;
         }
      }
   }
   
   // in case mixing two or more different particles in a point
   if (!common) {
      map<int, int> mapIdx;
      for (Int_t j = 0; j < fColClus->GetMcTracksN(); ++j) {
         Int_t idr = fColClus->GetMcTrackIdx(j);
         mapIdx[idr] = 1;
      }
      
      for (Int_t k = 0; k < fRowClus->GetMcTracksN(); ++k) {
         Int_t idc = fRowClus->GetMcTrackIdx(k);
         mapIdx[idc] = 1;
      }
      
      for (map<int,int>::iterator it=mapIdx.begin(); it!=mapIdx.end(); ++it)
         AddMcTrackIdx(it->first);
   }
}

//______________________________________________________________________________
// Clear
void TAMSDpoint::Clear(Option_t*)
{
}


/*!
  \file
  \version $Id: TAMSDntuPoint.cxx
  \brief   Implementation of TAMSDntuPoint.
*/

ClassImp(TAMSDntuPoint);

//##############################################################################

TString TAMSDntuPoint::fgkBranchName   = "msdpt.";

//------------------------------------------+-----------------------------------
//!
TAMSDntuPoint::TAMSDntuPoint()
: TAGdata(),
  fGeometry(0x0),
  fListOfPoints(0x0)
{
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuPoint::~TAMSDntuPoint()
{
	delete fListOfPoints;
}

//______________________________________________________________________________
//  standard
TAMSDpoint* TAMSDntuPoint::NewPoint( int iStation, double x, double y, TVector3 position )
{
  if ( iStation >= 0 && iStation < fGeometry->GetSensorsN()/2 ) {
    TClonesArray &pointArray = *GetListOfPoints(iStation);
    TAMSDpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint( iStation, x, y, position );
    return point;
  } else {
    cout << Form("Wrong sensor number %d\n", iStation);
    return 0x0;
  }
}

//______________________________________________________________________________
//  standard
TAMSDpoint* TAMSDntuPoint::NewPoint(Int_t iStation, Double_t x, Double_t dx, TAMSDcluster* clusX, Double_t y, Double_t dy, TAMSDcluster* clusY)
{
   if ( iStation >= 0 && iStation < fGeometry->GetSensorsN()/2 ) {
      TClonesArray &pointArray = *GetListOfPoints(iStation);
      TAMSDpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint( iStation, x, dx, clusX, y, dy, clusY);
      return point;
   } else {
      cout << Form("Wrong sensor number %d\n", iStation);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
int TAMSDntuPoint::GetPointN(int iStation) const
{
  if ( iStation >= 0 && iStation < fGeometry->GetSensorsN()/2 ) {
    TClonesArray* list = (TClonesArray*)fListOfPoints->At(iStation);
    return list->GetEntries();
  } else return -1;
}


//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAMSDpoint* TAMSDntuPoint::GetPoint(int iStation, int iPoint) const
{
	if ( iPoint >= 0  && iPoint < GetPointN( iStation ) ) {
    TClonesArray* list = GetListOfPoints(iStation);
    return (TAMSDpoint*)list->At(iPoint);
	} else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! return number of points
TClonesArray* TAMSDntuPoint::GetListOfPoints(int iStation) const
{
   if (iStation >= 0  && iStation < fGeometry->GetSensorsN()/2) {
	  TClonesArray* list = (TClonesArray*)fListOfPoints->At(iStation);
	  return list;
   } else return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TAMSDntuPoint::SetupClones()
{
  fGeometry = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();

   if (fListOfPoints) return;
   fListOfPoints = new TObjArray();
   for ( int i = 0; i < fGeometry->GetSensorsN()/2; ++i ){
     TClonesArray* arr = new TClonesArray("TAMSDpoint");
     arr->SetOwner(true);
     fListOfPoints->AddAt(arr, i);
   }
   fListOfPoints->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuPoint::Clear(Option_t*)
{
  //	fListOfPoints->Delete();
  for (int i = 0; i < fGeometry->GetSensorsN()/2; ++i) {
    TClonesArray* list = GetListOfPoints(i);
    list->Delete();
  }

}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMSDntuPoint::ToStream(ostream& os, Option_t* option) const
{
   // for (Int_t i = 0; i < m_vtxGeo->GetNSensors(); ++i) {

	  // os << "TATWntuPoint " << GetName()
	  // << Form("  nPixels=%3d", GetPixelsN(i))
	  // << endl;

	  // //TODO properly
	  // //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
	  // for (Int_t j = 0; j < GetPixelsN(i); j++) {  // all by default
		 // const TATWpoint*  pixel = GetPixel(i,j, "all");
		 // if (pixel)
			// os << Form("%4d", pixel->GetPixelIndex());
		 // os << endl;
	  // }
   // }
}
