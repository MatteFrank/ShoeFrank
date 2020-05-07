#ifndef _TAGactNtuCluster2D_HXX
#define _TAGactNtuCluster2D_HXX
/*!
 \file
 \version $Id: TAGactNtuCluster2D.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAGactNtuCluster2D.
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TArrayI.h"

#include "TAGobject.hxx"
#include "TAGaction.hxx"

class TAGactNtuCluster2D : public TAGaction {
   
public:
   explicit  TAGactNtuCluster2D(const char* name  =  0,
                                const char* title =  0);
   
   virtual ~TAGactNtuCluster2D();
   
   //! Get Cluster number
   Int_t               GetClusterNumber(Int_t line, Int_t col) const { return fFlagMap[line*fDimX+col]; }
   
   // Get object in list
   virtual TAGobject*  GetHitObject(Int_t /*idx*/)             const { return 0x0;                      }
   
protected:
   
   map<Int_t, Int_t> fPixelMap; // pixel map;
   map<Int_t, Int_t> fIndexMap; // index map of the pixel;
   TArrayI           fFlagMap;
   
   Int_t             fDimX;     //  dimension X of map
   Int_t             fDimY;     //  dimension Y of map
   
protected:
   void   FillMaps(Int_t IndX, Int_t IndY, Int_t idx);
   Bool_t ShapeCluster(Int_t noClus, Int_t IndX, Int_t IndY);
   Bool_t CheckLine(Int_t idx);
   Bool_t CheckCol(Int_t idx);
   void   ClearMaps();
   void   SetupMaps(Int_t size);

   
   ClassDef(TAGactNtuCluster2D,0)
};

#endif
