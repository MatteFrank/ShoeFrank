
#ifndef TCBMgeometryConstructor_h
#define TCBMgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCBMgeometryConstructor.hxx
 \brief  Building detector geometry
 
  Class to define a logical volume (properties, material) of Beam Monitor
 
 \author Ch. Finck
 */

class G4LogicalVolume;
class TABMparGeo;

class TCBMgeometryConstructor : public TCGbaseConstructor
{
public:
   TCBMgeometryConstructor(TABMparGeo* pParGeo);
   virtual ~TCBMgeometryConstructor();
   
   //! Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size of BM
   TVector3 GetBoxSize()     const { return fSizeBoxBm;  }
   //! Get min size of BM
   TVector3 GetMinPoistion() const { return fMinPosition; }
   //! Get max size of BM
   TVector3 GetMaxPoistion() const { return fMaxPosition; }
   
public:
   //! Get Sensitive detector name of BM
   static const Char_t* GetSDname()  { return fgkBmSDname.Data();}
   //! Get number of layers
   void GetLayersN();
   
private:
   G4LogicalVolume* fBoxLog;        ///< Logical volume of box
   G4LogicalVolume* fLayerLog;      ///< Logical volume of layers
   TABMparGeo*      fpParGeo;       ///< Geometrical parameters
   TVector3         fSizeBoxBm;     ///< Box size
   TVector3         fMinPosition;   ///< Minimun size of envelop
   TVector3         fMaxPosition;   ///< Maximum size of envelop
   
private:
   static TString fgkBmSDname;      ///< BM sensitive detector name

private:
   //! Define envelop size
   void DefineMaxMinDimension();
   //! Define sensitive detector
   void DefineSensitive();
   //! Define material
   void DefineMaterial();
};

#endif
