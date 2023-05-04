/*!
 \file TAVTtaskClusterMT.cxx
 \brief   Implementation of TAVTtaskClusterMT.
 */

#include "TAVTtaskClusterMT.hxx"
#include "TAVTbaseHit.hxx"

/*!
 \class TAVTtaskClusterMT
 \brief General class for 2D clustering **
 */

//! Class Imp
//ClassImp(TAVTtaskClusterMT);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name  name
//! \param[in] title  title name
TAVTtaskClusterMT::TAVTtaskClusterMT(TClonesArray* listOfPixels, Int_t* flagMap, Int_t dimX, Int_t dimY, Int_t* clustersN)
 : fListOfPixels(listOfPixels),
   fDimX(dimX),
   fDimY(dimY),
   fFlagMap(flagMap),
   fFlagSize(dimX*dimY),
   fClustersN(clustersN)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTtaskClusterMT::~TAVTtaskClusterMT()
{
}

//______________________________________________________________________________
//! Fill maps
//!
//! \param[in] line line number
//! \param[in] col column number
//! \param[in] idx cluster index
void TAVTtaskClusterMT::FillMaps(Int_t line, Int_t col, Int_t idx)
{
   fPixelMap[line*fDimX+col] = 1;
   fIndexMap[line*fDimX+col] = idx;
}

//______________________________________________________________________________
//! Filling cluster maps.
void TAVTtaskClusterMT::FillMaps()
{
   // Clear maps
   ClearMaps();
   printf("hitsN %d\n", fListOfPixels->GetEntriesFast());
   if (fListOfPixels->GetEntriesFast() == 0) return;
   
   // fill maps for cluster
   for (Int_t i = 0; i < fListOfPixels->GetEntriesFast(); i++) { // loop over hit pixels
      
      TAVTbaseHit* pixel = (TAVTbaseHit*)fListOfPixels->At(i);

      if (!pixel->IsValidFrames()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      FillMaps(line, col, i);
   }
}

//______________________________________________________________________________
//! Shape cluster
//!
//! \param[in] noClus cluster number
//! \param[in] IndX index in X
//! \param[in] IndY index in Y
Bool_t TAVTtaskClusterMT::ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY)
{
   Int_t idx = IndX*fDimX+IndY;
   if ( fPixelMap[idx] <= 0 ) return false;
   if ( fFlagMap[idx] != -1 ) return false;
   fFlagMap[idx] = noClus;
   
   TAVTbaseHit* pixel = (TAVTbaseHit*)fListOfPixels->At(fIndexMap[idx]);
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
//! Search for cluster.
void TAVTtaskClusterMT::SearchCluster()
{
   *fClustersN = 0;
   // Search for cluster
   
   for (Int_t iPix = 0; iPix < fListOfPixels->GetEntriesFast(); ++iPix) { // loop over hit pixels
      TAVTbaseHit* pixel = (TAVTbaseHit*)fListOfPixels->At(iPix);

      if (pixel->Found()) continue;
      Int_t line = pixel->GetPixelLine();
      Int_t col  = pixel->GetPixelColumn();
      if (!CheckLine(line)) continue;
      if (!CheckCol(col)) continue;
      
      // loop over lines & columns
      if ( ShapeCluster(*fClustersN, line, col) )
         (*fClustersN)++;
   }
}

//______________________________________________________________________________
//! Check line number
//!
//! \param[in] idx line number
Bool_t TAVTtaskClusterMT::CheckLine(Int_t idx)
{
   Int_t nLine = fDimY;
 
   if (idx >= nLine || idx < 0) return false;

   return true;
}

//______________________________________________________________________________
//! Check column number
//!
//! \param[in] idx column number
Bool_t TAVTtaskClusterMT::CheckCol(Int_t idx)
{
   Int_t nCol = fDimX;
   
   if (idx >= nCol || idx < 0) return false;
   
   return true;
}

//______________________________________________________________________________
//! Clear maps
void TAVTtaskClusterMT::ClearMaps()
{
   fPixelMap.clear();
   fIndexMap.clear();
   memset(fFlagMap, -1, fFlagSize*sizeof(Int_t));
}

