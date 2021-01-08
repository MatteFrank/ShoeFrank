
#ifndef TCBMgeometryConstructor_h
#define TCBMgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/** Building detector geometry
 
 \author Ch, Finck
 */

class G4LogicalVolume; //General class to define a logical volume (properties, material)

class TABMparGeo;

class TCBMgeometryConstructor : public TCGbaseConstructor
{
public:
   TCBMgeometryConstructor(TABMparGeo* pParGeo);
   virtual ~TCBMgeometryConstructor();
   
   virtual G4LogicalVolume* Construct(); //method in which the physical volume is constructed
   
   TVector3 GetBoxSize()     const { return fSizeBoxBm;  }
   TVector3 GetMinPoistion() const { return fMinPosition; }
   TVector3 GetMaxPoistion() const { return fMaxPosition; }
   
public:
   static const Char_t* GetSDname()  { return fgkBmSDname.Data();}
   void GetLayersN();
   
private:
   G4LogicalVolume* fBoxLog;
   G4LogicalVolume* fLayerLog;
   TABMparGeo*      fpParGeo;
   TVector3         fSizeBoxBm;
   TVector3         fMinPosition;
   TVector3         fMaxPosition;
   
private:
   static TString fgkBmSDname;

   
private:
   void DefineMaxMinDimension();
   void DefineSensitive();
   void DefineMaterial();

};

#endif
