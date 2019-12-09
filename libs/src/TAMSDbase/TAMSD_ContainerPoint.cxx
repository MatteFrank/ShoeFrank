/*!
  \file
  \version $Id: TAMSD_ContainerPoint.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TAMSD_ContainerPoint.
*/


/*!
  \class TAMSD_ContainerPoint TAMSD_ContainerPoint.hxx "TAMSD_ContainerPoint.hxx"
  \brief Container class for VTX ntu hit **
*/

////////////////////////////////////////////////////////////
// Class Description of TAMSD_Point                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TString.h"
#include "TClonesArray.h"

#include "TAMSD_ContainerPoint.hxx"




ClassImp(TAMSD_ContainerPoint);
TString TAMSD_ContainerPoint::fgkBranchName   = "PointContainer.";




//------------------------------------------+-----------------------------------
//! 
TAMSD_ContainerPoint::TAMSD_ContainerPoint() 
: TAGdata(),
  m_listOfPoints(0x0)
{
	m_msdGeo = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
	SetupClones();
}





//------------------------------------------+-----------------------------------
//! Destructor.
TAMSD_ContainerPoint::~TAMSD_ContainerPoint() {
	delete m_listOfPoints;
	m_pointVector.clear();
	// m_degeneratePointMap.clear();
}







//______________________________________________________________________________
//  standard 
TAMSD_Point* TAMSD_ContainerPoint::NewPoint( int layer, int iCol, int iRow, TVector3 pos ) {

	// get layer
	// if ( colHit->GetLayer() == rowHit->GetLayer() ) 		cout << "ERROR :: TAMSD_ContainerPoint::NewPoint  -->  colum and row layer doesn't match during point creation!" << endl, exit(0);
	// layer = rowHit->GetLayer();

	// check on aorigin
	TClonesArray &pointArray = *GetListOfPoints(layer);
	TAMSD_Point* point = new(pointArray[pointArray.GetEntriesFast()]) TAMSD_Point( layer, iCol, iRow, pos );

	// da sistemare!!

	// int mask = AlgoLinearizeCoord( layer, colHit->GetStrip(), rowHit->GetStrip() );

	// // fill DegeneratePoint map
	// if ( m_degeneratePointMap.find( mask ) == m_degeneratePointMap.end() ) {	// if element doesn't exist
	// 	// m_degeneratePointMap[mask].SetPrimaryPoint( point );
	// 	m_degeneratePointMap[mask].primaryPoint = point;
	// 	m_pointVector.push_back( mask );

	// }
	// else if ( !point->IsTrueGhost() ) {
	// 	// m_degeneratePointMap[mask].SetPrimaryPoint( point );
	// 	m_degeneratePointMap[mask].primaryPoint = point;

	// }
	// // m_degeneratePointMap[mask].AddPoint( point );
	// m_degeneratePointMap[mask].allPoints.push_back( point );

	return point;
}




//------------------------------------------+-----------------------------------
TClonesArray* TAMSD_ContainerPoint::GetListOfPoints(int iSensor)    {

    if (iSensor >= 0  && iSensor < m_msdGeo->GetNLayers()) {
        TClonesArray* list = (TClonesArray*)m_listOfPoints->At(iSensor);
        return list;
    } else {
        Error("GetListOfPoints()", "Wrong sensor number %d\n", iSensor);
        return 0x0;
    }   
}




// //------------------------------------------+-----------------------------------
// int TAMSD_ContainerPoint::AlgoLinearizeCoord( int layer, int col, int row ) {
// 	return (layer * m_msdGeo->GetNStrip() * m_msdGeo->GetNStrip() ) + (col * m_msdGeo->GetNStrip()) + row;
// }




// //------------------------------------------+-----------------------------------
// void TAMSD_ContainerPoint::AlgoLinearizeCoord_inverse( int mask, int* layer, int* col, int* row ) {
// 	*layer = trunc( mask / (m_msdGeo->GetNStrip() * m_msdGeo->GetNStrip()) );
// 	*col = trunc( (mask - *layer) / m_msdGeo->GetNStrip() );
// 	*row = trunc( mask - *layer - *col );
// }





//------------------------------------------+-----------------------------------
int TAMSD_ContainerPoint::GetPointN() { 
	// DegeneratePoint dimension
	GetPointN_includingGhosts();
	// return m_degeneratePointMap.size();
}



// //------------------------------------------+-----------------------------------
// //! return a point for a given sensor
// TAMSD_Point* TAMSD_ContainerPoint::GetPoint( int iPoint ) {

// 	if ( m_degeneratePointMap.size() != m_pointVector.size() ) 	{	// sanity check
// 		cout << "ERROR >> TAMSD_ContainerPoint::GetPoint  -->  different size of m_degeneratePointMap "<<m_degeneratePointMap.size()<<" and m_pointVector.size() " << m_pointVector.size() << endl;
//         exit(0);
// 	}

// 	if ( iPoint < 0  || iPoint >= GetPointN() ) {
// 		cout << "ERROR >> TAMSD_ContainerPoint::GetPoint  -->  number of point "<<iPoint<<" required is wrong. Max num  " << m_degeneratePointMap.size() << endl;
//         exit(0);
// 	}

// 	// from DegeneratePoint
// 	// return m_degeneratePointMap[ m_pointVector.at(iPoint) ].GetPrimaryPoint();
// 	return m_degeneratePointMap[ m_pointVector.at(iPoint) ].primaryPoint;
// }




// //------------------------------------------+-----------------------------------
// //! return a point for a given sensor
// vector<TAMSD_Point*> TAMSD_ContainerPoint::GetPoint_AllTheDegenerate( int iPoint ) {

// 	if ( m_degeneratePointMap.size() != m_pointVector.size() ) 	{	// sanity check
// 		cout << "ERROR >> TAMSD_ContainerPoint::GetPoint  -->  different size of m_degeneratePointMap "<<m_degeneratePointMap.size()<<" and m_pointVector.size() " << m_pointVector.size() << endl;
//         exit(0);
// 	}

// 	if ( iPoint < 0  || iPoint >= GetPointN() ) {
// 		cout << "ERROR >> TAMSD_ContainerPoint::GetPoint  -->  number of point "<<iPoint<<" required is wrong. Max num  " << m_degeneratePointMap.size() << endl;
//         exit(0);
// 	}

// 	// from DegeneratePoint
// 	// return m_degeneratePointMap[ m_pointVector.at(iPoint) ].GetAllPoints();
// 	return m_degeneratePointMap[ m_pointVector.at(iPoint) ].allPoints;
// }




//------------------------------------------+-----------------------------------
int TAMSD_ContainerPoint::GetPointN_includingGhosts() {
	return m_listOfPoints->GetEntries();   
}



//------------------------------------------+-----------------------------------
//! return a point for a given sensor
// TAMSD_Point* TAMSD_ContainerPoint::GetPoint_includingGhosts( int iPoint ) {
TAMSD_Point* TAMSD_ContainerPoint::GetPoint( int iPoint ) {

	if ( iPoint < 0  || iPoint >= GetPointN_includingGhosts() ) {
		cout << "ERROR --> TAMSD_ContainerPoint::GetPoint_includingGhosts  -->  number of point "<<iPoint<<" required is wrong. Max num  " << GetPointN_includingGhosts() << endl;
        exit(0);
	}

	return (TAMSD_Point*)m_listOfPoints->At( iPoint );
}



//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of points
void TAMSD_ContainerPoint::SetupClones()	{

   if (m_listOfPoints) return;

   m_listOfPoints = new TObjArray( m_msdGeo->GetNLayers() );
   
   for (Int_t i = 0; i < m_msdGeo->GetNLayers(); ++i) {
      TClonesArray* arr = new TClonesArray("TAMSD_Point", 500);
      arr->SetOwner(true);
      m_listOfPoints->AddAt(arr, i);
   }
   m_listOfPoints->SetOwner(true);
}





//------------------------------------------+-----------------------------------
//! Clear event.
void TAMSD_ContainerPoint::Clear(Option_t*) {

	for (Int_t i = 0; i < m_msdGeo->GetNLayers(); ++i) {
		TClonesArray* list = GetListOfPoints(i);
		list->Delete();   
		list->Clear();
	}
	// m_listOfPoints->Clear("C");
	m_pointVector.clear();
	// m_degeneratePointMap.clear();
}





/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAMSD_ContainerPoint::ToStream(ostream& os, Option_t* option) const
{
   // for (Int_t i = 0; i < m_vtxGeo->GetNLayers(); ++i) {
	  
	  // os << "TAMSD_ContainerPoint " << GetName()
	  // << Form("  npoints=%3d", GetpointsN(i))
	  // << endl;
	  
	  // //TODO properly
	  // //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
	  // for (Int_t j = 0; j < GetpointsN(i); j++) {  // all by default
		 // const TAMSD_Point*  point = Getpoint(i,j, "all");
		 // if (point)
			// os << Form("%4d", point->GetpointIndex());
		 // os << endl;
	  // }
   // }
}





























