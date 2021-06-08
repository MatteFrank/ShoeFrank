/*!
  \file
  \version $Id: TAMSDactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TAMSDactNtuRaw.
*/


#include "TAMSDactNtuRaw.hxx"
#include "TAGrecoManager.hxx"

/*!
  \class TAMSDactNtuRaw TAMSDactNtuRaw.hxx "TAMSDactNtuRaw.hxx"
  \brief Get MSD raw data from DAQ. **
*/

ClassImp(TAMSDactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDactNtuRaw::TAMSDactNtuRaw(const char* name,
                             TAGdataDsc* dscdatraw,
                             TAGdataDsc* dscdatdaq,
                             TAGparaDsc* dscparmap,
                             TAGparaDsc* dscpargeo)
  : TAGaction(name, "TAMSDactNtuRaw - Unpack MSD raw data"),
    fpDatRaw(dscdatraw),
    fpDatDaq(dscdatdaq),
    fpParMap(dscparmap),
    fpParGeo(dscpargeo)
{
  if (FootDebugLevel(1))
    cout<<"TAMSDactNtuRaw::default constructor::Creating the MSD hit Ntuplizer"<<endl;
   
  AddDataOut(dscdatraw, "TAMSDntuRaw");
  AddPara(dscparmap, "TAMSDparMap");
  AddPara(dscpargeo, "TAMSDparGeo");
  AddDataIn(dscdatdaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMSDactNtuRaw::~TAMSDactNtuRaw()
{
}

//------------------------------------------+-----------------------------------

void TAMSDactNtuRaw::CreateHistogram()
{
  DeleteHistogram();
  
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpParGeo->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {

      fpHisStripMap[i] = new TH1F(Form("msStripMap%d", i+1), Form("MSD - strip map for sensor %d", i+1),
                                  pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
      AddHistogram(fpHisStripMap[i]);
   }
   
  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAMSDactNtuRaw::Action()
{
   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();

   Int_t nFragments = p_datdaq->GetFragmentsN();

  if(FootDebugLevel(1))
    cout<<"TAMSDactNtuRaw::Action():: I'm going to charge "<<nFragments<<" number of fragments"<<endl;

   for (Int_t i = 0; i < nFragments; ++i) {
       TString type = p_datdaq->GetClassType(i);
       if (type.Contains("DEMSDEvent")) {
         const DEMSDEvent* evt = static_cast<const DEMSDEvent*> (p_datdaq->GetFragment(i));
          if (FootDebugLevel(1))
            evt->printData();
         DecodeHits(evt);
       }
   }

   fpDatRaw->SetBit(kValid);

  if(FootDebugLevel(2))
    cout<<"TAMSDactNtuRaw::Action():: done"<<endl;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAMSDactNtuRaw::DecodeHits(const DEMSDEvent* evt)
{

   TAMSDntuRaw*    p_datraw = (TAMSDntuRaw*)    fpDatRaw->Object();
   TAMSDparMap*    p_parmap = (TAMSDparMap*)    fpParMap->Object();
   TAMSDparGeo*    p_pargeo = (TAMSDparGeo*)    fpParGeo->Object();

   // decode here
   Int_t sensorId = (evt->boardHeader & 0xF)-1;
   
   for (Int_t i = 0; i < p_pargeo->GetStripsN(); ++i) {
      fpHisStripMap[2*sensorId]->Fill(evt->Xplane[i]);
      fpHisStripMap[2*sensorId+1]->Fill(evt->Yplane[i]);
   }
   
   return kTRUE;
}
