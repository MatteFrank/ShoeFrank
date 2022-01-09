
#ifndef TCTWgeometryConstructor_h
#define TCTWgeometryConstructor_h 1

#include "TVector3.h"

#include "TCGbaseConstructor.hxx"

/*!
 \file TCTWgeometryConstructor.hxx
 \brief Building TW detector geometry
 
 \author Ch. Finck
 */

class G4LogicalVolume; //General class to define a logical volume (properties, material)

class TATWparGeo;

class TCTWgeometryConstructor : public TCGbaseConstructor
{
public:
   TCTWgeometryConstructor(TATWparGeo* pParGeo);
   virtual ~TCTWgeometryConstructor();
   
    //! Method in which the physical volume is constructed
   virtual G4LogicalVolume* Construct();
   
   //! Get box size
   TVector3 GetBoxSize()     const { return fSizeBoxTw;  }
   //! Get min position
   TVector3 GetMinPoistion() const { return fMinPosition; }
   //! Get max position
   TVector3 GetMaxPoistion() const { return fMaxPosition; }
   
public:
   //! Get SD name
   static const Char_t* GetSDname()   { return fgkTwSDname.Data();}
   
private:
   G4LogicalVolume* fBoxLog;      ///< Logical box volume
   G4LogicalVolume* fTwLog;       ///< TW logical volume
   TATWparGeo*      fpParGeo;     ///< Geometry parameters
   TVector3         fSizeBoxTw;   ///< Box size
   TVector3         fMinPosition; ///< Min position
   TVector3         fMaxPosition; ///< Max position
   
private:
   static TString fgkTwSDname;    ///< SD name

private:
   //! Define envelop
   void DefineMaxMinDimension();
   //! Define sensitive detector
   void DefineSensitive();
   //! Define material
   void DefineMaterial();

};

#endif
