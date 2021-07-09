//
//
//
#include "EventReaderAndChecker.hh"
#include "fADCEvent.hh"
#include "DECardEvent.hh"
#include "EmptyEvent.hh"
#include "TDCEvent.hh"
#include "WDEvent.hh"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

// default constructor
EventReaderAndChecker::EventReaderAndChecker(int debugLevel) : 
  EventReader(debugLevel),
  m_eventOffset(0) {
}


// default destructor
EventReaderAndChecker::~EventReaderAndChecker(){
  // empty
}


// print statistics

void EventReaderAndChecker::printStatistics(){

  int errors = m_fileErrors;
  std::cout<<"Final statistics: "<<std::endl
	   <<"File errors:"<<m_fileErrors<<std::endl;

  // set to false the ifFound flags
  std::cout<<" Marker        What             Occurrencies     Errors     Flag"<<std::endl;
  for(std::map<u_int, ErrInfo>::iterator it=m_infos.begin();
      it!=m_infos.end(); ++it){ 
    std::cout<<std::setw(7)<<(std::hex)<<(it->first)<<"    "<<(std::dec)
	     <<std::setw(15)<<BaseFragment::fragmentName(it->first)
	     <<std::setw(10)<<it->second.counts
	     <<std::setw(14)<<it->second.errors;

    if( it->second.counts!= m_eventsRead || it->second.errors>0 ){
      std::cout<<"   @@@ """;
    }
    std::cout<<std::endl;
    errors += it->second.errors;
  }

  std::cout<<std::endl
	   <<"Total errors "<<errors<<" after reading "
	   <<m_eventsRead<<" events."<<std::endl;
}



// perform all possible checks on data

bool EventReaderAndChecker::check(){
  bool checkPassed = true;
  if( m_eventsRead==0 ){
    std::cout<<" Check must be called after the first event has been read"
	     <<std::endl;
    return false;
  } else if( m_eventsRead==1 ){
    // this is the first event get all the keys!
    ErrInfo nullInfo;
    for(std::map<u_int, BaseFragment*>::iterator it=m_fragments.begin();
	it!=m_fragments.end(); ++it){
      unsigned int key = it->first;
      m_infos[key] = nullInfo;
    }
    m_fileErrors=0;
  }


  // set to false the ifFound flags
  for(std::map<u_int, ErrInfo>::iterator it=m_infos.begin();
      it!=m_infos.end(); ++it){
    it->second.isFound = false;
  }
  //
  // Internal checks
  //
  InfoEvent*     infoEv = getInfoEvent();   // infoEvent
  TrgEvent*      trgEv = getTriggerEvent(); // Trigger Event

  if( m_eventsRead == 1 )
    m_eventOffset = infoEv->eventNumber; 
  u_int eventNumber = m_eventsRead + m_eventOffset -1;


  for(std::map<u_int, BaseFragment*>::iterator it=m_fragments.begin();
      it!=m_fragments.end(); ++it){
    unsigned int key = it->first;
    unsigned int keym = key&0xffffff00;

    std::map<u_int, ErrInfo>::iterator iterr = m_infos.find( key );
    if( iterr==m_infos.end() ){

      m_fileErrors++;
      // no other checks... since I do no know what to check!!
      std::cout<<" Fragment with ID "<<std::hex<<(it->first)<<std::dec
	       <<" is missing in first event and present in event nr "
	       <<eventNumber<<std::endl;
      checkPassed = false;
    } else {
      iterr->second.isFound = true;
      iterr->second.counts++;       // count fragment occurrencies
      bool chk = it->second->check();
      if( !chk ) {
	iterr->second.errors++;
	std::cout<<" Fragment with ID "<<std::hex<<(it->first)<<std::dec
		 <<" failed internal check in event nr "
		 <<eventNumber<<std::endl;
	checkPassed = false;
      }
      //
      // look inside objects
      //
      if( keym==EventHeaderID ){
	if( infoEv->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" Event with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has event number "<<infoEv->eventNumber<<" instead of "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
      } else if( keym==dataV1720 ){  // flash ADC  -> check event number
	fADCEvent* fadc = static_cast<fADCEvent*>(it->second);
	if( fadc->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" FlashADC with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has event number "<<fadc->eventNumber<<" instead of "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
      } else if( keym==dataEmpty ){ // empty check ev num
	EmptyEvent* emptyp = static_cast<EmptyEvent*>(it->second);
	if( emptyp->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" Empty Event with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has event number "<<emptyp->eventNumber<<" instead of "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
      } else if( keym==dataV1190 ){ // TDC check ev num
	TDCEvent* tdcp = static_cast<TDCEvent*>(it->second);
	if( tdcp->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" TDC Event with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has event number "<<tdcp->eventNumber<<" instead of "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
      } else if( keym==dataWD ){ // remote check ev num
	WDEvent* wdp = static_cast<WDEvent*>(it->second);
	if( wdp->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" WD Event with ID "<<std::hex<<(iterr->first)<<std::dec
	           <<" has event number "<<wdp->eventNumber<<"instead of "
	           <<eventNumber<<std::endl;
	  checkPassed = false;
	  }
	} else if( keym==dataVTX ){ // DE boards
	// check ev number
	DECardEvent* dep = static_cast<DECardEvent*>(it->second);
      	if( dep->eventNumber != eventNumber ){
	  iterr->second.errors++;
	  std::cout<<" DECard Event with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has event number "<<dep->eventNumber<<" instead of "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
	// check BCO
	if( abs((int)(dep->BCOofTrigger)-(int)(trgEv->BCOofTrigger))>20 ){
	  iterr->second.errors++;
	  std::cout<<" DECard Event with ID "<<std::hex<<(iterr->first)<<std::dec
		   <<" has BCO "<<dep->BCOofTrigger
		   <<" instead of "<<trgEv->BCOofTrigger<<" in event "
		   <<eventNumber<<std::endl;	  
	  checkPassed = false;
	}
      }
    }
  }


  // look for missing fragments (with respect to event 0)
  for(std::map<u_int, ErrInfo>::iterator it=m_infos.begin();
      it!=m_infos.end(); ++it){
    if( it->second.isFound == false ){
      it->second.errors++;
      std::cout<<" Fragment with ID "<<std::hex<<(it->first)<<std::dec
	       <<" is missing in event nr "<<eventNumber<<std::endl;
      checkPassed = false;
    }
  }
  return checkPassed;
}


// return the BCO difference wrt the previous event
u_int  EventReaderAndChecker::getBCOdiff() const{

  static u_int lastBCO=0;

  TrgEvent*  trgEv = getTriggerEvent(); // Trigger Event
  u_int BCO = trgEv->BCOofTrigger;

  if( lastBCO==0){
    lastBCO=0;
    return 0;
  }
  u_int diff = BCO-lastBCO;
  lastBCO = BCO;
  return diff;
}
