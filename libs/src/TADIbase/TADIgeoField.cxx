

#include "TAGroot.hxx"
#include "TADIgeoField.hxx"

#include "GlobalPar.hxx"


//______________________________________________________________________________
TADIgeoField::TADIgeoField (TADIparGeo* diGeo)
 : TVirtualMagField(),
   fpDiGeoMap(diGeo),
   fpFootGeo(0x0)
{
   
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
   fMagHistoX = new TH3F("MagHistoX", "Magnetic Field", 20, -5, 5, 20, -5, 5, 200, -33.5, 66.5);
   fMagHistoY = new TH3F("MagHistoY", "Magnetic Field", 20, -5, 5, 20, -5, 5, 200, -33.5, 66.5);
   fMagHistoZ = new TH3F("MagHistoZ", "Magnetic Field", 20, -5, 5, 20, -5, 5, 200, -33.5, 66.5);
   
   fMaxBinX = fMagHistoX->GetXaxis()->GetNbins();
   fMaxBinY = fMagHistoX->GetYaxis()->GetNbins();
   fMaxBinZ = fMagHistoX->GetZaxis()->GetNbins();

   TString fullFileName = fpDiGeoMap->GetMapName().Data();
   
   FromFile(fullFileName);

}

//______________________________________________________________________________
// first 3 variables are the input position components, last 3 var are the output b components
void TADIgeoField::FromFile(TString& fullFileName)
{
   ifstream ifile;

   Info("TADIgeoField()", "Going to open %s", fullFileName.Data());
   
   ifile.open( fullFileName.Data() );

   if ( !ifile.is_open() ) {
      Error("TADIgeoField::TADIgeoField", "cannot open magnetic map for file %s\n", fullFileName.Data());
      exit(0);
   }
   
   // read position and field  -->	 fill a multidimensional map called point3D = map< double, map< double, map< double, TVector3 > > >
   string line = "";
   
   //HACK
   getline(ifile,line);
   while( getline( ifile, line ) ) {
      
      if (line == "")  continue;
      if ( line.find("#") != string::npos || line.find("*") != string::npos )
         continue;
      // if ( line.find("#") != string::npos || line.find("//") != string::npos )     continue;
      
      double x = -1;
      double y = -1;
      double z = -1;
      double bx = -1;
      double by = -1;
      double bz = -1;
      
      istringstream getall(line);
      getall  >> x;
      getall  >> y;
      getall  >> z;
      getall  >> bx;
      getall  >> by;
      getall  >> bz;
      
      TVector3 pos(x, y, z);
      TVector3 posG = fpFootGeo->FromDILocalToGlobal(pos);
      
      Int_t bin = fMagHistoY->FindBin(posG[0], posG[1], posG[2]);
      fMagHistoX->SetBinContent(bin, bx);
      fMagHistoY->SetBinContent(bin, by);
      fMagHistoZ->SetBinContent(bin, bz);
   }
   
   ifile.close();
   
   if ( GlobalPar::GetPar()->Debug() > 0 ){
      std::cout << "TADIgeoField::TADIgeoField: exiting" << std::endl;
   }
}

//______________________________________________________________________________
// return B as a vector, given vector position
TVector3 TADIgeoField::GetField(const TVector3& position)
{
   return Interpolate(position);
}

//______________________________________________________________________________
void TADIgeoField::Field(const Double_t* pos, Double_t* fieldB)
{
   const TVector3 position = TVector3(pos[0], pos[1], pos[2] );
   
   TVector3 outField = Interpolate( position );
   
   fieldB[0] = outField.X();
   fieldB[1] = outField.Y();
   fieldB[2] = outField.Z();
}

//______________________________________________________________________________
// same for real and const field
TVector3 TADIgeoField::Interpolate(const TVector3& position)
{
   //   https://en.wikipedia.org/wiki/Trilinear_interpolation
   //	  Output in  10^4 Gauss (or 10^-1 T )
   //   10^4 Gauss = 1 T
   
   TVector3 outField = TVector3(0,0,0);
   
   // find position boundaries
   double xMin=99, xMax=-99, yMin=99, yMax=-99, zMin=99, zMax=-99;
   double posX = position.x();
   double posY = position.y();
   double posZ = position.z();
   
   // find position boundaries
   Int_t binX = fMagHistoX->GetXaxis()->FindBin(posX);
   if (binX > fMaxBinX) return outField;
   xMin = fMagHistoX->GetXaxis()->GetBinLowEdge(binX);
   xMax = fMagHistoX->GetXaxis()->GetBinUpEdge(binX);
   
   Int_t binY = fMagHistoY->GetYaxis()->FindBin(posY);
   if (binY > fMaxBinY) return outField;
   yMin = fMagHistoY->GetYaxis()->GetBinLowEdge(binY);
   yMax = fMagHistoY->GetYaxis()->GetBinUpEdge(binY);
   
   Int_t binZ = fMagHistoY->GetZaxis()->FindBin(posZ);
   if (binZ > fMaxBinZ) return outField;
   zMin = fMagHistoZ->GetZaxis()->GetBinLowEdge(binZ);
   zMax = fMagHistoZ->GetZaxis()->GetBinUpEdge(binZ);
   
   double xDiff = (position.x() - xMin)/(xMax-xMin);
   double yDiff = (position.y() - yMin)/(yMax-yMin);
   double zDiff = (position.z() - zMin)/(zMax-zMin);
   
   // interpolate
   // X
   Int_t c000 = fMagHistoX->FindBin(xMin, yMin, zMin);
   Int_t c001 = fMagHistoX->FindBin(xMin, yMin, zMax);
   Int_t c010 = fMagHistoX->FindBin(xMin, yMax, zMin);
   Int_t c011 = fMagHistoX->FindBin(xMin, yMax, zMax);
   
   Int_t c111 = fMagHistoX->FindBin(xMax, yMax, zMax);
   Int_t c100 = fMagHistoX->FindBin(xMax, yMin, zMin);
   Int_t c101 = fMagHistoX->FindBin(xMax, yMin, zMax);
   Int_t c110 = fMagHistoX->FindBin(xMax, yMax, zMin);
   
   double B_00x = fMagHistoX->GetBinContent(c000) * (1-xDiff) + fMagHistoX->GetBinContent(c100) * xDiff;
   double B_01x = fMagHistoX->GetBinContent(c001) * (1-xDiff) + fMagHistoX->GetBinContent(c101) * xDiff;
   double B_10x = fMagHistoX->GetBinContent(c010) * (1-xDiff) + fMagHistoX->GetBinContent(c110) * xDiff;
   double B_11x = fMagHistoX->GetBinContent(c011) * (1-xDiff) + fMagHistoX->GetBinContent(c111) * xDiff;
   
   double B_0x = B_00x * (1 - yDiff) + B_10x * yDiff;
   double B_1x = B_01x * (1 - yDiff) + B_11x * yDiff;
   
   outField.SetX(B_0x * (1 - zDiff) + B_1x * zDiff);
   
   // Y
   c000 = fMagHistoY->FindBin(xMin, yMin, zMin);
   c001 = fMagHistoY->FindBin(xMin, yMin, zMax);
   c010 = fMagHistoY->FindBin(xMin, yMax, zMin);
   c011 = fMagHistoY->FindBin(xMin, yMax, zMax);
   
   c111 = fMagHistoY->FindBin(xMax, yMax, zMax);
   c100 = fMagHistoY->FindBin(xMax, yMin, zMin);
   c101 = fMagHistoY->FindBin(xMax, yMin, zMax);
   c110 = fMagHistoY->FindBin(xMax, yMax, zMin);
   
   double B_00y = fMagHistoY->GetBinContent(c000) * (1-xDiff) + fMagHistoY->GetBinContent(c100) * xDiff;
   double B_01y = fMagHistoY->GetBinContent(c001) * (1-xDiff) + fMagHistoY->GetBinContent(c101) * xDiff;
   double B_10y = fMagHistoY->GetBinContent(c010) * (1-xDiff) + fMagHistoY->GetBinContent(c110) * xDiff;
   double B_11y = fMagHistoY->GetBinContent(c011) * (1-xDiff) + fMagHistoY->GetBinContent(c111) * xDiff;
   
   double B_0y = B_00y * (1 - yDiff) + B_10y * yDiff;
   double B_1y = B_01y * (1 - yDiff) + B_11y * yDiff;
   
   outField.SetY(B_0y * (1 - zDiff) + B_1y * zDiff );
   
   // Z
   c000 = fMagHistoZ->FindBin(xMin, yMin, zMin);
   c001 = fMagHistoZ->FindBin(xMin, yMin, zMax);
   c010 = fMagHistoZ->FindBin(xMin, yMax, zMin);
   c011 = fMagHistoZ->FindBin(xMin, yMax, zMax);
   
   c111 = fMagHistoZ->FindBin(xMax, yMax, zMax);
   c100 = fMagHistoZ->FindBin(xMax, yMin, zMin);
   c101 = fMagHistoZ->FindBin(xMax, yMin, zMax);
   c110 = fMagHistoZ->FindBin(xMax, yMax, zMin);
   
   double B_00z = fMagHistoZ->GetBinContent(c000) * (1-xDiff) + fMagHistoZ->GetBinContent(c100) * xDiff;
   double B_01z = fMagHistoZ->GetBinContent(c001) * (1-xDiff) + fMagHistoZ->GetBinContent(c101) * xDiff;
   double B_10z = fMagHistoZ->GetBinContent(c010) * (1-xDiff) + fMagHistoZ->GetBinContent(c110) * xDiff;
   double B_11z = fMagHistoZ->GetBinContent(c011) * (1-xDiff) + fMagHistoZ->GetBinContent(c111) * xDiff;
   
   double B_0z = B_00z * (1 - yDiff) + B_10z * yDiff;
   double B_1z = B_01z * (1 - yDiff) + B_11z * yDiff;
   
   outField.SetZ(B_0z * (1 - zDiff) + B_1z * zDiff);
   
   return outField*1e-3;    // 10^3 Gauss, 10^-1 T
}
