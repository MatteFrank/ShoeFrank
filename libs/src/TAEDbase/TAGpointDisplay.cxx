
#include "TClass.h"

#include "TAGpointDisplay.hxx"

//
ClassImp(TAGpointDisplay)

//__________________________________________________________
TAGpointDisplay::TAGpointDisplay(const Text_t* name) 
  : TEvePointSet(name)
{
  // default constructor
}

//__________________________________________________________
TAGpointDisplay::~TAGpointDisplay()
{
  // default destructor
}

//__________________________________________________________
void TAGpointDisplay::AddPoint(Float_t x, Float_t y, Float_t z)
{
   SetPoint(Size(), x, y, z);
}

//__________________________________________________________
void TAGpointDisplay::ResetPoints()
{
   Reset();
}

//__________________________________________________________
void TAGpointDisplay::PointSelected(Int_t idx)
{
   SecSelected(idx);
   fSelectedIdx = idx;
}

//__________________________________________________________
void TAGpointDisplay::SecSelected(Int_t idx)
{
   Long_t args[1];
   args[0] = (Long_t) idx;
   
   Emit("PointSelected(Int_t)", args);
}