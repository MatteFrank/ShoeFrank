#ifndef _TAEDcluster_HXX_
#define _TAEDcluster_HXX_

/*!
 \file TAEDcluster.hxx
 \brief  TAEDcluster a class to display hits on event.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TEveQuadSet.h"

class TAEDcluster : public TEveQuadSet
{
   
public:
   TAEDcluster(const Text_t* name);
   virtual ~TAEDcluster();
   
   //! Set width of quad
   void SetQuadWidth(Float_t w)  { fQuadWidth = w;        }
   //! Set height of quad
   void SetQuadHeight(Float_t h) { fQuadHeight = h;       }
   
   //! Get width of quad
   Float_t GetQuadWidth()        { return fQuadWidth;     }
   //! Get height of quad
   Float_t GetQuadHeight()       { return fQuadHeight;    }
   
   //! Get selected digit value
   Int_t GetSelectedValue()      { return fSelectedValue; }
   
   //! Get selected digit index
   Int_t GetSelectedIdx()        { return fSelectedIdx;   }
   
   //! Get Max energy
   Float_t GetMaxEnergy()        { return fMaxEnergy;     }
   
   // Set Max energy
   void SetMaxEnergy(Float_t e);
   
   // overwrite base function
   void DigitSelected(Int_t idx);
   
   // Add hit
   void AddHit(Float_t e, Float_t x, Float_t y, Float_t z);
   
   // Add hit
   void AddHit(Float_t e, Float_t x, Float_t y, Float_t z, Float_t dx, Float_t dy);
   
   // Reset hits
   void ResetHits();
   
private:
   TEveRGBAPalette *fPalette; ///< color palette
   
   Int_t   fMaxEnergy;        ///< max energy
   
   Float_t fQuadWidth;        ///< width of hit quad
   Float_t fQuadHeight;       ///< height of hit quad
   
   Int_t fSelectedIdx;        ///< selected digit index
   Int_t fSelectedValue;      ///< selected digit value
};

#endif
