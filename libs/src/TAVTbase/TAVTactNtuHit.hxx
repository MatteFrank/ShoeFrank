#ifndef _TAVTactNtuHit_HXX
#define _TAVTactNtuHit_HXX

#include <queue>

#include "TAVTactBaseNtuHit.hxx"

/*!
 \file
 \version $Id: TAVTactNtuHit.hxx
 \brief   Declaration of TAVTactNtuHit.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class DECardEvent;

using namespace std;
class TAVTactNtuHit : public TAVTactBaseNtuHit {
public:
   
   explicit TAVTactNtuHit(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0, TAGparaDsc* pParMap=0);
   virtual  ~TAVTactNtuHit();
   
   virtual Bool_t  Action();
   
public:
   //! Set tolerance for time stamp
   void   SetTStolerance(UInt_t ts) { fgTStolerance = ts;   }
   //! Get tolerance for time stamp
   UInt_t GetTStolerance()          { return fgTStolerance; }

protected:
   TAGdataDsc*         fpDatDaq;		     /// input data dsc
   UInt_t              fFirstBcoTrig;    /// First BCO trigger number
   queue<const DECardEvent*> fQueueEvt;  /// Event queue container
   UInt_t              fQueueEvtsN;      /// Number of events in queue
   
private:
   static UInt_t       fgTStolerance;    /// Tolerance for time stamp
   
   ClassDef(TAVTactNtuHit,0)
};

#endif
