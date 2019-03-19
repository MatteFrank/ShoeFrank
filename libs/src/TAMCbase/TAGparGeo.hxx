#ifndef _TAGparGeo_HXX
#define _TAGparGeo_HXX
/*!
  \file
  \brief   Declaration of TAGparGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"
#include "GlobalPar.hxx"


class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAGparGeo : public TAGparTools {
      
private:
   // Target parameters
   struct TargetParameter_t : public  TObject {
	  TString   Shape;    // Target shape
	  TVector3  Size;     // Target size 
	  TString   Material; // Target material
     Float_t   Density;  // Target density
     TVector3  Position; // Target position
   };
   TargetParameter_t  fTargetParameter;
   
   // Target insert parameters
   Int_t              fInsertsN;
   struct InsertParameter_t : public TObject {
      Int_t     InsertIdx;
      TString   Material;
      TString   Shape;
      TVector3  Size;
      TVector3  Position;
   };
   InsertParameter_t  fInsertParameter[20];

   // Beam parameters
   struct BeamParameter_t : public  TObject {
     TVector3  Position;      // Beam position (cm)
     TVector3  AngSpread;     // Angular spread (cos)
     Float_t   AngDiv;        // Angular divergence (mrad)
	  Float_t   Size;          // size of beam, -1 no size (cm)
     TString   Shape;         // Shape of beam
     Float_t   Energy;        // Beam energy
     Int_t     AtomicNumber;  // Z of the beam
     Float_t   AtomicMass;    // A of the beam
     Int_t     PartNumber;    // Number of particles in beam
   };
   BeamParameter_t  fBeamParameter;

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
private:
   TGeoVolume*  AddCubicTarget(const char *targetName);
   TGeoVolume*  AddCylindricTarget(const char *targetName);
   
public:
   TAGparGeo();
    virtual ~TAGparGeo();
   
   // Define material
   void    DefineMaterial();

   //! Read parameters from file
   Bool_t             FromFile(const TString& name = "");

   // Print out
   void               Print(Option_t* opt = "") const;

   //! Get target parameter
   TargetParameter_t& GetTargetPar()          { return fTargetParameter;      }

   //! Get inert number
   Int_t              GetInsertsN()     const { return fInsertsN;             }
   
   //! Get target insert parameter
   InsertParameter_t& GetInsertPar(Int_t idx) { return fInsertParameter[idx]; }

   //! Get beam parameter
   BeamParameter_t&   GetBeamPar()        { return fBeamParameter;   }
   
   //! Add Target
   TGeoVolume*        AddTarget(const char *targetName = "Target");
   
  //! build Target
  TGeoVolume*        BuildTarget(const char *targetName = "Target");
  
  string PrintBodies();
  string PrintRegions();
  string PrintAssignMaterial();
  string PrintSubtractBodiesFromAir();

   
public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

   ClassDef(TAGparGeo,1)
};

#endif
