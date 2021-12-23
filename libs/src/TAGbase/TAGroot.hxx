#ifndef _TAGroot_HXX
#define _TAGroot_HXX
/*!
  \file TAGroot.hxx
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

    //! Add required item
    void            AddRequiredItem(TAGnamed* p_item);
    //! Add required item name
    void            AddRequiredItem(const char* name);
    //! Clear required list
    void            ClearRequiredList();

    //! Begin event loop
    void            BeginEventLoop();
    //! End event loop
    void            EndEventLoop();
    //! Abort event loop
    void            AbortEventLoop();
    //! Next event
    Bool_t          NextEvent(long nEv=-999);

    //! Wait
    void            Wait(Int_t i_msec);

    //! Clear all actions
    void            ClearAllAction();
    //! Clear all data dsc
    void            ClearAllData();
    //! Clear all parameters dsc
    void            ClearAllPara();

    //! Return list of actions
    TList*          ListOfAction()  const { return fpActionList;  }
    //! Return list of data dsc
    TList*          ListOfDataDsc() const { return fpDataDscList; }
    //! Return list of parameter dsc
    TList*          ListOfParaDsc() const { return fpParaDscList; }

    // Find action
    TAGaction*      FindAction(const char* name, const char* type=0) const;
    // Find  data dsc
    TAGdataDsc*     FindDataDsc(const char* name, const char* type=0) const;
    // Find parameters dsc
    TAGparaDsc*     FindParaDsc(const char* name, const char* type=0) const;

    //! Default action name
    const char*     DefaultActionName();
    //! Default data dsc name
    const char*     DefaultDataDscName();
    //! Default parameter dsc name
    const char*     DefaultParaDscName();

    //! Set campaign name
    void            SetCampaignName(TString s_cam) { fRunInfo.SetCampaignName(s_cam); }
    //! Set run number
    void            SetRunNumber(Short_t i_run)    {  fRunInfo.SetRunNumber(i_run); fEventId.SetRunNumber(i_run);}
    //! Set event number
    void            SetEventNumber(Int_t i_evt)    {  fEventId.SetEventNumber(i_evt); }
    //! Set run info
    void            SetRunInfo(const TAGrunInfo& info);
    //! Set event id
    void            SetEventId(const TAGeventId& info);

    //! Get current campaign name
    const Char_t*   CurrentCampaignName() const { return fRunInfo.CampaignName(); }
    //! Ger current run number
    Short_t         CurrentRunNumber()    const { return fEventId.RunNumber();    }
    //! Get current event number
    Int_t           CurrentEventNumber()  const { return fEventId.EventNumber();  }

    //! Get current run info
    const TAGrunInfo&  CurrentRunInfo()   const {return fRunInfo;  }
    //! Get current event id
    const TAGeventId&  CurrentEventId()   const { return fEventId; }

    //! To stream
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

  private:
    TList*          fpActionList;              ///< action list
    TList*          fpDataDscList;             ///< data dsc list
    TList*          fpParaDscList;             ///< parameter dsc list
    TList*          fpRequiredActionList;      ///< Required action list
    TList*          fpRequiredDataDscList;     ///< Required data dsc list
    Int_t           fiDefActionSeqNum;         ///< Default action sequence number
    Int_t           fiDefDataDscSeqNum;        ///< Default data dsc sequence number
    Int_t           fiDefParaDscSeqNum;        ///< Default parameter dsc sequence number
    TSignalHandler* fpTAGrootInterruptHandler; ///< Interrupt handler
    Bool_t          fbDefaultHandlerRemoved;   ///< Default Handler removed flag
    Bool_t          fbTAGrootHandlerAdded;     ///< Handler added flag
    Bool_t          fbAbortEventLoop;          ///< ABort Handler loop flag
    TAGrunInfo      fRunInfo;                  ///< Run info
    TAGeventId      fEventId;                  ///< Event id
   
   ClassDef(TAGroot,0)
};

extern TAGroot* gTAGroot;

#endif
