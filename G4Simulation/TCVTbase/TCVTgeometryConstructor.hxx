
#ifndef TCVTgeometryConstructor_h
#define TCVTgeometryConstructor_h 1

#include "TVector3.h"
#include "TString.h"

#include "TCGbaseConstructor.hxx"

/** Building detector geometry
 
 \author Ch, Finck
 */

class G4LogicalVolume; //General class to define a logical volume (properties, material)
class G4VPhysicalVolume; //General class to define the position of the volume
class TAVTparGeo;
class TAVTbaseParGeo;

class TCVTgeometryConstructor : public TCGbaseConstructor
{
public:
   TCVTgeometryConstructor(TAVTbaseParGeo* pParGeo, Bool_t bmFlag = false);
   virtual ~TCVTgeometryConstructor();
   virtual G4LogicalVolume* Construct(); //method in which the physical volume is constructed
   
   TVector3 GetBoxSize()     const { return fSizeBoxVtx;  }
   TVector3 GetMinPosition() const { return fMinPosition; }
   TVector3 GetMaxPosition() const { return fMaxPosition; }

   const Char_t* GetEpiName() { return fEpiName.Data();}

    void SetEpiName(TString aEpiName) {fEpiName = aEpiName ; }

public:
   static const Char_t* GetCmosName()      { return fgkCmosName.Data();    }
   static const Char_t* GetPixName()       { return fgkPixName.Data();     }
   static const Char_t* GetSDname()        { return fgkVtxEpiSDname.Data();}
   static const Char_t* GetBmSDname()      { return fgkBmEpiSDname.Data(); }

   
   static Bool_t GetSmearFlag()            { return fgSmearFlag;           }
   static void   SetSmearFlag(Bool_t flag) { fgSmearFlag = flag;           }
   
protected:
   //logical volume
   G4LogicalVolume* fCmosLog;
   G4LogicalVolume* fEpiLog;
   G4LogicalVolume* fPixLog;
   G4LogicalVolume* fBoxVtxLog;

   TString fEpiName;

   //phys volume
   G4VPhysicalVolume* fEpiPhy;
   G4VPhysicalVolume* fPixPhy;
   
   TAVTbaseParGeo* fpParGeo;
   
   TVector3 fMinPosition;
   TVector3 fMaxPosition;
   TVector3 fSizeBoxVtx;
   
   Bool_t   fBmFlag;
   
protected:
   static TString fgkCmosName;
   static TString fgkPixName;
   static TString fgkVtxEpiSDname;
   static TString fgkBmEpiSDname;

   static Float_t fgDefSmearAng;
   static Float_t fgDefSmearPos;
   static Bool_t  fgSmearFlag;
   
protected:
   virtual void DefineMaterial();
   virtual void DefineMaxMinDimension();
   virtual void DefineSensitive();
   
   virtual void BuildSensor();
   void PlaceSensor();
};

#endif
