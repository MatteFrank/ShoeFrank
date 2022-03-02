
#include "TATWntuPoint.hxx"

#include "TString.h"
#include "TClonesArray.h"
#include "TATWparGeo.hxx"


ClassImp(TATWpoint) // Description of Single Detector TATWpoint


//______________________________________________________________________________
//  default constructor
TATWpoint::TATWpoint()
 : TAGcluster(),
   fPositionGlb(),
   fPosErrGlb(),
   fRow(-99),
   fColumn(-99),
   fRowHit(0x0),
   fColumnHit(0x0),
   fDe1(-99.),
   fDe2(-99.),
   fToF(-99.),
   fMainEloss(-99),
   fTof1(-99.),
   fTof2(-99.),
   fMatchCalIdx(-1),
   fLayer(-99),
   fId(-99),
   fChargeZ(-99),
   fChargeZProba(-99.)
{
}

//______________________________________________________________________________
//  build a point
TATWpoint::TATWpoint( Double_t x, Double_t dx, TATWhit* rowHit, Double_t y, Double_t dy, TATWhit* colHit, Int_t mainLayer )
 : TAGcluster(),
   fPositionGlb(-99, -99, -99),
   fRowHit(new TATWhit(*rowHit)),
   fColumnHit(new TATWhit(*colHit)),
   fMatchCalIdx(-1),
   fLayer(mainLayer),
   fId(-99),
   fChargeZ(-99),
   fChargeZProba(-99.),
   fToF(-99),
   fMainEloss(-99)
{
   
   fPosition1.SetXYZ(x, y, 0),
   fPosError1.SetXYZ(dx, dy, 0),
   fPosition2.SetXYZ(x, y, 0);
   fPosError2.SetXYZ(dx, dy, 0);
   fPosErrGlb = fPosError1;

   fRow    = fRowHit->GetBar();
   fColumn = fColumnHit->GetBar();
   
   fDe1    = fRowHit->GetEnergyLoss();
   fDe2    = fColumnHit->GetEnergyLoss();
   fTof1   = fRowHit->GetToF();
   fTof2   = fColumnHit->GetToF();

   // // assign to the point the matched MC track id if no Pile-Up, else for pile-up events assign -1
   // if(fLayer==(Int_t)LayerX) {
       
   //   if(fRowHit->GetMcTracksN()==1)
   //     fId     = fRowHit->GetMcTrackIdx(0);
   //   else {
   //     fId = -1; // pile-up
   //     // cout<<"NmctracksX::"<<fRowHit->GetMcTracksN()<<endl;
   //   }
     
   // } else {
   //    if(fColumnHit->GetMcTracksN()==1)
   //     fId     = fColumnHit->GetMcTrackIdx(0);
   //   else {
   //     fId = -1;  // pile-up
   //     // cout<<"NmctracksY::"<<fColumnHit->GetMcTracksN()<<endl;
   //   }
   // }
  
  Bool_t common = false;
  for (Int_t j = 0; j < fColumnHit->GetMcTracksN(); ++j) {
      Int_t idr = fColumnHit->GetMcTrackIdx(j);
      for (Int_t k = 0; k < fRowHit->GetMcTracksN(); ++k) {
         Int_t idc = fRowHit->GetMcTrackIdx(k);
        if (idr == idc) {
          AddMcTrackIdx(idr);
          common = true;
        }
      }
   }
  
  // in case mixing two or more different particles in a point
  if (!common) {
    map<int, int> mapIdx;
    for (Int_t j = 0; j < fColumnHit->GetMcTracksN(); ++j) {
      Int_t idr = fColumnHit->GetMcTrackIdx(j);
      mapIdx[idr] = 1;
    }
    
    for (Int_t k = 0; k < fRowHit->GetMcTracksN(); ++k) {
      Int_t idc = fRowHit->GetMcTrackIdx(k);
      mapIdx[idc] = 1;
    }
    
    for (map<int,int>::iterator it=mapIdx.begin(); it!=mapIdx.end(); ++it)
      AddMcTrackIdx(it->first);
  }
}

//______________________________________________________________________________
//
Bool_t TATWpoint::IsValid() const
{
  return (fColumnHit->IsValid() && fRowHit->IsValid());
}

//______________________________________________________________________________
//
void TATWpoint::SetPosition(TVector3& posLoc)
{
   fPosition1.SetXYZ(posLoc.X(), posLoc.Y(), posLoc.Z());
   fPosError1.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.15);
}

//______________________________________________________________________________
//
void TATWpoint::SetPositionG(TVector3& posG)
{
   fPosition2.SetXYZ(posG.X(), posG.Y(), posG.Z());
   fPosError2.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.15);
}

//______________________________________________________________________________
//
void TATWpoint::SetPositionGlb(TVector3& posGlb)
{
   fPositionGlb.SetXYZ(posGlb.X(), posGlb.Y(), posGlb.Z());
   fPosErrGlb.SetXYZ(fPosError1.X(), fPosError1.Y(), 0.15);
}

//______________________________________________________________________________
// Clear
void TATWpoint::Clear(Option_t*)
{
   delete fRowHit;
   delete fColumnHit;
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
  fListOfPoints(0x0)
{
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWntuPoint::~TATWntuPoint()
{
	delete fListOfPoints;
}

//______________________________________________________________________________
//  standard 
TATWpoint* TATWntuPoint::NewPoint(Double_t x, Double_t dx, TATWhit* rowHit, Double_t y, Double_t dy, TATWhit* colHit, Int_t mainLayer) {

	// check on aorigin
  TClonesArray &pointArray = *fListOfPoints;
  TATWpoint* point = new(pointArray[pointArray.GetEntriesFast()]) TATWpoint( x, dx, rowHit, y, dy, colHit, mainLayer);
  point->SetClusterIdx(pointArray.GetEntriesFast()-1);
  
  return point;
}

//------------------------------------------+-----------------------------------
Int_t TATWntuPoint::GetPointsN() const
{
	return fListOfPoints->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWpoint* TATWntuPoint::GetPoint(Int_t iPoint) const {

	if ( iPoint < 0  || iPoint >= GetPointsN() ) {
		cout << "ERROR >> TATWntuPoint::GetPoint_includingDuplicates  -->  number of point "<<iPoint<<" required is wrong. Max num  " << GetPointsN() << endl;
        exit(0);
	}

	return (TATWpoint*)fListOfPoints->At(iPoint);
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TATWntuPoint::SetupClones()
{
   if (fListOfPoints) return;
   fListOfPoints = new TClonesArray("TATWpoint", 500);
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TATWntuPoint::Clear(Option_t*)
{
	fListOfPoints->Delete();
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





























