

#include "TAGroot.hxx"
#include "TADIgeoField.hxx"

#include "GlobalPar.hxx"


ClassImp(TADIgeoField);

//______________________________________________________________________________
TADIgeoField::TADIgeoField (TADIparGeo* diGeo)
 : TVirtualMagField(),
   fpDiGeoMap(diGeo),
   fpFootGeo(0x0),
    properties_mc{ retrieve_properties(diGeo) }
{
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (fpFootGeo == 0x0)
      Error("TADIgeoField()", "cannot find TAGgeoTrafo pointer");

   Float_t mesh   = fpDiGeoMap->GetMapMesh();
   TVector3 limLo = fpFootGeo->FromDILocalToGlobal(fpDiGeoMap->GetMapLimLo());
   TVector3 limUp = fpFootGeo->FromDILocalToGlobal(fpDiGeoMap->GetMapLimUp());
    
   fMaxBinX = (limUp[0]-limLo[0])/float(mesh);
   fMaxBinY = (limUp[1]-limLo[1])/float(mesh);
   fMaxBinZ = (limUp[2]-limLo[2])/float(mesh);

   fMagHistoX = new TH3F("MagHistoX", "Magnetic Field", fMaxBinX, limLo[0], limUp[0], fMaxBinY, limLo[1], limUp[1], fMaxBinZ, limLo[2], limUp[2]);
   fMagHistoY = new TH3F("MagHistoY", "Magnetic Field", fMaxBinX, limLo[0], limUp[0], fMaxBinY, limLo[1], limUp[1], fMaxBinZ, limLo[2], limUp[2]);
   fMagHistoZ = new TH3F("MagHistoZ", "Magnetic Field", fMaxBinX, limLo[0], limUp[0], fMaxBinY, limLo[1], limUp[1], fMaxBinZ, limLo[2], limUp[2]);
   
       field_mc.reserve( (properties_mc.x.size) * (properties_mc.y.size) * (properties_mc.z.size) );
    
    
   TString fullFileName = fpDiGeoMap->GetMapName();
   FromFile(fullFileName);
       
//       std::cout << "size->(x,y,z): (" << properties_mc.x.size << ", " << properties_mc.y.size << ", " << properties_mc.z.size << "\n";
//       std::cout << "product v.s. vector size: " << properties_mc.x.size * properties_mc.y.size * properties_mc.z.size << "  " << field_mc.size() << "\n";
//       auto compute_index = []( auto const& input_p, auto const& properties_p ){
//           return static_cast<std::size_t>( (input_p - properties_p.lower_limit)/(properties_p.upper_limit- properties_p.lower_limit) * properties_p.size );
//       };
//
//       for( auto i{0}; i < fMagHistoY->GetZaxis()->GetNbins() ; ++i){
//           auto bin_index = fMagHistoY->FindBin(-5, -4.5, i*0.5 + properties_mc.z.lower_limit);
//
//           auto index_y = compute_index( -4.5, properties_mc.y );
//           auto index_x = compute_index( -5, properties_mc.x );
//           auto index = index_x * properties_mc.y.size * properties_mc.z.size +
//                        index_y * properties_mc.z.size +
//                        i;
//          // std::cout << "index->(i_x, i_y, i): " << index << "->(" << index_x << ", " << index_y << ", " << i << ")\n";
//
//           std::cout << "i->(old, new): " << i << "->(" << fMagHistoZ->GetBinContent(bin_index) << ", " << field_mc[ index ].z << ")\n";
//       }
       
}

//______________________________________________________________________________
TADIgeoField::~TADIgeoField()
{
   delete fMagHistoX;
   delete fMagHistoY;
   delete fMagHistoZ;
}

//______________________________________________________________________________
TADIgeoField::dimensions_properties TADIgeoField::retrieve_properties(TADIparGeo const* geo_ph) const
{
    auto * transformation_h = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    auto lower_limit = transformation_h->FromDILocalToGlobal(geo_ph->GetMapLimLo());
    auto upper_limit = transformation_h->FromDILocalToGlobal(geo_ph->GetMapLimUp());
    auto mesh_size = geo_ph->GetMapMesh();
    
    return dimensions_properties{
        {upper_limit.X(), lower_limit.X(), mesh_size, static_cast<std::size_t>((upper_limit.X()-lower_limit.X())/mesh_size +1)},
        {upper_limit.Y(), lower_limit.Y(), mesh_size, static_cast<std::size_t>((upper_limit.Y()-lower_limit.Y())/mesh_size +1)},
        {upper_limit.Z(), lower_limit.Z(), mesh_size, static_cast<std::size_t>((upper_limit.Z()-lower_limit.Z())/mesh_size +1)}
    };
}

//______________________________________________________________________________
// read from file
void TADIgeoField::FromFile(TString& fullFileName)
{
   ifstream ifile;

   Info("TADIgeoField()", "Going to open %s", fullFileName.Data());
   
   ifile.open( fullFileName.Data() );

   if ( !ifile.is_open() ) {
      Error("FromFile()", "cannot open magnetic map for file %s\n", fullFileName.Data());
      exit(0);
   }
   
   // read position and field
   string line = "";
   
    //HACK: first line in field is skipped (unnecessary infos)
   getline(ifile,line);
   while( getline( ifile, line ) ) {
      
      if (line == "")  continue;
      if ( line.find("#") != string::npos || line.find("*") != string::npos )
         continue;
      
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
      
       field_mc.push_back( point{bx, by ,bz} );
       
      Int_t bin = fMagHistoY->FindBin(posG[0], posG[1], posG[2]);
      fMagHistoX->SetBinContent(bin, bx);
      fMagHistoY->SetBinContent(bin, by);
      fMagHistoZ->SetBinContent(bin, bz);
   }
   
   ifile.close();
   
   FootDebug(1, "FromFile()", "exiting");
}

//______________________________________________________________________________
// return B as a vector, given vector position
TVector3 TADIgeoField::GetField(const TVector3& position) const
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
TVector3 TADIgeoField::Interpolate(const TVector3& position) const
{
   //   https://en.wikipedia.org/wiki/Trilinear_interpolation
   //	  Output in Gauss
   
//   TVector3 outField = TVector3(0,0,0);
//
//   // find position boundaries
//   double xMin=99, xMax=-99, yMin=99, yMax=-99, zMin=99, zMax=-99;
//   double posX = position.x();
//   double posY = position.y();
//   double posZ = position.z();
//
//   // find position boundaries
//   Int_t binX = fMagHistoX->GetXaxis()->FindBin(posX);
//   if (binX > fMaxBinX) return outField;
//   xMin = fMagHistoX->GetXaxis()->GetBinLowEdge(binX);
//   xMax = fMagHistoX->GetXaxis()->GetBinUpEdge(binX);
//
//   Int_t binY = fMagHistoY->GetYaxis()->FindBin(posY);
//   if (binY > fMaxBinY) return outField;
//   yMin = fMagHistoY->GetYaxis()->GetBinLowEdge(binY);
//   yMax = fMagHistoY->GetYaxis()->GetBinUpEdge(binY);
//
//   Int_t binZ = fMagHistoY->GetZaxis()->FindBin(posZ);
//   if (binZ > fMaxBinZ) return outField;
//   zMin = fMagHistoZ->GetZaxis()->GetBinLowEdge(binZ);
//   zMax = fMagHistoZ->GetZaxis()->GetBinUpEdge(binZ);
//
//   double xDiff = (position.x() - xMin)/(xMax-xMin);
//   double yDiff = (position.y() - yMin)/(yMax-yMin);
//   double zDiff = (position.z() - zMin)/(zMax-zMin);
   
//    std::cout << "evaluation_point: (" << position.x() << ", " << position.y() << ", " << position.z() << ")\n";
//    std::cout << "old_xp: " << xDiff << '\n';
//    std::cout << "old_yp: " << yDiff << '\n';
//    std::cout << "old_zp: " << zDiff << '\n';
//
   // interpolate
   // X
//   Int_t c000 = fMagHistoX->FindBin(xMin, yMin, zMin);
//   Int_t c001 = fMagHistoX->FindBin(xMin, yMin, zMax);
//   Int_t c010 = fMagHistoX->FindBin(xMin, yMax, zMin);
//   Int_t c011 = fMagHistoX->FindBin(xMin, yMax, zMax);
//
//   Int_t c111 = fMagHistoX->FindBin(xMax, yMax, zMax);
//   Int_t c100 = fMagHistoX->FindBin(xMax, yMin, zMin);
//   Int_t c101 = fMagHistoX->FindBin(xMax, yMin, zMax);
//   Int_t c110 = fMagHistoX->FindBin(xMax, yMax, zMin);
   
//    std::cout << "old_field_x_lll: " << fMagHistoX->GetBinContent(c000) << '\n';
//    std::cout << "old_field_x_lul: " << fMagHistoX->GetBinContent(c010) << '\n';
//    std::cout << "old_field_x_ull: " << fMagHistoX->GetBinContent(c011) << '\n';
//    std::cout << "old_field_x_uul: " << fMagHistoX->GetBinContent(c001) << '\n';
//    std::cout << "old_field_x_llu: " << fMagHistoX->GetBinContent(c110) << '\n';
//    std::cout << "old_field_x_luu: " << fMagHistoX->GetBinContent(c100) << '\n';
//    std::cout << "old_field_x_ulu: " << fMagHistoX->GetBinContent(c010) << '\n';
//    std::cout << "old_field_x_uuu: " << fMagHistoX->GetBinContent(c111) << '\n';
    
//   double B_00x = fMagHistoX->GetBinContent(c000) * (1-xDiff) + fMagHistoX->GetBinContent(c100) * xDiff;
//   double B_01x = fMagHistoX->GetBinContent(c001) * (1-xDiff) + fMagHistoX->GetBinContent(c101) * xDiff;
//   double B_10x = fMagHistoX->GetBinContent(c010) * (1-xDiff) + fMagHistoX->GetBinContent(c110) * xDiff;
//   double B_11x = fMagHistoX->GetBinContent(c011) * (1-xDiff) + fMagHistoX->GetBinContent(c111) * xDiff;
    
 //   std::cout << "old_field_x_ll: " << B_00x << '\n';
 //   std::cout << "old_field_x_lu: " << B_01x << '\n';
 //   std::cout << "old_field_x_ul: " << B_10x << '\n';
//    std::cout << "old_field_x_uu: " << B_11x << '\n';
   
//   double B_0x = B_00x * (1 - yDiff) + B_10x * yDiff;
//   double B_1x = B_01x * (1 - yDiff) + B_11x * yDiff;
    
 //   std::cout << "old_field_x_l: " << B_0x << '\n';
 //   std::cout << "old_field_x_u: " << B_1x << '\n';
   
//   outField.SetX(B_0x * (1 - zDiff) + B_1x * zDiff);
//    std::cout << "field_old_x : " << outField.X() << '\n';
   
   // Y
//   c000 = fMagHistoY->FindBin(xMin, yMin, zMin);
//   c001 = fMagHistoY->FindBin(xMin, yMin, zMax);
//   c010 = fMagHistoY->FindBin(xMin, yMax, zMin);
//   c011 = fMagHistoY->FindBin(xMin, yMax, zMax);
//
//   c111 = fMagHistoY->FindBin(xMax, yMax, zMax);
//   c100 = fMagHistoY->FindBin(xMax, yMin, zMin);
//   c101 = fMagHistoY->FindBin(xMax, yMin, zMax);
//   c110 = fMagHistoY->FindBin(xMax, yMax, zMin);
    
//    std::cout << "old_field_y_lll: " << fMagHistoY->GetBinContent(c000) << '\n';
//    std::cout << "old_field_y_lul: " << fMagHistoY->GetBinContent(c010) << '\n';
//    std::cout << "old_field_y_ull: " << fMagHistoY->GetBinContent(c011) << '\n';
//    std::cout << "old_field_y_uul: " << fMagHistoY->GetBinContent(c001) << '\n';
//    std::cout << "old_field_y_llu: " << fMagHistoY->GetBinContent(c110) << '\n';
//    std::cout << "old_field_y_luu: " << fMagHistoY->GetBinContent(c100) << '\n';
//    std::cout << "old_field_y_ulu: " << fMagHistoY->GetBinContent(c010) << '\n';
//    std::cout << "old_field_y_uuu: " << fMagHistoY->GetBinContent(c111) << '\n';
   
//   double B_00y = fMagHistoY->GetBinContent(c000) * (1-xDiff) + fMagHistoY->GetBinContent(c100) * xDiff;
//   double B_01y = fMagHistoY->GetBinContent(c001) * (1-xDiff) + fMagHistoY->GetBinContent(c101) * xDiff;
//   double B_10y = fMagHistoY->GetBinContent(c010) * (1-xDiff) + fMagHistoY->GetBinContent(c110) * xDiff;
//   double B_11y = fMagHistoY->GetBinContent(c011) * (1-xDiff) + fMagHistoY->GetBinContent(c111) * xDiff;
    
 //   std::cout << "old_field_y_ll: " << B_00y << '\n';
 //   std::cout << "old_field_y_lu: " << B_01y << '\n';
 //   std::cout << "old_field_y_ul: " << B_10y << '\n';
 //   std::cout << "old_field_y_uu: " << B_11y << '\n';
   
//   double B_0y = B_00y * (1 - yDiff) + B_10y * yDiff;
//   double B_1y = B_01y * (1 - yDiff) + B_11y * yDiff;
    
 //   std::cout << "old_field_x_l: " << B_0y << '\n';
//    std::cout << "old_field_x_u: " << B_1y << '\n';
   
//   outField.SetY(B_0y * (1 - zDiff) + B_1y * zDiff );
//   std::cout << "field_old_y : " << outField.Y() << '\n';
    
   // Z
//   c000 = fMagHistoZ->FindBin(xMin, yMin, zMin);
//   c001 = fMagHistoZ->FindBin(xMin, yMin, zMax);
//   c010 = fMagHistoZ->FindBin(xMin, yMax, zMin);
//   c011 = fMagHistoZ->FindBin(xMin, yMax, zMax);
//
//   c111 = fMagHistoZ->FindBin(xMax, yMax, zMax);
//   c100 = fMagHistoZ->FindBin(xMax, yMin, zMin);
//   c101 = fMagHistoZ->FindBin(xMax, yMin, zMax);
//   c110 = fMagHistoZ->FindBin(xMax, yMax, zMin);
    
//    std::cout << "old_field_z_lll: " << fMagHistoZ->GetBinContent(c000) << '\n';
//    std::cout << "old_field_z_lul: " << fMagHistoZ->GetBinContent(c010) << '\n';
//    std::cout << "old_field_z_ull: " << fMagHistoZ->GetBinContent(c011) << '\n';
//    std::cout << "old_field_z_uul: " << fMagHistoZ->GetBinContent(c001) << '\n';
//    std::cout << "old_field_z_llu: " << fMagHistoZ->GetBinContent(c110) << '\n';
//    std::cout << "old_field_z_luu: " << fMagHistoZ->GetBinContent(c100) << '\n';
//    std::cout << "old_field_z_ulu: " << fMagHistoZ->GetBinContent(c010) << '\n';
//    std::cout << "old_field_z_uuu: " << fMagHistoZ->GetBinContent(c111) << '\n';
//
//   double B_00z = fMagHistoZ->GetBinContent(c000) * (1-xDiff) + fMagHistoZ->GetBinContent(c100) * xDiff;
//   double B_01z = fMagHistoZ->GetBinContent(c001) * (1-xDiff) + fMagHistoZ->GetBinContent(c101) * xDiff;
//   double B_10z = fMagHistoZ->GetBinContent(c010) * (1-xDiff) + fMagHistoZ->GetBinContent(c110) * xDiff;
//   double B_11z = fMagHistoZ->GetBinContent(c011) * (1-xDiff) + fMagHistoZ->GetBinContent(c111) * xDiff;
    
  //  std::cout << "old_field_z_ll: " << B_00z << '\n';
  //  std::cout << "old_field_z_lu: " << B_01z << '\n';
  //  std::cout << "old_field_z_ul: " << B_10z << '\n';
  //  std::cout << "old_field_z_uu: " << B_11z << '\n';
   
//   double B_0z = B_00z * (1 - yDiff) + B_10z * yDiff;
//   double B_1z = B_01z * (1 - yDiff) + B_11z * yDiff;
    
   // std::cout << "old_field_z_l: " << B_0z << '\n';
   // std::cout << "old_field_z_u: " << B_1z << '\n';
   
//   outField.SetZ(B_0z * (1 - zDiff) + B_1z * zDiff);
//    std::cout << "field_old_z : " << outField.Z() << '\n';

    
    auto evaluation_point = TADIgeoField::point{ position.X(), position.Y(), position.Z() };
    if( is_outside( evaluation_point ) ){ return TVector3{0, 0, 0}; }  //return zero directly
    
    auto upper = compute_upper_point( evaluation_point );
    auto lower = compute_lower_point( evaluation_point );
    
    auto xp = (evaluation_point.x - lower.x)/(upper.x - lower.x);
    auto yp = (evaluation_point.y - lower.y)/(upper.y - lower.y);
    auto zp = (evaluation_point.z - lower.z)/(upper.z - lower.z);

//    std::cout << "new_xp: " << xp << '\n';
//    std::cout << "new_yp: " << yp << '\n';
//    std::cout << "new_zp: " << zp << '\n';
    
    auto const* r_h = retrieve_field( TADIgeoField::point{upper.x, lower.y, lower.z} );
    auto const& f_xu_yl_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field( TADIgeoField::point{lower.x, lower.y, lower.z} );
    auto const& f_xl_yl_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field(TADIgeoField::point{upper.x, upper.y, lower.z});
    auto const& f_xu_yu_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field(TADIgeoField::point{lower.x, upper.y, lower.z});
    auto const& f_xl_yu_zl = r_h ? *r_h : TADIgeoField::point{} ;
    
    
//    std::cout << "new_field_lll: (" << f_xl_yl_zl.x << ", " << f_xl_yl_zl.y << ", " <<  f_xl_yl_zl.z << ")\n";
//    std::cout << "new_field_ull: (" << f_xu_yl_zl.x << ", " << f_xu_yl_zl.y << ", " <<  f_xu_yl_zl.z << ")\n";
//    std::cout << "new_field_lul: (" << f_xl_yu_zl.x << ", " << f_xl_yu_zl.y << ", " <<  f_xl_yu_zl.z << ")\n";
//    std::cout << "new_field_uul: (" << f_xu_yu_zl.x << ", " << f_xu_yu_zl.y << ", " <<  f_xu_yu_zl.z << ")\n";
//
    

    
    r_h = retrieve_field(TADIgeoField::point{upper.x, lower.y, upper.z});
    auto const& f_xu_yl_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field(TADIgeoField::point{lower.x, lower.y, upper.z});
    auto const& f_xl_yl_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field(TADIgeoField::point{upper.x, upper.y, upper.z});
    auto const& f_xu_yu_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = retrieve_field(TADIgeoField::point{lower.x, upper.y, upper.z});
    auto const& f_xl_yu_zu = r_h ? *r_h : TADIgeoField::point{} ;
    
    
//    std::cout << "new_field_uuu: (" << f_xu_yu_zu.x << ", " << f_xu_yu_zu.y << ", " <<  f_xu_yu_zu.z << ")\n";
//    std::cout << "new_field_luu: (" << f_xl_yu_zu.x << ", " << f_xl_yu_zu.y << ", " <<  f_xl_yu_zu.z << ")\n";
//    std::cout << "new_field_ulu: (" << f_xu_yl_zu.x << ", " << f_xu_yl_zu.y << ", " <<  f_xu_yl_zu.z << ")\n";
//    std::cout << "new_field_llu: (" << f_xl_yl_zu.x << ", " << f_xl_yl_zu.y << ", " <<  f_xl_yl_zu.z << ")\n";
    
    auto f_x_yl_zl = xp * f_xu_yl_zl + (1-xp) * f_xl_yl_zl ;
    auto f_x_yu_zl = xp * f_xu_yu_zl + (1-xp) * f_xl_yu_zl;
    auto f_x_yl_zu = xp * f_xu_yl_zu + (1-xp) * f_xl_yl_zu;
    auto f_x_yu_zu = xp * f_xu_yu_zu + (1-xp) * f_xl_yu_zu;
    
   
    
    auto f_x_y_zl = yp * f_x_yu_zl + (1-yp) * f_x_yl_zl;
    auto f_x_y_zu = yp * f_x_yu_zu + (1-yp) * f_x_yl_zu;
    
    auto f_x_y_z = zp * f_x_y_zu + (1-zp) * f_x_y_zl ;
    
//    std::cout << "field_new_x : " << f_x_y_z.x << '\n';
//    std::cout << "field_new_y : " << f_x_y_z.y << '\n';
//    std::cout << "field_new_z : " << f_x_y_z.z << '\n';
    
//    outField.SetX( f_x_y_z.x );
//    outField.SetY( f_x_y_z.y );
//    outField.SetZ( f_x_y_z.z );
    
    
//   return outField;    // Gauss
    return TVector3{ f_x_y_z.x, f_x_y_z.y, f_x_y_z.z };
}


TADIgeoField::point const * TADIgeoField::retrieve_field( TADIgeoField::point const& input_p ) const
{
    if( is_outside(input_p) ){ return nullptr; }
    
    auto compute_index = []( auto const& input_p, auto const& properties_p ) -> std::size_t {
        return (input_p - properties_p.lower_limit)/(properties_p.upper_limit- properties_p.lower_limit) * (properties_p.size-1);
                         };
    
    auto index_z = compute_index( input_p.z, properties_mc.z );
    auto index_y = compute_index( input_p.y, properties_mc.y );
    auto index_x = compute_index( input_p.x, properties_mc.x );
    auto index = index_x * properties_mc.y.size * properties_mc.z.size +
                 index_y * properties_mc.z.size +
                 index_z;
   // std::cout << "index->(x,y,z): " << index << "->(" << index_x << ", " << index_y << ", " << index_z << ")\n";
    return std::addressof( field_mc[index] );
}


bool TADIgeoField::is_outside( TADIgeoField::point const& input_p ) const
{
    return ( input_p.y > properties_mc.y.upper_limit ||
             input_p.y < properties_mc.y.lower_limit ||
             input_p.x > properties_mc.x.upper_limit ||
             input_p.x < properties_mc.x.lower_limit ||
             input_p.z > properties_mc.z.upper_limit ||
             input_p.z < properties_mc.z.lower_limit     ) ? true : false;
}


TADIgeoField::point TADIgeoField::compute_upper_point( TADIgeoField::point const& input_p ) const
{
    auto compute_value = []( auto const& input_p, auto const& properties_p ){
        return static_cast<std::size_t>( (input_p - properties_p.lower_limit)/
                                         (properties_p.upper_limit - properties_p.lower_limit) *
                                         (properties_p.size-1)  +1)   * 1./
               (properties_p.size-1) * (properties_p.upper_limit - properties_p.lower_limit) + properties_p.lower_limit ;
                            };
    
   // std::cout << "upper_x: " <<  compute_value( input_p.x, properties_mc.x ) << '\n';
    
    auto upper = TADIgeoField::point{ compute_value( input_p.x, properties_mc.x ),
                                      compute_value( input_p.y, properties_mc.y ),
                                      compute_value( input_p.z, properties_mc.z ) };
    return upper;
}

TADIgeoField::point TADIgeoField::compute_lower_point( TADIgeoField::point const& input_p ) const
{
    auto compute_value = []( auto const& input_p, auto const& properties_p ){
        return static_cast<std::size_t>( (input_p - properties_p.lower_limit)/
                                        (properties_p.upper_limit - properties_p.lower_limit) *
                                        (properties_p.size-1)  )   * 1./
        (properties_p.size-1) * (properties_p.upper_limit - properties_p.lower_limit) + properties_p.lower_limit ;
    };
    
    auto lower = TADIgeoField::point{ compute_value( input_p.x, properties_mc.x ),
                                      compute_value( input_p.y, properties_mc.y ),
                                      compute_value( input_p.z, properties_mc.z ) };
    return lower;
}
