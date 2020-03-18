

#ifndef _TAEDtrack_HXX_
#include "TAEDtrack.hxx"
#endif

#ifndef ROOT_TEveRGBAPalette
#include "TEveRGBAPalette.h"
#endif

#ifndef ROOT_TEveVSDStructs
#include  "TEveVSDStructs.h"
#endif

#include <Riostream.h>

using namespace std;

//__________________________________________________________
TAEDtrack::TAEDtrack(const Text_t* name) 
  : TEveBoxSet(name),
    fPalette(new TEveRGBAPalette()),
    fMaxEnergy(-1),
    fBoxWidth(0.4),
    fBoxHeight(0.4),
    fSelectedIdx(-1),
    fSelectedValue(-1),
    fBoxVert(),
    fNofTracks(0),
    fHitIdPerTrack(0),
	fStyle("Rectangle")
{ 
  // default constructor
  fBoxVert.Set(32);
}

//__________________________________________________________
TAEDtrack::~TAEDtrack()
{
  // default destructor
  delete fPalette;
}

//__________________________________________________________
void TAEDtrack::DigitSelected(Int_t idx) 
{ 
  
  TEveDigitSet::DigitBase_t* selectedDigit = GetDigit(idx);
  
  if (selectedDigit)
    fSelectedValue = selectedDigit->fValue;
  
  fSelectedIdx = idx;
  
  SecSelected(this, idx);
}

//__________________________________________________________
void TAEDtrack::SetStyle(TString s)
{  
  s.ToLower();
  if (s.Contains("rec") )
    fStyle = "Rectangle";
  else if (s.Contains("cone") )
    fStyle = "Cone";
  else if (s.Contains("elli") )
	fStyle = "Elliptic";
  else {
   cout<< "Undefined style for tracks, set to Rectangle" << endl;
    fStyle = "Rectangle";
  }
  
}

//__________________________________________________________
void TAEDtrack::SetMaxEnergy(Float_t e) 
{ 
  fPalette->SetMax(Int_t(e+0.5));
  fMaxEnergy = Int_t(e+0.5); 
  SetPalette(fPalette);
} 

//__________________________________________________________
void TAEDtrack::TrackId(TObject* obj)
{
   DigitId(obj);  
}

//__________________________________________________________
void TAEDtrack::AddNewTrack()
{
  fNofTracks++;
  fHitIdPerTrack = 0;
}

//__________________________________________________________
void TAEDtrack::AddTracklet(Float_t e, Float_t x1, Float_t y1, Float_t z1, 
                                    Float_t x2, Float_t y2, Float_t z2, Bool_t eTot)
{
  Float_t* vert;

  MakeSize(e);
  if ( fStyle.Contains("Cone") ) {
    TEveVector dir, pos;
    pos.Set(x1, y1, z1);
    dir.Set(x2-x1, y2-y1, z2-z1);
    AddCone(pos, dir, fBoxHeight);   
  } else if ( fStyle.Contains("Elliptic") ) {
	TEveVector dir, pos;
    pos.Set(x1, y1, z1);
    dir.Set(x2-x1, y2-y1, z2-z1);	
	AddEllipticCone(pos, dir, fBoxHeight, fBoxHeight/2, 0);   
  } else {
    vert = MakeRecTVert(x1, y1, z1, x2, y2, z2);
    AddBox(vert);   
  }
  
  DigitValue(Int_t(e+0.5));  
  
  if (eTot)
    fTotalEnergyArray.push_back(Int_t(e+0.5));
  
  fHitIdPerTrack++;
}

//__________________________________________________________
void TAEDtrack::ResetTracks()
{

  if ( fStyle.Contains("Cone") ) 
	Reset(TEveBoxSet::kBT_Cone, kFALSE, 32);
  else if ( fStyle.Contains("Elliptic") )
	Reset(TEveBoxSet::kBT_EllipticCone, kFALSE, 32);
  else
    Reset(TEveBoxSet::kBT_FreeBox, kFALSE, 32);
  
  fTotalEnergyArray.clear();
  fNofTracks = 0;
}

//__________________________________________________________
void TAEDtrack::MakeSize(Float_t e)
{
  if (e != 0.)
    fBoxWidth = fBoxHeight = e*0.8*GetDefWidth()/fMaxEnergy + 0.2*GetDefWidth();
  else
    fBoxWidth = fBoxHeight = GetDefWidth()*0.2;
}

//__________________________________________________________
Float_t* TAEDtrack::MakeRecTVert(Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2)
{
  fBoxVert.Reset();
  fBoxVert[0] = x1+GetBoxWidth();  fBoxVert[3] = x1-GetBoxWidth();  fBoxVert[6] = x1-GetBoxWidth();  fBoxVert[9]  = x1+GetBoxWidth();
  fBoxVert[1] = y1+GetBoxHeight(); fBoxVert[4] = y1+GetBoxHeight(); fBoxVert[7] = y1-GetBoxHeight(); fBoxVert[10] = y1-GetBoxHeight();
  fBoxVert[2] = z1;                fBoxVert[5] = z1;                fBoxVert[8] = z1;                fBoxVert[11] = z1;
  
  fBoxVert[12] = x2+GetBoxWidth();  fBoxVert[15] = x2-GetBoxWidth();  fBoxVert[18] = x2-GetBoxWidth();  fBoxVert[21] = x2+GetBoxWidth();
  fBoxVert[13] = y2+GetBoxHeight(); fBoxVert[16] = y2+GetBoxHeight(); fBoxVert[19] = y2-GetBoxHeight(); fBoxVert[22] = y2-GetBoxHeight();
  fBoxVert[14] = z2;                fBoxVert[17] = z2;                fBoxVert[20] = z2;                fBoxVert[23] = z2;
  
  return fBoxVert.GetArray();
}

//__________________________________________________________
Int_t TAEDtrack::NextEnergy()
{
  Int_t energy = -1;
  
  if (fTotalEnergyIter != fTotalEnergyArray.end()) {
    energy = *fTotalEnergyIter;
    ++fTotalEnergyIter;
  }
  
  return energy;
}

