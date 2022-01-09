#ifndef _TAGactNtuClusterMT_HXX
#define _TAGactNtuClusterMT_HXX
/*!
 \file TAGactNtuClusterMT.hxx
 \brief   Declaration of TAGactNtuClusterMT.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TArrayI.h"

#include "TAGobject.hxx"
#include "TAGaction.hxx"

class TClonesArray;
class TAGactNtuClusterMT : public TAGaction {
   
public:
   explicit  TAGactNtuClusterMT(const char* name  =  0,
                                const char* title =  0);
   
   virtual ~TAGactNtuClusterMT();
   
   //! Get Cluster number
   Int_t               GetClusterNumber(Int_t line, Int_t col, Int_t thr);
   
   //! Get object in list
   virtual TAGobject*  GetHitObject(Int_t /*idx*/, TClonesArray* /*list*/) const { return 0x0; }

public:
   //! Set maximum number of threads
   static void  SetMaxThread(Int_t thr) { fgMaxThreadsN = thr;  }
   //! Get maximum number of threads
   static Int_t GetMaxThread()          { return fgMaxThreadsN; }

protected:
   map<Int_t, Int_t> fPixelMap[8]; ///< pixel map per thread
   map<Int_t, Int_t> fIndexMap[8]; ///< index map of the pixel per thread
   Int_t*            fFlagMap[8];  ///< Array flag map per thread
   Int_t             fFlagSize[8]; ///< Flag map size per thread

   Int_t             fDimX;        ///<  dimension X of map
   Int_t             fDimY;        ///<  dimension Y of map
   Int_t             fThreadsN;    ///< number of thread
   
protected:
   //! Fill maps per thread
   void   FillMaps(Int_t IndX, Int_t IndY, Int_t idx, Int_t thr);
   //! Shape cluster per thread
   Bool_t ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY, TClonesArray* listOfPixels, Int_t thr);
   //! Check line
   Bool_t CheckLine(Int_t idx);
   //! Check column
   Bool_t CheckCol(Int_t idx);
   //! Clear maps per thread
   void   ClearMaps(Int_t thr);
   //! Set up size map per thread
   void   SetupMaps(Int_t size, Int_t thr);

   //! Get pixels map per thread
   map<Int_t, Int_t>& GetPixelMap(Int_t thr);
   //! Get index map per thread
   map<Int_t, Int_t>& GetIndexMap(Int_t thr);
   //! Get flag map per thread
   Int_t*             GetFlagMap(Int_t thr);
   
protected:
   static Int_t fgMaxThreadsN;  ///< maximum number of thread
   static Int_t fgkLimThreadsN; ///< upper limit of thread (fix to 8)

   ClassDef(TAGactNtuClusterMT,0)
};

#endif
