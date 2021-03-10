/*!
  \class TATWntuHit TATWntuHit.hxx "TATWntuHit.hxx"
  \brief Container class for VTX ntu hit **
 */

////////////////////////////////////////////////////////////
// Class Description of TATWhit                        //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////


#include "TString.h"
#include "TClonesArray.h"

#include "TATWntuHit.hxx"

ClassImp(TATWhit) // Description of Single Detector TATWhit 

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWhit::TATWhit()
  : m_layer(-1),
    m_bar(-1),
    m_de(-1),
    m_time(-1),
    m_timeofflight(-1),
    m_coordinate(-99),
    m_z(-99),
    m_chargeZ(-99),
    m_chargeCOM(-1),
    m_ChargeA(-1),
    m_ChargeB(-1),
    m_AmplitudeA(-1),
    m_AmplitudeB(-1),
    m_TimeA(-1),
    m_TimeB(-1),
    m_time_oth(-1),
    m_TimeA_oth(-1),
    m_TimeB_oth(-1),
    m_IsValid(true)
{
}


//______________________________________________________________________________
//  build a hit from a rawHit
TATWhit::TATWhit( TATWrawHit* hit )
: TAGobject(),
  m_layer(-1),
  m_bar(-1),
  m_de(-1),
  m_time(-1),
  m_timeofflight(-1),
  m_coordinate(-99),
  m_z(-99),
  m_chargeZ(-99),
  m_chargeCOM(-1),
  m_ChargeA(-1),
  m_ChargeB(-1),
  m_AmplitudeA(-1),
  m_AmplitudeB(-1),
  m_TimeA(-1),
  m_TimeB(-1),
  m_time_oth(-1),
  m_TimeA_oth(-1),
  m_TimeB_oth(-1),
  m_IsValid(true)
{
}

//______________________________________________________________________________
//
TATWhit::TATWhit(const TATWhit& aHit)
:  TAGobject(aHit),
   m_layer(aHit.m_layer),
   m_bar(aHit.m_bar),
   m_de(aHit.m_de),
   m_time(aHit.m_time),
   m_timeofflight(aHit.m_timeofflight),
   m_coordinate(aHit.m_coordinate),
   m_z(aHit.m_z),
   m_chargeZ(aHit.m_chargeZ),
   m_chargeCOM(aHit.m_chargeCOM),
   m_MCindex(aHit.m_MCindex),
   m_McTrackId(aHit.m_McTrackId),
   m_ChargeA(aHit.m_ChargeA),
   m_ChargeB(aHit.m_ChargeB),
   m_AmplitudeA(aHit.m_AmplitudeA),
   m_AmplitudeB(aHit.m_AmplitudeB),
   m_TimeA(aHit.m_TimeA),
   m_TimeB(aHit.m_TimeB),
   m_time_oth(aHit.m_time_oth),
   m_TimeA_oth(aHit.m_TimeA_oth),
   m_TimeB_oth(aHit.m_TimeB_oth),
   m_IsValid(aHit.m_IsValid)
{
}

//______________________________________________________________________________
// Build the hit from its layerID and barID
TATWhit::TATWhit (Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime, Double_t aTime_oth,
			Double_t pos,Double_t chargeCOM,Double_t ChargeA,
			Double_t ChargeB,Double_t AmplitudeA,Double_t AmplitudeB,Double_t TimeA,Double_t TimeB, Double_t TimeA_oth,Double_t TimeB_oth):
  
  TAGobject(),
  m_layer(aView),
  m_bar(aBar),
  m_de(aDe),
  m_time(aTime),
  m_timeofflight(-1),
  m_coordinate(pos),
  m_z(-99),
  m_chargeZ(-99),
  m_chargeCOM(chargeCOM),
  m_ChargeA(ChargeA),
  m_ChargeB(ChargeB),
  m_AmplitudeA(AmplitudeA),
  m_AmplitudeB(AmplitudeB),
  m_TimeA(TimeA),
  m_TimeB(TimeB),
  m_time_oth(aTime_oth),
  m_TimeA_oth(TimeA_oth),
  m_TimeB_oth(TimeB_oth),
  m_IsValid(true)
{
}

//______________________________________________________________________________
//
void TATWhit::AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
	m_MCindex.Set(m_MCindex.GetSize()+1);
	m_MCindex[m_MCindex.GetSize()-1]   = mcId;
	m_McTrackId.Set(m_McTrackId.GetSize()+1);
	m_McTrackId[m_McTrackId.GetSize()-1] = trackId;
}

//______________________________________________________________________________
//
void TATWhit::Clear(Option_t* /*option*/)
{
	m_MCindex.Set(0);
	m_McTrackId.Set(0);
	m_layer=0;
	m_bar=0;
	m_de=0;
	m_time=0;
	m_timeofflight=0,
	m_coordinate=0;
	m_z=0;
        m_chargeZ = 0;
	m_chargeCOM=0;
	m_ChargeA=0;
	m_ChargeB=0;
	m_AmplitudeA=0;
	m_AmplitudeB=0;
	m_TimeA=0;
	m_TimeB=0;
	m_time_oth=0;
	m_TimeA_oth=0;
	m_TimeB_oth=0;
   m_IsValid = true;

}

//##############################################################################

ClassImp(TATWntuHit);
TString TATWntuHit::fgkBranchName   = "twrh.";


//------------------------------------------+-----------------------------------
//! 
TATWntuHit::TATWntuHit() 
: TAGdata(),
  m_listOfHits(0x0)
{
	m_hitlayY = 0;
	m_hitlayX = 0;
	SetupClones();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWntuHit::~TATWntuHit()
{
	delete m_listOfHits;
}

//______________________________________________________________________________
//  standard
TATWhit* TATWntuHit::NewHit( int layer, int bar, double energyLoss, double atime, double atime_oth, double pos,double chargeCOM,
				double ChargeA, double ChargeB, double AmplitudeA, double AmplitudeB, double TimeA, double TimeB, double TimeA_oth, double TimeB_oth) {

	TClonesArray &pixelArray = *m_listOfHits;
	if(layer == (int)LayerY) m_hitlayY++;
	else   if(layer == (int)LayerX) m_hitlayX++;
	TATWhit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TATWhit( layer, bar, energyLoss, atime, atime_oth, pos,
										   chargeCOM,ChargeA, ChargeB, AmplitudeA, AmplitudeB, TimeA, TimeB,  TimeA_oth, TimeB_oth);


	return hit;
}

//------------------------------------------+-----------------------------------
//! return number of hits for a given sensor.  
int TATWntuHit::GetHitN(int layer) {

  if(layer == (int)LayerY) return m_hitlayY;
  else if(layer == (int)LayerX) return m_hitlayX;
	else  return -1;

}

int TATWntuHit::GetHitN()
{
	return m_hitlayY+m_hitlayX;
}
//------------------------------------------+-----------------------------------
//! Access \a i 'th hit

TATWhit* TATWntuHit::Hit(Int_t i)
{
	return (TATWhit*) ((*m_listOfHits)[i]);;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWhit* TATWntuHit::GetHit( int hitID, int layer ) {
	int tmpId(0);
	for(int iD=0; iD<m_listOfHits->GetEntries(); iD++)
	{
		int CurrentLayer=((TATWhit*)m_listOfHits->At(iD))->GetLayer();
		if(CurrentLayer == layer)
		{
			if(tmpId == hitID)
				return (TATWhit*)(m_listOfHits->At(iD));
			tmpId++;

		}
	}
	return nullptr;
}

//------------------------------------------+-----------------------------------
//! return a pixel for a given sensor
TATWhit* TATWntuHit::GetHit( int hitID) {

  if(hitID<m_listOfHits->GetEntries())
    return (TATWhit*)(m_listOfHits->At(hitID));
  else
    return nullptr;

}

//------------------------------------------+-----------------------------------
TClonesArray* TATWntuHit::GetListOfHits() {

	return m_listOfHits;
}

//------------------------------------------+-----------------------------------
//! Setup clones. Crate and initialise the list of pixels
void TATWntuHit::SetupClones()   {

	if (m_listOfHits) return;
	m_listOfHits = new TClonesArray("TATWhit");
	//    m_listOfHits->SetOwner(true);

}


//------------------------------------------+-----------------------------------
//! Clear event.
void TATWntuHit::Clear(Option_t*) {

	TAGdata::Clear();
	m_listOfHits->Clear();
	m_hitlayY=0;
	m_hitlayX=0;

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
