#ifndef _TAEDpoint_HXX_
#define _TAEDpoint_HXX_

/*!
 \file TAEDpoint.hxx
 \brief Class to display point on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#ifndef ROOT_TEvePointSet
#include "TEvePointSet.h"
#endif

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
