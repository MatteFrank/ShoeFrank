
#ifndef _TAEDcluster_included_HXX
#include "TAEDcluster.hxx"
#endif

//
ClassImp(TAEDcluster)

//__________________________________________________________
TAEDcluster::TAEDcluster(const Text_t* name) 
  : TEveQuadSet(name),
    fPalette(new TEveRGBAPalette()),
    fMaxEnergy(-1),
    fQuadWidth(18.4),
    fQuadHeight(18.4),
    fSelectedIdx(-1),
    fSelectedValue(-1)
{ 
  // default constructor
}

//__________________________________________________________
TAEDcluster::~TAEDcluster()
{
  // default destructor
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
 fPalette->SetUIDoubleRep(true, 1., 0);
#endif

} 

//__________________________________________________________
void TAEDcluster::AddHit(Float_t e, Float_t x, Float_t y, Float_t z)
{
  AddQuad(x-GetDefWidth()/2., y-GetDefHeight()/2., z);
  DigitValue(Int_t(e+0.5));
}

//__________________________________________________________
void TAEDcluster::ResetHits()
{
  Reset(TEveQuadSet::kQT_RectangleXY, kFALSE, 32);
}
