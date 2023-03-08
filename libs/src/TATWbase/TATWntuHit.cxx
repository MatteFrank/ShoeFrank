/*!
 \file TATWntuHit.cxx
 \brief   Implementation of TATWntuHit.
 */

#include "TString.h"
#include "TClonesArray.h"

#include "TATWntuHit.hxx"

//! Class Imp
ClassImp(TATWhit) // Description of Single Detector TATWhit 

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWhit::TATWhit()
  : fLayer(-1),
    fBar(-1),
    fDe(-1),
    fTime(-1),
    fTimeofflight(-1),
    fCoordinate(-99),
    fZ(-99),
    fChargeZ(-99),
    fChargeA(-1),
    fChargeB(-1),
    fAmplitudeA(-1),
    fAmplitudeB(-1),
    fTimeA(-1),
    fTimeB(-1),
    fIsValid(true),
    fTrigType(-1000)
{
}

//______________________________________________________________________________
//  build a hit from a rawHit
TATWhit::TATWhit( TATWrawHit* hit )
: TAGobject(),
  fLayer(-1),
  fBar(-1),
  fDe(-1),
  fTime(-1),
  fTimeofflight(-1),
  fCoordinate(-99),
  fZ(-99),
  fChargeZ(-99),
  fChargeA(-1),
  fChargeB(-1),
  fAmplitudeA(-1),
  fAmplitudeB(-1),
  fTimeA(-1),
  fTimeB(-1),
  fIsValid(true),
  fTrigType(-1000)
{
}

//______________________________________________________________________________
//
TATWhit::TATWhit(const TATWhit& aHit)
:  TAGobject(aHit),
   fLayer(aHit.fLayer),
   fBar(aHit.fBar),
   fDe(aHit.fDe),
   fTime(aHit.fTime),
   fTimeofflight(aHit.fTimeofflight),
   fCoordinate(aHit.fCoordinate),
   fZ(aHit.fZ),
   fChargeZ(aHit.fChargeZ),
   fMCindex(aHit.fMCindex),
   fMCtrackId(aHit.fMCtrackId),
   fChargeA(aHit.fChargeA),
   fChargeB(aHit.fChargeB),
   fAmplitudeA(aHit.fAmplitudeA),
   fAmplitudeB(aHit.fAmplitudeB),
   fTimeA(aHit.fTimeA),
   fTimeB(aHit.fTimeB),
   fIsValid(aHit.fIsValid),
   fTrigType(aHit.fTrigType)
{
}

//______________________________________________________________________________
// Build the hit from its layerID and barID
TATWhit::TATWhit (Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime,
                  Double_t pos, Double_t ChargeA,
		  Double_t ChargeB,Double_t AmplitudeA,Double_t AmplitudeB,Double_t TimeA,Double_t TimeB, Int_t TrigType):
  
  TAGobject(),
  fLayer(aView),
  fBar(aBar),
  fDe(aDe),
  fTime(aTime),
  fTimeofflight(-1),
  fCoordinate(pos),
  fZ(-99),
  fChargeZ(-99),
  fChargeA(ChargeA),
  fChargeB(ChargeB),
  fAmplitudeA(AmplitudeA),
  fAmplitudeB(AmplitudeB),
  fTimeA(TimeA),
  fTimeB(TimeB),
  fIsValid(true),
  fTrigType(TrigType)
{
}

//______________________________________________________________________________
//
void TATWhit::AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
	fMCindex.Set(fMCindex.GetSize()+1);
	fMCindex[fMCindex.GetSize()-1]   = mcId;
	fMCtrackId.Set(fMCtrackId.GetSize()+1);
	fMCtrackId[fMCtrackId.GetSize()-1] = trackId;
}

//______________________________________________________________________________
//
void TATWhit::Clear(Option_t* /*option*/)
{
	fMCindex.Set(0);
	fMCtrackId.Set(0);
	fLayer=0;
	fBar=0;
	fDe=0;
	fTime=0;
	fTimeofflight=0,
	fCoordinate=0;
	fZ=0;
        fChargeZ = 0;
	fChargeA=0;
	fChargeB=0;
	fAmplitudeA=0;
	fAmplitudeB=0;
	fTimeA=0;
	fTimeB=0;
        fIsValid = true;

}

//##############################################################################

TString TATWntuHit::fgkBranchName   = "twrh.";


ClassImp(TATWntuHit);

//------------------------------------------+-----------------------------------
//! 
TATWntuHit::TATWntuHit() 
: TAGdata(),
  fListOfHits(0x0)
{
	fHitlayY = 0;
	fHitlayX = 0;
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWntuHit::~TATWntuHit()
{
	delete fListOfHits;
}

//______________________________________________________________________________
//  standard
TATWhit* TATWntuHit::NewHit( int layer, int bar, double energyLoss, double atime, double pos, double ChargeA, double ChargeB, double AmplitudeA, double AmplitudeB, double TimeA, double TimeB, Int_t TrigType) {

	TClonesArray &pixelArray = *fListOfHits;
	if(layer == (int)LayerY) fHitlayY++;
	else   if(layer == (int)LayerX) fHitlayX++;
	TATWhit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWhit( layer, bar, energyLoss, atime, pos, ChargeA, ChargeB, AmplitudeA, AmplitudeB, TimeA, TimeB, TrigType);


	return hit;
}

//------------------------------------------+-----------------------------------
//! return number of hits for a given sensor.  
int TATWntuHit::GetHitN(int layer) {

  if(layer == (int)LayerY) return fHitlayY;
  else if(layer == (int)LayerX) return fHitlayX;
	else  return -1;

}

int TATWntuHit::GetHitN()
{
	return fHitlayY+fHitlayX;
}
//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWhit* TATWntuHit::Hit(Int_t i)
{
	return (TATWhit*) ((*fListOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWhit* TATWntuHit::GetHit( int hitID, int layer ) {
	int tmpId(0);
	for(int iD=0; iD<fListOfHits->GetEntriesFast(); iD++)
	{
		int CurrentLayer=((TATWhit*)fListOfHits->At(iD))->GetLayer();
		if(CurrentLayer == layer)
		{
			if(tmpId == hitID)
				return (TATWhit*)(fListOfHits->At(iD));
			tmpId++;

		}
	}
	return nullptr;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWhit* TATWntuHit::GetHit( int hitID) {


  if(hitID >=0 && hitID<fListOfHits->GetEntriesFast())
    return (TATWhit*)(fListOfHits->At(hitID));
  else
    return nullptr;

}

//------------------------------------------+-----------------------------------
TClonesArray* TATWntuHit::GetListOfHits() {

	return fListOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TATWntuHit::SetupClones()   {

	if (fListOfHits) return;
	fListOfHits = new TClonesArray("TATWhit");
	//    fListOfHits->SetOwner(true);

}


//------------------------------------------+-----------------------------------
//! Clear event.
void TATWntuHit::Clear(Option_t*) {

	TAGdata::Clear();
	fListOfHits->Clear();
	fHitlayY=0;
	fHitlayX=0;

}

//------------------------------------------+-----------------------------------
//! ostream insertion.
void TATWntuHit::ToStream(ostream& os, Option_t* option) const
{
	// for (Int_t i = 0; i < m_vtxGeo->GetNSensors(); ++i) {

	// os << "TATWntuHit " << GetName()
	// << Form("  nPixels=%3d", GetPixelsN(i))
	// << endl;

	// //TODO properly
	// //os << "slat stat    adct    adcb    tdct    tdcb" << endl;
	// for (Int_t j = 0; j < GetPixelsN(i); j++) {  // all by default
	// const TATWhit*  pixel = GetPixel(i,j, "all");
	// if (pixel)
	// os << Form("%4d", pixel->GetPixelIndex());
	// os << endl;
	// }
	// }
}
