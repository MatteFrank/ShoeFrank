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

class TAGbaseTrack;
class TAGcluster : public TAGobject {
   
protected:
   TVector3           fPosition;                 // position of the cluster in plane frame / measured position in FOOT frame
   TVector3           fPosError;                 // position's errors of the cluster in plane frame / measured position's error in FOOT frame
   TVector3           fPositionG;                // position of the clus in tracker frame / / fitted position in FOOT frame
   TVector3           fPosErrorG;                // position's errors of the clus in tracker frame /  fitted position's error in FOOT frame
   TArrayI            fMcTrackIdx;               // Idx of the track created in the simulation
   map<int, int>      fMcTrackMap;               //! Map of MC track Id
   Int_t              fClusterIdx;               // cluster index
   Int_t              fElementsN;                // number of cluster elements (pixels, strips, points...)
   Int_t              fSensorIdx;                // sensor index
   Int_t              fDeviceType;               // type of sensor VT: 40, IT: 50, MSD: 60, TW: 70, CA: 80
   Bool_t             fIsValid;                  // validity flag
   
public:
                      TAGcluster();
                      TAGcluster(const TAGcluster& cluster);

   virtual           ~TAGcluster() {}

   //! Get validity
   Bool_t                   IsValid()          const { return fIsValid;       }
   //! Get type
   Int_t                    GetDeviceType()    const { return fDeviceType;    }
   //! Get major type
   Int_t                    GetDevMajorType()  const { return fDeviceType/10; }
   //! Get minor type
   Int_t                    GetDevMinorType()  const { return fDeviceType%10; }
   //! Get cluster index
   Int_t                    GetClusterIdx()    const { return fClusterIdx;    }
   //! Get sensor index
   Int_t                    GetSensorIdx()     const { return fSensorIdx;     }
   //! Get cluster elts
   virtual Int_t            GetElementsN()     const { return fElementsN;     }
   //! Get position in local frame
   virtual const TVector3&  GetPosition()      const { return fPosition;      }
   //! Get position error in local frame
   virtual const TVector3&  GetPosError()      const { return fPosError;      }
   //! Get position in global tracker frame
   virtual const TVector3&  GetPositionG()     const { return fPositionG;     }
   //! Get position in global tracker frame
   virtual const TVector3&  GetPosErrorG()     const { return fPosErrorG;     }
   
   //! Get measured position in FOOT frame
   virtual const TVector3&  GetMeasPosition()  const { return fPosition;      }
   //! Get measured position error in FOOT frame
   virtual const TVector3&  GetMeasPosError()  const { return fPosError;      }
   //! Get fitted position in FOOT frame
   virtual const TVector3&  GetFitPosition()   const { return fPositionG;     }
   //! Get fitted position in FOOT frame
   virtual const TVector3&  GetFitPosError()   const { return fPosErrorG;     }
   
   //! Get MC info
   Int_t                    GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   Int_t                    GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
   //! Compute distance from a track
   Float_t                  Distance(TAGbaseTrack *aTrack); 
   
   //! Set validy
   void                     SetValid(Bool_t v = true)                     { fIsValid = v;             }
   //! Set cluster type
   void                     SetDeviceType(Int_t nb)                       { fDeviceType = nb;         }
   //! Set cluster elements
   void                     SetElementsN(Int_t nb)                        { fElementsN = nb;          }
   //! Set cluster index
   void                     SetClusterIdx(Int_t nb)                       { fClusterIdx = nb;         }
   //! Set sensor index
   void                     SetSensorIdx(Int_t nb)                        { fSensorIdx = nb;          }
   //! Set position in local frame
   virtual void             SetPosition(TVector3& pos);
   virtual void             SetPosition(Float_t u, Float_t v, Float_t z)  { fPosition.SetXYZ(u,v,z);  }
   //! Set position error in local frame
   virtual void             SetPosError(TVector3& pos);
   virtual void             SetPosError(Float_t u, Float_t v, Float_t z)  { fPosError.SetXYZ(u,v,z);  }
   //! Set position in global tracker frame
   virtual void             SetPositionG(TVector3& pos);
   virtual void             SetPositionG(Float_t u, Float_t v, Float_t z) { fPositionG.SetXYZ(u,v,z); }
   //! Set position error in global tracker frame
   virtual void             SetPosErrorG(TVector3& pos);
   virtual void             SetPosErrorG(Float_t u, Float_t v, Float_t z) { fPosErrorG.SetXYZ(u,v,z); }
   
   //! Set measured position in FOOT frame
   virtual void             SetMeasPosition(TVector3& pos)                    { fPosition = pos;          }
   virtual void             SetMeasPosition(Float_t u, Float_t v, Float_t z)  { fPosition.SetXYZ(u,v,z);  }
   //! Set measured position error in FOOT frame
   virtual void             SetMeasPosError(TVector3& pos)                    { fPosError = pos;          }
   virtual void             SetMeasPosError(Float_t u, Float_t v, Float_t z)  { fPosError.SetXYZ(u,v,z);  }
   //! Set fitted position in FOOT frame
   virtual void             SetFitPosition(TVector3& pos)                     { fPositionG = pos;         }
   virtual void             SetFitPosition(Float_t u, Float_t v, Float_t z)   { fPositionG.SetXYZ(u,v,z); }
   //! Get fitted position in FOOT frame
   virtual void             SetFitPosError(TVector3& pos)                     { fPosErrorG = pos;         }
   virtual void             SetFitPosError(Float_t u, Float_t v, Float_t z)   { fPosErrorG.SetXYZ(u,v,z); }
   
   //! Add MC track Idx
   void                     AddMcTrackIdx(Int_t trackIdx);
   
    ClassDef(TAGcluster,3)
};

#endif
