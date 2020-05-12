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
map<Int_t, Int_t>& TAGactNtuClusterMT::GetPixelMap(Int_t thr)
{
   if (thr == 0) return fPixelMap0;
   else if (thr == 1) return fPixelMap1;
   else if (thr == 2) return fPixelMap2;
   else if (thr == 3) return fPixelMap3;

   else
      printf("No pixel map for %d thread, put thread 0\n", thr);
   
   return fPixelMap0;
      
}

//______________________________________________________________________________
//
map<Int_t, Int_t>& TAGactNtuClusterMT::GetIndexMap(Int_t thr)
{
   if (thr == 0) return fIndexMap0;
   else if (thr == 1) return fIndexMap1;
   else if (thr == 2) return fIndexMap2;
   else if (thr == 3) return fIndexMap3;
   
   else
      printf("No index map for %d thread, put thread 0\n", thr);
   
   return fIndexMap0;
}

//______________________________________________________________________________
//
TArrayI&  TAGactNtuClusterMT::GetFlagMap(Int_t thr)
{
   if (thr == 0) return fFlagMap0;
   else if (thr == 1) return fFlagMap1;
   else if (thr == 2) return fFlagMap2;
   else if (thr == 3) return fFlagMap3;
   
   else
      printf("No index map for %d thread, put thread 0\n", thr);
   
   return fFlagMap0;
}


//______________________________________________________________________________
//
void TAGactNtuClusterMT::FillMaps(Int_t line, Int_t col, Int_t idx, Int_t thr)
{
   GetPixelMap(thr)[line*fDimX+col] = 1;
   GetIndexMap(thr)[line*fDimX+col] = idx;
}

//______________________________________________________________________________
//
Bool_t TAGactNtuClusterMT::ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY, TClonesArray* listOfPixels, Int_t thr)
{
   Int_t idx = IndX*fDimX+IndY;
   if ( GetPixelMap(thr)[idx] <= 0 ) return false;
   if ( GetFlagMap(thr)[idx] != -1 ) return false;
   GetFlagMap(thr)[idx] = noClus;
   
   TAGobject* pixel = GetHitObject(GetIndexMap(thr)[idx], listOfPixels);
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
Int_t TAGactNtuClusterMT::GetClusterNumber(Int_t line, Int_t col, Int_t thr) 
{
   return GetFlagMap(thr)[line*fDimX+col];
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
   GetFlagMap(thr).Set(size);
}

//______________________________________________________________________________
//
void TAGactNtuClusterMT::ClearMaps(Int_t thr)
{
   GetPixelMap(thr).clear();
   GetIndexMap(thr).clear();
   GetFlagMap(thr).Reset(-1);
}

