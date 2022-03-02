/*!
 \file TAGactNtuClusterMT.cxx
 \brief   Implementation of TAGactNtuClusterMT.
 */

#include "TAGactNtuClusterMT.hxx"

/*!
 \class TAGactNtuClusterMT
 \brief General class for 2D clustering **
 */

//! Class Imp
ClassImp(TAGactNtuClusterMT);

Int_t TAGactNtuClusterMT::fgMaxThreadsN  = 4;
Int_t TAGactNtuClusterMT::fgkLimThreadsN = 8;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name  name
//! \param[in] title  title name
TAGactNtuClusterMT::TAGactNtuClusterMT(const char* name, const char* title )
 : TAGaction(name, title),
   fDimX(-1),
   fDimY(-1),
   fThreadsN(-1)
{
   for (Int_t i = 0; i < fgkLimThreadsN; ++i) {
      fFlagMap[i] = 0x0;
      fFlagSize[i] = 0;
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuClusterMT::~TAGactNtuClusterMT()
{
   for (Int_t i = 0; i < fgkLimThreadsN; ++i)
      delete[] fFlagMap[i];
}

//______________________________________________________________________________
//! Get pixel map for a given thread
//!
//! \param[in] thr thread number
map<Int_t, Int_t>& TAGactNtuClusterMT::GetPixelMap(Int_t thr)
{
   if (thr >= 0 && thr < fThreadsN)
      return fPixelMap[thr];
   else
      printf("No pixel map for %d thread, put thread 0\n", thr);
   
   return fPixelMap[0];
      
}

//______________________________________________________________________________
//! Get index map for a given thread
//!
//! \param[in] thr thread number
map<Int_t, Int_t>& TAGactNtuClusterMT::GetIndexMap(Int_t thr)
{
   if (thr >= 0 && thr < fThreadsN)
      return fIndexMap[thr];
   else
      printf("No index map for %d thread, put thread 0\n", thr);
   
   return fIndexMap[0];
}

//______________________________________________________________________________
//! Get flag map for a given thread
//!
//! \param[in] thr thread number
Int_t*  TAGactNtuClusterMT::GetFlagMap(Int_t thr)
{
   if (thr >= 0 && thr < fThreadsN)
      return fFlagMap[thr];
   else
      printf("No index map for %d thread, put thread 0\n", thr);
   
   return fFlagMap[0];
}

//______________________________________________________________________________
//! Fill maps per thread
//!
//! \param[in] line line number
//! \param[in] col column number
//! \param[in] idx cluster index
//! \param[in] thr thread number
void TAGactNtuClusterMT::FillMaps(Int_t line, Int_t col, Int_t idx, Int_t thr)
{
   GetPixelMap(thr)[line*fDimX+col] = 1;
   GetIndexMap(thr)[line*fDimX+col] = idx;
}

//______________________________________________________________________________
//! Shape cluster
//!
//! \param[in] noClus cluster number
//! \param[in] IndX index in X
//! \param[in] IndY index in Y
//! \param[in] listOfPixels list of pixels
//! \param[in] thr thread number
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
//! Get cluster number per thread
//!
//! \param[in] line line number
//! \param[in] col column number
//! \param[in] thr thread number
Int_t TAGactNtuClusterMT::GetClusterNumber(Int_t line, Int_t col, Int_t thr) 
{
   return GetFlagMap(thr)[line*fDimX+col];
}

//______________________________________________________________________________
//! Check line number
//!
//! \param[in] idx line number
Bool_t TAGactNtuClusterMT::CheckLine(Int_t idx)
{
   Int_t nLine = fDimY;
 
   if (idx >= nLine || idx < 0) return false;

   return true;
}

//______________________________________________________________________________
//! Check column number
//!
//! \param[in] idx column number
Bool_t TAGactNtuClusterMT::CheckCol(Int_t idx)
{
   Int_t nCol = fDimX;
   
   if (idx >= nCol || idx < 0) return false;
   
   return true;
}

//______________________________________________________________________________
//! Set up map size per thread
//!
//! \param[in] size map size
//! \param[in] thr thread number
void TAGactNtuClusterMT::SetupMaps(Int_t size, Int_t thr)
{
   if (fFlagMap[thr]  != 0x0)
      delete[] fFlagMap[thr];
   
   fFlagSize[thr] = size;
   fFlagMap[thr]  = new Int_t[size];

}

//______________________________________________________________________________
//! Clear maps per thread
//!
//! \param[in] thr thread number
void TAGactNtuClusterMT::ClearMaps(Int_t thr)
{
   GetPixelMap(thr).clear();
   GetIndexMap(thr).clear();
   
   memset(GetFlagMap(thr), -1, fFlagSize[thr]*sizeof(Int_t));
}

