#ifndef _TAGeventId_HXX
#define _TAGeventId_HXX
/*!
  \file TAGeventId.hxx
  \brief   Declaration of TAGeventId.
*/
/*------------------------------------------+---------------------------------*/

#include "Rtypes.h"

class TAGeventId {
  public:
                    TAGeventId();
                    TAGeventId(Short_t i_cam, Short_t i_run, Int_t i_evt);
    virtual         ~TAGeventId();

   //! Set campaign number
    void            SetCampaignNumber(Short_t i_cam)  { fiCam = i_cam; }
   //! Set run number
    void            SetRunNumber(Short_t i_run)       { fiRun = i_run; }
   //! Set event number
    void            SetEventNumber(Int_t i_evt)       { fiEvt = i_evt; }

   //! Get Campaign number
    Short_t         CampaignNumber()            const { return fiCam;  }
   //! Get run number
    Short_t         RunNumber()                 const { return fiRun;  }
   // Get event number
    Int_t           EventNumber()               const { return fiEvt;  }
   //! Clear
    void            Clear();

   //! operator==
    friend bool     operator==(const TAGeventId& lhs, 
			       const TAGeventId& rhs);
   //! operator=
   const TAGeventId& operator=(const TAGeventId &right);

  private:
    Short_t         fiCam;		    ///< campaign number
    Short_t         fiRun;		    ///< run number
    Int_t           fiEvt;		    ///< event number
   
   ClassDef(TAGeventId,1)

};

#endif
