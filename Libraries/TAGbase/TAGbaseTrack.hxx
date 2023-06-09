#ifndef _TAGbaseTrack_HXX
#define _TAGbaseTrack_HXX

/*!
 \file TAGbaseTrack.hxx
 \brief  class, simple container class for tracks with the associated clusters

 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <vector>

///< ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TArrayF.h"
#include "TArrayI.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAGcluster.hxx"
#include "TAGcluster.hxx"
#include "TAGgeoTrafo.hxx"

// Fit
#include "TFitResult.h"

class TClonesArray;
class TAGbaseTrack : public TAGobject {

protected:
   //!   origin x0,y0,z0
   TVector3*      fOrigin;                       //->   origin x0,y0,z0
   //! the slope (dx/dz, dy/dz, 1)
   TVector3*      fSlope;                        //->   the slope (dx/dz, dy/dz, 1)
   //! origin error dx0,dy0,dz0
   TVector3*      fOriginErr;                    //->   origin dx0,dy0,dz0
   //!  the error slope (dx/dz, dy/dz, 1)
   TVector3*      fSlopeErr;                     //->   the error slope (dx/dz, dy/dz, 1)


   Float_t        fLength;                       ///< Length of track

   Bool_t         fPileup;                       ///< true if track is part of pileup events
   UInt_t         fType;                         ///< 0 for straight, 1 inclined, 2 for bent
   Int_t          fTrackIdx;                     ///< number of the track
   TClonesArray*  fListOfClusters;               ///< list of cluster associated to the track

   Float_t        fChiSquare;                    ///< chisquare/ndf of track fit in 2D

   Float_t        fChiSquareU;                   ///< chisquare/ndf of track fit, U dim
   Float_t        fChiSquareV;                   ///< chisquare/ndf of track fit, V dim
   Float_t        fChiSquareRedU;                 
   Float_t        fChiSquareRedV;  

   Float_t        fPvalueU;                      ///< pvalue of fit U dim
   Float_t        fPvalueV;                      ///< pvalue of fit U dim
   TVector3       fPosVertex;                    ///< vertex position

   Int_t          fValidity;                     ///< if = 1 track attached to vertex,
    //! charge probability array
   TArrayF*       fChargeProba;                  //! charge probability array
   //! charge with maximum probability
   Int_t          fChargeWithMaxProba;           //! charge with maximum probability
   //! charge maximum probability
   Float_t        fChargeMaxProba;               //! charge maximum probability
   //! charge probability array for normalized charge disttribution
   TArrayF*       fChargeProbaNorm;              //! charge probability array for normalized charge disttribution
   //! charge with maximum probability for normalized charge disttribution
   Int_t          fChargeWithMaxProbaNorm;       //! charge with maximum probability for normalized charge disttribution
   //! charge maximum probability for normalized charge disttribution
   Float_t        fChargeMaxProbaNorm;           //! charge maximum probability for normalized charge disttribution
   Double32_t     fMeanEltsN;                    ///< Average number of pixels/strips per track
   Double32_t     fMeanCharge;                   ///< Average charge (for analogic sensor) per track

   TArrayI            fMcTrackIdx;               ///< Idx of the track created in the simulation
   //! Map of MC track Id
   std::map<int, int> fMcTrackMap;               //! Map of MC track Id
   TMatrixDSym fCovMatrixU;                          //! covariance matrix in X position for fit
   TMatrixDSym fCovMatrixV;                          //! covariance matrix in Y position for fit

public: 
   TAGbaseTrack();
   ~TAGbaseTrack();
   TAGbaseTrack(const TAGbaseTrack& aTrack);

   //! Get cluster
   virtual TAGcluster*   GetCluster(Int_t /*index*/) { return 0x0; }

   // Reset line
   void               Zero();
   // Get Distance to a point
   Float_t            Distance(TVector3 &p);
   //! Get origin of line
   TVector3&          GetOrigin()   const { return *fOrigin; }
   //! Get slope of line
   TVector3&          GetSlopeZ()   const { return *fSlope;  }
   //! Get error origin of line
   TVector3&          GetOriginErr() const { return *fOriginErr; }
   //! Get error slope of line
   TVector3&          GetSlopeErrZ() const { return *fSlopeErr;  }

   TMatrixDSym GetCovMatrixU() const {return fCovMatrixU; }
   TMatrixDSym GetCovMatrixV() const { return fCovMatrixV; }

   //! Get length of line
   Float_t            GetLength()   const { return fLength;  }
   // Get theta angle of line
   Float_t            GetTheta()    const;
   // Get phi angle of line
   Float_t            GetPhi()      const;

   // Set values of track
   void               SetValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
   // Set error values of track
   void               SetErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr);

   //! Set chi square
   void SetChi2(Float_t chi2) { fChiSquare = chi2; }
   //! Set chi squareU
   void SetChi2RedU(Float_t chi2U) { fChiSquareRedU = chi2U; }
   //! Set chi squareV
   void SetChi2RedV(Float_t chi2V){fChiSquareRedV = chi2V;}
   //! Set chi squareU
   void SetChi2U(Float_t chi2U) { fChiSquareU = chi2U; }
   //! Set chi squareV
   void SetChi2V(Float_t chi2V){fChiSquareV = chi2V;}
   //! Set chi squareRedU
   void SetChi2RedU(Float_t chi2U) { fChiSquareRedU = chi2U; }
   //! Set chi squareRedV
   void SetChi2RedV(Float_t chi2V){fChiSquareRedV = chi2V;}

   void SetPvalueU(Float_t pU) { fPvalueU = pU; }
   //! Set chi squareV
   void SetPvalueV(Float_t pV) { fPvalueV = pV; }

   // Get distance with another track
   Float_t Distance(TAGbaseTrack *track, Float_t z) const;
   // Get X-Y distance with another track
   TVector2       DistanceXY(TAGbaseTrack* track, Float_t z) const;
   // Get distance with another track
   TVector2       DistanceMin(TAGbaseTrack* track1, Float_t zMin = -10000., Float_t zMax =  10000., Float_t eps = 5.) const;
   // Reset track value
   void           Reset();
   // Get intersection point with plane
   TVector3       Intersection(Float_t posZ) const;
   //! Get track number
   Int_t          GetTrackIdx()              const { return   fTrackIdx;      }
    //! Get list of clusters
   TClonesArray*  GetListOfClusters()        const { return   fListOfClusters;   }
   //! Get number of clusters
   Int_t          GetClustersN()             const { return   fListOfClusters->GetEntriesFast(); }
   //! Get valid flag
   Bool_t         IsPileUp()                 const { return   fPileup;                }
   //! Get track type
   UInt_t         GetType()                  const { return   fType;                  }

   //! Set track number
   void           SetTrackIdx(Int_t number)          { fTrackIdx = number;          }
   //! Set track type
   void           SetType(UInt_t type)               { fType = type;                }
   //! Set pileup flag
   void           SetPileUp(Bool_t pileup = true)    { fPileup = pileup;            }
   //! Set Validity of track in vertex reconstruction
   void           SetValidity(Int_t q)               { fValidity = q;               }
   // Set values of line track
   void           SetLineValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
   // Set values of line track
   void           SetLineErrorValue(const TVector3& aOriginErr, const TVector3& aSlopeErr);
   // Set covariance of the fit of the track
   void           SetCovarianceMatrix(TMatrixDSym covMatrixU, TMatrixDSym covMatrixV);

       // Make chi square
       void MakeChiSquare(Float_t dhs = 0.);
   // Set charge proba
   void           SetChargeProba(const TArrayF* proba);
   //! Set charge with max proba
   void           SetChargeWithMaxProba(Int_t proba)     { fChargeWithMaxProba = proba; }
   //! Set charge  max proba
   void           SetChargeMaxProba(Float_t proba)       { fChargeMaxProba = proba;     }
   // Set charge proba normalize
   void           SetChargeProbaNorm(const TArrayF* proba);
   //! Set charge with max proba normalize
   void           SetChargeWithMaxProbaNorm(Int_t proba) { fChargeWithMaxProbaNorm = proba; }
   //! Set charge  max proba normalize
   void           SetChargeMaxProbaNorm(Float_t proba)   { fChargeMaxProbaNorm = proba; }

   //! Add cluster (virtual)
   virtual  void  AddCluster(TAGcluster* /*cluster*/) { return; }

   //! Get chi square
   Float_t        GetChi2()                 const { return fChiSquare;               }
   //! Get chi squareU (reduced)
   Float_t        GetChi2redU()                const { return fChiSquareRedU;              }
   //! Get chi squareV (reduced)
   Float_t        GetChi2redV()                const { return fChiSquareRedV;              }
   //! Get chi squareU (total)
   Float_t        GetChi2U()                const { return fChiSquareU;              }
   //! Get chi squareV (total)
   Float_t        GetChi2V()                const { return fChiSquareV;              }
   //! Get chi squareRedV (reduced)
   Float_t        GetChi2RedV()                const { return fChiSquareRedV;              }
   //! Get chi squareRedU (reduced)
   Float_t        GetChi2RedU()                const { return fChiSquareRedU;              }
   //! Get chi square p value U
   Float_t GetPvalueU() const { return fPvalueU; }
   //! Get chi square p value V
   Float_t GetPvalueV() const { return fPvalueV; }

   //! Get Validity
   Int_t         GetValidity()               const { return fValidity;               }
   //! Get charge proba
   TArrayF*      GetChargeProba()            const { return fChargeProba;            }
   //! Get charge with max proba
   Int_t         GetChargeWithMaxProba()     const { return fChargeWithMaxProba;     }
   //! Get charge max proba
   Float_t       GetChargeMaxProba()         const { return fChargeMaxProba;         }

   //! Get charge proba
   TArrayF*      GetChargeProbaNorm()        const { return fChargeProbaNorm;        }
   //! Get charge with max proba
   Int_t         GetChargeWithMaxProbaNorm() const { return fChargeWithMaxProbaNorm; }
   //! Get charge max proba
   Float_t       GetChargeMaxProbaNorm()     const { return fChargeMaxProbaNorm;     }

   //! Get mean number of pixels per tracks
   Double32_t    GetMeanEltsN()              const { return fMeanEltsN/(float) fListOfClusters->GetEntriesFast(); }

   //! Get mean charge per tracks
   Double32_t    GetMeanCharge()             const { return fMeanCharge/(float) fListOfClusters->GetEntriesFast(); }

   // Add MC track Idx
   void          AddMcTrackIdx(Int_t trackIdx);

   //! Get MC info
   Int_t         GetMcTrackIdx(Int_t index)  const { return fMcTrackIdx[index];    }
   //! Get MC info size
   Int_t         GetMcTracksN()              const { return fMcTrackIdx.GetSize(); }

   ClassDef(TAGbaseTrack,1)                      ///< Describes TAGbaseTrack
};

#endif
