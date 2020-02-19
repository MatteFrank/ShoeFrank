#ifndef _TAGcluster_HXX
#define _TAGcluster_HXX
/*!
  \file
  \version $Id: TAGcluster.hxx
  \brief   Declaration of TAGcluster.
*/
/*------------------------------------------+---------------------------------*/
#include <map>
#include "TVector3.h"
#include "TArrayI.h"

#include "TAGobject.hxx"

using namespace std;

class TAGcluster : public TAGobject {
   
  public:
                      TAGcluster();
                      TAGcluster(const TAGcluster& cluster);

   virtual           ~TAGcluster() {}

   //! Get position in local frame
   virtual const TVector3&  GetPosition() const = 0;
   
   //! Get position error in local frame
   virtual const TVector3&  GetPosError() const = 0;
   
   //! Get position in detector frame
   virtual const TVector3&  GetPositionG() const = 0;
   
   //! Get position error in detector frame
   virtual const TVector3&  GetPosErrorG() const = 0;
   
   //! Add MC track Idx
   void               AddMcTrackIdx(Int_t trackIdx);
   
   //! Get MC info
   Int_t              GetMcTrackIdx(Int_t index)      const   { return fMcTrackIdx[index];    }
   Int_t              GetMcTracksN()                  const   { return fMcTrackIdx.GetSize(); }
   
private:
   TArrayI            fMcTrackIdx;               // Idx of the track created in the simulation
   map<int, int>      fMcTrackMap;               // Map of MC track Id

   
    ClassDef(TAGcluster,1)
};

#endif
