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

private:
  map<string, int> fPdgCodeMap;

public:
   TAMCparTools();
    virtual ~TAMCparTools();
  
   void UpdatePDG();
  
   Bool_t IsParticleDefined( string partName );
   Int_t GetPdgCode( string partName );

public:
   // Get Fluka id from G4 particle name
   static       Int_t   GetFlukaId(TString g4PartName) { return fgkG4PartNameToFlukaId[g4PartName]; }
   // Get particle name from Fluka Id
   static const Char_t* GetFlukaPartName(Int_t id);

   ClassDef(TAMCparTools,1)
};

#endif
