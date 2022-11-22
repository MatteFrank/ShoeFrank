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
   
private:
   Int_t     fModulesN;
   TVector3  fSize;
   TString   fMaterial;
   Float_t   fDensity;
   Float_t   fIonisMat;
      
   TString   fkDefaultGeoName;  // default par geo file name
   
   struct ModuleParameter_t : public  TObject {
      Int_t     ModuleIdx;   ///< Module index
      TVector3  Position;    ///< current position
      TVector3  Tilt;        ///< current tilt angles
   };
   ModuleParameter_t  fModuleParameter[128]; ///< mdoule  parameters
   ///<
private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   
public:
   TANEparGeo();
   virtual       ~TANEparGeo();
   
   TVector3        GetSize()            const { return fSize;          }
   TString         GetMaterial()        const { return fMaterial;      }
   Float_t         GetDensity()         const { return fDensity;       }
   Float_t         GetMeanExc()         const { return fIonisMat;      }

   Bool_t          FromFile(const TString& name = "");

   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   //! Get Sensor parameter
   ModuleParameter_t& GetModulePar(Int_t idx) { return fModuleParameter[idx]; }

public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   
   ClassDef(TANEparGeo,1)
};

#endif
