//
//
//
#include "EventReader.hh"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// default constructor
EventReader::EventReader(int debugLevel){
  m_isOpened = false;
  m_errorOnRead = false;
  m_debugLevel = debugLevel;
  m_buffer = NULL;
  m_head=NULL;
  m_trg=NULL;
  m_info=NULL;
  m_eventsRead = 0;
}

// default destructor
EventReader::~EventReader(){
  
  // delete all the stored fragments
  for(std::map<u_int, BaseFragment*>::reverse_iterator it=m_fragments.rbegin();
      it!=m_fragments.rend(); ++it){
    delete it->second;
  }
  m_info = NULL;  // event info fragment already deleted
  m_trg = NULL;   // trigger fragment already deleted 
  if( m_head!=NULL ) delete m_head;
  if( m_buffer!=NULL ) free(m_buffer);
  if( m_isOpened )  closeFile();
}


// Open DAQ input file
// it returns false if unable to open file
bool EventReader::openFile( std::string filename ){
  std::cout<<"Opening file "<<filename<<std::endl;
  m_eventsRead = 0;
  m_errorOnRead = false;
  m_file = new std::ifstream;
  m_file->open( filename.c_str() , std::ifstream::in );
  if( m_file->good() ){
    m_isOpened = true;
  }
  else {
    m_isOpened = false;
  }

  return m_isOpened;
}


// simple close file method
void EventReader::closeFile(){
  m_file->close();
  m_isOpened = false;
}


// read the header of the whole DAQ file
void  EventReader::readFileHeader(){

  unsigned int partFile;
  unsigned int word = readWord();
  if( word == FileHeader){
    unsigned int size = readWord();
    readWord();
    partFile = readWord();
    m_isFirstFile = true;
    if ( partFile != 1 )
      m_isFirstFile = false;
    for( unsigned int i=0; i< size-4; i++)
      readWord();
  } else {
    m_errorOnRead = true;
    return;
  }
  //--------------HEADER PART------------------------------
  word = readWord();
  if( word == hFirst){ //first marker of header
    m_head = new DAQFileHeader;
    m_head->writer = readString();
    m_head->filename = readString();
    m_head->fileNumber = partFile;

  } else {
    // std::cout << "first part of header of file not working" << std::endl;
    m_errorOnRead = true;
    return;
  }
  word = readWord();
  if( word == hSecond){
    //std::cout << "hSecond found" << std::endl;

    readWord(); // -- empty reading ?
    m_head->guid = readString();
    m_head->stream = readString();
    m_head->project	= readString();
    m_head->lumiblock = readString();

  } else {
    //std::cout << "second part of header of file not working" << std::endl;
    m_errorOnRead = true;
    return;
  }
}


/************************************************/
// read the full event and stores it in memory
void  EventReader::getNextEvent(){
  
  unsigned int sizeROS = 0;
  // clear maps and derived pointers
  for(std::map<u_int, BaseFragment*>::reverse_iterator it=m_fragments.rbegin();
      it!=m_fragments.rend(); ++it){
    delete it->second;
  }
  m_fragments.clear();

  m_trg=0;
  m_info=0;

  preEvent();
  if( m_errorOnRead ) return;

  m_errorOnRead = true;
  unsigned int word = readWord();
  word = readWord();
  //  std::cout << "Next word " << std::hex<<word<<std::endl;
    
  // actual reading of all event
  unsigned int * base = (unsigned int *) readInEvent(); 
  if( base == NULL ){
    m_errorOnRead = true;
    // error reading file
    std::cout << "Error reading event 0 - " << (std::hex)<<*base<<std::endl;
    return;
  }
  unsigned int size = base[1];
  unsigned int * p = base;
  unsigned int * endBuffer = base+size;
  
  // go to event header ID (run, time, evt number)
  p +=5;
  if( ((*p) & 0xffff0000) != EventHeaderID) {
    std::cout << "Error reading event 1 - " << (std::hex)<<*p<<std::endl;
  }
  
  if( ((*p) & 0xffff0000) == EventHeaderID) {
    readInfoEvent(&p);  // read event info
    //std::cout << "after readinfo " << "and p is " << std::hex << *p << std::endl;
    //p = base+base[2];
    
    //    while( p < endBuffer ){
    //std::cout << "ROS1 Current word: " << (std::hex)<< *p << std::endl;
      sizeROS = getROSInformation(&p); // get Readout system information
      //std::cout << "after get ROS" <<std::endl;
      //}
  }
  //p = base+base[2]+sizeROS;
  
  //std::cout << "ROS2 Current word: " << (std::hex)<<*p<<std::endl;
  // if(*p == ROSHeader1) {
  //   getROSInformation(&p);
  //   std::cout << "after get ROS2" <<std::endl;
  // }
  // provide the pointer to the trigger object
  std::map<unsigned int,BaseFragment*>::iterator bp = m_fragments.find(dataV2495+0x30);
  //std::cout << "trigger fragment " <<bp->first<<std::endl;
  if( bp != m_fragments.end() ){
    m_trg = static_cast<TrgEvent*>(bp->second);
  }
  m_eventsRead++;
  //std::cout << "End read " <<std::endl;
}



/************************************************/
// skip full event
void  EventReader::skipEvent(){
   
   
   preEvent();
   if( m_errorOnRead ) return;
   
   m_errorOnRead = true;
   unsigned int word = readWord();
   word = readWord();

   //  reading size
   unsigned int size = readWord();
//   printf("skip size %x\n", size);

   m_file->seekg(size, std::ios::cur);
   
//   word = readWord();
//   printf("word %x\n", word);
//   m_file->seekg(-4, std::ios::cur);

}


//
// Printing of all information stored
//
void EventReader::printData(){

  std::cout<<"--------- Start of Event ------------"<<std::endl;  
  if( m_eventsRead>0 ){
    std::cout<<"Sequential event on file: "<<m_eventsRead<<std::endl;  
  } else {
    std::cout<<"No event read out yet !! "<<std::endl;  
  }
  for(std::map<u_int, BaseFragment*>::iterator it=m_fragments.begin();
      it!=m_fragments.end(); ++it){
    it->second->printData();
  }
  std::cout<<"--------- End of Event ------------"<<std::endl;  
}




//
// simple checks on data
// returns true if all OK
//
bool EventReader::check(){
  bool rccheck=true;
  if( m_eventsRead>0 ){
    for(std::map<u_int, BaseFragment*>::iterator it=m_fragments.begin();
	it!=m_fragments.end(); ++it){
      bool ok = it->second->check();
      if( !ok ){
        if( m_debugLevel>0)
          std::cout<<" Fragment "<<(std::hex)<<it->second->channelID
          <<" failed simple check"<<std::endl;
         rccheck = false;
      }
    }
  } else {
    rccheck = false;
  }
  return rccheck;
}


// *****************************************
//           PRIVATE PART
// *****************************************

//------------------------------------------
void EventReader::preEvent(){

  unsigned int word = readWord();
  if( m_errorOnRead ){
    std::cout << "\n End of file" << std::endl;
    return;
  }
  if( word==FileHeader2 ){
    int n=readWord();
    if( n<50 ){
      n-=1;
      do{
	word = readWord();
	n--;
      }while(n>0 ||m_errorOnRead );
    }
  }
  while( word!=EventMarker ){
    word = readWord();
    std::cout << "\n Looking for EventMarker or EndOfFile " 
	      << (std::hex)<<word<<std::endl;
    if ( word == EndOfFile || m_errorOnRead || m_file->eof()) {
      m_errorOnRead = true;
      std::cout << "\nEnd of file" << std::endl;
      return;
    }
  }
  if ( word==EventMarker ) {
    //unsigned int size = readWord();
    if( m_errorOnRead ){
      std::cout << "\n End of file" << std::endl;
      return;
    }
    // if( size==0 ) size =2;
    // for( unsigned int i=0; i< size-2; i++)
    //   readWord();
  }
  else {
    m_errorOnRead = true;
    return;
  }
}

/****************************************/
/* Read IN a full event in a local buffer */
char* EventReader::readInEvent(){

  m_errorOnRead = false;
  unsigned int size = readWord();
   printf("size2 %x\n", size);
  if( m_buffer!=NULL ) free(m_buffer);
  m_buffer = (char*) malloc(size+8);
  //  std::cout <<" Allocating "<< size+8 << " words p="<<m_buffer<<std::endl;
  unsigned int * p = (unsigned int *)m_buffer;
  unsigned int * checkpoint = p;
  *p =  EventHeader;
  p++;
  *p = size/4 + 2;
  p++;
  try{
    m_file->read((char*)p, size);
  }
  catch(std::exception& e){
    //end of file? file unreadable?
    if( m_debugLevel>0)
      std::cout << e.what() << std::endl;
    m_errorOnRead=true;
    return NULL;
  }
  /*  for (unsigned int k = 0; k < (size/4 + 2); ++k){
    std::cout << std::hex << *checkpoint << " ";
    checkpoint++;
    }*/
  return m_buffer;
}

/****************************************/
unsigned int EventReader::getROSInformation(unsigned int **p1){
  unsigned int * p = *p1;
  unsigned int * base = p;
  unsigned int sizeROS = p[1];

  // if( *base != ROSHeader2 ){
  //   std::cout<< "Error on ROSHeader2 " << (std::hex)<< *base << std::endl;
  // }
  // base = base + base[1];

  // here a Read out Buffer (ROB)
  //std::cout << "getROSInformation: entered and base is " << std::hex << *base << std::endl;
  BaseFragment* bp = NULL;
  if( *base!=0 ){
    do {
      u_int id = *base;
      //std::cout<< "Reading " << (std::hex)<< *base << std::endl;
      
      bp = BaseFragment::create(&base);
      if( bp!=NULL ){
	m_fragments[id] = bp;
      } else {
	std::cout<< "Null pointer ?? " << (std::hex)<< *base << std::endl;
      }
    } while (*base!=0 && bp!=NULL);
    //std::cout<< "After base=" << (std::hex)<< *base << std::endl;    
  }
  *p1 = base;
  return sizeROS;
}


/****************************************/
void EventReader::readInfoEvent(unsigned int ** p1){

  if (m_info!=0) {m_info=0; }
  m_info = new InfoEvent;
  m_info->readData(p1);
  m_fragments[EventHeaderID] = m_info;
}


//
//******* LOW LEVEL FUNCTIONS
//
unsigned int  EventReader::readWord(){
  unsigned int word;
  m_errorOnRead = false;
  try{
    m_file->read((char*)&word, 4);
  }
  catch(std::exception& e){
    if( m_debugLevel>0)
      std::cout << e.what() << std::endl;
    m_errorOnRead=true;
  }
  return word;
}

std::string EventReader::readString(){
  unsigned int size = readWord();
  unsigned nwords = (size+3)/4;
  return copyString(nwords);
}


std::string EventReader::copyString(unsigned int nwords){
  std::string str;
  for(unsigned int i= 0; i<nwords; i++){
    unsigned int dat = readWord();
    str.append((const char*) &dat, 4);
  }
  return str;
}

