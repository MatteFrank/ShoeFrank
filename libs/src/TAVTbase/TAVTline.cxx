//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// Class Description of TAVTline                                                            // 
//                                                                                          //
// a Line object is defined by its                                                          //
//                  origin     = (x_0,y_0,z_0),                                             //
//                  slope      = (dx/dz,dy/dz,1),                                           //
// and              length.                                                                 //
// Points on the line at r_i are given as a function                                        //
// of the parameter beta. beta has no dimension.                                            //
// r_i(beta) = origin_i + beta * direction_i                                                //
// If one wants the pair (x,y) as a function of z along (0,0,dz) then beta = z/dz           //
//    and r_1(beta) = x_0 + z * dx/dz                                                       //
//        r_2(beta) = y_0 + z * dy/dz                                                       //
//        r_3(beta) = z_0 + z * 1                                                           //  
// In case one needs pair (x,y,z) as a function of l along (dx,dy,dz) then beta' = l/dl     //
//    and r_1(beta') = x_0 + l * dx/dl                                                      //
//        r_2(beta') = y_0 + l * dy/dl                                                      //
//        r_3(beta') = z_0 + l * dz/dl                                                      //
// with the relation beta^2 = beta'^2 * (1-x^2/l^2-y^2/l^2) / (1-dx^2/dl^2-dy^2/dl^2)       //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TAVTline.hxx"


ClassImp(TAVTline) // Description of a Line

//______________________________________________________________________________
//  
TAVTline::TAVTline() 
: fOrigin(new TVector3()),
  fSlope(new TVector3()),
  fLength(0.0)
{
}

//______________________________________________________________________________
//  

TAVTline::TAVTline(TVector3 &aOrigin, TVector3 &aSlope, Float_t aLength)
{
   fOrigin    = new TVector3(aOrigin);
   fSlope     = new TVector3(aSlope);
   fLength    = aLength;
}

//______________________________________________________________________________
//  

TAVTline::TAVTline(const TAVTline& aLine)
:  TAGobject(aLine),
   fOrigin(new TVector3(*aLine.fOrigin)),
   fSlope(new TVector3(*aLine.fSlope)),
   fLength(aLine.fLength)
{
}

//______________________________________________________________________________
//  

TAVTline& TAVTline::operator=(const TAVTline& aLine)
{
   if (this == &aLine) return *this;

   fOrigin = new TVector3(*aLine.fOrigin);
   fSlope  = new TVector3(*aLine.fSlope);
   fLength = aLine.fLength;
   
   return *this;
}

//______________________________________________________________________________
//  
TAVTline::~TAVTline()
{
   delete  fOrigin;
   delete  fSlope;
}

//______________________________________________________________________________
//
Float_t TAVTline::GetTheta() const
{
   TVector3 direction = fSlope->Unit();
   Float_t theta      = direction.Theta()*TMath::RadToDeg();

   return theta;
}

//______________________________________________________________________________
//
Float_t TAVTline::GetPhi() const
{
   TVector3 origin = fOrigin->Unit();
   Float_t phi     = origin.Phi()*TMath::RadToDeg();

   return phi;
}

//______________________________________________________________________________
//  
void TAVTline::SetValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength)
{
   *fOrigin = aOrigin;
   *fSlope  = aSlope;
   fLength  = aLength;
}

//______________________________________________________________________________
//  
void TAVTline::Zero(){
   fOrigin->SetXYZ(0., 0., 0.);
   fSlope->SetXYZ(0., 0., 0.);
   fLength = 0;
}

//______________________________________________________________________________
// 
TVector3 TAVTline::GetPoint(Float_t beta)
{
   TVector3 result;
   result = (*fSlope) * beta;
   TVector3 offset = *fOrigin;
   offset(2) = 0.;
   return result += offset; 
}


//______________________________________________________________________________
//  
Float_t TAVTline::Distance(TVector3& p)
{
   // compute distance between a point M(x,y,z) and a
   // line defined by (origin, slope)
   // x = slopeX*z + originX
   // y = slopeY*z + originY
   // The distance is defined in a plane normal to the line
   //
   Float_t dX2 =  p.X()-fSlope->X()*p.Z()-fOrigin->X();
   dX2 *= dX2;
   Float_t dY2 =  p.Y()-fSlope->Y()*p.Z()-fOrigin->Y();
   dY2 *= dY2;
   Float_t prod2 = fSlope->Y()*(p.X()-fOrigin->X()) - fSlope->X()*(p.Y()-fOrigin->Y());
   prod2 *= prod2;
   
   Float_t distance = dX2 +dY2 + prod2;
   distance = TMath::Sqrt(distance);

   return distance;
}


