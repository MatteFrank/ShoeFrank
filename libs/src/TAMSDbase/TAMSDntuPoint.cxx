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
   m_layer(0),
   m_column(0),
   m_row(0),
   m_columnHitID(0),
   m_rowHitID(0),
   m_columnParticleID(0),
   m_rowParticleID(0),
   m_ParticleID(0),
   m_isMC(false),
   m_isTrueGhost(false),
   m_chargeZ(0),
   m_chargeZProba(0.)
{
}

//______________________________________________________________________________
//  build a point
TAMSDpoint::TAMSDpoint( int layer, double x, double y, TVector3 position )
: TAGcluster(),
   m_layer(layer),
   m_chargeZ(0),
   m_chargeZProba(0.)
{
   fPosition = position;
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
  m_geometry(0x0),
  m_listOfPoints(0x0)
{
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDntuPoint::~TAMSDntuPoint()
{
	delete m_listOfPoints;
}

//______________________________________________________________________________
//  standard
TAMSDpoint* TAMSDntuPoint::NewPoint( int iStation, double x, double y, TVector3 position ) {

  if ( iStation >= 0 && iStation < m_geometry->GetSensorsN()/2 ) {
    TClonesArray &pointArray = *GetListOfPoints(iStation);
    TAMSDpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSDpoint( iStation, x, y, position );
    return point;
  } else {
    cout << Form("Wrong sensor number %d\n", iStation);
    return 0x0;
  }
}

//------------------------------------------+-----------------------------------
int TAMSDntuPoint::GetPointN(int iStation) const
{
  if ( iStation >= 0 && iStation < m_geometry->GetSensorsN()/2 ) {
    TClonesArray* list = (TClonesArray*)m_listOfPoints->At(iStation);
    return list->GetEntries();
  } else return -1;
}



//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TAMSDpoint* TAMSDntuPoint::GetPoint(int iStation, int iPoint) const {

	if ( iPoint >= 0  && iPoint < GetPointN( iStation ) ) {
    TClonesArray* list = GetListOfPoints(iStation);
    return (TAMSDpoint*)list->At(iPoint);
	} else return 0x0;
}

//------------------------------------------+-----------------------------------
//! return number of points
TClonesArray* TAMSDntuPoint::GetListOfPoints(int iStation) const
{
   if (iStation >= 0  && iStation < m_geometry->GetSensorsN()/2) {
	  TClonesArray* list = (TClonesArray*)m_listOfPoints->At(iStation);
	  return list;
   } else return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TAMSDntuPoint::SetupClones()
{
  m_geometry = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();

   if (m_listOfPoints) return;
   m_listOfPoints = new TObjArray();
   for ( int i = 0; i < m_geometry->GetSensorsN()/2; ++i ){
     TClonesArray* arr = new TClonesArray("TAMSDpoint");
     arr->SetOwner(true);
     m_listOfPoints->AddAt(arr, i);
   }
   m_listOfPoints->SetOwner(true);
}





//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSDntuPoint::Clear(Option_t*)
{
  //	m_listOfPoints->Delete();
  for (int i = 0; i < m_geometry->GetSensorsN()/2; ++i) {
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
