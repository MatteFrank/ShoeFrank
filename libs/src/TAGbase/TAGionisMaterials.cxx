
/*!
 \file TAGionisMaterials.cxx
 \brief   Implementation of TAGionisMaterials.
 */

#include "TAGionisMaterials.hxx"

#include "TGeoManager.h"
#include "TAGgeoTrafo.hxx"


const TString TAGionisMaterials::fgkMeanExcitationEnergy   = "kExcitation";
const TString TAGionisMaterials::fgkShellCorrectionVector  = "kShellCorrection";
const TString TAGionisMaterials::fgkTaul                   = "kTaul";
const TString TAGionisMaterials::fgkBirks                  = "kBirks";

/*!
 \class TAGionisMaterials
 \brief Class handling material ionization properties **
 */

ClassImp(TAGionisMaterials);

//______________________________________________________________________________
TAGionisMaterials::TAGionisMaterials(TGeoMaterial* mat)
 : TAGobject(),
   fMaterial(mat),
   fMeanExcitationEnergy(0.),
   fShellCorrectionVector(new Double_t[3]),
   fTaul(0.),
   fBirks(0.)
{
}

//______________________________________________________________________________
TAGionisMaterials::TAGionisMaterials()
: TAGobject(),
   fMaterial(0x0),
   fMeanExcitationEnergy(0.),
   fShellCorrectionVector(new Double_t[3]),
   fTaul(0.),
   fBirks(0.)
{
}
//______________________________________________________________________________
TAGionisMaterials::~TAGionisMaterials()
{
   delete[] fShellCorrectionVector;
}

//______________________________________________________________________________
void TAGionisMaterials::AddMeanExcitationEnergy(Double_t value)
{
   
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   // Feature available only for root version >= 6.17
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   if (!fMaterial) {
      Error("AddMeanExcitationEnergy()", "Material nor defined");
      return;
   }
   Bool_t error;
   TString ref = fgkMeanExcitationEnergy + fMaterial->GetName();
   gGeoManager->GetProperty(ref.Data(), &error);
   
   if (error == true) {
      gGeoManager->AddProperty(ref.Data(), value);
      fMaterial->AddConstProperty(GetMeanExcitationEnergyName(), ref.Data());
   }
   
#else
   Warning("AddMeanExcitationEnergy()", "Method not compatible with present root version, need version >= 6.17");
   
#endif
   
}

//______________________________________________________________________________
void TAGionisMaterials::AddBirksFactor(Double_t value)
{
   
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   // Feature available only for root version >= 6.17
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   if (!fMaterial) {
      Error("AddBirksFactor()", "Material nor defined");
      return;
   }
   
   TString ref = fgkBirks + fMaterial->GetName();
   gGeoManager->AddProperty(ref.Data(), value);
   
   fMaterial->AddConstProperty(GetBirksName(), ref.Data());
   
#else
   Warning("AddMeanExcitationEnergy()", "Method not compatible with present root version, need version >= 6.17");
   
#endif
   
}



