
#ifndef TCITgeometryConstructor_h
#define TCITgeometryConstructor_h 1

#include "TString.h"
#include "TCVTgeometryConstructor.hxx"

/** Building IT detector geometry
 
 \author Ch, Finck
 */

class TAVTbaseParGeo;

class TCITgeometryConstructor : public TCVTgeometryConstructor
{
public:
   TCITgeometryConstructor(TAVTbaseParGeo* pParGeo);
   virtual ~TCITgeometryConstructor();
   
   G4LogicalVolume* Construct(); //method in which the physical volume is constructed
   G4LogicalVolume* BuildPlumeSupport();
   void             PlacePlumeSupport();

public:
   static const Char_t* GetSDname()  { return fgkItEpiSDname.Data();}

private:
   G4LogicalVolume* fSupportLog;

private:
   static TString fgkItEpiSDname;

private:
   void DefineSensitive();
   void DefineMaterial();
};

#endif
