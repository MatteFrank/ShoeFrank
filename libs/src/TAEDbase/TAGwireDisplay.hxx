#ifndef _TAGwireDisplay_HXX_
#define _TAGwireDisplay_HXX_

#ifndef ROOT_TEveStraightLineSet
#include "TEveStraightLineSet.h"
#endif

/** TAGwireDisplay a class to display wire on event
 
*/

class TAGwireDisplay : public  TEveStraightLineSet
{

public:
  TAGwireDisplay(const Text_t* name);
  virtual ~TAGwireDisplay();
  
  //! Add line
  void AddWire(Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2);
  
  //! Reset lines
  void ResetWires();
   
  //! Refit plex
  void RefitPlex();
    
private:
  
  ClassDef(TAGwireDisplay, 0);  
};


#endif
