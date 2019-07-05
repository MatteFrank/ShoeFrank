#ifndef _TAGpointDisplay_HXX_
#define _TAGpointDisplay_HXX_

#ifndef ROOT_TEvePointSet
#include "TEvePointSet.h"
#endif


/** TAGpointDisplay a class to display point on event
 
*/

class TAGpointDisplay : public TEvePointSet
{

public:
  TAGpointDisplay(const Text_t* name);
  virtual ~TAGpointDisplay();
   
  //! Add Point
  void AddPoint(Float_t x, Float_t y, Float_t z);
  
  //! Reset point
  void ResetPoints();
  
   //! overwrite base function
   void PointSelected(Int_t idx);

   //! second selection emit
   void SecSelected(Int_t idx);

   //! Get selected point index
   Int_t GetSelectedIdx() { return fSelectedIdx; }

private:
   Int_t fSelectedIdx;        // selected digit index

  ClassDef(TAGpointDisplay, 1);
};


#endif
