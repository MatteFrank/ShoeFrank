
#ifndef TCCAgeometryConstructor_h
#define TCCAgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCCAgeometryConstructor.hxx
 \brief  Building detector geometry
 
 Class to define a logical volume (properties, material) of CALorimter
 
 \author Ch. Finck
 */

class G4LogicalVolume;

class TACAparGeo;

class TCCAgeometryConstructor : public TCGbaseConstructor
{
public:
   TCCAgeometryConstructor(TACAparGeo* pParGeo);
   virtual ~TCCAgeometryConstructor();
   
   //! Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size of CAL
   TVector3 GetBoxSize()     const  { return fSizeBoxCal;         }
   //! Get min size of CAL
   TVector3 GetMinPoistion() const  { return fMinPosition;        }
   //! Get max size of CAL
   TVector3 GetMaxPoistion() const  { return fMaxPosition;        }
   
public:
   static const Char_t* GetSDname() { return fgkCalSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical volume of box
   G4LogicalVolume* fSupLog;      ///< Logical volume of support
   G4LogicalVolume* fCrysLog;     ///< Logical volume of crystal
   TACAparGeo*      fpParGeo;     ///< Geometrical parameters
   TVector3         fSizeBoxCal;  ///< Box size
   TVector3         fMinPosition; ///< Minimun size of envelop
   TVector3         fMaxPosition; ///< Maximum size of envelop
   
private:
   static TString fgkCalSDname;  ///< BM sensitive detector name

   
private:
   //! Define envelop size
   void DefineMaxMinDimension();
   //! Define sensitive detector
   void DefineSensitive();
   //! Define material
   void DefineMaterial();
};

#endif
