#ifndef _TABMvieTrackFOOT_HXX
#define _TABMvieTrackFOOT_HXX
/*!
  \file
  \version $Id: TABMvieTrackFOOT.hxx,v 1.3 2003/06/22 11:05:58 mueller Exp $
  \brief   Declaration of TABMvieTrackFOOT.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGview.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TABMntuTrack.hxx"
#include "TABMntuRaw.hxx"
#include "TABMparGeo.hxx"
#include "TABMntuTrack.hxx"
#include "TABMtrack.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMparGeo.hxx"

#include "TPad.h"
#include "TROOT.h"
#include "TString.h"
#include "TAttFill.h"
#include "TAttLine.h"
#include "TAttText.h"
#include "TArc.h"

#include <math.h>

class TABMvieTrackFOOT : public TAGview {
  public:
    //~ explicit        TABMvieTrackFOOT(TAGdataDsc* p_ntutrk=0, TAGdataDsc* p_nturaw=0, TAGparaDsc* p_bmgeo=0);
    explicit        TABMvieTrackFOOT(TABMntuTrack* p_ntutrk=nullptr, TABMntuRaw* p_nturaw=nullptr, TABMparGeo* p_bmgeo=nullptr, Int_t fTrackOk=-1000, Int_t fEventNumber=-1000);
    //~ explicit        TABMvieTrackFOOT(TABMparGeo* p_bmgeo=nullptr);
    virtual         ~TABMvieTrackFOOT();

    virtual char*   GetObjectInfo(Int_t px, Int_t py) const;
    virtual void    ExecuteEvent(Int_t i_event, Int_t i_px, Int_t i_py);//???

    virtual void    Paint(Option_t* option="");

    void SetCelloccupy(vector< vector<Int_t> > &vec_in){fCellOccupancy=&vec_in; return;};
    //~ void SetTrackRaw(TABMntuTrack* p_ntutrk_in, TABMntuRaw* p_nturaw_in){p_ntutrk=p_ntutrk_in;p_nturaw=p_nturaw_in; return;};

    ClassDef(TABMvieTrackFOOT,0)

  private:
    //~ TAGdataDsc*     fpNtuTrk;
    //~ TAGdataDsc*     fpNtuRaw;
    //~ TAGparaDsc*     fpParBMGeo;
    vector< vector<Int_t> >*  fCellOccupancy;// occupancy of the cell for a given event, first index is the cellid, the value stored is the position of the hit in the bmnturaw vector, calculated in BmBooter
    Int_t         fTrackOk; //if !=0 the track is not set and it will not be drawn
    Int_t         fEventNumber;//number of the event
    TABMntuTrack* p_ntutrk;
    TABMtrack* p_trk;
    TABMntuRaw* p_nturaw;
    TABMparGeo* p_bmgeo;

};

#endif
