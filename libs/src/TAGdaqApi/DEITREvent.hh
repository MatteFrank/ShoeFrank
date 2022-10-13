#ifndef DEITREVENT_HH
#define DEITREVENT_HH

/*!
 \file DEITREvent.hh
 
 \brief Declaration of DEITREvent
 */

#include "RemoteEvent.hh"

class DEITREvent : public RemoteEvent {

  public:
    // firmware data
    u_int detectorHeader;     ///< detector header
    u_int boardHeader;        ///< board header
    u_int hardwareEventNumber;///< hardware event number
    u_int triggerCounter;     ///< trigger counter
    u_int BCOofTrigger;       ///< BCO of trigger
    u_int clockCounter;       ///< clock counter

    virtual ~DEITREvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const;

   //! return class type
    virtual std::string classType() const {return "DEITREvent";   }
   
   DEITREvent();
   DEITREvent(const DEITREvent &right);
   const DEITREvent& operator=(const DEITREvent &right);
   
   
  public:
   //! Get VTX header file
   static  u_int  GetItrHeader()        { return m_itrHeader; }
   //! Get VTX tailer file
   static  u_int  GetItrTail()          { return m_itrTail;   }

  protected:
   static const u_int  m_itrHeader; ///< ITR header
   static const u_int  m_itrTail;   ///< ITR tailer
   
};

#endif
