#ifndef _TAGactNtuCluster1D_HXX
#define _TAGactNtuCluster1D_HXX
/*!
 \file TAGactNtuCluster1D.hxx
 \brief   Declaration of TAGactNtuCluster1D.
 */
/*------------------------------------------+---------------------------------*/
#include <map>

#include "TArrayI.h"

#include "TAGobject.hxx"
#include "TAGaction.hxx"

class TAGactNtuCluster1D : public TAGaction {
   
public:
   explicit  TAGactNtuCluster1D(const char* name  =  0,
                                const char* title =  0);
   
   virtual ~TAGactNtuCluster1D();
   
   //! Get Cluster number
   Int_t               GetClusterNumber(Int_t idx) const { return fFlagMap[idx]; }
   
   //! Get object in list
   virtual TAGobject*  GetHitObject(Int_t /*idx*/) const { return 0x0;           }
   
protected:
   map<Int_t, Int_t> fPixelMap; ///< pixel map;
   map<Int_t, Int_t> fIndexMap; ///< index map of the pixel;
   TArrayI           fFlagMap;  ///< Array flag map
   Int_t             fDimX;     ///<  dimension X of map
   
protected:
   //! Fill maps
   void   FillMaps(Int_t IndX, Int_t idx);
   //! Shape cluster
   Bool_t ShapeCluster(Int_t noClus, Int_t IndX);
   //! Check line
   Bool_t CheckLine(Int_t idx);
   //! Clear maps
   void   ClearMaps();
   //! Set up size map
   void   SetupMaps(Int_t size);

   ClassDef(TAGactNtuCluster1D,0)
};

#endif
