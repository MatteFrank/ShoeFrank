#ifndef _TAMCparTools_HXX
#define _TAMCparTools_HXX
/*!
  \file
  \brief   Declaration of TAMCparTools.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>

#include "TString.h"

#include "TAGpara.hxx"


//##############################################################################

class TAMCparTools : public TAGpara {
      
private:
   static map<TString, Int_t> fgkG4PartNameToFlukaId;
   static map<Int_t, TString> fgFlukaIdToPartName;
   static map<TString, Int_t> fgkUpdatePdgMap;
   static map<TString, double> fgkUpdatePdgMass;
   static map<TString, double> fgkUpdatePdgCharge;

public:
   TAMCparTools();
    virtual ~TAMCparTools();
  
public:
   // Get particle name from Fluka Id
   static const Char_t* GetFlukaPartName(Int_t id);
  
  // Check if particle exits
  static        Bool_t  IsParticleDefined( string partName );
  // return PDG code
  static        Int_t   GetPdgCode( string partName );
  // Update PDG
  static        void    UpdatePDG();
  
  //  retiurn Fluka id from G4 particle name
  static        Int_t   GetFlukaId(TString g4PartName)    { return fgkG4PartNameToFlukaId[g4PartName]; }
  
  //! return mass in atomic unit
  static        Double_t GetIsotopMass(TString iso)       { return fgkUpdatePdgMass[iso];              }

  //! return mass in GeV/c^2
  static        Double_t GetIsotopePdgMass(TString iso)   { return fgkUpdatePdgMass[iso]*0.9315;       }
  
  //! return atomic charge
  static        Double_t GetIsotopCharge(TString iso)     { return fgkUpdatePdgCharge[iso];            }
  
  //! return atomic charge in quark value (1/3q)
  static        Double_t GetIsotopePdgCharge(TString iso) { return fgkUpdatePdgCharge[iso]*3.;         }

   ClassDef(TAMCparTools,1)
};

#endif
