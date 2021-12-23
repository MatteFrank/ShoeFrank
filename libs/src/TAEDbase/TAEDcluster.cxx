/*!
 \file TAEDcluster.cxx
 \brief  Event display for cluster
 */

#ifndef _TAEDcluster_included_HXX
#include "TAEDcluster.hxx"
#endif

/*!
 \class TAEDcluster
 \brief Event display for cluster. **
 */

//__________________________________________________________
//! default constructor
TAEDcluster::TAEDcluster(const Text_t* name) 
  : TEveQuadSet(name),
    fPalette(new TEveRGBAPalette()),
    fMaxEnergy(-1),
    fQuadWidth(18.4),
    fQuadHeight(18.4),
    fSelectedIdx(-1),
    fSelectedValue(-1)
{ 
}

//__________________________________________________________
//! default destructor
TAEDcluster::~TAEDcluster()
{
  delete fPalette;
}

//__________________________________________________________
void TAEDcluster::DigitSelected(Int_t idx) 
{ 
  
  TEveDigitSet::DigitBase_t* selectedDigit = GetDigit(idx);

  if (selectedDigit)
    fSelectedValue = selectedDigit->fValue;
  
  fSelectedIdx = idx;
  
  SecSelected(this, idx);
}

//__________________________________________________________
void TAEDcluster::SetMaxEnergy(Float_t e) 
{ 
  fPalette->SetMax(Int_t(e+0.5));
  fMaxEnergy = Int_t(e+0.5); 
  SetPalette(fPalette);
  fPalette->SetUIDoubleRep(true, 1., 0);
} 

//__________________________________________________________
void TAEDcluster::AddHit(Float_t e, Float_t x, Float_t y, Float_t z)
{
  AddQuad(x-GetDefWidth()/2., y-GetDefHeight()/2., z);
  DigitValue(Int_t(e+0.5));
}

//__________________________________________________________
void TAEDcluster::AddHit(Float_t e, Float_t x, Float_t y, Float_t z, Float_t dx, Float_t dy)
{
   AddQuad(x-dx/2., y-dy/2., z, dx, dy);
   DigitValue(Int_t(e+0.5));
}

//__________________________________________________________
void TAEDcluster::ResetHits()
{
  Reset(TEveQuadSet::kQT_RectangleXY, kFALSE, 32);
}
