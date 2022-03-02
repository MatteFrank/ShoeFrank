/*!
 \file TAGactNtuCluster2D.cxx
 \brief   Implementation of TAGactNtuCluster2D.
 */

#include "TAGactNtuCluster2D.hxx"

/*!
 \class TAGactNtuCluster2D 
 \brief General class for 2D clustering **
 */

//! Class Imp
ClassImp(TAGactNtuCluster2D);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name  name
//! \param[in] title  title name
TAGactNtuCluster2D::TAGactNtuCluster2D(const char* name, 
											 const char* title )
: TAGaction(name, title),
  fFlagSize(-1)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuCluster2D::~TAGactNtuCluster2D()
{
   delete[] fFlagMap;
}

//______________________________________________________________________________
//! Fill maps
//!
//! \param[in] line line number
//! \param[in] col column number
//! \param[in] idx cluster index
void TAGactNtuCluster2D::FillMaps(Int_t line, Int_t col, Int_t idx)
{
   fPixelMap[line*fDimX+col] = 1;
   fIndexMap[line*fDimX+col] = idx;
}

//______________________________________________________________________________
//! Shape cluster
//!
//! \param[in] noClus cluster number
//! \param[in] IndX index in X
//! \param[in] IndY index in Y
Bool_t TAGactNtuCluster2D::ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY)
{
   Int_t idx = IndX*fDimX+IndY;
   if ( fPixelMap[idx] <= 0 ) return false;
   if ( fFlagMap[idx] != -1 ) return false;
   fFlagMap[idx] = noClus;
   
   TAGobject* pixel = GetHitObject(fIndexMap[idx]);
   pixel->SetFound(true);
   
   for(Int_t i = -1; i <= 1 ; ++i)
      if (CheckLine(IndX+i))
         ShapeCluster(noClus, IndX+i, IndY);
   
   for(Int_t j = -1; j <= 1 ; ++j)
      if (CheckCol(IndY+j))
         ShapeCluster(noClus, IndX  , IndY+j);
   
   return true;
}

//______________________________________________________________________________
//! Check line number
//!
//! \param[in] idx line number
Bool_t TAGactNtuCluster2D::CheckLine(Int_t idx)
{
   Int_t nLine = fDimY;
 
   if (idx >= nLine || idx < 0) return false;

   return true;
}

//______________________________________________________________________________
//! Check column number
//!
//! \param[in] idx column number
Bool_t TAGactNtuCluster2D::CheckCol(Int_t idx)
{
   Int_t nCol = fDimX;
   
   if (idx >= nCol || idx < 0) return false;
   
   return true;
}

//______________________________________________________________________________
//! Set up map size
//!
//! \param[in] size map size
void TAGactNtuCluster2D::SetupMaps(Int_t size)
{
   fFlagSize = size;
   fFlagMap  = new Int_t[size];
}

//______________________________________________________________________________
//! Clear maps
void TAGactNtuCluster2D::ClearMaps()
{
   fPixelMap.clear();
   fIndexMap.clear();
   memset(fFlagMap, -1, fFlagSize*sizeof(Int_t));
}

