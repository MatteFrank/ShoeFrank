#ifndef _TAGactNtuClusterMT_HXX
#define _TAGactNtuClusterMT_HXX
/*!
 \file
 \version $Id: TAGactNtuClusterMT.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAGactNtuClusterMT.
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
   
   // Get object in list
   virtual TAGobject*  GetHitObject(Int_t /*idx*/, TClonesArray* /*list*/) const { return 0x0;                      }

public:
   static void  SetMaxThread(Int_t thr) { fgMaxThread = thr;  }
   static Int_t GetMaxThread()          { return fgMaxThread; }

   
protected:
   
   map<Int_t, Int_t> fPixelMap[4]; // pixel map;
   map<Int_t, Int_t> fIndexMap[4]; // index map of the pixel;
   TArrayI           fFlagMap[4];
   
   Int_t             fDimX;     //  dimension X of map
   Int_t             fDimY;     //  dimension Y of map
   
protected:
   void   FillMaps(Int_t IndX, Int_t IndY, Int_t idx, Int_t thr);
   Bool_t ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY, TClonesArray* listOfPixels, Int_t thr);
   Bool_t CheckLine(Int_t idx);
   Bool_t CheckCol(Int_t idx);
   void   ClearMaps(Int_t thr);
   void   SetupMaps(Int_t size, Int_t thr);

   map<Int_t, Int_t>& GetPixelMap(Int_t thr);
   map<Int_t, Int_t>& GetIndexMap(Int_t thr);
   TArrayI&           GetFlagMap(Int_t thr);
   
protected:
   static Int_t fgMaxThread;
   
   ClassDef(TAGactNtuClusterMT,0)
};

#endif
