#ifndef _TAGroot_HXX
#define _TAGroot_HXX
/*!
  \file
  \version $Id: TAGroot.hxx,v 1.11 2003/07/08 18:55:38 mueller Exp $
  \brief   Declaration of TAGroot.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGobject.hxx"
#include "TAGnamed.hxx"
#include "TAGrunInfo.hxx"
#include "TAGeventId.hxx"

class TSignalHandler;
class TList;

class TAGaction;
class TAGdataDsc;
class TAGparaDsc;

class TAGroot : public TAGobject {
  public:
                    TAGroot();
    virtual         ~TAGroot();

    void            AddRequiredItem(TAGnamed* p_item);
    void            AddRequiredItem(const char* name);
    void            ClearRequiredList();

    void            BeginEventLoop();
    void            EndEventLoop();
    void            AbortEventLoop();

    Bool_t          NextEvent(long nEv=-999);

    void            Wait(Int_t i_msec);

    void            ClearAllAction();
    void            ClearAllData();
    void            ClearAllPara();

    TList*          ListOfAction()  const { return fpActionList;  }
    TList*          ListOfDataDsc() const { return fpDataDscList; }
    TList*          ListOfParaDsc() const { return fpParaDscList; }

    TAGaction*      FindAction(const char* name, const char* type=0) const;
    TAGdataDsc*     FindDataDsc(const char* name, const char* type=0) const;
    TAGparaDsc*     FindParaDsc(const char* name, const char* type=0) const;

    const char*     DefaultActionName();
    const char*     DefaultDataDscName();
    const char*     DefaultParaDscName();

    void            SetCampaignName(TString s_cam) { fRunInfo.SetCampaignName(s_cam); }
    void            SetRunNumber(Short_t i_run)    {  fRunInfo.SetRunNumber(i_run); fEventId.SetRunNumber(i_run);}
    void            SetEventNumber(Int_t i_evt)    {  fEventId.SetEventNumber(i_evt); }
    void            SetRunInfo(const TAGrunInfo& info);
    void            SetEventId(const TAGeventId& info);

    const Char_t*   CurrentCampaignName() const { return fRunInfo.CampaignName(); }
    Short_t         CurrentRunNumber()    const { return fEventId.RunNumber();    }
    Int_t           CurrentEventNumber()  const { return fEventId.EventNumber();  }

    const TAGrunInfo&  CurrentRunInfo()   const {return fRunInfo;  }
    const TAGeventId&  CurrentEventId()   const { return fEventId; }

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    ClassDef(TAGroot,0)

  private:
    TList*          fpActionList;
    TList*          fpDataDscList;
    TList*          fpParaDscList;
    TList*          fpRequiredActionList;
    TList*          fpRequiredDataDscList;
    Int_t           fiDefActionSeqNum;
    Int_t           fiDefDataDscSeqNum;
    Int_t           fiDefParaDscSeqNum;
    TSignalHandler* fpTAGrootInterruptHandler;
    Bool_t          fbDefaultHandlerRemoved;
    Bool_t          fbTAGrootHandlerAdded;
    Bool_t          fbAbortEventLoop;
    TAGrunInfo      fRunInfo;
    TAGeventId      fEventId;
};

extern TAGroot* gTAGroot;

#endif
