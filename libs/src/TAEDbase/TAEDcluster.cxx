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
//!
//! \param[in] name cluster list name
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
//! Selected digit
//!
//! \param[in] idx digit id
void TAEDcluster::DigitSelected(Int_t idx)
{
  TEveDigitSet::DigitBase_t* selectedDigit = GetDigit(idx);

  if (selectedDigit)
    fSelectedValue = selectedDigit->fValue;
  
  fSelectedIdx = idx;
  
  SecSelected(this, idx);
}

//__________________________________________________________
//! Set maximum energy
//!
//! \param[in] e energy max
void TAEDcluster::SetMaxEnergy(Float_t e) 
{ 
  fPalette->SetMax(Int_t(e+0.5));
  fMaxEnergy = Int_t(e+0.5); 
  SetPalette(fPalette);
  fPalette->SetUIDoubleRep(true, 1., 0);
} 

//__________________________________________________________
//! Add quad point
//!
//! \param[in] e energy
//! \param[in] x position in X-direction
//! \param[in] y position in Y-direction
//! \param[in] z position in Z-direction
void TAEDcluster::AddHit(Float_t e, Float_t x, Float_t y, Float_t z)
{
  AddQuad(x-GetDefWidth()/2., y-GetDefHeight()/2., z);
  DigitValue(Int_t(e+0.5));
}

//__________________________________________________________
//! Add quad point with size argument
//!
//! \param[in] e energy
//! \param[in] x position in X-direction
//! \param[in] y position in Y-direction
//! \param[in] z position in Z-direction
//! \param[in] dx size in X-direction
//! \param[in] dy size in Y-direction
void TAEDcluster::AddHit(Float_t e, Float_t x, Float_t y, Float_t z, Float_t dx, Float_t dy)
{
   AddQuad(x-dx/2., y-dy/2., z, dx, dy);
   DigitValue(Int_t(e+0.5));
}

//__________________________________________________________
//! Reset quad
void TAEDcluster::ResetHits()
{
  Reset(TEveQuadSet::kQT_RectangleXY, kFALSE, 32);
}
