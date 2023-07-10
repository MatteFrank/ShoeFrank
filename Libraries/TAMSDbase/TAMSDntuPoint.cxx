/*!
 \file TAMSDntuPoint.cxx
 \brief   Implementation of TAMSDntuPoint.
 */

#include "TAMSDntuPoint.hxx"
#include "TString.h"
#include "TClonesArray.h"
#include "TAGnameManager.hxx"
#include "TAMSDparGeo.hxx"
#include "TAMSDntuPoint.hxx"
#include "TAMSDtrack.hxx"

//! Class Imp
ClassImp(TAMSDpoint) // Description of Single Detector TAMSDpoint

//______________________________________________________________________________
//! Default constructor
TAMSDpoint::TAMSDpoint()
: TAGcluster(),
   fStation(0),
   fDe1(0),
   fDe2(0),
   fTime(0),
   fChargeZ(0),
   fChargeZProba(0.)
{
}

//______________________________________________________________________________
//! New point for copy constructor
//!
//! \param[in] point point to copy
TAMSDpoint::TAMSDpoint(const TAMSDpoint& point)
:  TAGcluster(point),
   fStation(point.fStation),
   fDe1(point.fDe1),
   fDe2(point.fDe2),
   fTime(point.fTime),
   fChargeZ(point.fChargeZ),
   fChargeZProba(point.fChargeZProba)
{
}

//______________________________________________________________________________
//! New point
//!
//! \param[in] layer station id
//! \param[in] x x position
//! \param[in] y y position
//! \param[in] position position
TAMSDpoint::TAMSDpoint( int layer, double x, double y, TVector3 position )
 : TAGcluster(),
   fStation(layer),
   fChargeZ(0),
   fChargeZProba(0.)
{
   fPosition1 = position;
}

//______________________________________________________________________________
//! New point
//!
//! \param[in] layer layer id
//! \param[in] x position in x
//! \param[in] dx error position in x
//! \param[in] clusX cluster in X plane
//! \param[in] y position in y
//! \param[in] dy error position in y
//! \param[in] clusY cluster in Y plane
TAMSDpoint::TAMSDpoint(Int_t layer, TAMSDcluster* clusX, TAMSDcluster* clusY)
 : TAGcluster(),
   fStation(layer),
   fChargeZ(0),
   fChargeZProba(0.)
{
   Double_t y1  = clusY->GetPosition().X() + clusY->GetPosition().Y();
   Double_t dy1 = clusY->GetPosError().X() + clusY->GetPosError().Y();
   
   Double_t x1  = clusX->GetPosition().X() + clusX->GetPosition().Y();
   Double_t dx1 = clusX->GetPosError().X() + clusX->GetPosError().Y();

   fPosition1.SetXYZ(x1,y1,0);
   fPosError1.SetXYZ(dx1,dy1,0);
      
   fDe1 = clusX->GetEnergyLoss();
   fDe2 = clusY->GetEnergyLoss();

   Bool_t common = false;
   for (Int_t j = 0; j < clusX->GetMcTracksN(); ++j) {
      Int_t idr = clusX->GetMcTrackIdx(j);
      for (Int_t k = 0; k < clusY->GetMcTracksN(); ++k) {
         Int_t idc = clusY->GetMcTrackIdx(k);
         if (idr == idc) {
            AddMcTrackIdx(idr);
            common = true;
         }
      }
   }
   
   // in case mixing two or more different particles in a point
   if (!common) {
      map<int, int> mapIdx;
      for (Int_t j = 0; j < clusX->GetMcTracksN(); ++j) {
         Int_t idr = clusX->GetMcTrackIdx(j);
         mapIdx[idr] = 1;
      }
      
      for (Int_t k = 0; k < clusY->GetMcTracksN(); ++k) {
         Int_t idc = clusY->GetMcTrackIdx(k);
         mapIdx[idc] = 1;
      }
      
      for (map<int,int>::iterator it=mapIdx.begin(); it!=mapIdx.end(); ++it)
         AddMcTrackIdx(it->first);
   }
   
   fElementsN = clusX->GetElementsN() + clusY->GetElementsN();
}

//______________________________________________________________________________
//! Clear
void TAMSDpoint::Clear(Option_t*)
{
}

//! Class imp
ClassImp(TAMSDntuPoint);

//##############################################################################

//------------------------------------------+-----------------------------------
//! Default detector
TAMSDntuPoint::TAMSDntuPoint(Int_t stationsN)
 : TAGdata(),
   fStationsN(stationsN),
   fListOfPoints(0x0)
{
   if (stationsN == 0) {
      Warning("TAMSDntuPoint()", "Size of hit array not set, set to %d\n", TAMSDparGeo::GetDefSensorsN());
      fStationsN = TAMSDparGeo::GetDefSensorsN();
   }
   
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuPoint::~TAMSDntuPoint()
{
	delete fListOfPoints;
}

//______________________________________________________________________________
//! New point
//!
//! \param[in] iStation station id
//! \param[in] x x position
//! \param[in] y y position
//! \param[in] position position
TAMSDpoint* TAMSDntuPoint::NewPoint( int iStation, double x, double y, TVector3 position )
{
  if ( iStation >= 0 && iStation < fStationsN ) {
    TClonesArray &pointArray = *GetListOfPoints(iStation);
    TAMSDpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint( iStation, x, y, position );
    return point;
  } else {
    Error("NewPoint()", "Wrong station number %d\n", iStation);
    return 0x0;
  }
}

//______________________________________________________________________________
//! New point
//!
//! \param[in] iStation station id
//! \param[in] x position in x
//! \param[in] dx error position in x
//! \param[in] clusX cluster in X plane
//! \param[in] y position in y
//! \param[in] dy error position in y
//! \param[in] clusY cluster in Y plane
TAMSDpoint* TAMSDntuPoint::NewPoint(Int_t iStation, TAMSDcluster* clusX, TAMSDcluster* clusY)
{
   if ( iStation >= 0 && iStation < fStationsN ) {
      TClonesArray &pointArray = *GetListOfPoints(iStation);
      TAMSDpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint( iStation, clusX, clusY);
      return point;
   } else {
      Error("NewPoint()", "Wrong station number %d\n", iStation);
      return 0x0;
   }
}

//______________________________________________________________________________
//! New point for copy constructor
//!
//! \param[in] point point to copy
//! \param[in] iStation station id
TAMSDpoint* TAMSDntuPoint::NewPoint(TAMSDpoint* point, Int_t iStation)
{
   if (iStation >= 0  && iStation < fStationsN) {
      TClonesArray &pointArray = *GetListOfPoints(iStation);
      TAMSDpoint* pt = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint(*point);
      pt->SetClusterIdx(pointArray.GetEntriesFast()-1);
      return pt;
   } else {
      Error("NewPoint()", "Wrong station number %d\n", iStation);
      return 0x0;
   }
}

//------------------------------------------+-----------------------------------
//! Get nunber of points
//!
//! \param[in] iStation station id
int TAMSDntuPoint::GetPointsN(int iStation) const
{
  if ( iStation >= 0 && iStation < fStationsN ) {
    TClonesArray* list = (TClonesArray*)fListOfPoints->At(iStation);
    return list->GetEntriesFast();
  } else return -1;
}

//------------------------------------------+-----------------------------------
//! Get point
//!
//! \param[in] iStation station id
//! \param[in] iPoint point index
TAMSDpoint* TAMSDntuPoint::GetPoint(int iStation, int iPoint) const
{
	if ( iPoint >= 0  && iPoint < GetPointsN( iStation ) ) {
    TClonesArray* list = GetListOfPoints(iStation);
    return (TAMSDpoint*)list->At(iPoint);
	} else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! Get list of point
//!
//! \param[in] iStation station id
TClonesArray* TAMSDntuPoint::GetListOfPoints(int iStation) const
{
   if (iStation >= 0  && iStation < fStationsN) {
	  TClonesArray* list = (TClonesArray*)fListOfPoints->At(iStation);
	  return list;
   } else return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones. Create and initialise the list of points
void TAMSDntuPoint::SetupClones()
{
   if (fListOfPoints) return;
   fListOfPoints = new TObjArray(fStationsN);
   for ( int i = 0; i < fStationsN; ++i ){
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
  for (int i = 0; i < fStationsN; ++i) {
    TClonesArray* list = GetListOfPoints(i);
    list->Delete();
  }

}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
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
