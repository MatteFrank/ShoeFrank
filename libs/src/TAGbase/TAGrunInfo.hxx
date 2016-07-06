#ifndef _TAGrunInfo_HXX
#define _TAGrunInfo_HXX
/*!
  \file
  \version $Id: TAGrunInfo.hxx,v 1.2 2003/06/09 18:50:00 mueller Exp $
  \brief   Declaration of TAGrunInfo.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGobject.hxx"

class TAGrunInfo : public TAGobject {
  public:
                    TAGrunInfo();
                    TAGrunInfo(Short_t i_cam, Short_t i_run);
    virtual         ~TAGrunInfo();

    void            SetCampaignNumber(Short_t i_cam);
    void            SetRunNumber(Short_t i_run);

    Short_t         CampaignNumber() const;
    Short_t         RunNumber() const;

    virtual void    Clear(Option_t* opt="");

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    friend bool     operator==(const TAGrunInfo& lhs, 
			       const TAGrunInfo& rhs);

    ClassDef(TAGrunInfo,1)

  private:
    Short_t         fiCam;		    // campaign number
    Short_t         fiRun;		    // run number
};

#include "TAGrunInfo.icc"

#endif
