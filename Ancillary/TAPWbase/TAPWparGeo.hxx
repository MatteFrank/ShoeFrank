#ifndef _TAPWparGeo_HXX
#define _TAPWparGeo_HXX
/*!
 \file
 \version $Id: TAPWparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
 \brief   Declaration of TAPWparGeo.
 */
/*------------------------------------------+---------------------------------*/
#include "TGeoVolume.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"

//##############################################################################

class TAPWparGeo : public TAGparTools {
public:
   
   TAPWparGeo();
   virtual         ~TAPWparGeo();
   
   TVector3        GetSize1()      const { return fSize1;     }
   TString         GetMaterial1()  const { return fMaterial1; }
   Float_t         GetDensity1()   const { return fDensity1;  }

   TVector3        GetSize2()      const { return fSize2;     }
   TString         GetMaterial2()  const { return fMaterial2; }
   Float_t         GetDensity2()   const { return fDensity2;  }
   
   Bool_t          FromFile(const TString& name = "");

   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   void            DefineMaterial();

   TGeoVolume*     BuildPhoswich(const char *bmName = GetBaseName());

public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   
private:
   TVector3  fSize1;
   TString   fMaterial1;
   Float_t   fDensity1;
   
   TVector3  fSize2;
   TString   fMaterial2;
   Float_t   fDensity2;
   
   TString   fkDefaultGeoName;  // default par geo file name
   Int_t     fDebugLevel;

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
   ClassDef(TAPWparGeo,1)
};

#endif
