/*!
 \file TADIgeoField.cxx
 \brief Implementation of TADIgeoField.cxx
 */

#include "TAGroot.hxx"
#include "TADIgeoField.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TADIgeoField
 \brief  Class to handle the Mag field **
 */

//! Class Imp
ClassImp(TADIgeoField);

const TString TADIgeoField::fgkDefParaName = "geoField";

//______________________________________________________________________________
//! Constructor
TADIgeoField::TADIgeoField (TADIparGeo* diGeo)
 : TAGpara(),
   fpDiGeoMap(diGeo),
   fpFootGeo(0x0)
{
  if (fpDiGeoMap->GetType() == 2) {
    fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    if (fpFootGeo == 0x0)
      Error("TADIgeoField()", "cannot find TAGgeoTrafo pointer");

    fProperties = RetrieveProperties(diGeo);

    Float_t mesh   = fpDiGeoMap->GetMapMesh();
    TVector3 limLo = fpFootGeo->FromDILocalToGlobal(fpDiGeoMap->GetMapLimLo());
    TVector3 limUp = fpFootGeo->FromDILocalToGlobal(fpDiGeoMap->GetMapLimUp());
    
    fField.reserve( (fProperties.x.size) * (fProperties.y.size) * (fProperties.z.size) );
    
    TString fullFileName = fpDiGeoMap->GetMapName();
    FromFile(fullFileName);
     
  } else if(fpDiGeoMap->GetType() == 0)
     Info("TADIgeoField()", "Constant field with value (%.1f %.1f %.1f)\n", fpDiGeoMap->GetMagCstValue()[0], fpDiGeoMap->GetMagCstValue()[1], fpDiGeoMap->GetMagCstValue()[2]);
}

//______________________________________________________________________________
//! Destructor
TADIgeoField::~TADIgeoField()
{
}

//______________________________________________________________________________
//! Retrieve information from dipole geometry
//!
//! \param[in] geo_ph dipole geometry
TADIgeoField::DimensionsProperties TADIgeoField::RetrieveProperties(TADIparGeo const* geo_ph) const
{
    auto * transformation_h = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    auto lower_limit = transformation_h->FromDILocalToGlobal(geo_ph->GetMapLimLo());
    auto upper_limit = transformation_h->FromDILocalToGlobal(geo_ph->GetMapLimUp());
    auto mesh_size = geo_ph->GetMapMesh();
    
    return DimensionsProperties{
        {upper_limit.X(), lower_limit.X(), mesh_size, static_cast<std::size_t>((upper_limit.X()-lower_limit.X())/mesh_size +1)},
        {upper_limit.Y(), lower_limit.Y(), mesh_size, static_cast<std::size_t>((upper_limit.Y()-lower_limit.Y())/mesh_size +1)},
        {upper_limit.Z(), lower_limit.Z(), mesh_size, static_cast<std::size_t>((upper_limit.Z()-lower_limit.Z())/mesh_size +1)}
    };
}

//______________________________________________________________________________
//! Read from file
//!
//! \param[in] fullFileName dipole geometry file name
void TADIgeoField::FromFile(TString& fullFileName)
{
  if (fpDiGeoMap->GetType() != 2) return;

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
     
      fField.push_back( point{bx, by ,bz} );
   }
   
   ifile.close();
   
   //FootDebugLine(1, "FromFile()", "exiting");
}

//______________________________________________________________________________
//! return B as a vector, given vector position
//!
//! \param[in] position a given position
TVector3 TADIgeoField::GetField(const TVector3& position) const
{
   return Interpolate(position);
}

//______________________________________________________________________________
//! Get field as a vector, given vector position
//!
//! \param[in] pos a given position
//! \param[in] fieldB field vector
void TADIgeoField::Field(const Double_t* pos, Double_t* fieldB)
{
  if (fpDiGeoMap->GetType() == 2) {
    const TVector3 position = TVector3(pos[0], pos[1], pos[2] );
   
    TVector3 outField = Interpolate( position );
   
    fieldB[0] = outField.X();
    fieldB[1] = outField.Y();
    fieldB[2] = outField.Z();
  }
  
  if (fpDiGeoMap->GetType() == 0) {
    fieldB[0] = fpDiGeoMap->GetMagCstValue()[0];
    fieldB[1] = fpDiGeoMap->GetMagCstValue()[1];
    fieldB[2] = fpDiGeoMap->GetMagCstValue()[2];
  }
}

//______________________________________________________________________________
//! Get interpolated field as a vector, given vector position
//!
//! \param[in] position a given position
TVector3 TADIgeoField::Interpolate(const TVector3& position) const
{
   //   https://en.wikipedia.org/wiki/Trilinear_interpolation
   //	  Output in Gauss
    
    auto evaluation_point = TADIgeoField::point{ position.X(), position.Y(), position.Z() };
    if( IsOutside( evaluation_point ) ){ return TVector3{0, 0, 0}; }  //return zero directly
    
    auto upper = ComputeUpperPoint( evaluation_point );
    auto lower = ComputeLowerPoint( evaluation_point );
    
    auto xp = (evaluation_point.x - lower.x)/(upper.x - lower.x);
    auto yp = (evaluation_point.y - lower.y)/(upper.y - lower.y);
    auto zp = (evaluation_point.z - lower.z)/(upper.z - lower.z);
   
    auto const* r_h = RetrieveField( TADIgeoField::point{upper.x, lower.y, lower.z} );
    auto const& f_xu_yl_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField( TADIgeoField::point{lower.x, lower.y, lower.z} );
    auto const& f_xl_yl_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField(TADIgeoField::point{upper.x, upper.y, lower.z});
    auto const& f_xu_yu_zl = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField(TADIgeoField::point{lower.x, upper.y, lower.z});
    auto const& f_xl_yu_zl = r_h ? *r_h : TADIgeoField::point{} ;
  
    r_h = RetrieveField(TADIgeoField::point{upper.x, lower.y, upper.z});
    auto const& f_xu_yl_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField(TADIgeoField::point{lower.x, lower.y, upper.z});
    auto const& f_xl_yl_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField(TADIgeoField::point{upper.x, upper.y, upper.z});
    auto const& f_xu_yu_zu = r_h ? *r_h : TADIgeoField::point{} ;
    r_h = RetrieveField(TADIgeoField::point{lower.x, upper.y, upper.z});
    auto const& f_xl_yu_zu = r_h ? *r_h : TADIgeoField::point{} ;
  
    auto f_x_yl_zl = xp * f_xu_yl_zl + (1-xp) * f_xl_yl_zl ;
    auto f_x_yu_zl = xp * f_xu_yu_zl + (1-xp) * f_xl_yu_zl;
    auto f_x_yl_zu = xp * f_xu_yl_zu + (1-xp) * f_xl_yl_zu;
    auto f_x_yu_zu = xp * f_xu_yu_zu + (1-xp) * f_xl_yu_zu;
  
    auto f_x_y_zl = yp * f_x_yu_zl + (1-yp) * f_x_yl_zl;
    auto f_x_y_zu = yp * f_x_yu_zu + (1-yp) * f_x_yl_zu;

    auto f_x_y_z = zp * f_x_y_zu + (1-zp) * f_x_y_zl ;
   
    return TVector3{ f_x_y_z.x, f_x_y_z.y, f_x_y_z.z };
}

//______________________________________________________________________________
//! return field point for a given point
//!
//! \param[in] inputPos a given point
TADIgeoField::point const* TADIgeoField::RetrieveField(TADIgeoField::point const& inputPos) const
{
    if( IsOutside(inputPos) ){ return nullptr; }
    
    auto compute_index = []( auto const& inputPos, auto const& properties ) -> std::size_t {
        return (inputPos - properties.lower_limit)/(properties.upper_limit- properties.lower_limit) * (properties.size-1);
    };
    
    auto index_z = compute_index( inputPos.z, fProperties.z );
    auto index_y = compute_index( inputPos.y, fProperties.y );
    auto index_x = compute_index( inputPos.x, fProperties.x );
    auto index = index_x * fProperties.y.size * fProperties.z.size +
                 index_y * fProperties.z.size +
                 index_z;

   return std::addressof( fField[index] );
}

//______________________________________________________________________________
//! Check if point is outside the mag field box
//!
//! \param[in] inputPos a given point
bool TADIgeoField::IsOutside( TADIgeoField::point const& inputPos ) const
{
    return ( inputPos.y > fProperties.y.upper_limit ||
             inputPos.y < fProperties.y.lower_limit ||
             inputPos.x > fProperties.x.upper_limit ||
             inputPos.x < fProperties.x.lower_limit ||
             inputPos.z > fProperties.z.upper_limit ||
             inputPos.z < fProperties.z.lower_limit     ) ? true : false;
}

//______________________________________________________________________________
//! Compute Upper Point
//!
//! \param[in] inputPos a given point
TADIgeoField::point TADIgeoField::ComputeUpperPoint(TADIgeoField::point const& inputPos) const
{
    auto compute_value = []( auto const& inputPos, auto const& properties ){
        return static_cast<std::size_t>( (inputPos - properties.lower_limit)/
                                         (properties.upper_limit - properties.lower_limit) *
                                         (properties.size-1)  +1)   * 1./
               (properties.size-1) * (properties.upper_limit - properties.lower_limit) + properties.lower_limit ;
                            };
    auto upper = TADIgeoField::point{ compute_value( inputPos.x, fProperties.x ),
                                      compute_value( inputPos.y, fProperties.y ),
                                      compute_value( inputPos.z, fProperties.z ) };
    return upper;
}

//______________________________________________________________________________
//! Compute Lower Point
//!
//! \param[in] inputPos a given point
TADIgeoField::point TADIgeoField::ComputeLowerPoint( TADIgeoField::point const& inputPos ) const
{
    auto compute_value = []( auto const& inputPos, auto const& properties ) {
        return static_cast<std::size_t>( (inputPos - properties.lower_limit)/
                                        (properties.upper_limit - properties.lower_limit) *
                                        (properties.size-1)  )   * 1./
        (properties.size-1) * (properties.upper_limit - properties.lower_limit) + properties.lower_limit ;
    };
    
    auto lower = TADIgeoField::point{ compute_value( inputPos.x, fProperties.x ),
                                      compute_value( inputPos.y, fProperties.y ),
                                      compute_value( inputPos.z, fProperties.z ) };
    return lower;
}
