#ifndef TADIgeoField_H
#define TADIgeoField_H

#include <vector>
#include <utility>

#include <TVector3.h>
#include <TH3F.h>
#include "TVirtualMagField.h"

#include "TAGgeoTrafo.hxx"
#include "TADIparGeo.hxx"



class TADIgeoField : public TVirtualMagField {
    
public:
    struct point{
        double x;
        double y;
        double z;
        
        friend point operator+( point const& lhs_p, point const& rhs_p )
        { return point{ lhs_p.x + rhs_p.x, lhs_p.y + rhs_p.y, lhs_p.z + rhs_p.z  }; }
        friend point operator*( double factor_p, point const& value_p )
        { return point{ value_p.x * factor_p, value_p.y  * factor_p , value_p.z * factor_p  }; }
    };
    
    struct DimensionsProperties {
        struct {
            double upper_limit;
            double lower_limit;
            double mesh_size;
            std::size_t size;
        } x, y, z;
    };
    
public:

	TADIgeoField(TADIparGeo* diGeo);

   ~TADIgeoField();

   TVector3 GetField(const TVector3& position)    const;
   TVector3 Interpolate(const TVector3& position) const;
   
   void     Field(const Double_t* pos, Double_t* fieldB);
   
   void     FromFile(TString& name);

private:
   DimensionsProperties RetrieveProperties(TADIparGeo const* ) const;
   point    ComputeUpperPoint( point const& input_p ) const;
   point    ComputeLowerPoint( point const& input_p ) const;
   point    const * RetrieveField( point const& input_p ) const;
   bool     IsOutside( point const& input_p ) const;
  
private:
   TADIparGeo*          fpDiGeoMap;
   TAGgeoTrafo*         fpFootGeo;

   DimensionsProperties fProperties;  //not const because of ROOT
   std::vector<point>   fField;
   
   ClassDef(TADIgeoField,2)
};

#endif
