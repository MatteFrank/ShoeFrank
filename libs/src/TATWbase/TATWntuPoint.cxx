
#include "TATWntuPoint.hxx"

#include "TString.h"
#include "TClonesArray.h"
#include "TATWparGeo.hxx"


ClassImp(TATWpoint) // Description of Single Detector TATWpoint


//______________________________________________________________________________
//  default constructor
TATWpoint::TATWpoint()
: TAGcluster(),
   m_position(),
   m_posErr(),
   m_positionG(),
   m_posErrG(),
   m_positionGlb(),
   m_posErrGlb(),
   m_row(-99),
   m_column(-99),
   m_rowHit(0x0),
   m_columnHit(0x0),
   m_de1(-99.),
   m_de2(-99.),
   m_time(-99.),
   m_tof1(-99.),
   m_tof2(-99.),
   m_layer(-99),
   m_id(-99),
   m_chargeZ(-99),
   m_chargeZProba(-99.)
{
}

//______________________________________________________________________________
//  build a point
TATWpoint::TATWpoint( double x, double dx, TATWntuHit* rowHit, double y, double dy, TATWntuHit* colHit, Int_t mainLayer )
: TAGcluster(),
   m_position(x, y, 0),
   m_posErr(dx, dy, 0),
   m_positionG(m_position),
   m_posErrG(m_posErr),
   m_positionGlb(-99, -99, -99),
   m_posErrGlb(m_posErr),
   m_rowHit(new TATWntuHit(*rowHit)),
   m_columnHit(new TATWntuHit(*colHit)),
   m_layer(mainLayer),
   m_id(-99),
   m_chargeZ(-99),
   m_chargeZProba(-99.)
{
   m_row = m_rowHit->GetBar();
   m_column    = m_columnHit->GetBar();
   
   m_de1    = m_rowHit->GetEnergyLoss();
   m_de2    = m_columnHit->GetEnergyLoss();
   m_tof1   = m_rowHit->GetTime();
   m_tof2   = m_columnHit->GetTime();
   m_time   = m_rowHit->GetTime();

   // assign to the point the matched MC track id if no Pile-Up, else for pile-up events assign -1
   if(m_layer==(Int_t)LayerX) {
       
     // if(m_rowHit->GetMcTracksN())
     if(m_rowHit->GetMcTracksN()==1)
       m_id     = m_rowHit->GetMcTrackIdx(0);
     else {
       m_id = -1; // pile-up
       // cout<<"NmctracksX::"<<m_rowHit->GetMcTracksN()<<endl;
     }
     
   } else {
     
     // if(m_columnHit->GetMcTracksN())
     if(m_columnHit->GetMcTracksN()==1)
       m_id     = m_columnHit->GetMcTrackIdx(0);
     else {
       m_id = -1;  // pile-up
       // cout<<"NmctracksY::"<<m_columnHit->GetMcTracksN()<<endl;
     }
   }
  
  Bool_t common = false;
  for (Int_t j = 0; j < m_columnHit->GetMcTracksN(); ++j) {
      Int_t idr = m_columnHit->GetMcTrackIdx(j);
      for (Int_t k = 0; k < m_rowHit->GetMcTracksN(); ++k) {
         Int_t idc = m_rowHit->GetMcTrackIdx(k);
        if (idr == idc) {
          AddMcTrackIdx(idr);
          common = true;
        }
      }
   }
  
  // in case mixing two or more different particles in a point
  if (!common) {
    map<int, int> mapIdx;
    for (Int_t j = 0; j < m_columnHit->GetMcTracksN(); ++j) {
      Int_t idr = m_columnHit->GetMcTrackIdx(j);
      mapIdx[idr] = 1;
    }
    
    for (Int_t k = 0; k < m_rowHit->GetMcTracksN(); ++k) {
      Int_t idc = m_rowHit->GetMcTrackIdx(k);
      mapIdx[idc] = 1;
    }
    
    for (map<int,int>::iterator it=mapIdx.begin(); it!=mapIdx.end(); ++it)
      AddMcTrackIdx(it->first);
  }
}


//______________________________________________________________________________
//
bool TATWpoint::IsValid() const
{
  return (m_columnHit->IsValid() && m_rowHit->IsValid());
}


//______________________________________________________________________________
//
void TATWpoint::SetPosition(TVector3& posLoc)
{
   m_position.SetXYZ(posLoc.X(), posLoc.Y(), posLoc.Z());
   m_posErr.SetXYZ(m_posErr.X(), m_posErr.Y(), 0.15);
}

//______________________________________________________________________________
//
void TATWpoint::SetPositionG(TVector3& posG)
{
   m_positionG.SetXYZ(posG.X(), posG.Y(), posG.Z());
   m_posErrG.SetXYZ(m_posErr.X(), m_posErr.Y(), 0.15);
}

//______________________________________________________________________________
//
void TATWpoint::SetPositionGlb(TVector3& posGlb)
{
   m_positionGlb.SetXYZ(posGlb.X(), posGlb.Y(), posGlb.Z());
   m_posErrGlb.SetXYZ(m_posErr.X(), m_posErr.Y(), 0.15);
}

//______________________________________________________________________________
// Clear
void TATWpoint::Clear(Option_t*)
{
   delete m_rowHit;
   delete m_columnHit;
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
TATWpoint* TATWntuPoint::NewPoint(double x, double dx, TATWntuHit* rowHit, double y, double dy, TATWntuHit* colHit, int mainLayer) {

	// check on aorigin
  TClonesArray &pointArray = *m_listOfPoints;
  TATWpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TATWpoint( x, dx, rowHit, y, dy, colHit, mainLayer);
  point->SetClusterIdx(pointArray.GetEntriesFast()-1);
  
  return point;
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





























