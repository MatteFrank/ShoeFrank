#ifndef _TAEDwire_HXX_
#define _TAEDwire_HXX_

/*!
 \file TAEDwire.hxx
 \brief Class to display wire on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#ifndef ROOT_TEveStraightLineSet
#include "TEveStraightLineSet.h"
#endif

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
