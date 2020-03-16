#ifndef _TAEDwire_HXX_
#define _TAEDwire_HXX_

#ifndef ROOT_TEveStraightLineSet
#include "TEveStraightLineSet.h"
#endif

/** TAEDwire a class to display wire on event
 
*/

class TAEDwire : public  TEveStraightLineSet
{

public:
  TAEDwire(const Text_t* name);
  virtual ~TAEDwire();
  
  //! Add line
  void AddWire(Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2);
  
  //! Reset lines
  void ResetWires();
   
  //! Refit plex
  void RefitPlex();

};


#endif
