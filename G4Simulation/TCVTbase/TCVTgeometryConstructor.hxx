
#ifndef TCVTgeometryConstructor_h
#define TCVTgeometryConstructor_h 1

#include "TVector3.h"
#include "TString.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCVTgeometryConstructor.hxx
 \brief Building VTX detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume;
class G4VPhysicalVolume;
class TAVTparGeo;
class TAVTbaseParGeo;

class TCVTgeometryConstructor : public TCGbaseConstructor
{
public:
   TCVTgeometryConstructor(TAVTbaseParGeo* pParGeo);
   virtual ~TCVTgeometryConstructor();
   
   // Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size
   TVector3 GetBoxSize()     const { return fSizeBoxVtx;  }
   //! Get min position
   TVector3 GetMinPosition() const { return fMinPosition; }
   //! Get max position
   TVector3 GetMaxPosition() const { return fMaxPosition; }

   //! Get epitaxial name
   const Char_t* GetEpiName() { return fEpiName.Data();}
   //! Set epitaxial name
   void SetEpiName(TString aEpiName) {fEpiName = aEpiName ; }

public:
   //! Get Cmos name
   static const Char_t* GetCmosName()      { return fgkCmosName.Data();    }
   //! Get pixel name
   static const Char_t* GetPixName()       { return fgkPixName.Data();     }
   //! Get SD name
   static const Char_t* GetSDname()        { return fgkVtxEpiSDname.Data();}
  
   //! Set smearing flag
   static Bool_t GetSmearFlag()            { return fgSmearFlag;           }
   //! Get smearing flag
   static void   SetSmearFlag(Bool_t flag) { fgSmearFlag = flag;           }
   
protected:
   G4LogicalVolume*   fCmosLog;     ///< Logical CMOS volume
   G4LogicalVolume*   fEpiLog;      ///< Logical expitaxial volume
   G4LogicalVolume*   fPixLog;      ///< Logical pixel volume
   G4LogicalVolume*   fBoxVtxLog;   ///< Logical box volume

   TString            fEpiName;     ///< Epitaxial name

   G4VPhysicalVolume* fEpiPhy;      ///< Physical epitaxial volume
   G4VPhysicalVolume* fPixPhy;      ///< Physical pixel volume
   
   TAVTbaseParGeo*    fpParGeo;     ///< Geometry parameters
   
   TVector3           fMinPosition; ///< Box size
   TVector3           fMaxPosition; ///< Min position
   TVector3           fSizeBoxVtx;  ///< Max position
   
protected:
   static TString fgkCmosName;      ///< Cmos name
   static TString fgkPixName;       ///< Pixel name
   static TString fgkVtxEpiSDname;  ///< Epitaxial SD name

   static Float_t fgDefSmearAng;    ///< Angular smearing flag
   static Float_t fgDefSmearPos;    ///< Position smearing flag
   static Bool_t  fgSmearFlag;      ///< Smearing flag
   
protected:
   // Define envelop
   void DefineMaxMinDimension();
   // Define sensitive detector
   void DefineSensitive();
   // Define material
   void DefineMaterial();
   
   // Build sensor
   virtual void BuildSensor();
   // Place sensor
   void PlaceSensor();
};

#endif
