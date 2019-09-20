// Macro to build the geometry file for CA module position/angle
// September 2019, Ch. Finck

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include "TMath.h"
#include "TString.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoTrd2.h"
#include "TColor.h"
#include "TList.h"
#include "TMath.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"

#endif

TVector3 GetAngles(TGeoMatrix* rot)
{
   // valable only for angle +/- 90º
   const Double_t* mat = rot->GetRotationMatrix();
   
   Double_t angleX = TMath::ATan2(mat[7], mat[8])*TMath::RadToDeg();
   Double_t angleY = TMath::ASin(-mat[6])*TMath::RadToDeg();
   Double_t  angleZ = TMath::ATan2(mat[0], mat[3])*TMath::RadToDeg();
   
   if (angleZ < 180) angleZ = 90 - angleZ;
   else angleZ -= 90;
   
   return TVector3(angleX, angleY, angleZ);
}

// main
void BuildCaGeoFile(TString fileOutName = "TACAmodule.map")
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoMaterial* matBGO = TAGmaterials::Instance()->CreateMaterial("BGO", 7.13);
   TGeoMaterial* matEJ_232 = TAGmaterials::Instance()->CreateMaterial("EJ232", 1.023);
   TGeoMaterial* matAir = TAGmaterials::Instance()->CreateMaterial("AIR", 1.2e-3);
   
   TGeoMedium *medAir = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
   TGeoMedium *medAl  =  (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("EJ232");
   TGeoMedium *medBGO = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("BGO");
   
   TGeoVolume *top = gGeoManager->GetTopVolume();
   if (!top) {
      top = gGeoManager->MakeBox("TOP", medAir, 100, 100, 100);
      gGeoManager->SetTopVolume(top);
   }
   
   TGeoVolumeAssembly * detector = new TGeoVolumeAssembly("CALO");
   
   // config values (TODO: get from file)
   // half dimensions of BGO crystal
   double xdim1 = 1.; // fCaloSize[0]/2.0;
   double xdim2 = 1.45; // fCaloSize1[0]/2.0;
   double ydim1 = xdim1; // assume squart
   double ydim2 = xdim2;
   double zdim = 12.;
   
   // half open angle of the truncate piramide
   double deltaX = (xdim2 - xdim1);
   double trp_hipot = TMath::Sqrt( zdim * zdim * 4  + deltaX * deltaX );
   double alfa = TMath::ASin (deltaX / trp_hipot);
   //alfa += 0.0005; // small increase to correct the 1mm between the crystals
   double alfa_degree = alfa  * 180./ TMath::Pi();
   //cout << "alfa_degree " << alfa_degree << endl;
   
   // compute some values of the full piramid dimensions
   double piramid_hipot = xdim2 / TMath::Sin(alfa);
   
   //cout << "piramid_hipot " << piramid_hipot << endl;
   double piramid_base = piramid_hipot * TMath::Cos(alfa);
   double piramid_base_c = piramid_base - zdim; // distance from center to the piramid vertex
   
   // translation of crystal center after 2*alfa rotation about vetex piramid
   double delta  = 0.1; // 1mm between crystals
   double deltax = delta * TMath::Cos(alfa*2);
   double deltaz = - delta * TMath::Sin(alfa*2);
   double posx = TMath::Sin(alfa*2) * piramid_base_c + deltax;
   double posz = TMath::Cos(alfa*2) * piramid_base_c + deltaz;
   
   // set rotations/translations
   Float_t dirX[] = {1, 1, 0,-1,-1,-1, 0, 1, 0};
   Float_t dirY[] = {0, 1 ,1, 1, 0,-1,-1,-1, 0};
   Float_t angX[] = {0,-1,-1,-1, 0, 1, 1, 1, 0};
   Float_t angY[] = {1, 1, 0,-1,-1,-1, 0, 1, 0};
   
   Int_t crystalsNperModule = 9;
   TVector3 crystalAng[crystalsNperModule];
   TVector3 crystalPos[crystalsNperModule];

   for (Int_t i = 0; i < crystalsNperModule; ++i) {
      crystalAng[i] = TVector3(alfa_degree * 2 * angX[i], alfa_degree * 2 * angY[i], 0);
      crystalPos[i] = TVector3(posx*dirX[i], posx*dirY[i], posz - piramid_base_c);
   }
   
   // create on BGO crystal
   TGeoVolume *caloCristal = gGeoManager->MakeTrd2("caloCrystal", medBGO, xdim1, xdim2, ydim1, ydim2, zdim);
   caloCristal->SetLineColor(kAzure+6);
   caloCristal->SetFillColor(kAzure+6);
   //  caloCristal->SetTransparency(.5);
   
   ////////////   MODULE
   ////////////   Create a 3x3 module by assembly 3 ROWs
   TGeoVolumeAssembly * module = new TGeoVolumeAssembly("CAL_MOD");
   
   TGeoRotation* rot     = new TGeoRotation();
   TGeoTranslation* tras = new TGeoTranslation();
   
   for (Int_t i = 0; i < crystalsNperModule; ++i) {
      TGeoVolume* crystal   = gGeoManager->MakeTrd2("Crystal", medBGO, xdim1, xdim2, ydim1, ydim2, zdim);
      crystal->SetLineColor(kAzure+6);
      crystal->SetFillColor(kAzure+6);
      
      rot->Clear();
      rot->RotateX(crystalAng[i].X());
      rot->RotateY(crystalAng[i].Y());
      tras->SetTranslation(crystalPos[i].X(), crystalPos[i].Y(), crystalPos[i].Z());
      module->AddNode(crystal, i, new TGeoCombiTrans(*tras, *rot));
   }
   
   //---- Crystal Support as truncate piramid (just for visual propouses)
   double xdimS1 = ((xdim1+xdim2)*0.5) * 3 + 0.2 ;
   double xdimS2 = xdim2 * 3 + 0.2;
   double ydimS1 = xdimS1;
   double ydimS2 = xdimS2;
   double zdimS = zdim / 2. - 0.1;
   
   TGeoVolume* support = gGeoManager->MakeTrd2("modSup", medAl, xdimS1, xdimS2, ydimS1, ydimS2, zdimS);
   support->SetLineColor(kGray);
   support->SetLineColor(kGray);
   module->AddNode(support, 0, new TGeoTranslation(0, 0, zdimS - 0.1));
   
   
   Int_t kModule = 32;
   Int_t nModule = 0;

   
   ////////////////////////////////////////////////////////////
   ////////////    create Detector
   
   FILE* fp = fopen(fileOutName.Data(), "w");
   
   fprintf(fp,"ModulesN:       %d\n", kModule);
           
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the modules used in the run\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   
   // displacement of module center
   double deltaM  = 0.2; // 2mm between MODULES
   double deltaMx = deltaM * TMath::Cos(alfa*3);
   double deltaMz = - deltaM * TMath::Sin(alfa*3);
   double posModx = TMath::Sin(alfa*3) * piramid_base_c + deltaMx;
   double posModz = TMath::Cos(alfa*3) * piramid_base_c + deltaMz;
   
   // set rotations/translation (left/right/up/down)
   TGeoRotation * rotUpDet = new TGeoRotation (); rotUpDet->RotateX(-alfa_degree * 3);
   TGeoRotation * rotDnDet = new TGeoRotation (); rotDnDet->RotateX(alfa_degree * 3);
   TGeoRotation * rotPDet  = new TGeoRotation (); rotPDet->RotateY(alfa_degree * 3);
   TGeoRotation * rotNDet  = new TGeoRotation (); rotNDet->RotateY(-alfa_degree * 3);
   TGeoTranslation * trasUpDet = new TGeoTranslation(0, posModx, posModz - piramid_base_c );
   TGeoTranslation * trasDnDet = new TGeoTranslation(0, -posModx, posModz - piramid_base_c );
   TGeoTranslation * trasPDet  = new TGeoTranslation(posModx, 0, posModz - piramid_base_c );
   TGeoTranslation * trasNDet  = new TGeoTranslation(-posModx, 0, posModz - piramid_base_c );
   
   TGeoHMatrix* mat = 0x0;
   TVector3 angles;
   Double_t* trans;
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) ;
   detector->AddNode(module, 1, mat);
   
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);

   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet, *rotUpDet) );
   detector->AddNode(module, 2,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet, *rotDnDet) );
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet, *rotUpDet) );
   detector->AddNode(module, 4,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   // displacement of module center (SECOND LEVEL)
   deltaM  = 0.6;
   deltaMx = deltaM * TMath::Cos(alfa*9);
   deltaMz = - deltaM * TMath::Sin(alfa*9);
   posModx = TMath::Sin(alfa*9) * (piramid_base_c + 1) + deltaMx;
   posModz = TMath::Cos(alfa*9) * (piramid_base_c + 1) + deltaMz;
   
   TGeoRotation * rotUpDet2 = new TGeoRotation (); rotUpDet2->RotateX(-alfa_degree * 9);
   TGeoRotation * rotDnDet2 = new TGeoRotation (); rotDnDet2->RotateX(alfa_degree * 9);
   TGeoRotation * rotPDet2  = new TGeoRotation (); rotPDet2->RotateY(alfa_degree * 9);
   TGeoRotation * rotNDet2  = new TGeoRotation (); rotNDet2->RotateY(-alfa_degree * 9);
   TGeoTranslation * trasUpDet2 = new TGeoTranslation(0, posModx, posModz - piramid_base_c );
   TGeoTranslation * trasDnDet2 = new TGeoTranslation(0, -posModx, posModz - piramid_base_c );
   TGeoTranslation * trasPDet2  = new TGeoTranslation(posModx, 0, posModz - piramid_base_c );
   TGeoTranslation * trasNDet2  = new TGeoTranslation(-posModx, 0, posModz - piramid_base_c );
   
   TGeoRotation * rotLeftZ  = new TGeoRotation (); //rotLeftZ->RotateZ(-0.001);
   TGeoRotation * rotRightZ = new TGeoRotation (); //rotRightZ->RotateZ(0.001);
   rotLeftZ->RotateZ(-0.30);
   rotRightZ->RotateZ(0.30);
   // 2 modules left / 2 right
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet, *rotDnDet) * TGeoHMatrix(*rotLeftZ) );
   detector->AddNode(module, 5, mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet, *rotUpDet) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module, 6,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet, *rotDnDet) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module, 7,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet, *rotUpDet) * TGeoHMatrix(*rotLeftZ) );
   detector->AddNode(module, 8,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   rotLeftZ->RotateZ(-0.10);
   rotRightZ->RotateZ(0.10);
   // 2  modules top / 2 bottom
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ) ) ;
   detector->AddNode(module, 9,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ)) ;
   detector->AddNode(module,10, mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,11,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ) ) ;
   detector->AddNode(module,12,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   // 4 diagonals
   rotLeftZ->RotateZ(-0.3);
   rotRightZ->RotateZ(0.3);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,13,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,14,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,15,  mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat =  new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,16, mat);
   detector->AddNode(module, 3,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   // displacement of module center (THIRD LEVEL)
   deltaM  = 0.9;
   deltaMx = deltaM * TMath::Cos(alfa*15);
   deltaMz = - deltaM * TMath::Sin(alfa*15);
   posModx = TMath::Sin(alfa*15) * (piramid_base_c + 2.5) + deltaMx;
   posModz = TMath::Cos(alfa*15) * (piramid_base_c + 2.5) + deltaMz;
   
   TGeoRotation * rotUpDet3 = new TGeoRotation (); rotUpDet3->RotateX(-alfa_degree * 15);
   TGeoRotation * rotDnDet3 = new TGeoRotation (); rotDnDet3->RotateX(alfa_degree * 15);
   TGeoRotation * rotPDet3  = new TGeoRotation (); rotPDet3->RotateY(alfa_degree * 15);
   TGeoRotation * rotNDet3  = new TGeoRotation (); rotNDet3->RotateY(-alfa_degree * 15);
   TGeoTranslation * trasUpDet3 = new TGeoTranslation(0, posModx, posModz - piramid_base_c );
   TGeoTranslation * trasDnDet3 = new TGeoTranslation(0, -posModx, posModz - piramid_base_c );
   TGeoTranslation * trasPDet3  = new TGeoTranslation(posModx, 0, posModz - piramid_base_c );
   TGeoTranslation * trasNDet3  = new TGeoTranslation(-posModx, 0, posModz - piramid_base_c );
   
   // 4 modules left  and 4 right
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasDnDet, *rotDnDet) );
   detector->AddNode(module,17,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasUpDet, *rotUpDet) ) ;
   detector->AddNode(module,18, mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) ;
   detector->AddNode(module,19, mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasUpDet, *rotUpDet) );
   detector->AddNode(module,20, mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   // 4 top / 4 bottom
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotLeftZ)) ;
   detector->AddNode(module,25, mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,26,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,27,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,28,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   rotLeftZ->RotateZ(-0.45);
   rotRightZ->RotateZ(0.45);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,21,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,22,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,23,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,24,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,29,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,30,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotRightZ));
   detector->AddNode(module,31,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   mat = new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotLeftZ));
   detector->AddNode(module,32,  mat);
   trans = mat->GetTranslation();
   angles = GetAngles(mat);
   fprintf(fp,"ModuleId:     %d\n", nModule++);
   fprintf(fp,"PositionX:  %5.1f   PositionY:   %5.1f   PositionZ:   %5.1f\n", trans[0], trans[1], trans[2]);
   fprintf(fp,"TiltX:      %5.1f   TiltY:       %5.1f   TiltZ:       %5.1f\n", angles[0], angles[1], angles[2]);
   
   fclose(fp);
   
   if (kModule != nModule+1)
      Warning("BuildCaGeoFile", "number of expected module (%d) different from the one created (%d)", kModule, nModule);
   
   // Add module to top volumen
   top->AddNode(detector, 1);
   
   top->Draw();
}


