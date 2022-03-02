
#ifndef TCITgeometryConstructor_h
#define TCITgeometryConstructor_h 1

#include "TString.h"
#include "TCVTgeometryConstructor.hxx"

/*!
 \file TCITgeometryConstructor.hxx
 \brief Building ITR detector geometry
 
 \author Ch. Finck
 */

class TAVTbaseParGeo;

class TCITgeometryConstructor : public TCVTgeometryConstructor
{
public:
   TCITgeometryConstructor(TAVTbaseParGeo* pParGeo);
   virtual ~TCITgeometryConstructor();
   
   // Method in which the physical volume is constructed
   G4LogicalVolume* Construct();
   // Build support
   G4LogicalVolume* BuildPlumeSupport();
   // Place support
   void             PlacePlumeSupport();

public:
   //! Get SD name
   static const Char_t* GetSDname()  { return fgkItEpiSDname.Data();}

private:
   G4LogicalVolume* fSupportLog; ///< Logical support volume

private:
   static TString fgkItEpiSDname; ///< SD name

private:
   // Define sensitive detector
   void DefineSensitive();
   // Define material
   void DefineMaterial();
};

#endif
