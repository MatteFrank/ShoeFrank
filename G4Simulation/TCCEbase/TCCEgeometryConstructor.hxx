
#ifndef TCCEgeometryConstructor_h
#define TCCEgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCCEgeometryConstructor.hxx
 \brief Building TW detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume; 

class TACEparGeo;

class TCCEgeometryConstructor : public TCGbaseConstructor
{
public:
   TCCEgeometryConstructor(TACEparGeo* pParGeo);
   virtual ~TCCEgeometryConstructor();
   
   // Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size
   TVector3 GetBoxSize()     const  { return fSizeBoxSt;         }
   //! Get min position
   TVector3 GetMinPoistion() const  { return fMinPosition;       }
   //! Get max position
   TVector3 GetMaxPoistion() const  { return fMaxPosition;       }
   
public:
   //! Get SD name
   static const Char_t* GetSDname() { return fgkCeSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical box volume
   G4LogicalVolume* fTofLog;      ///< STC logical volume
   G4LogicalVolume* fTofWLog;      ///< STC logical volume
   TACEparGeo*      fpParGeo;     ///< Geometry parameters
   TVector3         fSizeBoxSt;   ///< Box size
   TVector3         fMinPosition; ///< Min position
   TVector3         fMaxPosition; ///< Max position
   
private:
   static TString fgkCeSDname;     ///< SD name

private:
   // Define envelop
   void DefineMaxMinDimension();
   // Define sensitive detector
   void DefineSensitive();
   // Define material
   void DefineMaterial();
};

#endif
