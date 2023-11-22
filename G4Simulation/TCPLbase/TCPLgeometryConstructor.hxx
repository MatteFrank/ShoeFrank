
#ifndef TCPLgeometryConstructor_h
#define TCPLgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCPLgeometryConstructor.hxx
 \brief Building STC detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume; 

class TAPLparGeo;

class TCPLgeometryConstructor : public TCGbaseConstructor
{
public:
   TCPLgeometryConstructor(TAPLparGeo* pParGeo);
   virtual ~TCPLgeometryConstructor();
   
   // Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get Box size
   TVector3 GetBoxSize()     const  { return fSizeBoxSt;         }
   //! Get min position
   TVector3 GetMinPoistion() const  { return fMinPosition;       }
   //! Get max position
   TVector3 GetMaxPoistion() const  { return fMaxPosition;       }
   
public:
   //! Get SD name
   static const Char_t* GetSDname() { return fgkPlSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical box volume
   G4LogicalVolume* fStcLog;      ///< STC logical volume
   TAPLparGeo*      fpParGeo;     ///< Geometry parameters
   TVector3         fSizeBoxSt;   ///< Box size
   TVector3         fMinPosition; ///< Min position
   TVector3         fMaxPosition; ///< Max position
   
private:
   static TString fgkPlSDname;     ///< SD name

private:
   // Define envelop
   void DefineMaxMinDimension();
   // Define sensitive detector
   void DefineSensitive();
   // Define material
   void DefineMaterial();
};

#endif
