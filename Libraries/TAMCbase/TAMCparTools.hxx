#ifndef _TAMCparTools_HXX
#define _TAMCparTools_HXX
/*!
  \file TAMCparTools.hxx
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
   static map<TString, Int_t>  fgkG4PartNameToFlukaId; ///< Map of G4 particle name to Fluka Id conversion
   static map<Int_t, TString>  fgFlukaIdToPartName;    ///< Map of Fluka Id to G4 particle name conversion
   static map<TString, Int_t>  fgkUpdatePdgMap;        ///< Fluka Pdg map
   static map<TString, double> fgkUpdatePdgMass;       ///< Update pdg mass map
   static map<TString, double> fgkUpdatePdgCharge;     ///< Update pdg charge map

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
  
  //! retiurn Fluka id from G4 particle name
  static        Int_t   GetFlukaId(TString g4PartName)    { return fgkG4PartNameToFlukaId.find(g4PartName) == fgkG4PartNameToFlukaId.end() ? -1 : fgkG4PartNameToFlukaId[g4PartName]; }
  
  //! return mass in atomic unit
  static        Double_t GetIsotopMass(TString iso)       { return fgkUpdatePdgMass.find(iso) == fgkUpdatePdgMass.end() ? -1.0 : fgkUpdatePdgMass[iso];          }

  //! return mass in GeV/c^2
  static        Double_t GetIsotopePdgMass(TString iso)   { return fgkUpdatePdgMass.find(iso) == fgkUpdatePdgMass.end() ? -1.0 : fgkUpdatePdgMass[iso]*0.9315;   }
  
  //! return atomic charge
  static        Double_t GetIsotopCharge(TString iso)     { return fgkUpdatePdgCharge.find(iso) == fgkUpdatePdgCharge.end() ? -1.0 : fgkUpdatePdgCharge[iso];    }
  
  //! return atomic charge in quark value (1/3q)
  static        Double_t GetIsotopePdgCharge(TString iso) { return fgkUpdatePdgCharge.find(iso) == fgkUpdatePdgCharge.end() ? -1.0 : fgkUpdatePdgCharge[iso]*3.; }

   ClassDef(TAMCparTools,1)
};

#endif
