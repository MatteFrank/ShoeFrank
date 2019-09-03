
#ifndef _TAGionisMaterials_HXX
#define _TAGionisMaterials_HXX


#include "TString.h"
#include "TGeoMaterial.h"
#include "TAGobject.hxx"

class TAGionisMaterials : public TAGobject
{
public:
   TAGionisMaterials(TGeoMaterial* mat);
   TAGionisMaterials();
   virtual ~TAGionisMaterials();
   
   // Set materials
   void SetMaterial(TGeoMaterial* mat)                 { fMaterial = mat;                }

   // parameters for mean energy loss calculation:
   void      AddMeanExcitationEnergy(Double_t value);
   void      SetMeanExcitationEnergy(Double_t value)   { fMeanExcitationEnergy = value;  }
   Double_t  GetMeanExcitationEnergy()           const { return fMeanExcitationEnergy;   }
   
   void      SetShellCorrectionVector(Double_t* value) { fShellCorrectionVector = value; }
   Double_t* GetShellCorrectionVector()          const { return fShellCorrectionVector;  }

   void      SetTaul(Double_t value)                   { fTaul = value;                  }
   Double_t  GetTaul()                           const { return fTaul;                   }
   
   
   // parameters for Birks attenuation:
   void      AddBirksFactor(Double_t value);
   void      SetBirksConstant(Double_t value)          { fBirks = value;                 }
   Double_t  GetBirksConstant()                  const { return fBirks;                  }
   
public:
   static const Char_t* GetMeanExcitationEnergyName()  { return fgkMeanExcitationEnergy.Data();  }
   static const Char_t* GetShellCorrectionVectorName() { return fgkShellCorrectionVector.Data(); }
   static const Char_t* GetTaulName()                  { return fgkTaul.Data();                  }
   static const Char_t* GetBirksName()                 { return fgkBirks.Data();                 }

   
   static const Char_t* GetMeanExcitationEnergyName(TString matName)  { return (fgkMeanExcitationEnergy+matName).Data();  }
   static const Char_t* GetShellCorrectionVectorName(TString matName) { return (fgkShellCorrectionVector+matName).Data(); }
   static const Char_t* GetTaulName(TString matName)                  { return (fgkTaul+matName).Data();                  }
   static const Char_t* GetBirksName(TString matName)                 { return (fgkBirks+matName).Data();                 }
   
private:
   TGeoMaterial* fMaterial;                     //! pointer to material
   
   // parameters for mean energy loss calculation
   Double_t      fMeanExcitationEnergy;          // Mean excitation energy
   Double_t*     fShellCorrectionVector;         // shell correction coefficients
   Double_t      fTaul;                          // lower limit of Bethe-Bloch formula
   
   // parameter for Birks attenuation
   Double_t      fBirks;
   
private:
   static const TString fgkMeanExcitationEnergy;
   static const TString fgkShellCorrectionVector;
   static const TString fgkTaul;
   static const TString fgkBirks;
   
   ClassDef(TAGionisMaterials,1)
};

#endif
