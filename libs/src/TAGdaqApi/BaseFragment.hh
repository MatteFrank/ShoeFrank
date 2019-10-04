#ifndef BASEFRAGMENT_HH
#define BASEFRAGMENT_HH
/*
  Base class for all fragments..
  Provides a common interface
  It is a Factory method for all derived classes...
  */
#include <string>
#include <map>

typedef unsigned int u_int;
  
class BaseFragment {

public:
  u_int channelID;
  int   debugLevel;

   BaseFragment() : channelID(0), debugLevel(0){};
  virtual ~BaseFragment();
  
  virtual void readData(unsigned int **p);
  virtual void printData() const;
  virtual bool check() const { return true;};

  virtual std::string classType() const {return "BaseFragment";};
  
  // create a concrete class based on channelID pointed ; it reads it; returns 0 if error 
  static BaseFragment* create(unsigned int **p);
  // static method to find names
  static std::string fragmentName(unsigned int key);
  
protected:

  static std::map<unsigned int, std::string> fragnames;
};

#endif



