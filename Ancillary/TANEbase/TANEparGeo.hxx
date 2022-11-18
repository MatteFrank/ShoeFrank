#ifndef _TANEparGeo_HXX
#define _TANEparGeo_HXX
/*!
 \file
 \version $Id: TANEparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
 \brief   Declaration of TANEparGeo.
 */
/*------------------------------------------+---------------------------------*/
#include "TGeoVolume.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"

//##############################################################################

class TANEparGeo : public TAGparTools {
public:
   
   TANEparGeo();
   virtual         ~TANEparGeo();
   
   TVector3        GetSize()            const { return fSize;          }
   TString         GetMaterial()        const { return fMaterial;      }
   Float_t         GetDensity()         const { return fDensity;       }
 
   TVector3        GetWindowSize()      const { return fSizeWindow;    }
   TString         GetWindowMaterial()  const { return fMatWindow;     }
   Float_t         GetWindowDensity()   const { return fDensityWindow; }
   
   Bool_t          FromFile(const TString& name = "");

   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   
private:
   TVector3  fSize;
   TString   fMaterial;
   Float_t   fDensity;
   
   TVector3  fSizeWindow;
   TString   fMatWindow;
   Float_t   fDensityWindow;
   
   TString   fkDefaultGeoName;  // default par geo file name
   Int_t     fDebugLevel;

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
   ClassDef(TANEparGeo,1)
};

#endif
