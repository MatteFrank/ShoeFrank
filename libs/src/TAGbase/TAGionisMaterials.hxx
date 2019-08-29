
#ifndef _TAGionisMaterials_HXX
#define _TAGionisMaterials_HXX


#include "TString.h"
#include "TAGobject.hxx"

class TAGionisMaterials : public TAGobject
{
public:
   TAGionisMaterials();
   virtual ~TAGionisMaterials();
   

   // parameters for mean energy loss calculation:
   void      SetMeanExcitationEnergy(Double_t value)   { fMeanExcitationEnergy = value;  }
   Double_t  GetMeanExcitationEnergy()           const { return fMeanExcitationEnergy;   }
   
   void      SetShellCorrectionVector(Double_t* value) { fShellCorrectionVector = value; }
   Double_t* GetShellCorrectionVector()          const { return fShellCorrectionVector;  }

   void      SetTaul(Double_t value)                   { fTaul = value;                  }
   Double_t  GetTaul()                           const { return fTaul;                   }
   
   
   // parameters for Birks attenuation:
   void      SetBirksConstant(Double_t value)          { fBirks = value;                 }
   Double_t  GetBirksConstant()                  const { return fBirks;                  }
   
private:
 
   // parameters for mean energy loss calculation
   Double_t  fMeanExcitationEnergy;          // Mean excitation energy
   Double_t* fShellCorrectionVector;         // shell correction coefficients
   Double_t  fTaul;                          // lower limit of Bethe-Bloch formula
   
   // parameter for Birks attenuation
   Double_t  fBirks;
   
   ClassDef(TAGionisMaterials,1)

};

#endif
