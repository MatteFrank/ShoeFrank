//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//

#include "TAGionisMaterials.hxx"

#include "TGeoManager.h"
#include "TAGgeoTrafo.hxx"


const TString TAGionisMaterials::fgkMeanExcitationEnergy   = "kExcitation";
const TString TAGionisMaterials::fgkShellCorrectionVector  = "kShellCorrection";
const TString TAGionisMaterials::fgkTaul                   = "kTaul";
const TString TAGionisMaterials::fgkBirks                  = "kBirks";

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
   
   TString ref = fgkMeanExcitationEnergy + fMaterial->GetName();
   gGeoManager->AddProperty(ref.Data(), value);

   fMaterial->AddConstProperty(GetMeanExcitationEnergyName(), ref.Data());
   
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



