
#include "TClass.h"

#include "TAEDpoint.hxx"

//__________________________________________________________
TAEDpoint::TAEDpoint(const Text_t* name) 
  : TEvePointSet(name)
{
  // default constructor
}

//__________________________________________________________
TAEDpoint::~TAEDpoint()
{
  // default destructor
}

//__________________________________________________________
void TAEDpoint::AddPoint(Float_t x, Float_t y, Float_t z)
{
   SetPoint(Size(), x, y, z);
}

//__________________________________________________________
void TAEDpoint::ResetPoints()
{
   Reset();
}

//__________________________________________________________
void TAEDpoint::PointSelected(Int_t idx)
{
   SecSelected(idx);
   fSelectedIdx = idx;
}

//__________________________________________________________
void TAEDpoint::SecSelected(Int_t idx)
{
   Long_t args[1];
   args[0] = (Long_t) idx;
   
   Emit("PointSelected(Int_t)", args);
}
