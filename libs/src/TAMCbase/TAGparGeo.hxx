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
#include "TAGrecoManager.hxx"
#include "TAGmaterials.hxx"


class TGeoHMatrix;
class TGeoVolume;
class TAGionisMaterials;

//##############################################################################

class TAGparGeo : public TAGparTools {

private:
   TAGionisMaterials* fIonisation; //! pointer for ionisation property
   TString    fDefaultGeoName;  // default par geo file name
   TString    fFileName;         // par geo file name

   // Target parameters
   struct TargetParameter_t : public  TObject {
	  TString   Shape;    // Target shape
	  TVector3  Size;     // Target size
	  TString   Material; // Target material
     Float_t   Density;  // Target density
     Float_t   ExcEnergy; // Target mean excitation energy
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
     TString   Material;      // Beam material
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
   TargetParameter_t&        GetTargetPar()           { return fTargetParameter;      }
   TargetParameter_t const & GetTargetPar()     const { return fTargetParameter;      }

   //! Get inert number
   Int_t              GetInsertsN()     const { return fInsertsN;             }

   //! Get target insert parameter
   InsertParameter_t& GetInsertPar(Int_t idx) { return fInsertParameter[idx]; }

   //! Get beam parameter
   BeamParameter_t&          GetBeamPar()         { return fBeamParameter;   }
   BeamParameter_t const &   GetBeamPar()   const { return fBeamParameter;   }

   //! Add Target
   TGeoVolume*        AddTarget(const char *targetName = "Target");

  //! build Target
  TGeoVolume*        BuildTarget(const char *targetName = "Target");

  //crossing regions:
  Int_t          GetRegTarget(){TString regname("TARGET");return GetCrossReg(regname);};
  //N.B.: the are different air region around the calo, check the flair file if you are interested in
  Int_t          GetRegAirPreTW(){TString regname("AIR1");return GetCrossReg(regname);}; //air region from the ST to the MSD (included)
  Int_t          GetRegAirTW(){TString regname("AIR2");return GetCrossReg(regname);};    //air region around the TW

  //blackbody and air
  string PrintStandardBodies();
  string PrintStandardRegions1();
  string PrintStandardRegions2();
  string PrintStandardAssignMaterial();

  //target
  string PrintTargRotations();
  string PrintTargBody();
  string PrintTargRegion();
  string PrintTargAssignMaterial(TAGmaterials *Material);
  string PrintSubtractTargBodyFromAir();

  //beam cards
  string PrintBeam();
  //physics cards
  string PrintPhysics();


public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

   ClassDef(TAGparGeo,2)
};

#endif
