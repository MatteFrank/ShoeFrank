#ifndef _TAITntuTrack_HXX
#define _TAITntuTrack_HXX

// ROOT classes
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TArrayF.h"

#include "TAGdata.hxx"
#include "TAITntuCluster.hxx"

/**
 TAITline class, Line object is defined by its    
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

class TAITline : public TObject {
   
private:
   // origin x0,y0,z0 
   TVector3* fOrigin;       //-> 
   // the slope (dx/dz, dy/dz, 1)
   TVector3* fSlope;        //-> 
   Float_t   fLength;
   Float_t   fTheta;
   Float_t   fPhi;

public:
   TAITline();                       
   TAITline(TVector3 &aOrigin, TVector3 &aSlope, Float_t aLength);
   TAITline(const TAITline& aLine);
   TAITline& operator=(const TAITline& aLine);

   ~TAITline();
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
   Float_t            GetTheta()    const { return fTheta;   }  
   //! Get phi angle of line
   Float_t            GetPhi()      const { return fPhi;     }
   
   //! Get point on line at beta, parameter along the line
   TVector3           GetPoint(Float_t beta); 
   //! Set values of lines
   void               SetValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
   
   ClassDef(TAITline,2)   // Describes TAITline
   
};

//##############################################################################

/** TAITtrack class, simple container class for tracks with the associated clusters                    
 
 \author Ch. Finck
 */

class TAITcluster;
class TClonesArray;
class TAITtrack : public TObject {
   
private:
   Bool_t         fPileup;                       // true if track is part of pileup events
   UInt_t         fType;                         // 0 for straight, 1 inclined, 2 for bent
   Int_t          fTrackNumber;                  // number of the track
   TAITline*      fLineTrajectory;               // line trajectory
   TClonesArray*  fListOfClusters;               // list of cluster associated to the track
   Int_t          fShareCluster;                 // number of the track with common cluster, 0 otherwise 
   
   Float_t        fChiSquare;                    // chisquare/ndf of track fit in 2D
   Float_t        fChiSquareU;                   // chisquare/ndf of track fit, U dim
   Float_t        fChiSquareV;                   // chisquare/ndf of track fit, V dim
   Float_t        fVertexZ;                      // vertex z-position
  
   Int_t          fValidity;                     //if = 1 track attached to vertex,
   Double_t       fImpactParameter;              //impact parameter, if fValidity = -1 impact Parameter = 0
   TArrayF*       fChargeProba;                  // charge probability array
   Int_t          fChargeWithMaxProba;           // charge with maximum probability
   Float_t        fChargeMaxProba;               // charge maximum probability
   TArrayF*       fChargeProbaNorm;              // charge probability array for normalized charge disttribution
   Int_t          fChargeWithMaxProbaNorm;       // charge with maximum probability for normalized charge disttribution
   Float_t        fChargeMaxProbaNorm;           // charge maximum probability for normalized charge disttribution

   Int_t          fDebugLevel;                   // debug level

public:
   TAITtrack();                                 
   ~TAITtrack();
   TAITtrack(const TAITtrack& aTrack);
   
  
   //! Get distance with another track
   Float_t        Distance(TAITtrack* track, Float_t z) const;
   //! Get X-Y distance with another track
   TVector2       DistanceXY(TAITtrack* track, Float_t z) const;
   //! Get distance with another track
   TVector2       DistanceMin(TAITtrack* track1, Float_t zMin = -10000., Float_t zMax =  10000., Float_t eps = 5.) const;
   //! Reset track value
   void           Reset();
   //! Get intersection point with plane
   TVector3       Intersection(Float_t posZ) const;
   //! Get track number
   Int_t          GetNumber()               const { return   fTrackNumber;      }
   //! Get track line parameter
   TAITline&      GetTrackLine()            const { return  *fLineTrajectory;   }
    //! Get list of clusters
   TClonesArray*  GetListOfClusters()       const { return   fListOfClusters;   }
   //! Get number of clusters
   Int_t          GetClustersN()            const { return   fListOfClusters->GetEntries(); }
   //! Get valid flag
   Bool_t         IsPileUp()                const { return   fPileup;                }
   //! Get number of shared cluster 
   Int_t          GetShareCluster()         const { return   fShareCluster;          } 
   //! Get track type
   UInt_t         GetType()                 const { return   fType;                  }
   //! Get cluster
   TAITcluster*   GetCluster(Int_t index)         { return (TAITcluster*)fListOfClusters->At(index); }
 
  
   //! Add cluster
   void           AddCluster(TAITcluster* cluster);
   //! Set track number
   void           SetNumber(Int_t number)            { fTrackNumber = number;       }
   //! Set number of shared cluster 
   void           SetShareCluster(Int_t shared)      { fShareCluster = shared;      } 
   //! Set track type
   void           SetType(UInt_t type)               { fType = type;                }
   //! Set pileup flag
   void           SetPileUp(Bool_t pileup = true)    { fPileup = pileup;            }
   //Set Validity of track in vertex reconstruction
   void           SetValidity(Int_t q)               { fValidity = q;               }
   //! Set Z vertex
   void           SetVertexZ(Float_t z)              { fVertexZ = z;                }
   //Set Impact Parameter
   void           SetIP(Double_t q)                  { fImpactParameter = q;        } 
   //! Set values of line track
   void           SetLineValue(const TVector3& aOrigin, const TVector3& aSlope, const Float_t aLength = 0.);
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
   Float_t        GetChi2()           const { return   fChiSquare;     }
   //! Get chi squareU 
   Float_t        GetChi2U()          const { return   fChiSquareU;    }   
   //! Get chi squareV 
   Float_t        GetChi2V()          const { return   fChiSquareV;    }
   //! Get Z vertex
   Float_t       GetVertexZ()         const { return fVertexZ;         }
   //Get Validity
   Int_t         GetValidity()        const { return fValidity;        }
   //Get impact Parameter
   Double_t      GetIP()              const { return fImpactParameter; }
   //Get charge proba
   TArrayF*      GetChargeProba()     const { return fChargeProba; }
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

   
   ClassDef(TAITtrack,6)                      // Describes TAITtrack
};


//##############################################################################

class TAITntuTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		// tracks
   TVector3         fBeamPosition;       // Position of beam in x-y direction at z-target given by BM
   Bool_t           fPileup;             // true if pileup events
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   
public:
   TAITntuTrack();
   virtual         ~TAITntuTrack();
   
   TAITtrack*       GetTrack(Int_t i);
   const TAITtrack* GetTrack(Int_t i) const;
   Int_t            GetTracksN()      const; 
   
   TVector3         GetBeamPosition() const         { return fBeamPosition; }
   void             SetBeamPosition(TVector3& pos)  { fBeamPosition = pos;  }
   Bool_t           IsPileUp()        const         { return   fPileup;     }
   void             SetPileUp(Bool_t pileup = true) { fPileup = pileup;     }

   TClonesArray*    GetListOfTracks() { return fListOfTracks; }

   TAITtrack*       NewTrack();
   TAITtrack*       NewTrack(TAITtrack& track);
      
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }  
   
   ClassDef(TAITntuTrack,2)
};

#endif

