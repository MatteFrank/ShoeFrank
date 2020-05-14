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

#include "TAGmaterials.hxx"

#include "TGeoManager.h"
#include "TObjString.h"

#include "TAGgeoTrafo.hxx"

        TAGmaterials* TAGmaterials::fgInstance    = 0;
              TString TAGmaterials::fgkWhat       = "                                                                                            ";
                 Int_t TAGmaterials::fgkWhatWidth = 10;

map<TString, TString> TAGmaterials::fgkCommonName = {{"SmCo", "Sm2Co17"}, {"Polyethy", "C2H4"}, {"Kapton", "C22H10N2O5"}, {"Epoxy", "C18H19O3"},
                                                     {"BGO", "Bi4Ge3O12"}, {"SiCFoam", "SiC/AIR"}, {"TUNGSTEN", "W"}, {"Graphite", "C3"}, {"EJ232", "C9H10"},
                                                     {"EJ228", "C9H10"}, {"Mylar", "C10H8O4"}  };

map<TString, Int_t>   TAGmaterials::fgkLowMat     = {{"Graphite", 1}};

ClassImp(TAGmaterials);

//______________________________________________________________________________
TAGmaterials::TAGmaterials()
 : TAGobject(),
   fTable(0x0),
   fDegugLevel(0)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   fTable = gGeoManager->GetElementTable();
   CreateDefaultMaterials();
}

//______________________________________________________________________________
TAGmaterials::~TAGmaterials()
{
}

//______________________________________________________________________________
TAGmaterials* TAGmaterials::Instance()
{
   if (fgInstance == 0)
      fgInstance = new TAGmaterials();

   return fgInstance;
}

//______________________________________________________________________________
TString TAGmaterials::FindByValue(TString value)
{
   for (auto& pair : fgkCommonName)
      if (pair.second == value)
         return pair.first;
   
   return TString("");
}

//______________________________________________________________________________
TGeoMaterial* TAGmaterials::CreateMaterial(TString name, Float_t density,
                                            Float_t temperature, Float_t pressure)
{
   fIsotope.clear();
   fIsotopeWeight.clear();
   TGeoMaterial* mat = 0x0;
   TGeoMedium* med   = 0x0;

   TString formula = name;

   // check if already define
   if ( (mat = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->FindObject(name.Data())) != 0x0 )
      return mat;

   
   if (fgkCommonName[name] != "") {
      formula = fgkCommonName[name];
      Info("CreateMaterial()", "Using formula %s for material %s", formula.Data(), name.Data());
   }

   GetIsotopes(formula);

   if (fIsotope.size() == 1 && fIsotopeWeight[0] == 1) { // single element
      TGeoElement* element = fTable->FindElement(fIsotope[0].Data());
      mat = new TGeoMaterial(formula.Data(), element, density);
      med = new TGeoMedium(formula.Data(), mat->GetIndex(), mat);

      return mat;
   } else {
      mat =  new TGeoMixture(name.Data(), fIsotope.size(), density);
   }
   
   // mixture
   for (Int_t i = 0; i < fIsotope.size(); ++i) {
      if (FootMcDebugLevel(1))
         printf("%s %g\n", fIsotope[i].Data(), fIsotopeWeight[i]);
      TGeoElement* element = fTable->FindElement(fIsotope[i].Data());
      if (element == 0x0) {
         Error("CreateMixture()", "Unknown element %s in formula %s\n", fIsotope[i].Data(), formula.Data());
         return 0x0;
      }
      TString tmp = Form("%g", fIsotopeWeight[i]);
      if (tmp.Contains(".")) {
         ((TGeoMixture*)mat)->AddElement(element, fIsotopeWeight[i]);
      } else
         ((TGeoMixture*)mat)->AddElement(element, TMath::Nint(fIsotopeWeight[i]));

   }
   
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(name.Data())) == 0x0 )
      med = new TGeoMedium(name, mat->GetIndex(), mat);

   mat->SetTemperature(temperature);
   mat->SetPressure(pressure);
   return mat;
}

//______________________________________________________________________________
TGeoMixture* TAGmaterials::CreateMixture(TString formula, const TString densities, const TString prop, Float_t density)
{
   // create mixture with differents materials
   // the material, densities and proportion are seperated by a /
   TGeoMedium*  med = 0x0;
   
   vector<TString> listMat = GetStrings(formula);
   Float_t compDensity[listMat.size()];
   Float_t compProp[listMat.size()];
   
   GetCoeff(densities, compDensity, listMat.size());
   GetCoeff(prop, compProp, listMat.size());
   
   TGeoMixture* mix = 0x0;
   
   // check if already define
   if ( (mix = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(formula.Data())) != 0x0 )
      return mix;
   
   mix = new TGeoMixture(formula.Data(), listMat.size(), density);
   
   for (Int_t i = 0; i < listMat.size(); ++i) {
      if (FootMcDebugLevel(1))
         printf("%s %f %f\n", listMat[i].Data(), compDensity[i], compProp[i]);
      
      TString name;
      if ((name = FindByValue(listMat[i])) != "") {
         Info("CreateMaterial()", "Using formula %s for material %s", name.Data(), listMat[i].Data());
         listMat[i] = name;
      }

      TGeoMaterial* subMat = 0x0;
      if ( (subMat = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(listMat[i].Data())) == 0x0 )
         subMat = TAGmaterials::Instance()->CreateMaterial(listMat[i].Data(), compDensity[i]);
      
      if (subMat == 0x0) {
         Error("CreateMixture()", "Unknown material %s in formula %s\n", listMat[i].Data(), formula.Data());
         return 0x0;
      }
      mix->AddElement(subMat, compProp[i]);
   }
   
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(formula.Data())) == 0x0 )
      med = new TGeoMedium(formula.Data(), mix->GetIndex(), mix);
   
   return mix;
}

//_____________________________________________________________________________
vector<TString> TAGmaterials::GetStrings(TString key, const Char_t delimiter)
{
   vector<TString> coeff;
   TObjArray* list = key.Tokenize(delimiter);
   
   for (Int_t k = 0; k < list->GetEntries(); k++) {
      TObjString* obj = (TObjString*)list->At(k);
      TString item = obj->GetString();
      Int_t pos = item.Length();
      TString value(item(0, pos));
      coeff.push_back(TString(value));
   }
   
   if (FootMcDebugLevel(2)) {
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         cout << coeff[i] << " ";
      }
      cout << endl;
   }
   delete list;
   
   return coeff;
}

//_____________________________________________________________________________
void TAGmaterials::GetCoeff(TString key, Float_t* coeff, Int_t size,  const Char_t delimiter)
{
   TObjArray* list = key.Tokenize(delimiter);
   if (list->GetEntries() != size)
      Error("ReadItem()","wrong tokenize for [%s] with size %d", key.Data(), size);
   
   for (Int_t k = 0; k < list->GetEntries(); k++) {
      TObjString* obj = (TObjString*)list->At(k);
      TString item = obj->GetString();
      Int_t pos = item.Length();
      TString value(item(0, pos));
      coeff[k] = item.Atof();
   }
   
   if (FootMcDebugLevel(2)) {
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         cout << coeff[i] << " " ;
      }
      cout << endl;
   }
   delete list;
}

//______________________________________________________________________________
void TAGmaterials::GetIsotopes(const TString formula)
{
   
   TString key = formula;
   Int_t length = formula.Length();
   
   Int_t i = 0;
   while (i < length) {
      TString subFormula = GetSubFormula(key);
      
      Int_t subLength = subFormula.Length();
      i += subLength;

      key = formula(i, length);
      
      GetIsotopeAndWeight(subFormula);
   }
}

//______________________________________________________________________________
TString TAGmaterials::GetSubFormula(const TString formula)
{
   
   TString key = formula;
   Int_t length = key.Length();
   
   Int_t i = 0;
   
   while (i < length) {
      i++;
      if (isupper(key[i])) break;
   }
   
   TString tmp = key(0, i);
   
   return tmp;
}

//______________________________________________________________________________
void TAGmaterials::GetIsotopeAndWeight(const TString formula)
{
   
   TString key = formula;
   Int_t length = key.Length();
   
   Int_t i = 0;
   
   while (i < length) {
      if (isdigit(key[i])) break;
      i++;
   }

   // Isotope
   TString sIsotope = key(0, i);
   
   // Weight
   TString sWeight = key(i, length);
   
   Float_t weight  = sWeight.Atof();
   if (weight < 1) weight = 1.;
   
   if (fDegugLevel > 0)
      printf("%s %g\n", sIsotope.Data(), weight);
   
   fIsotope.push_back(sIsotope);
   fIsotopeWeight.push_back(weight);
}

//______________________________________________________________________________
void TAGmaterials::CreateDefaultMaterials()
{
   // create vacuum material
   TGeoMaterial* mat = 0x0;;
   TGeoMedium*   med = 0x0;
   
   const Char_t* matNameVacuum = "VACUUM";
   if ( (mat = (TGeoMaterial*)gGeoManager->GetListOfMaterials()->FindObject(matNameVacuum)) == 0x0 )
      mat  = new TGeoMaterial(matNameVacuum, 0, 0, 0);
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matNameVacuum)) == 0x0 )
      med  = new TGeoMedium(matNameVacuum, 1, mat);

   
   // create air mixture
   TGeoMixture* mix = 0x0;;
   
   const Char_t* mixNameAir = "AIR";
   if ( (mix = (TGeoMixture*)gGeoManager->GetListOfMaterials()->FindObject(mixNameAir)) == 0x0 ) {
      
      TGeoElement* elementO = fTable->GetElement(8);
      TGeoElement* elementN = fTable->GetElement(7);
      
      mix = new TGeoMixture(mixNameAir, 2, 1.29e-3);
      mix->AddElement(elementO, 0.79);
      mix->AddElement(elementN, 0.21);
   }
   
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(mixNameAir)) == 0x0 )
      med = new TGeoMedium(mixNameAir,1,mix);
   
  }

//______________________________________________________________________________
// Emulate automatically FLUKA file
// Taking into account the MATERIAL, COMPOUND and LOW-MAT
// Aligned variables on the dedicated column
void TAGmaterials::SaveFileFluka(const TString filename)
{
   
   TList* list = gGeoManager->GetListOfMaterials();
   
   FILE* fp = fopen(filename.Data(), "w");
   
   fprintf(fp, "TITLE\n");
   fprintf(fp, "FOOT setup\n");
   fprintf(fp, "* ****************************************************************************\n");
   fprintf(fp, "GLOBAL         9999.                                      1.\n");
   fprintf(fp, "DEFAULTS                                                              PRECISIO\n");
   fprintf(fp, "PHYSICS           3.                                                  EVAPORAT\n");
   fprintf(fp, "IONTRANS    HEAVYION\n");
   fprintf(fp, "RADDECAY          2.                  1.\n");
   fprintf(fp, " * ******************************************************************************\n");
   fprintf(fp, "*                           GENERAL & PRIMARY                                  *\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* command| what(1) | what(2) | what(3) | what(4) | what(5) | what(6) | SDUM    |\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* @@@START GENERATED, DO NOT MODIFY:GENERAL@@@ *********************************\n");
   fprintf(fp, "PHYSICS           1.                                                  COALESCE\n");
   fprintf(fp, "BEAM       -0.200000       0.0       0.0 -0.480000 -0.480000        1.HEAVYION\n");
   fprintf(fp, "HI-PROPE          8.       16.\n");
   fprintf(fp, "BEAMPOS          0.0       0.0      -30.       0.0       0.0\n");
   fprintf(fp, "EMFCUT           -1.        1.               BLACK  @LASTREG        1.\n");
   fprintf(fp, "EMFCUT           -1.        1.        1.  BLCKHOLE  @LASTMAT        1.PROD-CUT\n");
   fprintf(fp, "DELTARAY          1.                      BLCKHOLE  @LASTMAT        1.\n");
   fprintf(fp, "PAIRBREM         -3.                      BLCKHOLE  @LASTMAT\n");
   fprintf(fp, "* @@@END GENERATED:GENERAL@@@ **************************************************\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "*                          GEOMETRY                                            *\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "GEOBEGIN                             15.                              COMBNAME\n");
   fprintf(fp, "foot.geo\n");
   fprintf(fp, "GEOEND\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "*                          MEDIA                                               *\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* command| what(1) | what(2) | what(3) | what(4) | what(5) | what(6) | SDUM    |\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* *** Added manually\n");
   fprintf(fp, "MATERIAL          0.        0.                                        BLCKHOLE\n");

   
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      
      TGeoMixture *mix = (TGeoMixture *)gGeoManager->GetListOfMaterials()->At(i);
      
      Int_t nElements = mix->GetNelements();
      
      // Single material
      if (nElements == 1) {
         
         if (fgkLowMat[mix->GetName()] == 0) {
            
            TGeoElement *element = mix->GetElement(0);
            if (fPrintedElt[element->GetName()] == 1)
               break;

            fPrintedElt[element->GetName()] = 1;
            TString cmd;
            if ( mix->GetDensity() > 0)
               cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) +  PrependFluka(Form("%g",mix->GetDensity())) +
                     PrependFlukaName(element->GetName(), 3);
            else
               cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) + PrependFlukaName(element->GetName(), 4);
            fprintf(fp, "%s\n", cmd.Data());
            
         } else { // LOW-MAT
            TGeoElement *element = mix->GetElement(0);
            if (fPrintedElt[element->GetName()] == 1)
               break;

            fPrintedElt[element->GetName()] = 1;
            
            TString cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) + PrependFlukaName(element->GetName(), 4);
            fprintf(fp, "%s\n", cmd.Data());

            cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g",mix->GetDensity()), 2) +  PrependFlukaName(mix->GetName(), 3);
            fprintf(fp, "%s\n", cmd.Data());
         }
      }
      
      // COMPOUND
      if (nElements != 1) {

         for (Int_t e = 0; e < nElements; ++e) {
            
            TGeoElement *element = mix->GetElement(e);
            
            if (fPrintedElt[element->GetName()] == 1)
               continue;
            
            TString cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%-g", element->A())) + PrependFlukaName(element->GetName(), 4);
            fprintf(fp, "%s\n", cmd.Data());
            fPrintedElt[element->GetName()] = 1;
         }

         TString cmd = AppendFluka("MATERIAL") + PrependFluka(Form("%g",mix->GetDensity()), 3) +  PrependFlukaName(mix->GetName(), 3);
         fprintf(fp, "%s\n", cmd.Data());

          cmd = AppendFluka("COMPOUND");
         
         for (Int_t e = 0; e < nElements; ++e) {
            
            TGeoElement *element = mix->GetElement(e);
            if (mix->GetNmixt() != 0x0) {
               
               if (fgkLowMat[mix->GetName()] != 0)
                  cmd += PrependFluka(Form("%d.", mix->GetNmixt()[e])) + PrependFluka(Form("%-g", mix->GetTemperature())) +  PrependFlukaName(element->GetName(), 2);
               else
                  cmd += PrependFluka(Form("%d.", mix->GetNmixt()[e])) + PrependFlukaName(element->GetName(), 1, -1);
               
            } else
               cmd += PrependFluka(Form("%-g", -mix->GetWmixt()[e])) + PrependFlukaName(element->GetName(), 1, -1);

         }
         if (nElements == 2)
            cmd += PrependFlukaName(mix->GetName(), 2);
         else if (nElements == 3)
            cmd += PrependFlukaName(mix->GetName(), 0);
         else
            Warning("SaveFileFluka()", "Number of element in the compund material is %d (max: 3)", nElements);

         fprintf(fp, "%s\n", cmd.Data());
      }
      
      
      // LOW-MAT
      if (fgkLowMat[mix->GetName()] != 0 && nElements == 1) {
         
         TGeoElement *element = mix->GetElement(0);
         
         if (mix->GetNmixt() != 0x0) {
            
            if (fgkLowMat[mix->GetName()] != 0) {
               
               TString cmd = AppendFluka("LOW-MAT") +  PrependFlukaName(mix->GetName(), 1, -1) + PrependFluka(Form("%4d.", element->Z())) + PrependFluka(Form("%4d.", -mix->GetNmixt()[0]));
               cmd += PrependFluka(Form("%-g", mix->GetTemperature())) +  PrependFlukaName(element->GetName(), 2);
               fprintf(fp, "%s\n", cmd.Data());
            }
         }
      }
      
   }
   
   fprintf(fp, "*\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "*                         MEDIA & MAGFIELD                                     *\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* command| what(1) | what(2) | what(3) | what(4) | what(5) | what(6) | SDUM    |\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* @@@START GENERATED, DO NOT MODIFY:MATERIAL&MAGFIELD@@@ ***********************\n");
   fprintf(fp, "ASSIGNMA    BLCKHOLE     BLACK                                                  \n");
   fprintf(fp, "ASSIGNMA         AIR       AIR                                                  \n");
   fprintf(fp, "ASSIGNMA       EJ232       STC                                                  \n");
   fprintf(fp, "ASSIGNMA          AL   BMN_SHI                                                  \n");
   fprintf(fp, "ASSIGNMA       Mylar  BMN_MYL0                                                  \n");
   fprintf(fp, "ASSIGNMA       Mylar  BMN_MYL1                                                  \n");
   fprintf(fp, "ASSIGNMA      Ar-CO2  BMN_C000  BMN_C017        1.                              \n");
   fprintf(fp, "ASSIGNMA      Ar-CO2  BMN_C100  BMN_C117        1.                              \n");
   fprintf(fp, "ASSIGNMA      Ar-CO2   BMN_GAS                                                  \n");
   fprintf(fp, "ASSIGNMA          AL   BMN_FWI                                                  \n");
   fprintf(fp, "ASSIGNMA          W    BMN_SWI                                                  \n");
   fprintf(fp, "ASSIGNMA    Polyethy    TARGET                            1.                    \n");
   fprintf(fp, "ASSIGNMA          SI     VTXP0     VTXS3        1.        1.                    \n");
   fprintf(fp, "ASSIGNMA          AL     ITRP2    ITRP73        1.                              \n");
   fprintf(fp, "ASSIGNMA       Epoxy     ITRP0    ITRP75        1.                              \n");
   fprintf(fp, "ASSIGNMA      Kapton     ITRP1    ITRP74        1.                              \n");
   fprintf(fp, "ASSIGNMA          SI    ITRP13   ITRS313        1.        1.                    \n");
   fprintf(fp, "ASSIGNMA     SiC/AIR     ITRP6    ITRP69        1.                              \n");
   fprintf(fp, "ASSIGNMA        SmCo   MAG_PM0                                                  \n");
   fprintf(fp, "ASSIGNMA          AL   MAG_CV0                                                  \n");
   fprintf(fp, "ASSIGNMA        SmCo   MAG_PM1                                                  \n");
   fprintf(fp, "ASSIGNMA          AL   MAG_CV1                                                  \n");
   fprintf(fp, "ASSIGNMA         AIR   MAG_AIR                            1.                    \n");
   fprintf(fp, "ASSIGNMA          SI     MSDS0     MSDS2        1.        1.                    \n");
   fprintf(fp, "ASSIGNMA         AIR       BOX                                                  \n");
   fprintf(fp, "ASSIGNMA       EJ232    SCN000    SCN121        1.                              \n");
   fprintf(fp, "ASSIGNMA         BGO    CAL000    CAL359        1.                              \n");
   fprintf(fp, "MGNFIELD    0.100000  0.000010                 0.0       0.0       0.0\n");
   fprintf(fp, "* @@@END GENERATED:MATERIAL&MAGFIELD@@@ ****************************************\n");
   fprintf(fp, "*\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "* command| idbflg  | FragTrg |Eth(Mev) |unused   |unused   |unused | SDUM      |\n");
   fprintf(fp, "* -------1---------2---------3---------4---------5---------6---------7---------8\n");
   fprintf(fp, "USRICALL         0.0        6.       0.1       0.0       0.0       0.0\n");
   fprintf(fp, "USERDUMP        100.       69.       0.0        1.                    Opt\n");
   fprintf(fp, "USROCALL\n");
   fprintf(fp, "RANDOMIZ          1.   593585.\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "*                          RUN                                                 *\n");
   fprintf(fp, "* ******************************************************************************\n");
   fprintf(fp, "START            10.\n");
   fprintf(fp, "STOP\n");

   fclose(fp);
}

//______________________________________________________________________________
TString TAGmaterials::AppendFluka(const Char_t* string, Int_t what)
{
   TString name = string;
   Int_t len = name.Length();
   
   Int_t blank = fgkWhatWidth*what - len;
   
   if (len > 10)
      Warning("AppendFluka()", "Digit number is %d (max: 10)", len);
      
   TString wBlank = fgkWhat(0, blank);
   
   name += wBlank;
   
   return name;
}

//______________________________________________________________________________
TString TAGmaterials::PrependFluka(const Char_t* string, Int_t what)
{
   TString name = string;
   
   if (name.IsDigit())
      name  += ".";
   
   Int_t len = name.Length();
   
   if (len > 10)
      Warning("PrependFluka()", "Digit number is %d (max: 10)", len);

   Int_t blank = fgkWhatWidth*what - len;
   TString wBlank = fgkWhat(0, blank);

   name.Prepend(wBlank);
   
   return name;
}

//______________________________________________________________________________
TString TAGmaterials::PrependFlukaName(const Char_t* string, Int_t what, Int_t align)
{
   TString name = string;
   
   Int_t len = 0;
   if (align != 1)
      len = name.Length();
   
   Int_t blank = fgkWhatWidth*what - len;
   TString wBlank = fgkWhat(0, blank);
   
   name.Prepend(wBlank);
   
   return name;
}





