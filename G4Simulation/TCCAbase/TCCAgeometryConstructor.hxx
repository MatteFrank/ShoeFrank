
#ifndef TCCAgeometryConstructor_h
#define TCCAgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/** Building detector geometry
 
 \author Ch, Finck
 */

class G4LogicalVolume; //General class to define a logical volume (properties, material)

class TACAparGeo;

class TCCAgeometryConstructor : public TCGbaseConstructor
{
public:
   TCCAgeometryConstructor(TACAparGeo* pParGeo);
   virtual ~TCCAgeometryConstructor();
   
   virtual G4LogicalVolume* Construct(); //method in which the physical volume is constructed
   
   TVector3 GetBoxSize()     const  { return fSizeBoxCal;         }
   TVector3 GetMinPoistion() const  { return fMinPosition;        }
   TVector3 GetMaxPoistion() const  { return fMaxPosition;        }
   
public:
   static const Char_t* GetSDname() { return fgkCalSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;
   G4LogicalVolume* fCalLog;
   G4LogicalVolume* fSupLog;
   G4LogicalVolume* fModLog;
   TACAparGeo*      fpParGeo;
   TVector3         fSizeBoxCal;
   TVector3         fMinPosition;
   TVector3         fMaxPosition;
   
private:
   static TString fgkCalSDname;

   
private:
   void DefineMaxMinDimension();
   void DefineSensitive();
   void DefineMaterial();

};

#endif
