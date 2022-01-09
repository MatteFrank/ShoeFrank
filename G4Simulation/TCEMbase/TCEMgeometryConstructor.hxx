
#ifndef TCEMgeometryConstructor_h
#define TCEMgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCEMgeometryConstructor.hxx
 \brief Building magnetic field geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume;

class TADIparGeo;

class TCEMgeometryConstructor : public TCGbaseConstructor
{
public:
   TCEMgeometryConstructor(TADIparGeo* pParGeo);
   virtual ~TCEMgeometryConstructor();
   
   //! Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size of dipole
   TVector3 GetBoxSize()     const { return fSizeBoxMg;  }
   //! Get min size of dipole
   TVector3 GetMinPoistion() const { return fMinPosition; }
   //! Get max size of dipole
   TVector3 GetMaxPoistion() const { return fMaxPosition; }
   
private:
   G4LogicalVolume* fBoxLog;       ///< Logical volume of box
   TADIparGeo*      fpParGeo;      ///< Geometrical parameters
   TVector3         fSizeBoxMg;    ///< Box size
   TVector3         fMinPosition;  ///< Minimun size of envelop
   TVector3         fMaxPosition;  ///< Maximum size of envelop

private:
   //! Define envelop size
   void DefineMaxMinDimension();
   //! Define sensitive detector
   void DefineSensitive();
   //! Define material
   void DefineMaterial();
};

#endif
