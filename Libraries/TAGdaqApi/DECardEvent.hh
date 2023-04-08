#ifndef DECARDEVENT_HH
#define DECARDEVENT_HH

/*!
 \file DECardEvent.hh
 
 \brief Declaration of DECardEvent
 */

#include "RemoteEvent.hh"

class DECardEvent : public RemoteEvent {

  public:
    // firmware data
    u_int detectorHeader;     ///< detector header
    u_int boardHeader;        ///< board header
    u_int hardwareEventNumber;///< hardware event number
    u_int triggerCounter;     ///< trigger counter
    u_int BCOofTrigger;       ///< BCO of trigger
    u_int clockCounter;       ///< clock counter

    virtual ~DECardEvent();
  
    virtual void readData(unsigned int **p);
    virtual void printData() const;
    virtual bool check() const;

   //! return class type
    virtual std::string classType() const {return "DECardEvent";   }
   
   DECardEvent();
   DECardEvent(const DECardEvent &right);
   const DECardEvent& operator=(const DECardEvent &right);
   
   
  public:
   //! Get VTX header file
   static  u_int  GetVertexHeader()        { return m_vtxHeader; }
   //! Get VTX tailer file
   static  u_int  GetVertexTail()          { return m_vtxTail;   }

  protected:
   static const u_int  m_vtxHeader; ///< VTX header
   static const u_int  m_vtxTail;   ///< VTX tailer
   
};

#endif
