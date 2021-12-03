#ifndef _TAVTrawEvent_HXX
#define _TAVTrawEvent_HXX
/*!
  \file
  \version $Id: TAVTrawEvent.hxx
  \brief   Declaration of TAVTrawEvent.
*/
/*------------------------------------------+---------------------------------*/
#include "TAGdata.hxx"

class TAVTrawEvent : public TAGdata {
public:
   TAVTrawEvent();
   virtual         ~TAVTrawEvent();
   
   //! Add event
   void            AddEvent(Int_t size, const UInt_t data[]);
   //! Get event trigger
   Int_t           EventTrigger() const { return fEventTrigger; }
   //! Get event size
   Int_t           EventSize()    const { return fEventSize;    }
   //! Get pointer to event data
   const UInt_t*   EventData()    const { return fData;         }
   
   //! Auto delete
   virtual Bool_t  NeedAutoDelete() const;
   //! Clear
   virtual void    Clear(Option_t* opt="");
   //! To stream
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
private:				    
   UInt_t* fData;          //!< container of data
   Int_t   fEventTrigger;  /// trigger event if ever
   Int_t   fEventSize;     /// event sise 
   
   ClassDef(TAVTrawEvent,2)

};

#endif
