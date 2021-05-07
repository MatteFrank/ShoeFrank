#ifndef _TAEDglbTrackList_HXX_
#define _TAEDglbTrackList_HXX_


#include "TEveElement.h"
#include "TVector3.h"

/** TAEDglbTrackList a class to display global tracks on event
 
*/
class TAEDtrack;

class TAEDglbTrackList : public TEveElementList {

public:
   TAEDglbTrackList(const Char_t* name = "");
   virtual ~TAEDglbTrackList();
  
   //! return number of tracks
   Int_t      GetNofTracks()     const { return fNofTracks; }
   
   //! Add Track
   TAEDtrack* NewTrack(const Char_t* label);
  
   //! Reset tracks
   void       ResetTracks();
   
   //! Set width/height of box
   void        SetBoxDefWidth(Float_t w)   { fBoxDefWidth = w;       }
   void        SetBoxDefHeight(Float_t h)  { fBoxDefHeight = h;      }
   
   //! Get width/height of box
   Float_t     GetBoxDefWidth()    const   { return fBoxDefWidth;    }
   Float_t     GetBoxDefHeight()   const   { return fBoxDefHeight;   }
   
   // set/get max energy
   void         SetMaxEnergy(Float_t e)     { fMaxEnergy = e;         }
   Float_t      GetMaxEnergy()      const   { return fMaxEnergy;      }
   
private:
   Int_t       fNofTracks;        // number of tracks
   Float_t     fBoxDefWidth;      // default width of track box
   Float_t     fBoxDefHeight;     // default height of track box
   Float_t     fMaxEnergy;        // maximum energy for palette

};


#endif
