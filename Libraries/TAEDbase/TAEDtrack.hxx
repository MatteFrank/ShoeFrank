#ifndef _TAEDtrack_HXX_
#define _TAEDtrack_HXX_

/*!
 \file TAEDtrack.hxx
 \brief Class to display tracks on event
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/
#include <vector>

#ifndef ROOT_TArrayF
#include "TArrayF.h"
#endif

#ifndef ROOT_TEveBoxSet
#include "TEveBoxSet.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif

/** TAEDtrack a class to display tracks on event
 
*/

class TAEDtrack : public TEveBoxSet
{

public:
  TAEDtrack(const Text_t* name);
  virtual ~TAEDtrack();
  
  //! Set width of Box
  void SetBoxWidth(Float_t w)  { fBoxWidth = w;}
   //! Set height of Box
  void SetBoxHeight(Float_t h) { fBoxHeight = h; }
  
  //! Get width of Box
  Float_t GetBoxWidth()  { return fBoxWidth; }
   //! Get height of Box
  Float_t GetBoxHeight() { return fBoxHeight; }
  
  //! Get current digit value
  Int_t GetSelectedValue() { return fSelectedValue; }
  
  //! Get selected digit index
  Int_t GetSelectedIdx() { return fSelectedIdx; }
  
  //! return first total energy
  void FirstEnergy() { fTotalEnergyIter = fTotalEnergyArray.begin(); }
  
  // return next total energy
  Int_t NextEnergy();
  
  //! return number of tracks
  Int_t GetNofTracks() { return fNofTracks; }
  
  // overwrite base function
  void DigitSelected(Int_t idx);
  
  // Add new track, resetting counter
  void AddNewTrack();
  
  // Add Track
  void AddTracklet(Float_t e, Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2, Bool_t eTot = false);
  // Add Track
  void AddTracklet(Float_t Z, TVector3 pos1, TVector3 pos2);

  // Reset tracks
  void ResetTracks();
  
  // Set Max energy
  void SetMaxEnergy(Float_t e);
  
  //! Get Max energy
  Float_t GetMaxEnergy() { return fMaxEnergy; } 
  
  // Set style (rec/cone)
  void SetStyle(TString s);
   
  // Add id for track
  void TrackId(TObject* obj);
  
private:
  TEveRGBAPalette *fPalette; ///< color palette
  
  Int_t fMaxEnergy;          ///< max energy

  Float_t fBoxWidth;         ///< width of track box
  Float_t fBoxHeight;        ///< height of track box
  
  Int_t fSelectedIdx;        ///< selected digit index
  Int_t fSelectedValue;      ///< selected digit value

  TArrayF fBoxVert;          ///< vertices of track box
  
  Int_t fNofTracks;          ///< track id
  Int_t fHitIdPerTrack;      ///< hit id for a given track
  
  TString fStyle;            ///< flag for display
      
  std::vector<Int_t>  fTotalEnergyArray;                 ///< total energy array
  
  std::vector<Int_t>::const_iterator  fTotalEnergyIter;  ///< total energy iterator

protected:
  // Compute rectangle vertices for tracks
  Float_t* MakeRecTVert(Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2); 

  // Make track size
  void MakeSize(Float_t e);  
};

#endif
