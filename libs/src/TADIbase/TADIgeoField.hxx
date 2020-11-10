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
    
    struct dimensions_properties {
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
   dimensions_properties retrieve_properties(TADIparGeo const* ) const;
   point compute_upper_point( point const& input_p ) const;
   point compute_lower_point( point const& input_p ) const;
   point const * retrieve_field( point const& input_p ) const;
    bool is_outside( point const& input_p ) const;
    
   TADIparGeo*  fpDiGeoMap;
   TAGgeoTrafo* fpFootGeo;

   TH3F*        fMagHistoX;
   TH3F*        fMagHistoY;
   TH3F*        fMagHistoZ;
   
   Int_t        fMaxBinX;
   Int_t        fMaxBinY;
   Int_t        fMaxBinZ;
    
    
    dimensions_properties properties_mc;  //not const because of ROOT
    std::vector<point> field_mc;
   
    
   ClassDef(TADIgeoField,2)
};





#endif
