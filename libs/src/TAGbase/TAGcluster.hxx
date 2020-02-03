#ifndef _TAGcluster_HXX
#define _TAGcluster_HXX
/*!
  \file
  \version $Id: TAGcluster.hxx
  \brief   Declaration of TAGcluster.
*/
/*------------------------------------------+---------------------------------*/
#include "TVector3.h"

#include "TAGobject.hxx"

using namespace std;

class TAGcluster : public TAGobject {
   
  public:
                      TAGcluster();
   virtual           ~TAGcluster() {}

   //! Get position in local frame
   virtual const TVector3&  GetPosition() const = 0;
   
   //! Get position error in local frame
   virtual const TVector3&  GetPosError() const = 0;
   
   //! Get position in detector frame
   virtual const TVector3&  GetPositionG() const = 0;
   
   //! Get position error in detector frame
   virtual const TVector3&  GetPosErrorG() const = 0;
   
    ClassDef(TAGcluster,1)
};

#endif
