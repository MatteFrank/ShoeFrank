#ifndef _TAGrunInfo_HXX
#define _TAGrunInfo_HXX
/*!
  \file
  \version $Id: TAGrunInfo.hxx
  \brief   Declaration of TAGrunInfo.
*/
/*------------------------------------------+---------------------------------*/
#include "TString.h"

#include "TAGobject.hxx"

class TAGrunInfo : public TAGobject {
  public:
                    TAGrunInfo();
                    TAGrunInfo(Short_t i_cam, Short_t i_run, TString s_cam);
    virtual         ~TAGrunInfo();

    void            SetCampaignName(TString s_cam);
    void            SetCampaignNumber(Short_t i_cam);
    void            SetRunNumber(Short_t i_run);

    Short_t         CampaignNumber() const;
    Short_t         RunNumber() const;
    const Char_t*   CampaignName() const;
   
    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    friend bool     operator==(const TAGrunInfo& lhs, 
			       const TAGrunInfo& rhs);

    ClassDef(TAGrunInfo,2)

  private:
    Short_t         fiCam;		    // campaign number
    Short_t         fiRun;        // run number
    TString         fsCam;        // campaign name
};

#include "TAGrunInfo.icc"

#endif
