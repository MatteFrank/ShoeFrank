
#ifndef TCEMgeometryConstructor_h
#define TCEMgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/** Building detector geometry
 
 \author Ch, Finck
 */

class G4LogicalVolume; //General class to define a logical volume (properties, material)

class TADIparGeo;

class TCEMgeometryConstructor : public TCGbaseConstructor
{
public:
   TCEMgeometryConstructor(TADIparGeo* pParGeo);
   virtual ~TCEMgeometryConstructor();
   
   virtual G4LogicalVolume* Construct(); //method in which the physical volume is constructed
   
   TVector3 GetBoxSize()     const { return fSizeBoxMg;  }
   TVector3 GetMinPoistion() const { return fMinPosition; }
   TVector3 GetMaxPoistion() const { return fMaxPosition; }
   
private:
   G4LogicalVolume* fBoxLog;
   TADIparGeo*      fpParGeo;
   TVector3         fSizeBoxMg;
   TVector3         fMinPosition;
   TVector3         fMaxPosition;
   
private:
   void DefineMaxMinDimension();
   void DefineSensitive();
   void DefineMaterial();
};

#endif
