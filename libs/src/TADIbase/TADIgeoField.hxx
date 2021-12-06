#ifndef TADIgeoField_H
#define TADIgeoField_H

/*!
\file TADIgeoField.hxx
\brief   Declaration of TADIgenField.

\author Ch. Finck & A. Sécher
*/
/*------------------------------------------+---------------------------------*/


#include <vector>
#include <utility>

#include <TVector3.h>
#include <TH3F.h>

#include "TAGpara.hxx"
#include "TAGgeoTrafo.hxx"
#include "TADIparGeo.hxx"



class TADIgeoField : public TAGpara {
    
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

   //! Get field from position
   TVector3 GetField(const TVector3& position)    const;
   //! Interpolate position
   TVector3 Interpolate(const TVector3& position) const;
   
   //! Get field from psoition
   void     Field(const Double_t* pos, Double_t* fieldB);
   //! Read field from file
   void     FromFile(TString& name);

public:
   //! Get default parameter name
   static const Char_t* GetDefParaName()      { return fgkDefParaName.Data(); }
   
private:
   //! Compute dimensions
   DimensionsProperties RetrieveProperties(TADIparGeo const* ) const;
   //! Compute upper point
   point    ComputeUpperPoint( point const& input_p ) const;
   //! Compute lower point
   point    ComputeLowerPoint( point const& input_p ) const;
   //! Retrieve field point
   point    const * RetrieveField( point const& input_p ) const;
   //! Compute is outside the field box
   bool     IsOutside( point const& input_p ) const;
  
private:
   TADIparGeo*          fpDiGeoMap; ///< dipole geometry
   TAGgeoTrafo*         fpFootGeo;  ///< FOOT global tranformation

   DimensionsProperties fProperties;  ///< not const because of ROOT
   std::vector<point>   fField;       ///< field vector
 
private:
   static const TString fgkDefParaName; ///< default paramater name
   
   ClassDef(TADIgeoField,2)
};

#endif
