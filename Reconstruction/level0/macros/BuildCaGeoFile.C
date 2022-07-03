// Macro to build the geometry file for CA module position/angle
// October 2019, Ch. Finck, E. Lopez L. Scavarda

// July 2022: Updated by E. Lopez

  
#if !defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__)
   // 
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
   #include "TString.h"
   #include "TStyle.h"
   #include "TCanvas.h"
   #include "TBox.h"
   #include "TH2F.h"
   #include "TText.h"

   #include "TAGgeoTrafo.hxx"
   #include "TAGmaterials.hxx"

   using namespace std;

#endif


///////////////////////////////////////////////////
   //////// Set config values

   // type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD, FIVE_MOD, SEVEN_MOD, FULL_DET_V1)
   // TODO: FULL_DET_V1. the new full setup need to be created
   TString fConfig_typegeo = "SEVEN_MOD";

   // half dimensions of BGO crystal
   double xdim1 = 1.;
   double xdim2 = 1.45;
   double ydim1 = xdim1; // assume square
   double ydim2 = xdim2;
   double zdim  = 12.;
   double delta = 0.1; // 1mm between crystals

///////////////////////////////////////////////////

TObjArray *    fListOfCrystals;  //!
TObjArray *    fListOfModules;   //!
TVector3       fcalSize;
const TString fgkDefaultCrysName = "caCrys";
const TString fDetectorName = "CA";
TString fgPath;

void EndGeometry(FILE* fp);

//##############################################################################
class NodeTrafo : public TObject {

public:
   TGeoNode  * node; //! pointer to node
   TGeoHMatrix mat;  // Matrix to convert from/to local to detector frame

   NodeTrafo(TGeoNode  * nodeIn, TGeoHMatrix matt) {
      node = nodeIn;
      mat = matt;
   }
};


///////////////////////////////////////////////////////////////////////////
// main
void BuildCaGeoFile(TString fileOutName = "./geomaps/testCALO/TACAdetector.geo")
{

   FILE* fp = fopen(fileOutName.Data(), "w");

   if (fp == NULL) { 
      cerr << "Can't open file: " << fileOutName.Data() << endl;
      return;
   }

   fgPath = gSystem->DirName(fileOutName.Data()); 

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"//\n");
   fprintf(fp,"// Geometry file for FOOT Calorimeter\n");
   fprintf(fp,"//\n");
   fprintf(fp,"// Created with macro: Reconstruction/level0/BuildCaGeoFile.C\n");
   fprintf(fp,"//\n");

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of Geometry \n");
   fprintf(fp,"// type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD, FIVE_MOD)\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"TypeGeo:        \"%s\"\n\n", fConfig_typegeo.Data());

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameters of the Crystals\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Material:      \"BGO\"\n");
   fprintf(fp,"Density:       7.13\n");
   fprintf(fp,"Excitation:    4.341e-4\n");

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the Crystals (cm)\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Widthfront:   %f   Heightfront:  %f    DummyLength:       0.\n", xdim1, ydim1);
   fprintf(fp,"Widthback :   %f	 Heightback :  %f	   Length:           %f\n", xdim2, ydim2, zdim);
   fprintf(fp,"Delta:        %f\n\n", delta);

   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      cout << TAGgeoTrafo::GetDefaultGeomName() << " : " << TAGgeoTrafo::GetDefaultGeomName() << std::flush << endl;
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }

   TGeoMaterial* matBGO = TAGmaterials::Instance()->CreateMaterial("BGO", 7.13);
   TGeoMaterial* matEJ_232 = TAGmaterials::Instance()->CreateMaterial("EJ232", 1.023);
   TGeoMaterial* matAir = TAGmaterials::Instance()->CreateMaterial("AIR", 1.2e-3);

   TGeoMedium *medAir = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
   TGeoMedium *medAl  = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("EJ232");
   TGeoMedium *medBGO = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("BGO");

   TGeoVolume *top = gGeoManager->GetTopVolume();
   if (!top) {
      top = gGeoManager->MakeBox("TOP", medAir, 100, 100, 100);
      gGeoManager->SetTopVolume(top);
   }

   TGeoVolumeAssembly * detector = new TGeoVolumeAssembly(fDetectorName.Data());

   // half open angle of the truncate pyramid
   double deltaX = (xdim2 - xdim1);
   double trp_hipot = TMath::Sqrt( zdim * zdim * 4  + deltaX * deltaX );
   double alfa = TMath::ASin (deltaX / trp_hipot);
   double alfa_degree = alfa  * 180./ TMath::Pi();
   cout << "alfa_degree " << alfa_degree << std::flush << endl;

   // compute some values of the full pyramid dimensions
   double pyramid_hipot = xdim2 / TMath::Sin(alfa);

   //cout << "pyramid_hipot " << pyramid_hipot << std::flush << endl;
   double pyramid_base = pyramid_hipot * TMath::Cos(alfa);
   double pyramid_base_c = pyramid_base - zdim; // distance from center to the pyramid vertex

   // translation of crystal center after 2*alfa rotation about vertex pyramid
   double deltax = delta * TMath::Cos(alfa*2);
   double deltaz = - delta * TMath::Sin(alfa*2);
   double posx = TMath::Sin(alfa*2) * pyramid_base_c + deltax;
   double posz = TMath::Cos(alfa*2) * pyramid_base_c + deltaz;

   // create on BGO crystal
   TGeoVolume *caloCristal = gGeoManager->MakeTrd2(fgkDefaultCrysName, medBGO, xdim1, xdim2, ydim1, ydim2, zdim);
   caloCristal->SetLineColor(kAzure+6);
   caloCristal->SetFillColor(kAzure+6);
   //  caloCristal->SetTransparency(25);

   ///////////////////////////////////////
   // Create only one crystal geometry (used for Test beam simulations)
   if ( fConfig_typegeo.CompareTo("ONE_CRY") == 0 ) {
      detector->AddNode(caloCristal, 1);
      EndGeometry(fp);
      return;
   }

   ///////////   Create a ROW assembly of 3 BGO crystals

   // set rotations/translation
   TGeoRotation * rotP = new TGeoRotation ();
   rotP->RotateY(alfa_degree * 2);
   TGeoTranslation * trasP = new TGeoTranslation(posx, 0, posz - pyramid_base_c );
   TGeoRotation * rotN = new TGeoRotation ();
   rotN->RotateY(-alfa_degree * 2);
   TGeoTranslation * trasN = new TGeoTranslation(-posx, 0, posz - pyramid_base_c );

   TGeoVolumeAssembly * row = new TGeoVolumeAssembly("CAL_ROW");
   row->AddNode(caloCristal, 1);
   row->AddNode(caloCristal, 2, new TGeoCombiTrans(*trasP, *rotP));
   row->AddNode(caloCristal, 3, new TGeoCombiTrans(*trasN, *rotN));

   ////////////   MODULE
   ////////////   Create a 3x3 module by assembly 3 ROWs
   TGeoVolumeAssembly * mod = new TGeoVolumeAssembly("CAL_MOD");

   // set rotations/translation
   TGeoRotation * rotUp = new TGeoRotation ();
   rotUp->RotateX(-alfa_degree * 2);
   TGeoTranslation * trasUp = new TGeoTranslation(0, posx, posz - pyramid_base_c );
   TGeoRotation * rotDn = new TGeoRotation ();
   rotDn->RotateX(alfa_degree * 2);
   TGeoTranslation * trasDn = new TGeoTranslation(0, -posx, posz - pyramid_base_c );

   //---- Crystal Support as truncate pyramid (just for visual proposes)
   double deltaZsup = -0.5; // delta distance from back
   double zdimS  = (zdim  + deltaZsup - deltaZsup *TMath::Sin(alfa*3)) *.5;
   double xdimS1 = (((xdim1+xdim2)*0.5) * 3 + 2 * delta) * TMath::Cos(alfa*3) ;
   double xdimS2 = (xdim2 * 3 + 2 * delta)* TMath::Cos(alfa*3) + deltaZsup * TMath::Tan(alfa*3);
   double ydimS1 = xdimS1;
   double ydimS2 = xdimS2;
   double shiftS = zdimS * TMath::Cos(alfa*3);

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the Support (temporally AIR)\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Material:      \"AIR\"\n");
   fprintf(fp,"Density:       1.2e-3\n");

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the Support (cm)\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Widthfront:   %f   Heightfront:  %f    DummyLength:       0.\n", xdimS1, ydimS1);
   fprintf(fp,"Widthback :   %f	 Heightback :  %f	   Length:           %f\n", xdimS2, ydimS2, zdimS);
   fprintf(fp,"PositionZ:     %f\n\n", shiftS);

   TGeoVolume* support = gGeoManager->MakeTrd2("MOD_SUPPORT", medAl, xdimS1, xdimS2, ydimS1, ydimS2, zdimS);
   support->SetLineColor(kGray);
   support->SetLineColor(kGray);
   mod->AddNode(support, 0, new TGeoTranslation(0, 0, shiftS));


   //---------------- Container air as truncate pyramid
   //------------------- needed for FLUKA
   //
   double delZ = .15;
   double delXY = 0.01;
   double xdimA1 = xdim1 * 3 + delta * TMath::Cos(alfa) + delXY / TMath::Cos(alfa*3) + delZ/2. * TMath::Tan(alfa*3);
   double xdimA2 = xdim2 * 3 + delta * TMath::Cos(alfa) +
                  delXY / TMath::Cos(alfa*3) +
                  delZ/2. * TMath::Tan(alfa*3) +
                  xdim2 *TMath::Sin(alfa*3) *  TMath::Tan(alfa*3);
   double ydimA1 = xdimA1;
   double ydimA2 = xdimA2;
   double zdimA = zdim + delZ;
   double shiftA = -delZ * 0.55;
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the Air around each module. It is need ONLY by FLUKA geometry (cm)\n");
   fprintf(fp,"// it will be removed if a truncate pyramid body is implemented in FLUKA \n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Widthfront:   %f   Heightfront:  %f    DummyLength:    0.\n",   xdimA1, ydimA1);
   fprintf(fp,"Widthback :   %f	 Heightback :  %f	   Length:         %f\n", xdimA2, ydimA2, zdimA);
   fprintf(fp,"PositionZ:    %f\n\n", shiftA);

   TGeoVolume * modAir = gGeoManager->MakeTrd2("MOD_AIR", medAir, xdimA1, xdimA2, ydimA1, ydimA2, zdimA);
   modAir->SetLineColor(kGreen);
   modAir->SetFillColor(kGreen);
   modAir->SetTransparency(80);
   mod->AddNode(modAir, 0, new  TGeoTranslation(0, 0, shiftA));
   // -------------------------------------------------------------------

   // Add ROWs
   mod->AddNode(row, 1);
   mod->AddNode(row, 2, new TGeoCombiTrans(*trasUp, *rotUp));
   mod->AddNode(row, 3, new TGeoCombiTrans(*trasDn, *rotDn));


   ///////////////////////////////////////
   // Create only one Module (3x3 crystals) geometry (used for Test beam simulations)
   if ( fConfig_typegeo.CompareTo("ONE_MOD") == 0 ) {
      detector->AddNode(mod, 1);
      EndGeometry(fp);
      return;
   }


   ///////////////////////////////////////
   // Detector with 5 or 7 Modules in a row: geometry for the HIT test beam
   if ( fConfig_typegeo.CompareTo("FIVE_MOD") == 0 ||
        fConfig_typegeo.CompareTo("SEVEN_MOD") == 0 ) {

      //first module, centered on the beam
      detector->AddNode(mod, 1);

      //calculate the translation and rotation of the first two neighbors
      double deltaM  = 4 * delta;
      double deltaMx = deltaM * TMath::Cos(alfa*6);
      double deltaMz = - deltaM * TMath::Sin(alfa*6);
      double posModx = TMath::Sin(alfa*6) * pyramid_base_c + deltaMx;
      double posModz = TMath::Cos(alfa*6) * pyramid_base_c + deltaMz;

      TGeoRotation * rotPDet  = new TGeoRotation (); rotPDet->RotateY(alfa_degree * 6); //positive
      TGeoRotation * rotNDet  = new TGeoRotation (); rotNDet->RotateY(-alfa_degree * 6); //negative
      TGeoTranslation * trasPDet  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
      TGeoTranslation * trasNDet  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );
      TGeoRotation * rotLeftZ  = new TGeoRotation ();
      TGeoRotation * rotRightZ = new TGeoRotation ();
      rotLeftZ->RotateZ(-0.2);
      rotRightZ->RotateZ(0.2);

      detector->AddNode(mod, 2, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoHMatrix(*rotLeftZ) ) );
      detector->AddNode(mod, 3, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoHMatrix(*rotRightZ)) );

      //calculate the translation and rotation of the furthest crystals
      deltaM  = 8 * delta  ;
      deltaMx = deltaM * TMath::Cos(alfa*12);
      deltaMz = - deltaM * TMath::Sin(alfa*12);
      posModx = TMath::Sin(alfa*12) * (pyramid_base_c) + deltaMx;
      posModz = TMath::Cos(alfa*12) * (pyramid_base_c) + deltaMz;

      TGeoRotation * rotPDet2  = new TGeoRotation (); rotPDet2->RotateY(alfa_degree * 12);
      TGeoRotation * rotNDet2  = new TGeoRotation (); rotNDet2->RotateY(-alfa_degree * 12);
      TGeoTranslation * trasPDet2  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
      TGeoTranslation * trasNDet2  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );

      detector->AddNode(mod, 4, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) ) );
      detector->AddNode(mod, 5, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) ) );

      if ( fConfig_typegeo.CompareTo("FIVE_MOD") == 0 ) {
         EndGeometry(fp);
         return;
      }

      //calculate the translation and rotation of the furthest crystals
      deltaM  = 12 * delta  ;
      deltaMx = deltaM * TMath::Cos(alfa*18);
      deltaMz = - deltaM * TMath::Sin(alfa*18);
      posModx = TMath::Sin(alfa*18) * (pyramid_base_c) + deltaMx;
      posModz = TMath::Cos(alfa*18) * (pyramid_base_c) + deltaMz;

      TGeoRotation * rotPDet3  = new TGeoRotation (); rotPDet3->RotateY(alfa_degree * 18);
      TGeoRotation * rotNDet3  = new TGeoRotation (); rotNDet3->RotateY(-alfa_degree * 18);
      TGeoTranslation * trasPDet3  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
      TGeoTranslation * trasNDet3  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );

      detector->AddNode(mod, 6, new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) ) );
      detector->AddNode(mod, 7, new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) ) );
      
      EndGeometry(fp);
      return;
   }



   ////////////////////////////////////////////////////////////
   ////////////    create Detector

   // Detector with 4 central modules (FIRST LEVEL)

   // displacement of module center
   double deltaM  = 2 * delta; // 2mm between MODULES
   double deltaMx = deltaM * TMath::Cos(alfa*3);
   double deltaMz = - deltaM * TMath::Sin(alfa*3);
   double posModx = TMath::Sin(alfa*3) * pyramid_base_c + deltaMx;
   double posModz = TMath::Cos(alfa*3) * pyramid_base_c + deltaMz;

   // set rotations/translation (left/right/up/down)
   TGeoRotation * rotUpDet = new TGeoRotation (); rotUpDet->RotateX(-alfa_degree * 3);
   TGeoRotation * rotDnDet = new TGeoRotation (); rotDnDet->RotateX(alfa_degree * 3);
   TGeoRotation * rotPDet  = new TGeoRotation (); rotPDet->RotateY(alfa_degree * 3);
   TGeoRotation * rotNDet  = new TGeoRotation (); rotNDet->RotateY(-alfa_degree * 3);
   TGeoTranslation * trasUpDet = new TGeoTranslation(0, posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasDnDet = new TGeoTranslation(0, -posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasPDet  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
   TGeoTranslation * trasNDet  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );
   TGeoRotation * rotLeftZ  = new TGeoRotation ();
   TGeoRotation * rotRightZ = new TGeoRotation ();
   rotLeftZ->RotateZ(-0.2);
   rotRightZ->RotateZ(0.2);

   detector->AddNode(mod, 1, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet, *rotDnDet) * TGeoHMatrix(*rotLeftZ) ) );
   detector->AddNode(mod, 2, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet, *rotUpDet) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod, 3, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet, *rotDnDet) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod, 4, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet, *rotUpDet) * TGeoHMatrix(*rotLeftZ) ) );

   ///////////////////////////////////////
   // Create only one 2x2 Modules geometry (Central modules)
   if ( fConfig_typegeo.CompareTo("CENTRAL_DET") == 0
       || fConfig_typegeo.CompareTo("FULL_DET") != 0
       ) {
      EndGeometry(fp);
      return;
   }

   //if ( fConfig_typegeo.CompareTo("FULL_DET") == 0 ) {
   //   cout << "****  WARNING : FULL_DET geometry is not ready for FLUKA simulation!" << std::flush << endl;
   //}

   // displacement of module center (SECOND LEVEL)
   deltaM  = 6 * delta  ;
   deltaMx = deltaM * TMath::Cos(alfa*9);
   deltaMz = - deltaM * TMath::Sin(alfa*9);
   posModx = TMath::Sin(alfa*9) * (pyramid_base_c + 1.2) + deltaMx;
   posModz = TMath::Cos(alfa*9) * (pyramid_base_c + 1.2) + deltaMz;

   TGeoRotation * rotUpDet2 = new TGeoRotation (); rotUpDet2->RotateX(-alfa_degree * 9);
   TGeoRotation * rotDnDet2 = new TGeoRotation (); rotDnDet2->RotateX(alfa_degree * 9);
   TGeoRotation * rotPDet2  = new TGeoRotation (); rotPDet2->RotateY(alfa_degree * 9);
   TGeoRotation * rotNDet2  = new TGeoRotation (); rotNDet2->RotateY(-alfa_degree * 9);
   TGeoTranslation * trasUpDet2 = new TGeoTranslation(0, posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasDnDet2 = new TGeoTranslation(0, -posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasPDet2  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
   TGeoTranslation * trasNDet2  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );

   // 2 modules left / 2 right
   detector->AddNode(mod, 5, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) );
   detector->AddNode(mod, 6, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet, *rotUpDet) ) );
   detector->AddNode(mod, 7, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) );
   detector->AddNode(mod, 8, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet, *rotUpDet) ) );

   // 2  modules top / 2 bottom
   rotLeftZ->RotateZ(-0.15);
   rotRightZ->RotateZ(0.15);
   detector->AddNode(mod, 9, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ) )  );
   detector->AddNode(mod,10, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,11, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,12, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ) )  );

   // 4 diagonals
   rotLeftZ->RotateZ(-0.35);
   rotRightZ->RotateZ(0.35);
   detector->AddNode(mod,13, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ)) );
   detector->AddNode(mod,14, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,15, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,16, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ)) );

   // displacement of module center (THIRD LEVEL)
   deltaM  = 9 * delta;
   deltaMx = deltaM * TMath::Cos(alfa*15);
   deltaMz = - deltaM * TMath::Sin(alfa*15);
   posModx = TMath::Sin(alfa*15) * (pyramid_base_c + 3.3) + deltaMx;
   posModz = TMath::Cos(alfa*15) * (pyramid_base_c + 3.3) + deltaMz;
   *trasNDet *= TGeoTranslation(-(delta) * TMath::Sin(alfa*15), 0, 0);
   *trasPDet *= TGeoTranslation( (delta) * TMath::Sin(alfa*15), 0, 0);

   TGeoRotation * rotUpDet3 = new TGeoRotation (); rotUpDet3->RotateX(-alfa_degree * 15);
   TGeoRotation * rotDnDet3 = new TGeoRotation (); rotDnDet3->RotateX(alfa_degree * 15);
   TGeoRotation * rotPDet3  = new TGeoRotation (); rotPDet3->RotateY(alfa_degree * 15);
   TGeoRotation * rotNDet3  = new TGeoRotation (); rotNDet3->RotateY(-alfa_degree * 15);
   TGeoTranslation * trasUpDet3 = new TGeoTranslation(0, posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasDnDet3 = new TGeoTranslation(0, -posModx, posModz - pyramid_base_c );
   TGeoTranslation * trasPDet3  = new TGeoTranslation(posModx, 0, posModz - pyramid_base_c );
   TGeoTranslation * trasNDet3  = new TGeoTranslation(-posModx, 0, posModz - pyramid_base_c );

   // 2 modules left  and 2 right
   detector->AddNode(mod,17, new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) );
   detector->AddNode(mod,18, new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasUpDet, *rotUpDet) ) );
   detector->AddNode(mod,19, new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasDnDet, *rotDnDet) ) );
   detector->AddNode(mod,20, new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasUpDet, *rotUpDet) ) );


   // 2 top / 2 bottom
   rotLeftZ->RotateZ(-0.2);
   rotRightZ->RotateZ(0.2);
   detector->AddNode(mod,25, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotLeftZ)) );
   detector->AddNode(mod,26, new TGeoHMatrix( TGeoCombiTrans(*trasNDet, *rotNDet) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,27, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,28, new TGeoHMatrix( TGeoCombiTrans(*trasPDet, *rotPDet) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotLeftZ)) );

   rotLeftZ->RotateZ(-0.3);
   rotRightZ->RotateZ(0.3);
   detector->AddNode(mod,21, new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotLeftZ)) );
   detector->AddNode(mod,22, new TGeoHMatrix( TGeoCombiTrans(*trasNDet3, *rotNDet3) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,23, new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasDnDet2, *rotDnDet2) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,24, new TGeoHMatrix( TGeoCombiTrans(*trasPDet3, *rotPDet3) * TGeoCombiTrans(*trasUpDet2, *rotUpDet2) * TGeoHMatrix(*rotLeftZ)) );

   //rotLeftZ->RotateZ(-0.1);
   //rotRightZ->RotateZ(0.1);
   *trasNDet2 *= TGeoTranslation(-(6*delta) * TMath::Sin(alfa*15), 0, 0);
   *trasPDet2 *= TGeoTranslation( (6*delta) * TMath::Sin(alfa*15), 0, 0);

   detector->AddNode(mod,29, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotLeftZ)) );
   detector->AddNode(mod,30, new TGeoHMatrix( TGeoCombiTrans(*trasNDet2, *rotNDet2) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,31, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasDnDet3, *rotDnDet3) * TGeoHMatrix(*rotRightZ)) );
   detector->AddNode(mod,32, new TGeoHMatrix( TGeoCombiTrans(*trasPDet2, *rotPDet2) * TGeoCombiTrans(*trasUpDet3, *rotUpDet3) * TGeoHMatrix(*rotLeftZ)) );

   EndGeometry(fp);

}

//-----------------------------------------------------------------------------
TVector3 GetAngles(TGeoHMatrix rot)
{
   // valable only for angle +/- 90º
   const Double_t* mat = rot.GetRotationMatrix();

   Double_t angleX = TMath::ATan2(mat[7], mat[8])*TMath::RadToDeg();
   Double_t angleY = TMath::ASin(-mat[6])*TMath::RadToDeg();
   Double_t angleZ = TMath::ATan2(mat[0], mat[3])*TMath::RadToDeg();

   if (angleZ < 180) angleZ = 90 - angleZ;
   else angleZ -= 90;

   return TVector3(angleX, angleY, angleZ);
}

//-----------------------------------------------------------------------------
void PrintCrystals(FILE * fp)
{
   TVector3 angles;
   Double_t* trans;

   Int_t nCry = fListOfCrystals->GetEntriesFast();

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the crystals used in the run\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");

   for (int ii=0; ii<nCry; ii++) {
      NodeTrafo *crt = ((NodeTrafo *)(fListOfCrystals->At(ii)));
      TGeoHMatrix mat = crt->mat;

      trans = mat.GetTranslation();
      angles = GetAngles(mat);
      fprintf(fp,"CrystalId:  %d\n", ii);
      fprintf(fp,"PositionX:  %.12f   PositionY:   %.12f   PositionZ:   %.12f\n", trans[0], trans[1], trans[2]);
      fprintf(fp,"TiltX:      %.12f   TiltY:       %.12f   TiltZ:       %.12f\n", angles[0], angles[1], angles[2]);
   }
   fprintf(fp,"\n");
}

//-----------------------------------------------------------------------------
void PrintModules(FILE * fp)
{
   TVector3 angles;
   Double_t* trans;

   Int_t nModule = fListOfModules->GetEntriesFast();

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the modules used in the run\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");

   for (int ii=0; ii<nModule; ii++) {
      NodeTrafo *crt = ((NodeTrafo *)(fListOfModules->At(ii)));
      TGeoHMatrix mat = crt->mat;

      trans = mat.GetTranslation();
      angles = GetAngles(mat);
      fprintf(fp,"ModuleId:   %d\n", ii);
      fprintf(fp,"PositionX:  %.12f   PositionY:   %.12f   PositionZ:   %.12f\n", trans[0], trans[1], trans[2]);
      fprintf(fp,"TiltX:      %.12f   TiltY:       %.12f   TiltZ:       %.12f\n", angles[0], angles[1], angles[2]);
   }
}

//-----------------------------------------------------------------------------
TVector3 PrintCalorimeterSize(FILE * fp, double &shift)
{
   TVector3 fCaloSize;
   TVector3 delta(0.1, 0.1, 0.1); // Add some space around
   TVector3 maxpoint(-999., -999., -999.);
   TVector3 minpoint( 999.,  999.,  999.);

   Int_t fCrystalsN = fListOfCrystals->GetEntriesFast();

   if (fCrystalsN <= 0) fCaloSize = maxpoint;

   Double_t * local  = new Double_t [3];
   Double_t * point  = new Double_t [3];

   for (int ii=0; ii<fCrystalsN; ii++) {

      NodeTrafo *crt = ((NodeTrafo *)(fListOfCrystals->At(ii)));

      local[0] = local[1] = local[2] = 0;
 
      TGeoNode *node = crt->node;

      //TGeoVolume *vol = node->GetVolume();
      TGeoHMatrix matCurrent(crt->mat);

      TGeoTrd2 * trd2Vol = (TGeoTrd2 *)(node->GetVolume()->GetShape());
      Double_t xdim1 = trd2Vol->GetDx1();
      Double_t xdim2 = trd2Vol->GetDx2();
      Double_t ydim1 = trd2Vol->GetDy1();
      Double_t ydim2 = trd2Vol->GetDy2();
      Double_t zdim  = trd2Vol->GetDz();

      for (int sx = -1; sx<2; sx+=2) {
         for (int sy = -1; sy<2; sy+=2) {
            for( int sz = -1; sz<2; sz+=2) {
               local[0] = sx * ((sz<0) ? xdim1 : xdim2);
               local[1] = sy * ((sz<0) ? ydim1 : ydim2);
               local[2] = sz * zdim;
               matCurrent.LocalToMaster(local, point);

               if ( point[0] > maxpoint[0] ) maxpoint[0] = point[0];
               if ( point[1] > maxpoint[1] ) maxpoint[1] = point[1];
               if ( point[2] > maxpoint[2] ) maxpoint[2] = point[2];

               if ( point[0] < minpoint[0] ) minpoint[0] = point[0];
               if ( point[1] < minpoint[1] ) minpoint[1] = point[1];
               if ( point[2] < minpoint[2] ) minpoint[2] = point[2];
            }
         }
      }
   }

   fCaloSize = maxpoint - minpoint + delta;
   fCaloSize[2] = fCaloSize[2]*1.05; //to build the bounding box a little bit bigger (safer for FLUKA)

   cout << "  Calorimeter Size: "
        << fCaloSize[0] << ", " << fCaloSize[1] << ", " << fCaloSize[2] << " cm" << std::flush << endl;
   shift = (maxpoint[2] + minpoint[2])/2;
   cout << "Shift " << (maxpoint[2] + minpoint[2] + delta[2])/2 << std::flush << endl;

   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"// Parameter of the Calorimeter bounding box (cm)\n");
   fprintf(fp,"// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-\n");
   fprintf(fp,"Width:          %f     Height:      %f      Thick:     %f, \n",
               fCaloSize[0], fCaloSize[1], fCaloSize[2] );
   fprintf(fp,"PositionZ:      %f\n\n", shift);

   return fCaloSize;
}

//-----------------------------------------------------------------------------
void GetListOfCrystals( TGeoNode * node, TGeoHMatrix matrix )
{

   TGeoVolume *vol = node->GetVolume();
   Int_t nd = node->GetNdaughters();

   TGeoHMatrix matCurrent(matrix);
   matCurrent.Multiply(node->GetMatrix());

   if (strcmp(vol->GetName(), fgkDefaultCrysName) == 0 ) {
      NodeTrafo * crt = new NodeTrafo(node, matCurrent);
      fListOfCrystals->Add(crt);
   }

   for (Int_t ii=0; ii<nd; ii++) {
      TGeoNodeMatrix * noded = (TGeoNodeMatrix *)vol->GetNode(ii);
      GetListOfCrystals( noded, matCurrent );
   }

}

//-----------------------------------------------------------------------------
void GetListOfModules( TGeoNode * node, TGeoHMatrix matrix )
{
   TGeoVolume *vol = node->GetVolume();
   Int_t nd = node->GetNdaughters();

   TGeoHMatrix matCurrent(matrix);
   matCurrent.Multiply(node->GetMatrix());

   if (strcmp(vol->GetName(), "CAL_MOD") == 0 ) {
      NodeTrafo * modA = new NodeTrafo(node, matCurrent);
      fListOfModules->Add(modA);
   }

   for (Int_t ii=0; ii<nd; ii++) {
      TGeoNodeMatrix * noded = (TGeoNodeMatrix *)vol->GetNode(ii);
      GetListOfModules( noded, matCurrent );
   }
}

//-----------------------------------------------------------------------------
void ShowDetectorIdsMap()
{
   // Draw cristal id position on modules
   // This help to create the map of cystal ID to board/channel

   double aspectradio = (fcalSize[1]/fcalSize[0]);
   gStyle->SetTitleFontSize(gStyle->GetTitleFontSize()/aspectradio);
   gStyle->SetFrameLineWidth(0);

   // Draw modules position 
   TCanvas * c = new TCanvas("modules", "Modules ID", 900, 900 * aspectradio + 20);

   Int_t nModule = fListOfModules->GetEntriesFast();
   gStyle->SetOptStat(0);
   TH2F* modGrid = new TH2F("Modules"," Modules ID", 200, -fcalSize[0]/2, fcalSize[0]/2 , 200, -fcalSize[1]/2, fcalSize[1]/2  + 3*xdim1  );

   double xdimA1 = xdim1 * 3 + delta + 0.6 ;
   modGrid->Draw("A");

   for (int ii=0; ii<nModule; ii++) {
      NodeTrafo *crt = ((NodeTrafo *)(fListOfModules->At(ii)));
      // Get module transformation matrix (module/detector)
      TGeoHMatrix mat = crt->mat;
      // Get only the center coordinates
      Double_t *trans = mat.GetTranslation();
      TBox* mod = new TBox(trans[0]-xdimA1, trans[1]-xdimA1, trans[0]+xdimA1, trans[1]+xdimA1);
      mod->Draw();
      
      TText* id = new TText(trans[0], trans[1], Form("%3d", ii));
      id->SetTextAlign(22);
      id->SetTextSize(0.07/aspectradio);
      id->Draw();
   }

   // Draw cristal id position on modules
   TCanvas * c1 = new TCanvas("crytals", "Crystals ID ", 900, 900 * aspectradio + 30);

   Int_t nCry = fListOfCrystals->GetEntriesFast();
   TH2F* crimap = new TH2F(""," Crystal ID to be mapped with Board/channel", 200, -fcalSize[0]/2, fcalSize[0]/2, 200 , -fcalSize[1]/2, fcalSize[1]/2  + 3*xdim1 );
   crimap->Draw("A");

   for (int ii=0; ii<nModule; ii++) {
      NodeTrafo *mod = ((NodeTrafo *)(fListOfModules->At(ii)));
      // Get module transformation matrix (module/detector)
      TGeoHMatrix mat = mod->mat;
      // Get only the center coordinates
      Double_t *trans = mat.GetTranslation();
      TBox* modBox = new TBox(trans[0]-xdimA1, trans[1]-xdimA1, trans[0]+xdimA1, trans[1]+xdimA1);
      modBox->SetFillColor(kGray);
      modBox->Draw();
   }

   for (int ii=0; ii<nCry; ii++) {
      NodeTrafo *crt = ((NodeTrafo *)(fListOfCrystals->At(ii)));
      // Get module transformation matrix (module/detector)
      TGeoHMatrix mat = crt->mat;
      // Get only the center coordinates
      Double_t *trans = mat.GetTranslation();
      TBox* cry = new TBox(trans[0]-xdim1, trans[1]-xdim1, trans[0]+xdim1, trans[1]+xdim1);
      cry->SetFillColor(kYellow);
      cry->Draw();
      TText* id = new TText(trans[0], trans[1]+xdim1/4, Form("%3d", ii));
      id->SetTextAlign(22);
      id->SetTextSize(0.02/aspectradio);
      id->Draw();
   }

   // Save the cristal id position on modules as PDF
   TString path = fgPath + TString("/CA_crystal_pos_map.pdf");
   c1->SaveAs(path.Data());

   // Create a .map file 
   TString filemap = fgPath + TString("TACAdetector.map");
   FILE* fmap = fopen(filemap.Data(), "w");
   fprintf(fmap, "#number of crystal present\n");
   fprintf(fmap, "CrystalsN: %d\n", nCry );

   fprintf(fmap, "#crysid crymodule channelID crysBoard(HW) activecrys\n");
   for (int ii=0; ii<nCry; ii++) {
      fprintf(fmap, " %d     %d         %d         %d           %d\n", ii, 0, 0, 161, 1);
   }
   fclose(fmap);


}

void ComputeCrystalIndexes()
{
   // Build a row/col index to by used in clustering
   // Here, we only check if the algohorimy is OK
   // the real fuction is TACAparGeo::ComputeCrystalIndexes()

   // DEBUG
   double aspectradio = (fcalSize[1]/fcalSize[0]);
   TCanvas * c1 = new TCanvas("crytals11", "Crystals ID ", 900, 900 * aspectradio + 20);
   TH2F* crimap = new TH2F("11"," check to col/row map", 200, -fcalSize[0]/2, fcalSize[0]/2, 200 , -fcalSize[1]/2, fcalSize[1]/2  + 3*xdim1 );
   crimap->Draw();

   Double_t * local  = new Double_t [3];
   Double_t * point  = new Double_t [3];

   // Find X and Y dimension of calo front face to get the number of columns and rows
   TVector3 maxpoint(-999., -999., -999.);
   TVector3 minpoint( 999.,  999.,  999.);
   TVector3 lastpoint;
   Double_t width = 0;
   Double_t n = 0;
   int nCry = fListOfCrystals->GetEntriesFast();
   for (Int_t iCry = 0; iCry < nCry; ++iCry) {
      //TVector3 pos = GetCrystalPosition(iCry);
      NodeTrafo *crt = ((NodeTrafo *)(fListOfCrystals->At(iCry)));
      // Get module transformation matrix (module/detector)
      TGeoHMatrix mat = crt->mat;
      TGeoHMatrix matCurrent(crt->mat);

      // central point in front
      local[0] = 0 ;
      local[1] = 0 ;
      local[2] = -zdim;
      matCurrent.LocalToMaster(local, point);

      if ( point[0] > maxpoint[0] ) maxpoint[0] = point[0];
      if ( point[1] > maxpoint[1] ) maxpoint[1] = point[1];

      if ( point[0] < minpoint[0] ) minpoint[0] = point[0];
      if ( point[1] < minpoint[1] ) minpoint[1] = point[1];
      
      // get distance beetwing two crystals
      if (iCry > 0 && iCry < 2 ) {
         width += TMath::Abs(lastpoint[0] - point[0]);
         cout << width << endl;
         n += 1;
      }
      lastpoint = point;
   }
   width /= n;
   Double_t interModWidth = nCry/9 * 0.1;
   
   TVector3 dim = maxpoint - minpoint;
   //dim.Print();

   Int_t cols   = (int)(dim[0]-interModWidth+width)/width + 1;
   Int_t rows   = (int)(dim[1]-interModWidth+width)/width + 1;  
   cout  << "   width: " << width << "   rows: " << rows << " cols: " << cols << endl;


   for (Int_t iCry = 0; iCry < nCry; ++iCry) {
      NodeTrafo *crt = ((NodeTrafo *)(fListOfCrystals->At(iCry)));
      // Get module transformation matrix (module/detector)
      TGeoHMatrix mat = crt->mat;
      TGeoHMatrix matCurrent(crt->mat);

      // central point in front
      local[0] = 0 ;
      local[1] = 0 ;
      local[2] = -zdim;
      matCurrent.LocalToMaster(local, point);

      // Find col,row index
      for (Int_t i = 0; i<cols; ++i) {
         for (Int_t j = 0; j<rows; ++j) {
            if( iCry == 0) {
               // DEBUG: draw the if condition TBox for row/column index
               TBox* modBox1 = new TBox(i*width-cols*width/2+.05, j*width-rows*width/2+.05, (i+1)*width-cols*width/2-.05, (j+1)*width-rows*width/2-.05);
               modBox1->SetLineColor(kYellow);
               modBox1->Draw();
            }
            if ( (point[0] >= i*width - cols*width/2 && point[0] <= (i+1)*width - cols*width/2) && 
                 (point[1] >= j*width - rows*width/2 && point[1] <= (j+1)*width - rows*width/2)) {
 
               pair<int, int> idx(i, j);
          
               cout  << "   iCry: "  << iCry << " " << i << " " << j
                     << " pos[0] " << point[0] << " pos[1] " << point[1]
                     << " col "    << idx.first << " row " << idx.second << endl;
            } 
         }
      }

      // DEBUG: draw the point to check (it should get inside a condition TBox)
      TBox* modBox = new TBox(point[0]-0.1, point[1]-0.1, point[0]+0.1, point[1]+0.1);
      modBox->SetFillColor(kGray);
      modBox->Draw();

   }
}

//-----------------------------------------------------------------------------
void EndGeometry(FILE* fp)
{
   // Compute the number of crystals and
   // the transformation matrix

   fListOfCrystals = new TObjArray();
   fListOfModules = new TObjArray();

   TGeoVolume* detector = gGeoManager->FindVolumeFast(fDetectorName.Data());
   cout << "  Detector NAME " << detector->GetName() << std::flush << endl;

   TGeoVolumeAssembly * dummy = new TGeoVolumeAssembly("dummyVol");
   dummy->AddNode(detector, 1);
   TGeoNode* nodeDet = dummy->GetNode(0);

   TGeoHMatrix postMat;

   GetListOfCrystals(nodeDet, postMat);
   // Air around each module (needed for FLUKA geometry)
   GetListOfModules(nodeDet, postMat);

   Int_t nCry = fListOfCrystals->GetEntriesFast();
   Int_t nModule = fListOfModules->GetEntriesFast();

   cout << "  Number of modules: " << nModule << std::flush << endl;
   cout << "  Number of Calorimeter crystals: " << nCry << std::flush << endl;

   double shift;
   fcalSize = PrintCalorimeterSize(fp, shift);

   fprintf(fp,"CrystalN:  %d\n", nCry);
   fprintf(fp,"ModulesN:  %d\n", nModule);

   PrintCrystals(fp);
   PrintModules(fp);

   fclose(fp);


   ShowDetectorIdsMap();
   ComputeCrystalIndexes();

   /////// Draw ROOT geometry
   // Add module to top volumen
   TGeoVolume *top = gGeoManager->GetTopVolume();
   TGeoMedium *medAir = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");

   // Add AIR box around detector to check dimensions
   TGeoVolume * detBox = gGeoManager->MakeBox("CAL_BOX", medAir, fcalSize[0]/2, fcalSize[1]/2, fcalSize[2]/2);
   detBox->SetTransparency(85);
   TCanvas * c = new TCanvas("CAL3D", "Calorimeter", 300, 300);
   top->AddNode(detector, 1);
   top->AddNode(detBox, 1, new TGeoTranslation(0, 0, shift));
   top->Draw("ogl");

}
