



#include "TATWntuPoint.hxx"


#include "TString.h"
#include "TClonesArray.h"
#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"



ClassImp(TATWpoint) // Description of Single Detector TATWpoint


//______________________________________________________________________________
//  default constructor
TATWpoint::TATWpoint()
: TAGcluster(),
   m_position(),
   m_posErr(),
   m_positionG(),
   m_posErrG(),
   m_column(0),
   m_row(0),
   m_columnHit(0x0),
   m_rowHit(0x0),
   m_de1(0.),
   m_de2(0.),
   m_time(0.),
   m_chargeZ(0),
   m_chargeZProba(0.)
{
}

//______________________________________________________________________________
//  build a point
TATWpoint::TATWpoint( double x, double dx, TATWntuHit* colHit, double y, double dy, TATWntuHit* rowHit )
: TAGcluster(),
   m_position(x, y, 0),
   m_posErr(dx, dy, 0),
   m_columnHit(new TATWntuHit(*colHit)),
   m_rowHit(new TATWntuHit(*rowHit)),
   m_chargeZ(0),
   m_chargeZProba(0.)
{
   m_column = m_columnHit->GetBar();
   m_row    = m_rowHit->GetBar();
   
   m_de1    = m_columnHit->GetEnergyLoss();
   m_de2    = m_rowHit->GetEnergyLoss();
   m_time   = m_columnHit->GetTime();
   
   for (Int_t j = 0; j < m_rowHit->GetMcTracksN(); ++j) {
      Int_t idr = m_rowHit->GetMcTrackIdx(j);
      for (Int_t k = 0; k < m_columnHit->GetMcTracksN(); ++k) {
         Int_t idc = m_columnHit->GetMcTrackIdx(k);
         if (idr == idc)
            AddMcTrackIdx(idr);
      }
   }
}

//______________________________________________________________________________
//
void TATWpoint::SetPositionG(TVector3& posGlo)
{
   m_positionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   m_posErrG.SetXYZ(m_posErr.X(), m_posErr.Y(), 0.1);
}


//______________________________________________________________________________
// Clear
void TATWpoint::Clear(Option_t*)
{
   delete m_columnHit;
   delete m_rowHit;
}


/*!
  \file
  \version $Id: TATWntuPoint.cxx
  \brief   Implementation of TATWntuPoint.
*/

ClassImp(TATWntuPoint);

//##############################################################################

TString TATWntuPoint::fgkBranchName   = "twpt.";

//------------------------------------------+-----------------------------------
//! 
TATWntuPoint::TATWntuPoint() 
: TAGdata(),
  m_listOfPoints(0x0)
{
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWntuPoint::~TATWntuPoint()
{
	delete m_listOfPoints;
}

//______________________________________________________________________________
//  standard 
TATWpoint* TATWntuPoint::NewPoint(double x, double dx, TATWntuHit* colHit, double y, double dy, TATWntuHit* rowHit ) {

	// check on aorigin
	TClonesArray &pixelArray = *m_listOfPoints;
	TATWpoint* pixel = new(pixelArray[pixelArray.GetEntriesFast()]) TATWpoint( x, dx, colHit, y, dy, rowHit );

	return pixel;
}

//------------------------------------------+-----------------------------------
int TATWntuPoint::GetPointN() const
{
	return m_listOfPoints->GetEntries();
}



//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWpoint* TATWntuPoint::GetPoint(int iPoint) const {

	if ( iPoint < 0  || iPoint >= GetPointN() ) {
		cout << "ERROR >> TATWntuPoint::GetPoint_includingDuplicates  -->  number of point "<<iPoint<<" required is wrong. Max num  " << GetPointN() << endl;
        exit(0);
	}

	return (TATWpoint*)m_listOfPoints->At(iPoint);
}



//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TATWntuPoint::SetupClones()
{
   if (m_listOfPoints) return;
   m_listOfPoints = new TClonesArray("TATWpoint", 500);
}





//------------------------------------------+-----------------------------------
//! Clear event.
void TATWntuPoint::Clear(Option_t*)
{
	m_listOfPoints->Delete();
}





/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TATWntuPoint::ToStream(ostream& os, Option_t* option) const
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





























