
#ifndef _TAGionisMaterials_HXX
#define _TAGionisMaterials_HXX

/*!
 \file TAGionisMaterials.hxx
 \brief   Declaration of TAGionisMaterials.
 \author Ch.Finck
 */
/*------------------------------------------+---------------------------------*/


#include "TString.h"
#include "TGeoMaterial.h"
#include "TAGobject.hxx"

class TAGionisMaterials : public TAGobject
{
public:
   TAGionisMaterials(TGeoMaterial* mat);
   TAGionisMaterials();
   virtual ~TAGionisMaterials();
   
   //! Set materials
   void SetMaterial(TGeoMaterial* mat)                 { fMaterial = mat;                }

   //! parameters for mean energy loss calculation:
   //! Add mean excitation energy
   void      AddMeanExcitationEnergy(Double_t value);
   //! Setmean excitation energy
   void      SetMeanExcitationEnergy(Double_t value)   { fMeanExcitationEnergy = value;  }
   //! Get mean excitation energy
   Double_t  GetMeanExcitationEnergy()           const { return fMeanExcitationEnergy;   }
   //! Set shell correction
   void      SetShellCorrectionVector(Double_t* value) { fShellCorrectionVector = value; }
   //! Get shell correction
   Double_t* GetShellCorrectionVector()          const { return fShellCorrectionVector;  }
   //! Set tau parameter
   void      SetTaul(Double_t value)                   { fTaul = value;                  }
   //! Get tau parameter
   Double_t  GetTaul()                           const { return fTaul;                   }
   
   
   //! parameters for Birks attenuation:
   //! Add Birk factor
   void      AddBirksFactor(Double_t value);
   //! Set Birk factor
   void      SetBirksConstant(Double_t value)          { fBirks = value;                 }
   //! Get Birk factor
   Double_t  GetBirksConstant()                  const { return fBirks;                  }
   
public:
   //! Get mean excitation energy name
   static const Char_t* GetMeanExcitationEnergyName()  { return fgkMeanExcitationEnergy.Data();  }
   //! Get shell correction vector name
   static const Char_t* GetShellCorrectionVectorName() { return fgkShellCorrectionVector.Data(); }
   //! Get Tau parameter name
   static const Char_t* GetTaulName()                  { return fgkTaul.Data();                  }
   //! Get Birk factor name
   static const Char_t* GetBirksName()                 { return fgkBirks.Data();                 }

   //! Get mean excitation energy name for a given material
   static const Char_t* GetMeanExcitationEnergyName(TString matName)  { return (fgkMeanExcitationEnergy+matName).Data();  }
   //! Get shell correction vector name for a given material
   static const Char_t* GetShellCorrectionVectorName(TString matName) { return (fgkShellCorrectionVector+matName).Data(); }
   //! Get Tau parameter name for a given material
   static const Char_t* GetTaulName(TString matName)                  { return (fgkTaul+matName).Data();                  }
   //! Get Birk factor name for a given material
   static const Char_t* GetBirksName(TString matName)                 { return (fgkBirks+matName).Data();                 }
   
private:
   TGeoMaterial* fMaterial;                        //!< pointer to material
   
   // parameters for mean energy loss calculation
   Double_t      fMeanExcitationEnergy;            ///< Mean excitation energy
   Double_t*     fShellCorrectionVector;           ///< shell correction coefficients
   Double_t      fTaul;                            ///< lower limit of Bethe-Bloch formula
   
   // parameter for Birks attenuation
   Double_t      fBirks;                           ///< Birks factor
   
private:
   static const TString fgkMeanExcitationEnergy;   ///< Mean excitation energy name
   static const TString fgkShellCorrectionVector;  ///<  shell correction vector name
   static const TString fgkTaul;                   ///< Tau parameter name
   static const TString fgkBirks;                  ///<  Birk factor name
   
   ClassDef(TAGionisMaterials,1)
};

#endif
