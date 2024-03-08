/*!
 \file TANAactBaseNtu.cxx
 \brief   Implementation of TANAactBaseNtu.
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"

// GLB
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactBaseNtu.hxx"

/*!
 \class TANAactBaseNtu
 \brief Base class for global analysis
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuTrack global track container descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
TANAactBaseNtu::TANAactBaseNtu(const char* name,TAGdataDsc* pNtuTrack, TAGparaDsc* pgGeoMap, TTree* p_tree)
:  TAGaction(name, "TANAactBaseNtu - Global analysis base class"),
   fpNtuTrack(pNtuTrack),
   fpGeoMapG(pgGeoMap),
   fpTree(p_tree)
{
   AddDataIn(pNtuTrack,  "TAGntuGlbTrack");

   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (!fpFootGeo)
      Error("TANAactBaseNtu()", "No GeoTrafo action available yet\n");
   
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
TANAactBaseNtu::~TANAactBaseNtu()
{
}


//! \brief Get the name of a particle from its charge
//! \param[in] ch Charge of the particle
//! \return Name of the particle
TString TANAactBaseNtu::GetParticleNameFromCharge(int ch)
{
	TString name;
	switch(ch)
	{
		case 1:	name = "H";	break;
		case 2:	name = "He";	break;
		case 3:	name = "Li";	break;
		case 4:	name = "Be";	break;
		case 5:	name = "B";	break;
		case 6:	name = "C";	break;
		case 7:	name = "N";	break;
		case 8:	name = "O";	break;
		default:
			name = "fail";	break;
	}

	return name;
}
