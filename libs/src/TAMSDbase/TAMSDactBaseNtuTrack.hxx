#ifndef _TAMSDactBaseNtuTrack_HXX
#define _TAMSDactBaseNtuTrack_HXX
/*!
 \file
 \version $Id: TAMSDactBaseNtuTrack.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
 \brief   Declaration of TAMSDactBaseNtuTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"

#include "TAVTactBaseTrack.hxx"

#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"


class TAGgeoTrafo;
class TABMtrack;
class TGraphErrors;
class TAVTbaseCluster;
class TAGbaseTrack;
class TH2F;
class TH1F;
class TAMSDactBaseNtuTrack : public TAVTactBaseTrack {
   
public:
   explicit  TAMSDactBaseNtuTrack(const char* name       = 0,
                                 TAGdataDsc* p_ntuclus  = 0,
                                 TAGdataDsc* p_ntutrack = 0,
                                 TAGparaDsc* p_config   = 0,
                                 TAGparaDsc* p_geomap   = 0);
   
   virtual ~TAMSDactBaseNtuTrack();
   
   // Base action
   virtual         Bool_t  Action();
   
   //! Base creation of histogram
   virtual  void   CreateHistogram();
   
protected:
   TH1F*            fpHisStripTot;    // Total number of strips per tracked point
   TH1F*            fpHisStrip[6];   // number of strip per tracked cluster for each point
   TH1F*            fpHisChargeTot;    // Total number of charge per tracked point
   TH1F*            fpHisCharge[6];   // number of charge per tracked cluster for each point
   TH1F*            fpHisPointLeft;   // number of point left (not tracked)
   
protected:
   virtual void   FillHistogramm(TAGbaseTrack* track);
   virtual void   FillHistogramm();
   virtual Bool_t FindStraightTracks();
   virtual Bool_t IsGoodCandidate(TAMSDtrack* /*track*/) { return true; }
   virtual Bool_t FindTiltedTracks() { return true; }
   virtual Bool_t FindVertices()     { return false; }

   TAGbaseTrack* GetTrack(Int_t idx);
   Int_t          GetTracksN() const;
   
   ClassDef(TAMSDactBaseNtuTrack,0)
};

#endif
