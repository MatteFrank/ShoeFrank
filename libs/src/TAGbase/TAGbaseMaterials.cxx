// ********************************************************************
//
// Base class to define materials
// author: M. Vanstalle

#include "TAGbaseMaterials.hxx"
#include "TAGionisMaterials.hxx"

#include "TGeoManager.h"
#include "TObjString.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"

TString TAGbaseMaterials::fgkWhat = "                                                                                            ";
Int_t TAGbaseMaterials::fgkWhatWidth = 10;

map<TString, TString> TAGbaseMaterials::fgkCommonName = {{"SmCo", "Sm2Co17"}, {"Polyethy", "C2H4"}, {"Kapton", "C22H10N2O5"}, {"Epoxy", "C18H19O3"},
                                                     {"BGO", "Bi4Ge3O12"}, {"SiCFoam", "SiC/AIR"}, {"TUNGSTEN", "W"}, {"Graphite", "C3"}, {"EJ232", "C9H10"},
                                                     {"EJ228", "C9H10"}, {"Mylar", "C10H8O4"}  };

map<TString, Int_t>   TAGbaseMaterials::fgkLowMat = {{"Graphite", 1}};


ClassImp(TAGbaseMaterials);

//______________________________________________________________________________
TAGbaseMaterials::TAGbaseMaterials()
 : TAGobject(),
   fIonisation(new TAGionisMaterials()),
   fTable(0x0),
   fDegugLevel(0)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   fTable = gGeoManager->GetElementTable();
}

//______________________________________________________________________________
TAGbaseMaterials::~TAGbaseMaterials()
{
   delete fIonisation;
}

//______________________________________________________________________________
TString TAGbaseMaterials::FindByValue(TString value)
{
   for (auto& pair : fgkCommonName)
      if (pair.second == value)
         return pair.first;
   
   return TString("");
}

//_____________________________________________________________________________
vector<TString> TAGbaseMaterials::GetStrings(TString key, const Char_t delimiter)
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
   
   if(FootDebugLevel(1)) {
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         cout << coeff[i] << " ";
      }
      cout << endl;
   }
   delete list;
   
   return coeff;
}

//_____________________________________________________________________________
void TAGbaseMaterials::GetCoeff(TString key, Float_t* coeff, Int_t size,  const Char_t delimiter)
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
   
   if(FootDebugLevel(1)) {
      for (Int_t i = 0; i < list->GetEntries(); ++i) {
         cout << coeff[i] << " " ;
      }
      cout << endl;
   }
   delete list;
}

//______________________________________________________________________________
void TAGbaseMaterials::GetIsotopes(const TString formula)
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
TString TAGbaseMaterials::GetSubFormula(const TString formula)
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
void TAGbaseMaterials::GetIsotopeAndWeight(const TString formula)
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
void TAGbaseMaterials::CreateDefaultMaterials()
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
       
       
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
       fIonisation->SetMaterial(mix);
       fIonisation->AddMeanExcitationEnergy(85.9e-6);
       
#else
       fIonisation->SetMeanExcitationEnergy(85.9e-6);
       // put it under Cerenkov since only this EM property is available
       mix->SetCerenkovProperties(fIonisation);
       
#endif
    }

    if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(mixNameAir)) == 0x0 )
        med = new TGeoMedium(mixNameAir,1,mix);
    
}

//______________________________________________________________________________
Bool_t TAGbaseMaterials::ReadFlukaDefMat()
{

  ifstream file;
 
  string buff;
   
  string fileName = Form("./config/FlukaMaterials.par");
  file.open( fileName.c_str(), ios::in );
  if ( !file.is_open() )
    cout<< "ERROR  -->  wrong input FlukaMaterials.par file:: " << fileName.c_str() << endl, exit(0);
  
  Int_t m = 0;
  while (!file.eof()) {
    getline(file,buff,'\n');
    if (buff[0] == '/')
      continue; 
    else{
      file >> fFlukaMat[m].Name >> fFlukaMat[m].Z >> fFlukaMat[m].Amean >> fFlukaMat[m].Density;      
      m++;      
    }
  }
   
  return true;


}



//______________________________________________________________________________
Int_t TAGbaseMaterials::GetFlukaMatId(Double_t Z)
{

  int m;
  
  for(int i=0; i<(sizeof(fFlukaMat)/sizeof(*fFlukaMat)); i++)
    if (fFlukaMat[i].Z == Z){
      m =i;
      break;
    }
  
  return m;
  
}

//______________________________________________________________________________
void TAGbaseMaterials::CheckFlukaMat(Double_t density, Double_t A, Double_t Z)
{

  int id = GetFlukaMatId(Z);
  
  if(fabs(fFlukaMat[id].Amean - A) > 0.00001*fFlukaMat[id].Amean)
    cout << "Warning: redefined FLUKA element: " << fFlukaMat[id].Name << " new mean mass number = " <<  A << " instead of " << fFlukaMat[id].Amean << endl; 
        
  if(fabs(fFlukaMat[id].Density - density) > 0.00001*fFlukaMat[id].Density)
    cout << "Warning: redefined FLUKA element: " << fFlukaMat[id].Name << " new density = " << density << " instead of " << fFlukaMat[id].Density << endl; 
   
  return;
  
}
