/*!
  \file
  \version $Id: TATWactCalibTW.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TATWactCalibTW.
*/

#include <map>

#include "TError.h"
#include "TMath.h"
#include "TH1F.h"

#include "TATWactCalibTW.hxx"
#include "Parameters.h"

/*!
  \class TATWactCalibTW TATWactCalibTW.hxx "TATWactCalibTW.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TATWactCalibTW);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactCalibTW::TATWactCalibTW(const char* name,
			     TAGdataDsc* p_datraw,
			     TAGdataDsc* p_nturaw,
			     TAGdataDsc* p_STnturaw,
			     TAGparaDsc* p_pargeom,
			     TAGparaDsc* p_parmap,
			     TAGparaDsc* p_calmap,
			     TAGparaDsc* p_pargeoG)
  : TAGaction(name, "TATWactCalibTW - Unpack TW raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpSTNtuRaw(p_STnturaw),
    fpParGeo(p_pargeom),
    fpParMap(p_parmap),
    fpCalPar(p_calmap),
    fpParGeo_Gl(p_pargeoG),
    fTofPropAlpha(67.43-03), // velocity^-1 of light propagation in the TW bar (ns/cm)
    fTofErrPropAlpha(0.09e-03),  // ns/cm
    f_geoTrafo(nullptr),
    fEvtCnt(0)
{

  AddDataIn(p_datraw, "TATWdatRaw");
  AddDataIn(p_STnturaw, "TASTntuRaw");
  AddDataOut(p_nturaw, "TATWntuRaw");

  AddPara(p_pargeom, "TATWparGeo");
  AddPara(p_parmap, "TATWparMap");
  AddPara(p_calmap, "TATWparCal");
  AddPara(p_pargeoG, "TAGparGeo");


  f_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  // global geo object
  f_pargeo_gl = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();

  fParticleID = ZbeamToParticleID(f_pargeo_gl->GetBeamPar().AtomicNumber);

  // TW geo cal and map object
  f_pargeo = (TATWparGeo*) fpParGeo->Object();
  f_parmap = (TATWparMap*) fpParMap->Object();
  f_parcal = (TATWparCal*) fpCalPar->Object();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactCalibTW::~TATWactCalibTW()
{}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactCalibTW::CreateHistogram()
{
  DeleteHistogram();

	//Cycle on the TW positions
	for(Int_t PosID=0; PosID < nSlatCross; ++PosID)
	{
		//Allocate memory for all the histograms
		_hQLayerX.push_back(new TH1D(Form("hQLayerX-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), Form("hQLayerX-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), QBINS, QMIN, QMAX));

		_hQLayerY.push_back(new TH1D(Form("hQLayerY-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), Form("hQLayerY-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), QBINS, QMIN, QMAX));

		_hTOFLayerX.push_back(new TH1D(Form("hTOFLayerX-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), Form("hTOFLayerX-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), TOFBINS, TOFMIN, TOFMAX));

		_hTOFLayerY.push_back(new TH1D(Form("hTOFLayerY-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), Form("hTOFLayerY-PartID%d-En%.3lf-Pos%d", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID), TOFBINS, TOFMIN, TOFMAX));

		//Setup the histogram and axis title for readability when debugging
		_hQLayerX.at(PosID)->SetTitle(Form("hQLayerX-PartID%d-En%.3lf-Pos%d;Q [V*ns]; Entries", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID));
		_hQLayerY.at(PosID)->SetTitle(Form("hQLayerX-PartID%d-En%.3lf-Pos%d;Q [V*ns]; Entries", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID));
		_hTOFLayerX.at(PosID)->SetTitle(Form("hTOFLayerX-PartID%d-En%.3lf-Pos%d;TOF_{raw} [ns]; Entries", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID));
		_hTOFLayerY.at(PosID)->SetTitle(Form("hTOFLayerY-PartID%d-En%.3lf-Pos%d;TOF_{raw} [ns]; Entries", fParticleID, f_pargeo_gl->GetBeamPar().Energy, PosID));

		AddHistogram(_hQLayerX.at(PosID));
		AddHistogram(_hQLayerY.at(PosID));
		AddHistogram(_hTOFLayerX.at(PosID));
		AddHistogram(_hTOFLayerY.at(PosID));
	}

  SetValidHistogram(kTRUE);

  return;
}
//------------------------------------------+-----------------------------------
//! Action.

Bool_t TATWactCalibTW::Action() {

  TATWdatRaw*   p_datraw = (TATWdatRaw*) fpDatRaw->Object();
  TASTntuRaw*   p_STnturaw = (TASTntuRaw*)  fpSTNtuRaw->Object();
  TATWntuRaw*   p_nturaw = (TATWntuRaw*)  fpNtuRaw->Object();

  //////////// Time Trigger info from ST ///////////////

  if(FootDebugLevel(1)) {
    cout<<""<<endl;
    Info("Action()","Evt N::%d",fEvtCnt);
    fEvtCnt++;
  }

  double STtrigTime = p_STnturaw->GetTriggerTime();
  double STtrigTimeOth = p_STnturaw->GetTriggerTimeOth();

  Int_t SThitN = p_STnturaw->GetHitsN();  // same of STtrigTime

  TASTntuHit *stHit = (TASTntuHit*)p_STnturaw->GetHit(0);
  Double_t time_st = stHit->GetTime();

  if(FootDebugLevel(1))
    cout<<"ST hitN::"<<SThitN<<" trigTime::"<<STtrigTime<<"  trigTimeOth::"<<STtrigTimeOth<<" time_ST::"<<time_st<<endl;

  /////////////////////////////////////////////////////

  p_nturaw->SetupClones();
  TATWchannelMap *chmap=f_parmap->GetChannelMap();
  int nhit = p_datraw->GetHitsN();

	Int_t NumberOfValidHits = 0;

  int ch_num, bo_num;
  map<int, vector<TATWrawHit*> > PMap;
  // loop over the hits to populate a map with key boardid and value std::vector of TATWrawHit  pointer
  for(int ih = 0; ih< nhit; ih++)
	{
    TATWrawHit *aHi = p_datraw->GetHit(ih);
		if(aHi->GetCharge() < .5){continue;} //ignore the hit if the charge is too low -> CALIBRATION
    ch_num = aHi->GetChID();
    bo_num = aHi->GetBoardId();
    aHi->SetTime(aHi->GetTime());
    aHi->SetTimeOth(aHi->GetTimeOth());
    if (PMap.find(aHi->GetBoardId())==PMap.end())
		{
      PMap[aHi->GetBoardId()].resize(NUMBEROFCHANNELS);
      for (int ch=0;ch<NUMBEROFCHANNELS;++ch)
			{
				PMap[aHi->GetBoardId()][ch]=nullptr;
    	}
  	}
    PMap[aHi->GetBoardId()][aHi->GetChID()]=aHi;
		NumberOfValidHits++;
  }

	//Consider only clean events during calibration, i.e. one hit per layer, i.e 2 channels per layer
	if(NumberOfValidHits != 4)
	{
		fpNtuRaw->SetBit(kValid);
		return kTRUE;
	}

	// create dummy variables to store hits before filling the histograms
	// tuple(PosID, Qraw, TOFraw)
	std::tuple<Int_t, Double_t, Double_t> HitLayerX(-1, -1., -1.), HitLayerY(-1, -1., -1.);

	// loop over all the bars
	for (auto it=chmap->begin();it!=chmap->end();++it)
	{
    //
    Int_t BarId = it->first;

    Int_t boardid  = get<0>(it->second);
    Int_t channelA = get<1>(it->second);
    Int_t channelB = get<2>(it->second);

    Int_t Layer = (Int_t)chmap->GetBarLayer(BarId);

    //
    if (PMap.find(boardid)!=PMap.end())
		{

			if(FootDebugLevel(1))
		  	Info("LoadChannelMap()","BAR_ID, %d LAYER_ID %d - BOARD ID %d, Channel A %d, Channel B %d",BarId,Layer,boardid,channelA,channelB);

			TATWrawHit* hita = PMap[boardid][channelA];
			TATWrawHit* hitb = PMap[boardid][channelB];

			// if one of the channels was not acquired
			// not present, do not create the Hit
			if (hita!=nullptr && hitb!=nullptr )
			{
		    // this to be consistent with the the bar id of TATWdetector.map
		    Int_t ShoeBarId = (BarId)%nSlatsPerLayer;

		    // get raw energy
		    Double_t rawEnergy = GetRawEnergy(hita,hitb);

		    // get raw time in ns
		    Double_t rawTime    = GetRawTime(hita,hitb) - STtrigTime;

		    // get position from the TOF between the two channels
		    Double_t posAlongBar = GetPosition(hita,hitb);

				// get the PosId (0-399) of the cross btw horizontal and vertical bars
		    Int_t    PosId  = GetBarCrossId(Layer,ShoeBarId,posAlongBar);

				if(Layer == LayerX)
				{
					HitLayerX = std::make_tuple(PosId, rawEnergy, rawTime);
				}
				else if(Layer == LayerY)
				{
					HitLayerY = std::make_tuple(PosId, rawEnergy, rawTime);
				}
				else
				{
					Error("Action()","Layer %d doesn't exist",Layer);
				}
  		}
    }
  }

	//Fill the histograms if the event is "clean"
	if(std::get<0>(HitLayerX) != -1 && std::get<0>(HitLayerY) != -1 && std::get<0>(HitLayerX) == std::get<0>(HitLayerY))
	{
		_hQLayerX.at(std::get<0>(HitLayerX))->Fill(std::get<1>(HitLayerX));
		_hTOFLayerX.at(std::get<0>(HitLayerX))->Fill(std::get<2>(HitLayerX));

		_hQLayerY.at(std::get<0>(HitLayerY))->Fill(std::get<1>(HitLayerY));
		_hTOFLayerY.at(std::get<0>(HitLayerY))->Fill(std::get<2>(HitLayerY));
	}

  fpNtuRaw->SetBit(kValid);

	return kTRUE;

}

//________________________________________________________________

Double_t TATWactCalibTW::GetRawEnergy(TATWrawHit*a,TATWrawHit*b)
{

  // if the waveform is strange/corrupted it is likely that the charge is <0
  if (a->GetCharge()<0 || b->GetCharge()<0)
    {
      return -1;
    }

  return TMath::Sqrt(a->GetCharge()*b->GetCharge());

}

//________________________________________________________________

Double_t TATWactCalibTW::GetRawTime(TATWrawHit*a,TATWrawHit*b)
{
  return (a->GetTime()+b->GetTime())/2;
}


//________________________________________________________________

Double_t TATWactCalibTW::GetPosition(TATWrawHit*a,TATWrawHit*b)
{
  return (b->GetTime()-a->GetTime())/(2*fTofPropAlpha);  // local (TW) ref frame
}

//________________________________________________________________

Int_t TATWactCalibTW::GetBarCrossId(Int_t layer, Int_t barId, Double_t rawPos)
{

  Int_t barX(-1), barY(-1);  // id of horizontal and vertical bars
  Int_t barCrossId = -1;     // id of the cross btw vertical and horizontal bars

  // get the bar perpendicular to the position along the bar rawPos
  Int_t perp_BarId = GetPerpBarId(layer,barId,rawPos);

  if(perp_BarId==-1)
    return -1;

  if(layer==LayerX) {

    barX = barId;
    barY = perp_BarId;

  } else if(layer==LayerY) {

    barX = perp_BarId;
    barY = barId;

  }
  else {
    Error("GetBarCrossId()","Layer %d doesn't exist",layer);
    return -1;
  }

  barCrossId = barY + barX * nSlatsPerLayer;  // left top corner is (0,0) --> pos=0, right bottom corner is (19,19) --> pos = 399

  if(FootDebugLevel(1))
    Info("GetBarCrossId()","%s, bar::%d, rawPos::%f, perp_BarId::%d, barCrossId::%d\n",LayerName[(TLayer)layer].data(),barId,rawPos,perp_BarId,barCrossId);


  return barCrossId;

}


//_____________________________________________________________________________

Int_t TATWactCalibTW::GetPerpBarId(Int_t layer, Int_t barId, Double_t rawPos)
{

  Double_t xloc(-99.), yloc(-99.);
  Double_t zloc = f_pargeo->GetLayerPosition(layer)[2];

  Int_t  perp_BarId = -1; //barId of the perpendicular bar corresponding to rawPos

  if(layer==LayerX) {
    xloc = rawPos;
    yloc = f_pargeo->GetBarPosition(layer,barId)[1];
    double x_glb  = xloc;
    double y_glb  = yloc;
    perp_BarId = f_pargeo->GetBarId(LayerY,x_glb,y_glb);
  }
  else if(layer==LayerY) {
    xloc = f_pargeo->GetBarPosition(layer,barId)[0];
    yloc = rawPos;
    perp_BarId = f_pargeo->GetBarId(LayerX,xloc,yloc);
  }
  else {
    Error("GetPerpBarId()","Layer %d doesn't exist",layer);
    return -1;
  }

  if(perp_BarId==-1)
    return -1;

  perp_BarId -= TATWparGeo::GetLayerOffset()*(1-layer);

  if(FootDebugLevel(1))
    Info("GetPerpBarId()","%s, bar::%d, x::%f, y::%f, z::%f, perp_BarId::%d\n",LayerName[(TLayer)layer].data(),barId,xloc,yloc,zloc,perp_BarId);

  return perp_BarId;

}


/*Function needed for notation coherence
*
*/
ParticleType TATWactCalibTW::ZbeamToParticleID(Int_t Zbeam)
{
	switch(Zbeam)
	{
		case 1: return Proton; break;
		case 2: return Helium; break;
		case 6: return Carbon; break;
		case 8: return Oxygen; break;

		default: return None; break;
	}
}
