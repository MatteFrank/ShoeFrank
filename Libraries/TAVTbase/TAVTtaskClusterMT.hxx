#ifndef _TAVTtaskClusterMT_HXX
#define _TAVTtaskClusterMT_HXX
/*!
 \file TAVTtaskClusterMT.hxx
 \brief   Declaration of TAVTtaskClusterMT.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TArrayI.h"
#include "TClonesArray.h"

#include "TThreadPool.h"
#include "TThread.h"

#include "TAGobject.hxx"

using namespace std;
// define a custom parameters type for task objects
enum EProc {start, clean};

// a class defining task objects
class TAVTtaskClusterMT : public TThreadPoolTaskImp<TAVTtaskClusterMT, EProc> {
   
public:
   explicit  TAVTtaskClusterMT(TClonesArray* listOfPixels,  Int_t* flagMap, Int_t dimX, Int_t dimY, Int_t* clustersN);
   
   virtual ~TAVTtaskClusterMT();
      
   bool runTask(EProc /*_param*/) {
      fThId = TThread::SelfId();
      FillMaps();
      SearchCluster();
      return true;
   }
   
   ULong_t threadID() const {
      return fThId;
   }
   
protected:
   TClonesArray*     fListOfPixels;
   map<Int_t, Int_t> fPixelMap; ///< pixel map;
   map<Int_t, Int_t> fIndexMap; ///< index map of the pixel;
   Int_t*            fFlagMap;  ///< Array of flag map
   Int_t             fFlagSize; ///< flag map size
   
   Int_t             fDimX;     ///<  dimension X of map
   Int_t             fDimY;     ///<  dimension Y of map
   ULong_t           fThId;
   Int_t*             fClustersN;         ///< number of cluster


protected:
   // Search cluster
   void   SearchCluster();
   // Fill map
   void   FillMaps();
   // Fill maps
   void   FillMaps(Int_t IndX, Int_t IndY, Int_t idx);
   // Shape cluster
   Bool_t ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY);
   // Check line
   Bool_t CheckLine(Int_t idx);
   // Check column
   Bool_t CheckCol(Int_t idx);
   // Clear maps
   void   ClearMaps();

  // ClassDef(TAVTtaskClusterMT,0)
};

#endif
