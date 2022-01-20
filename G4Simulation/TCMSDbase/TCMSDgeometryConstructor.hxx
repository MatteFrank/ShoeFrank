
#ifndef TCMSDgeometryConstructor_h
#define TCMSDgeometryConstructor_h 1

#include "TString.h"
#include "TCVTgeometryConstructor.hxx"

/*!
 \file TCMSDgeometryConstructor.hxx
 \brief Building MSD detector geometry
 
 \author Ch. Finck
 */

class TAVTbaseParGeo;

class TCMSDgeometryConstructor : public TCVTgeometryConstructor
{
public:
   TCMSDgeometryConstructor(TAVTbaseParGeo* pParGeo);
   virtual ~TCMSDgeometryConstructor();
  
public:
   //! Get SD name
   static const Char_t* GetSDname()  { return fgkMsdEpiSDname.Data();}

private:
   static TString fgkMsdEpiSDname; ///< SD name

private:
   // Define sensitive detector
   void DefineSensitive();
   // Define envelop
   void DefineMaxMinDimension();
   // Define material
   void DefineMaterial();

};

#endif
