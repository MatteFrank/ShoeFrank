#ifndef _TAPLparGeo_HXX
#define _TAPLparGeo_HXX
/*!
 \file
 \version $Id: TAPLparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
 \brief   Declaration of TAPLparGeo.
 */
/*------------------------------------------+---------------------------------*/
#include "TGeoVolume.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"

//##############################################################################

class TAPLparGeo : public TAGparTools {
public:
   
   TAPLparGeo();
   virtual         ~TAPLparGeo();
   
   TVector3        GetSize()      const { return fSize;     }
   TString         GetMaterial()  const { return fMaterial; }
   Float_t         GetDensity()   const { return fDensity;  }

   Bool_t          FromFile(const TString& name = "");

   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   void            DefineMaterial();

   TGeoVolume*     BuildStartCounter(const char *bmName = GetBaseName());

public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   
private:
   TVector3  fSize;
   TString   fMaterial;
   Float_t   fDensity;
   TString   fkDefaultGeoName;  // default par geo file name
   Int_t     fDebugLevel;

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
   ClassDef(TAPLparGeo,1)
};

#endif
