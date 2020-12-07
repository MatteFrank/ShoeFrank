#ifndef _TAGeventId_HXX
#define _TAGeventId_HXX
/*!
  \file
  \version $Id: TAGeventId.hxx,v 1.2 2003/06/09 18:46:45 mueller Exp $
  \brief   Declaration of TAGeventId.
*/
/*------------------------------------------+---------------------------------*/

#include "Rtypes.h"

class TAGeventId {
  public:
                    TAGeventId();
                    TAGeventId(Short_t i_cam, Short_t i_run, Int_t i_evt);
    virtual         ~TAGeventId();

    void            SetCampaignNumber(Short_t i_cam);
    void            SetRunNumber(Short_t i_run);
    void            SetEventNumber(Int_t i_evt);

    Short_t         CampaignNumber() const;
    Short_t         RunNumber() const;
    Int_t           EventNumber() const;

    void            Clear();

    friend bool     operator==(const TAGeventId& lhs, 
			       const TAGeventId& rhs);
   
   const TAGeventId& operator=(const TAGeventId &right);


    ClassDef(TAGeventId,1)

  private:				    // NOTE: CUSTOM STREAMER
    Short_t         fiCam;		    // campaign number
    Short_t         fiRun;		    // run number
    Int_t           fiEvt;		    // event number
};

#include "TAGeventId.icc"

#endif
