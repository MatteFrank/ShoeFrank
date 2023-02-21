#ifndef _TAGClockCounter_HXX
#define _TAGClockCounter_HXX

/*------------------------------------------+---------------------------------*/
#include <time.h>
#include <iostream>
#include <string>

constexpr double SECS_PER_CLOCK = 1./((double) CLOCKS_PER_SEC);

class ClockCounter {

public:

  ClockCounter(std::string aName) : name(aName), measures(0), startTime(0), accumulate(0.0){};

  ~ClockCounter(){
    print();
  };

  int getMeasures() const {return measures;};
  double getPassedTime() const { return accumulate;};

  void clear(){ measures=0; startTime=0; accumulate=0.0;}

  void start(){
    startTime = clock();
  };

  void stop(){
    clock_t  stopTime = clock();
    if( startTime>0 && stopTime>startTime){
      // valid measure
      measures++;
      accumulate += ((double)(stopTime-startTime)*SECS_PER_CLOCK);
      startTime=0;
    }
  }

  void print(){
    std::cout<<"Clock "<<name.c_str()<<" called "<<measures<<" times. "
	     <<"Total recorded time: "<<accumulate<<" seconds."<<std::endl;
  }

private:
  std::string name;
  clock_t startTime;
  double accumulate;
  int measures;
};

#endif
