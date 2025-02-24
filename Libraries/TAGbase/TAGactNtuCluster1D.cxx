/*!
 \file TAGactNtuCluster1D.cxx
 \brief   Implementation of TAGactNtuCluster1D.
 */

#include "TAGactNtuCluster1D.hxx"

/*!
 \class TAGactNtuCluster1D 
 \brief General class for 1D clustering **
 */

//! Class Imp
ClassImp(TAGactNtuCluster1D);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name  name
//! \param[in] title  title name
TAGactNtuCluster1D::TAGactNtuCluster1D(const char* name, 
											 const char* title )
: TAGaction(name, title)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuCluster1D::~TAGactNtuCluster1D()
{
}

//______________________________________________________________________________
//! Fill maps
//!
//! \param[in] line line number
//! \param[in] idx cluster index
void TAGactNtuCluster1D::FillMaps(Int_t line, Int_t idx)
{
   fPixelMap[line] = 1;
   fIndexMap[line] = idx;
}

//______________________________________________________________________________
//! Shape cluster
//!
//! \param[in] noClus cluster number
//! \param[in] IndX index
Bool_t TAGactNtuCluster1D::ShapeCluster(Int_t noClus, Int_t IndX)
{
   Int_t idx = IndX;
   if ( fPixelMap[idx] <= 0 ) return false;
   if ( fFlagMap[idx] != -1 ) return false;
   fFlagMap[idx] = noClus;
   
   TAGobject* pixel = GetHitObject(fIndexMap[idx]);
   pixel->SetFound(true);
   
   for(Int_t i = -1; i <= 1 ; ++i)
      if (CheckLine(IndX+i))
         ShapeCluster(noClus, IndX+i);
   
   return true;
}

//______________________________________________________________________________
//! Check line number
//!
//! \param[in] idx line number
Bool_t TAGactNtuCluster1D::CheckLine(Int_t idx)
{
   Int_t nCol = fDimX;
   
   if (idx >= nCol || idx < 0) return false;
   
   return true;
}

//______________________________________________________________________________
//! Set up map size
//!
//! \param[in] size map size
void TAGactNtuCluster1D::SetupMaps(Int_t size)
{
   fFlagMap.Set(size);
}

//______________________________________________________________________________
//! Clear maps
void TAGactNtuCluster1D::ClearMaps()
{
   fPixelMap.clear();
   fIndexMap.clear();
   fFlagMap.Reset(-1);
}

