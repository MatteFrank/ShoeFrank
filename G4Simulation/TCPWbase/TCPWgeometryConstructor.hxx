
#ifndef TCPWgeometryConstructor_h
#define TCPWgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCPWgeometryConstructor.hxx
 \brief Building TW detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume; 

class TAPWparGeo;

class TCPWgeometryConstructor : public TCGbaseConstructor
{
public:
   TCPWgeometryConstructor(TAPWparGeo* pParGeo);
   virtual ~TCPWgeometryConstructor();
   
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
   static const Char_t* GetSDname() { return fgkPwSDname.Data(); }

   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical box volume
   G4LogicalVolume* fLaBrLog;      ///< STC logical volume
   G4LogicalVolume* fCsILog;      ///< STC logical volume
   TAPWparGeo*      fpParGeo;     ///< Geometry parameters
   TVector3         fSizeBoxSt;   ///< Box size
   TVector3         fMinPosition; ///< Min position
   TVector3         fMaxPosition; ///< Max position
   
private:
   static TString fgkPwSDname;     ///< SD name

private:
   // Define envelop
   void DefineMaxMinDimension();
   // Define sensitive detector
   void DefineSensitive();
   // Define material
   void DefineMaterial();
};

#endif
