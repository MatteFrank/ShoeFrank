/*!
 \file TANAactPtReso.cxx
 \brief   Implementation of TANAactPtReso
 \author R. Zarrella
 */

#include "TANAactPtReso.hxx"

/*!
 \class TANAactPtReso
 \brief Base class for global reconstruction momentum performance analysis
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuTrack global track container descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
TANAactPtReso::TANAactPtReso(const char* name,TAGdataDsc* pNtuTrack, TAGparaDsc* pgGeoMap)
:  TANAactBaseNtu(name, pNtuTrack, pgGeoMap)
{
   AddDataIn(pNtuTrack,  "TAGntuGlbTrack");

   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (!fpFootGeo)
      Error("TANAactPtReso()", "No GeoTrafo action available yet\n");
   
   TAGparGeo* pGeoMapG = (TAGparGeo*)   fpGeoMapG->Object();

   // beam loss in target
   fBeamEnergyTarget  = pGeoMapG->GetBeamPar().Energy;
   fBeamA             = pGeoMapG->GetBeamPar().AtomicMass;
   fBeamZ             = pGeoMapG->GetBeamPar().AtomicNumber;
   
   TString matTgt     = pGeoMapG->GetTargetPar().Material;
   Float_t thickTgt   = pGeoMapG->GetTargetPar().Size[2]/2.; // in average
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANAactPtReso::~TANAactPtReso()
{
}


//! Create the histograms for momentum resolution
void TANAactPtReso::CreateHistogram()
{

}


//! Action for momentum resolution
Bool_t TANAactPtReso::Action()
{
	return true;
}