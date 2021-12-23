
#ifndef TCSTgeometryConstructor_h
#define TCSTgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCSTgeometryConstructor.hxx
 \brief Building STC detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume; 

class TASTparGeo;

class TCSTgeometryConstructor : public TCGbaseConstructor
{
public:
   TCSTgeometryConstructor(TASTparGeo* pParGeo);
   virtual ~TCSTgeometryConstructor();
   
   //! Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size
   TVector3 GetBoxSize()     const  { return fSizeBoxSt;         }
   //! Get min position
   TVector3 GetMinPoistion() const  { return fMinPosition;       }
   //! Get max position
   TVector3 GetMaxPoistion() const  { return fMaxPosition;       }
   
public:
   //! Get SD name
   static const Char_t* GetSDname() { return fgkIrSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical box volume
   G4LogicalVolume* fStcLog;      ///< STC logical volume
   TASTparGeo*      fpParGeo;     ///< Geometry parameters
   TVector3         fSizeBoxSt;   ///< Box size
   TVector3         fMinPosition; ///< Min position
   TVector3         fMaxPosition; ///< Max position
   
private:
   static TString fgkIrSDname;     ///< SD name

private:
   //! Define envelop
   void DefineMaxMinDimension();
   //! Define sensitive detector
   void DefineSensitive();
   //! Define material
   void DefineMaterial();
};

#endif
