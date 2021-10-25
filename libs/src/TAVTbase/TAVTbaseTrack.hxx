#ifndef _TAVTbaseTrack_HXX
#define _TAVTbaseTrack_HXX

#include <map>

// ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TArrayF.h"
#include "TArrayI.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAVTntuCluster.hxx"

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

//##############################################################################

/** TAVTbaseTrack class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TClonesArray;
class TAVTbaseTrack : public TAGobject {
   
protected:
   TVector3*      fOrigin;                       //->   origin x0,y0,z0
   TVector3*      fSlope;                        //->   the slope (dx/dz, dy/dz, 1)
   TVector3*      fOriginErr;                    //->   origin dx0,dy0,dz0
   TVector3*      fSlopeErr;                     //->   the error slope (dx/dz, dy/dz, 1)

   Float_t        fLength;

   Bool_t         fPileup;                       // true if track is part of pileup events
   UInt_t         fType;                         // 0 for straight, 1 inclined, 2 for bent
   Int_t          fTrackNumber;                  // number of the track
   TClonesArray*  fListOfClusters;               // list of cluster associated to the track
   
   Float_t        fChiSquare;                    // chisquare/ndf of track fit in 2D
   Float_t        fChiSquareU;                   // chisquare/ndf of track fit, U dim
   Float_t        fChiSquareV;                   // chisquare/ndf of track fit, V dim
   TVector3       fPosVertex;                    // vertex position
  
   Int_t          fValidity;                     // if = 1 track attached to vertex,
   TArrayF*       fChargeProba;                  //! charge probability array
   Int_t          fChargeWithMaxProba;           //! charge with maximum probability
   Float_t        fChargeMaxProba;               //! charge maximum probability
   TArrayF*       fChargeProbaNorm;              //! charge probability array for normalized charge disttribution
   Int_t          fChargeWithMaxProbaNorm;       //! charge with maximum probability for normalized charge disttribution
   Float_t        fChargeMaxProbaNorm;           //! charge maximum probability for normalized charge disttribution
   Double32_t     fMeanEltsN;                  // Average number of pixels/strips per track
   Double32_t     fMeanCharge;                   // Average charge (for analogic sensor) per track

   TArrayI            fMcTrackIdx;               // Idx of the track created in the simulation
   std::map<int, int> fMcTrackMap;               //! Map of MC track Id
   
public:
   TAVTbaseTrack();                                 
   ~TAVTbaseTrack();
   TAVTbaseTrack(const TAVTbaseTrack& aTrack);
   
   //! Get cluster
   virtual TAGcluster*   GetCluster(Int_t /*index*/) { return 0x0; }
   
   //! Reset line
   void               Zero();
   //! Get Distance to a point
   Float_t            Distance(TVector3 &p);
   //! Get origin of line
   TVector3&          GetOrigin()   const { return *fOrigin; }
   //! Get slope of line
   TVector3&          GetSlopeZ()   const { return *fSlope;  }
   //! Get error origin of line
   TVector3&          GetOriginErr() const { return *fOriginErr; }
   //! Get error slope of line
   TVector3&          GetSlopeErrZ() const { return *fSlopeErr;  }
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
   void               SetErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr);

  
   //! Get distance with another track
   Float_t        Distance(TAVTbaseTrack* track, Float_t z) const;
   //! Get X-Y distance with another track
   TVector2       DistanceXY(TAVTbaseTrack* track, Float_t z) const;
   //! Get distance with another track
   TVector2       DistanceMin(TAVTbaseTrack* track1, Float_t zMin = -10000., Float_t zMax =  10000., Float_t eps = 5.) const;
   //! Reset track value
   void           Reset();
   //! Get intersection point with plane
   TVector3       Intersection(Float_t posZ) const;
   //! Get track number
   Int_t          GetNumber()               const { return   fTrackNumber;      }
    //! Get list of clusters
   TClonesArray*  GetListOfClusters()       const { return   fListOfClusters;   }
   //! Get number of clusters
   Int_t          GetClustersN()            const { return   fListOfClusters->GetEntries(); }
   //! Get valid flag
   Bool_t         IsPileUp()                const { return   fPileup;                }
   //! Get track type
   UInt_t         GetType()                 const { return   fType;                  }
   
   //! Set track number
   void           SetNumber(Int_t number)            { fTrackNumber = number;       }
   //! Set track type
   void           SetType(UInt_t type)               { fType = type;                }
   //! Set pileup flag
   void           SetPileUp(Bool_t pileup = true)    { fPileup = pileup;            }
   //Set Validity of track in vertex reconstruction
   void           SetValidity(Int_t q)               { fValidity = q;               }
   //! Set pos vertex
   void           SetPosVertex(TVector3& pos)        { fPosVertex = pos;            }
   //! Set values of line track
   void           SetLineValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
   //! Set values of line track
   void           SetLineErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr);
   //! Make chi square 
   void           MakeChiSquare(Float_t dhs = 0.);
   //Set charge proba
   void           SetChargeProba(const TArrayF* proba);
   //Set charge with max proba
   void           SetChargeWithMaxProba(Int_t proba)     { fChargeWithMaxProba = proba; }
   //Set charge  max proba
   void           SetChargeMaxProba(Float_t proba)       { fChargeMaxProba = proba;     }
   //Set charge proba normalize
   void           SetChargeProbaNorm(const TArrayF* proba);
   //Set charge with max proba normalize
   void           SetChargeWithMaxProbaNorm(Int_t proba) { fChargeWithMaxProbaNorm = proba; }
   //Set charge  max proba normalize
   void           SetChargeMaxProbaNorm(Float_t proba)   { fChargeMaxProbaNorm = proba;     }

 
    //! Get chi square 
   Float_t        GetChi2()           const { return fChiSquare;     }
   //! Get chi squareU 
   Float_t        GetChi2U()          const { return fChiSquareU;    }
   //! Get chi squareV 
   Float_t        GetChi2V()          const { return fChiSquareV;    }
   //! Get pos vertex
   const TVector3& GetPosVertex()     const { return fPosVertex;     }
   //Get Validity
   Int_t         GetValidity()        const { return fValidity;      }
   //Get charge proba
   TArrayF*      GetChargeProba()     const { return fChargeProba;   }
   //Get charge with max proba
   Int_t         GetChargeWithMaxProba() const { return fChargeWithMaxProba; }
   //Get charge max proba
   Float_t       GetChargeMaxProba()  const { return fChargeMaxProba; }
   
   //Get charge proba
   TArrayF*      GetChargeProbaNorm()        const { return fChargeProbaNorm; }
   //Get charge with max proba
   Int_t         GetChargeWithMaxProbaNorm() const { return fChargeWithMaxProbaNorm; }
   //Get charge max proba
   Float_t       GetChargeMaxProbaNorm()     const { return fChargeMaxProbaNorm; }

   //Get mean number of pixels per tracks
   Double32_t    GetMeanEltsN()             const { return fMeanEltsN/(float) fListOfClusters->GetEntries(); }
   
   //Get mean charge per tracks
   Double32_t    GetMeanCharge()             const { return fMeanCharge/(float) fListOfClusters->GetEntries(); }
   
   // Add MC track Idx
   void          AddMcTrackIdx(Int_t trackIdx);
   
   // Get MC info
   Int_t         GetMcTrackIdx(Int_t index)  const { return fMcTrackIdx[index];    }
   Int_t         GetMcTracksN()              const { return fMcTrackIdx.GetSize(); }
   
   ClassDef(TAVTbaseTrack,10)                      // Describes TAVTbaseTrack
};

#endif

