#ifndef _TAEDpoint_HXX_
#define _TAEDpoint_HXX_

#ifndef ROOT_TEvePointSet
#include "TEvePointSet.h"
#endif


/** TAEDpoint a class to display point on event
 
*/

class TAEDpoint : public TEvePointSet
{

public:
  TAEDpoint(const Text_t* name);
  virtual ~TAEDpoint();
   
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

};


#endif
