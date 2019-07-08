#ifndef _TAGntuGlbTrack_HXX
#define _TAGntuGlbTrack_HXX
/*!
 \file
 \version $Id: TAGntuGlbTrack.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGntuGlbTrack.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
#include "TVector3.h"

using namespace std;


#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAGntuPoint.hxx"

class TAGtrack : public TAGobject {
   
public:
   
   TAGtrack();
   TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t id, Int_t trkID);

   virtual         ~TAGtrack();
   
   void             SetMass(Double_t amass)    { fMass = amass; }
   Double_t         GetMass()     const { return fMass; }
   
   void             SetMomentum(Double_t amom) { fMom = amom; }
   Double_t         GetMomentum() const { return fMom;}
   
   void             SetCharge(Double_t acharge) { fCharge = acharge; }
   Double_t         GetCharge()   const { return fCharge; }
   
   void             SetTof(Double_t atoff)     { fTof = atoff; }
   Double_t         GetTof()     const { return fTof; }
   
   void             SetId(Int_t id)             { fId = id; }
   Int_t            GetId()       const { return fId; }
   
   void             SetTgtDirection(TVector3 dir) { fTgtDir = dir; }
   TVector3         GetTgtDirection() { return fTgtDir; }
   
   void             SetTgtPosition(TVector3 pos) { fTgtPos = pos; }
   TVector3         GetTgtPosition() { return fTgtPos; }
   
   void             SetTofPosition(TVector3 pos) { fTofPos = pos; }
   TVector3         GetTofPosition() { return fTofPos; }
   
   void             SetTofDirection(TVector3 dir) { fTofDir = dir; }
   TVector3         GetTofDirection() { return fTofDir; }
      
   void             SetTrackId(Int_t trid) { fTrkID = trid; }
   Int_t            GetTrackId() const { return fTrkID; }
   
   void             SetEnergy(Double_t e) { fEnergy = e; }
   Double_t         GetEnergy() const { return fEnergy; }
   
   //! Get list of points
   TClonesArray*    GetListOfPoints()       const { return   fListOfPoints;   }
   
   //! Get number of points
   Int_t            GetPointsN()            const { return   fListOfPoints->GetEntries(); }
   
   //! Get point
   TAGpoint*        GetPoint(Int_t index)  { return (TAGpoint*)fListOfPoints->At(index); }
   
   //! Add point
   void             AddPoint(TAGpoint* point);

   void             Clear(Option_t* opt="");
   
   void             SetupClones();
   
private:
   Double32_t       fMass;
   Double32_t       fMom;
   Double32_t       fCharge;
   Double32_t       fTof;
   Double32_t       fEnergy;
   Int_t            fId;
   Int_t            fTrkID;
   
   //Particle directions and positions computed on ToF Wall
   TVector3         fTgtDir;
   TVector3         fTgtPos;
   
   //Particle directions and positions computed on ToF Wall
   TVector3         fTofPos;
   TVector3         fTofDir;
   
   TClonesArray*    fListOfPoints;		    // Attached points
   
   ClassDef(TAGtrack,1)
   
};

//##############################################################################

class TAGntuGlbTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		// tracks
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   
public:
   TAGntuGlbTrack();
   virtual         ~TAGntuGlbTrack();
   
   TAGtrack*        GetTrack(Int_t i);
   const TAGtrack*  GetTrack(Int_t i) const;
   Int_t            GetTracksN()      const;
   
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }
   
   TAGtrack*        NewTrack();
   TAGtrack*        NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof, Double_t energy, Int_t id, Int_t trkID);
   TAGtrack*        NewTrack(TAGtrack& track);
   
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAGntuGlbTrack,2)
};


#endif
