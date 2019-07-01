#ifndef _TAVTline_HXX
#define _TAVTline_HXX

// ROOT classes
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"

/**
 Line  is defined by its
 origin     = (x_0,y_0,z_0),                                            
 direction  = (dx,dy,dz),                                                
 and              length.                                                                 
 Points on the line at r_i are given as a function                                        
 of the parameter beta. beta has no dimension.                                            
 r_i(beta) = origin_i + beta * direction_i                                                
 If one wants the pair (x,y) as a function of z along (0,0,dz) then beta = z/dz           
 and r_1(beta) = x_0 + z * dx/dz                                                       
 r_2(beta) = y_0 + z * dy/dz                                                       
 r_3(beta) = z_0 + z * 1                                                            
 In case one needs pair (x,y,z) as a function of l along (dx,dy,dz) then beta' = l/dl     
 and r_1(beta') = x_0 + l * dx/dl                                                      
 r_2(beta') = y_0 + l * dy/dl                                                    
 r_3(beta') = z_0 + l * dz/dl                                                     
 with the relation beta^2 = beta'^2 * (1-x^2/l^2-y^2/l^2) / (1-dx^2/dl^2-dy^2/dl^2)  

 */

class TAVTline : public TAGobject {
   
private:
   // origin x0,y0,z0 
   TVector3* fOrigin;       //-> 
   // the slope (dx/dz, dy/dz, 1)
   TVector3* fSlope;        //-> 
   Float_t   fLength;

public:
   TAVTline();                       
   TAVTline(TVector3 &aOrigin, TVector3 &aSlope, Float_t aLength);
   TAVTline(const TAVTline& aLine);
   TAVTline& operator=(const TAVTline& aLine);

   ~TAVTline();
   //! Reset line
   void               Zero();
   //! Get Distance to a point
   Float_t            Distance(TVector3 &p);
   //! Get origin of line
   TVector3&          GetOrigin()   const { return *fOrigin; }
    //! Get slope of line
   TVector3&          GetSlopeZ()   const { return *fSlope;  } 
   //! Get length of line
   Float_t            GetLength()   const { return fLength;  }
   //! Get theta angle of line
   Float_t            GetTheta()    const;
   //! Get phi angle of line
   Float_t            GetPhi()      const;
   
   //! Get point on line at beta, parameter along the line
   TVector3           GetPoint(Float_t beta); 
   //! Set values of lines
   void               SetValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
   
   ClassDef(TAVTline,2)   // Describes TAVTline
   
};


#endif

