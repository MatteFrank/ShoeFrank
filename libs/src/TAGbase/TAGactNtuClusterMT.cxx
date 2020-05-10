/*!
 \file
 \version $Id: TAGactNtuClusterMT.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAGactNtuClusterMT.
 */

#include "TAGactNtuClusterMT.hxx"

/*!
 \class TAGactNtuClusterMT
 \brief General class for 2D clustering **
 */

ClassImp(TAGactNtuClusterMT);

Int_t TAGactNtuClusterMT::fgMaxThread = 4;

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactNtuClusterMT::TAGactNtuClusterMT(const char* name,
											 const char* title )
: TAGaction(name, title)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuClusterMT::~TAGactNtuClusterMT()
{
}

//______________________________________________________________________________
//
void TAGactNtuClusterMT::FillMaps(Int_t line, Int_t col, Int_t idx, Int_t thr)
{
   fPixelMap[thr][line*fDimX+col] = 1;
   fIndexMap[thr][line*fDimX+col] = idx;
}

//______________________________________________________________________________
//
Bool_t TAGactNtuClusterMT::ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY, TClonesArray* listOfPixels, Int_t thr)
{
   Int_t idx = IndX*fDimX+IndY;
   if ( fPixelMap[thr][idx] <= 0 ) return false;
   if ( fFlagMap[thr][idx] != -1 ) return false;
   fFlagMap[thr][idx] = noClus;
   
   TAGobject* pixel = GetHitObject(fIndexMap[thr][idx], listOfPixels);
   pixel->SetFound(true);
   
   for(Int_t i = -1; i <= 1 ; ++i)
      if (CheckLine(IndX+i))
         ShapeCluster(noClus, IndX+i, IndY, listOfPixels, thr);
   
   for(Int_t j = -1; j <= 1 ; ++j)
      if (CheckCol(IndY+j))
         ShapeCluster(noClus, IndX  , IndY+j, listOfPixels, thr);
   
   return true;
}

//______________________________________________________________________________
//
Bool_t TAGactNtuClusterMT::CheckLine(Int_t idx)
{
   Int_t nLine = fDimY;
 
   if (idx >= nLine || idx < 0) return false;

   return true;
}

//______________________________________________________________________________
//
Bool_t TAGactNtuClusterMT::CheckCol(Int_t idx)
{
   Int_t nCol = fDimX;
   
   if (idx >= nCol || idx < 0) return false;
   
   return true;
}

//______________________________________________________________________________
//
void TAGactNtuClusterMT::SetupMaps(Int_t size, Int_t thr)
{
   fFlagMap[thr].Set(size);
}

//______________________________________________________________________________
//
void TAGactNtuClusterMT::ClearMaps(Int_t thr)
{
   fPixelMap[thr].clear();
   fIndexMap[thr].clear();
   fFlagMap[thr].Reset(-1);
}

