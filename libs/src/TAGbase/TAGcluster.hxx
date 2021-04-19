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

   //! Get cluster index
   Int_t                    GetClusterIdx()    const { return fClusterIdx; }
   //! Get sensor index
   Int_t                    GetSensorIdx()     const { return fSensorIdx;  }
   //! Set position in local frame
   virtual void             SetPosition(TVector3& pos);
   virtual void             SetPosition(Float_t u, Float_t v, Float_t z) { fPosition.SetXYZ(u,v,z); }
   //! Set position error in local frame
   virtual void             SetPosError(TVector3& pos);
   virtual void             SetPosError(Float_t u, Float_t v, Float_t z) { fPosError.SetXYZ(u,v,z); }
   //! Set position in global tracker frame
   virtual void             SetPositionG(TVector3& pos);
   virtual void             SetPositionG(Float_t u, Float_t v, Float_t z) { fPositionG.SetXYZ(u,v,z); }
   //! Set position error in global tracker frame
   virtual void             SetPosErrorG(TVector3& pos);
   virtual void             SetPosErrorG(Float_t u, Float_t v, Float_t z) { fPosErrorG.SetXYZ(u,v,z); }

   //! Set cluster number
   void                     SetClusterIdx(Int_t nb)  { fClusterIdx = nb;   }
   //! Set plane number
   void                     SetSensorIdx(Int_t nb)   { fSensorIdx = nb;    }
   //! Get position in local frame
  virtual const TVector3&   GetPosition()      const { return fPosition;   }
   //! Get position error in local frame
  virtual const TVector3&   GetPosError()      const { return fPosError;   }
   //! Get position in global tracker frame
  virtual const TVector3&   GetPositionG()     const { return fPositionG ; }
   //! Get position in global tracker frame
  virtual const TVector3&   GetPosErrorG()     const { return fPosErrorG ; }
   
   //! Add MC track Idx
   void               AddMcTrackIdx(Int_t trackIdx);
   
   //! Get MC info
   Int_t              GetMcTrackIdx(Int_t index)      const   { return fMcTrackIdx[index];    }
   Int_t              GetMcTracksN()                  const   { return fMcTrackIdx.GetSize(); }
   
protected:
   TVector3           fPosition;                 // position of the cluster in plane frame
   TVector3           fPosError;                 // position's errors of the cluster in plane frame
   TVector3           fPositionG;                // position of the clus in tracker frame
   TVector3           fPosErrorG;                // position's errors of the clus in tracker frame
   TArrayI            fMcTrackIdx;               // Idx of the track created in the simulation
   map<int, int>      fMcTrackMap;               // Map of MC track Id
   Int_t              fClusterIdx;               // cluster index
   Int_t              fSensorIdx;                // sensor index
   
    ClassDef(TAGcluster,2)
};

#endif
