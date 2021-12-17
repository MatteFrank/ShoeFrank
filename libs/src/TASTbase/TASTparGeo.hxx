#ifndef _TASTparGeo_HXX
#define _TASTparGeo_HXX
/*!
 \file
 \version $Id: TASTparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
 \brief   Declaration of TASTparGeo.
 */
/*------------------------------------------+---------------------------------*/
#include "TGeoVolume.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"

#include "TAGrecoManager.hxx"

//##############################################################################

class TASTparGeo : public TAGparTools {
public:
   
   TASTparGeo();
   virtual         ~TASTparGeo();
   
   TVector3        GetSize()      const { return fSize;     }
   TString         GetMaterial()  const { return fMaterial; }
   Float_t         GetDensity()   const { return fDensity;  }

   Bool_t          FromFile(const TString& name = "");
   void            DefineMaterial();

   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   TGeoVolume*     BuildStartCounter(const char *bmName = GetBaseName());

  //crossing regions
  Int_t          GetRegSensor(){TString regname("STC");return GetCrossReg(regname);};
  Int_t          GetRegMylar1(){TString regname("STCMYL1");return GetCrossReg(regname);};
  Int_t          GetRegMylar2(){TString regname("STCMYL2");return GetCrossReg(regname);};

  string PrintRotations();
  string PrintBodies();
  string PrintRegions();
  string PrintAssignMaterial(TAGmaterials *Material);
  string PrintSubtractBodiesFromAir();

public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   
private:
   TVector3  fSize;
   TString   fMaterial;
   Float_t   fDensity;
   TString   fkDefaultGeoName;  // default par geo file name

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
   ClassDef(TASTparGeo,1)
};

#endif
