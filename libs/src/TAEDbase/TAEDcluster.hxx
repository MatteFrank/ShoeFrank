#ifndef _TAEDcluster_HXX_
#define _TAEDcluster_HXX_

#ifndef ROOT_TEveQuadSet
#include "TEveQuadSet.h"
#endif



/** TAEDcluster a class to display hits on event
 
*/

class TAEDcluster : public TEveQuadSet
{

public:
  TAEDcluster(const Text_t* name);
  virtual ~TAEDcluster();
  
  //! Set width/height of quad
  void SetQuadWidth(Float_t w)  { w = fQuadWidth;}
  void SetQuadHeight(Float_t h) { h = fQuadHeight; }
  
  
  //! Get width/height of quad
  Float_t GetQuadWidth()  { return fQuadWidth; }
  Float_t GetQuadHeight() { return fQuadHeight; }
  
  //! Get selected digit value
  Int_t GetSelectedValue() { return fSelectedValue; }
  
  //! Get selected digit index
  Int_t GetSelectedIdx() { return fSelectedIdx; }

  //! overwrite base function
  void DigitSelected(Int_t idx);
    
  //! Add hit
  void AddHit(Float_t e, Float_t x, Float_t y, Float_t z);
  
  //! Add hit
   void AddHit(Float_t e, Float_t x, Float_t y, Float_t z, Float_t dx, Float_t dy);

  //! Reset hits
  void ResetHits();
  
  //! Set Max energy
  void SetMaxEnergy(Float_t e);
  
  //! Get Max energy
  Float_t GetMaxEnergy() { return fMaxEnergy; } 
  
private:
  
  TEveRGBAPalette *fPalette; // color palette 

  Int_t   fMaxEnergy;        // max energy

  Float_t fQuadWidth;        // width of hit quad
  Float_t fQuadHeight;       // height of hit quad
  
  Int_t fSelectedIdx;        // selected digit index
  Int_t fSelectedValue;      // selected digit value
  
  ClassDef(TAEDcluster, 1);   
};


#endif
