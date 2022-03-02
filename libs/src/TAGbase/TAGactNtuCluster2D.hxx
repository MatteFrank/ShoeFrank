#ifndef _TAGactNtuCluster2D_HXX
#define _TAGactNtuCluster2D_HXX
/*!
 \file TAGactNtuCluster2D.hxx
 \brief   Declaration of TAGactNtuCluster2D.
 \author Ch. Finck
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
   
   //! Get object in list
   virtual TAGobject*  GetHitObject(Int_t /*idx*/)             const { return 0x0;                      }
   
protected:
   
   map<Int_t, Int_t> fPixelMap; ///< pixel map;
   map<Int_t, Int_t> fIndexMap; ///< index map of the pixel;
   Int_t*            fFlagMap;  ///< Array of flag map
   Int_t             fFlagSize; ///< flag map size
   
   Int_t             fDimX;     ///<  dimension X of map
   Int_t             fDimY;     ///<  dimension Y of map
   
protected:
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
   // Set up size map
   void   SetupMaps(Int_t size);

   ClassDef(TAGactNtuCluster2D,0)
};

#endif
