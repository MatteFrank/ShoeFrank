/*!
  \file
  \version $Id: TATWactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TATWactNtuMC.
*/
#include "TAGgeoTrafo.hxx"

#include "TATWdatRaw.hxx"
#include "TATWactNtuMC.hxx"

#include "TATWdigitizer.hxx"

/*!
  \class TATWactNtuMC TATWactNtuMC.hxx "TATWactNtuMC.hxx"
  \brief NTuplizer for BM raw hits. **
*/

ClassImp(TATWactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWactNtuMC::TATWactNtuMC(const char* name,
                           TAGdataDsc* p_hitraw,
                           EVENT_STRUCT* evStr)
  : TAGaction(name, "TATWactNtuMC - NTuplize ToF raw data"),
   m_hitContainer(p_hitraw),
    m_eventStruct(evStr)
{
  AddDataOut(p_hitraw, "TATWntuRaw");
  CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TATWactNtuMC::~TATWactNtuMC()
{
   delete m_Digitizer;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TATWactNtuMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   fpHisHitCol = new TH1F("twHitCol", "ToF Wall - Column hits", 22, 0., 22);
   AddHistogram(fpHisHitCol);
   
   fpHisHitLine = new TH1F("twHitLine", "ToF Wall - Line hits", 22, 0., 22);
   AddHistogram(fpHisHitLine);

   fpHisHitMap = new TH2F("twHitMap", "ToF Wall - Hit Map", 22, 0., 22, 22, 0, 22);
   AddHistogram(fpHisHitMap);

   fpHisDeTot = new TH1F("twDeTot", "ToF Wall - Total energy loss", 1000, 0., 200*3e4);
   AddHistogram(fpHisDeTot);

   fpHisDeTotMc = new TH1F("twMcDeTot", "ToF wall - MC Total energy loss", 1000, 0., 200);
   AddHistogram(fpHisDeTotMc);
   
   fpHisTimeTot = new TH1F("twTimeTot", "ToF Wall - Total time", 1000, 0., 200e3);
   AddHistogram(fpHisTimeTot);
   
   fpHisTimeTotMc = new TH1F("twMcTimeTot", "ToF wall - MC Total time", 1000, 0., 200e3);
   AddHistogram(fpHisTimeTotMc);
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TATWactNtuMC::CreateDigitizer()
{
   TATWntuRaw* pNtuRaw = (TATWntuRaw*) m_hitContainer->Object();
   
   m_Digitizer = new TATWdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Action.

bool TATWactNtuMC::Action() {

   if ( fDebugLevel> 0 )     cout << "TATWactNtuMC::Action() start" << endl;

   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   TATWntuRaw* containerHit = (TATWntuRaw*) m_hitContainer->Object();
   TATWparGeo* geoMap = (TATWparGeo*) gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();

    //The number of hits inside the Start Counter is stn
    if ( fDebugLevel> 0 )     cout << "Processing n Scint " << m_eventStruct->SCNn << endl;

    // clear map
    m_Digitizer->ClearMap();
   
    // fill the container of hits, divided by layer, i.e. the column at 0 and row at 1
    for (int i=0; i < m_eventStruct->SCNn; i++) { 
    
       Int_t id      = m_eventStruct->SCNibar[i];
       Int_t trackId = m_eventStruct->SCNid[i] - 1;
       Float_t x0    = m_eventStruct->SCNxin[i];
       Float_t y0    = m_eventStruct->SCNyin[i];
       Float_t z0    = m_eventStruct->SCNzin[i];
       Float_t z1    = m_eventStruct->SCNzout[i];
       Float_t edep  = m_eventStruct->SCNde[i]*TAGgeoTrafo::GevToMev();
       Float_t time  = m_eventStruct->SCNtim[i]*TAGgeoTrafo::SecToPs();
       

        // layer, bar, de, time, ntupID, parentID
       int view = m_eventStruct->SCNiview[i];    // in ntuple now layers are 0 and 1
       if ( fDebugLevel> 0 )
          printf("%d %d\n", view,  m_eventStruct->SCNibar[i]);

       TVector3 posIn(x0, y0, z0);
       TVector3 posInLoc = geoTrafo->FromGlobalToTWLocal(posIn);

       Int_t Z = m_eventStruct->TRcha[trackId];
       m_Digitizer->Process(edep, posInLoc[0], posInLoc[1], z0, z1, time, id+TATWparGeo::GetLayerOffset()*view, Z);
       TATWntuHit* hit = m_Digitizer->GetCurrentHit();
       hit->AddMcTrackIdx(trackId, i);

       if (ValidHistogram()) {
          fpHisDeTotMc->Fill(edep);
          fpHisDeTot->Fill(hit->GetEnergyLoss());
          
          fpHisTimeTotMc->Fill(time);
          fpHisTimeTot->Fill(hit->GetTime());
          
          if (hit->IsColumn())
             fpHisHitCol->Fill(hit->GetBar());
          else
             fpHisHitLine->Fill(hit->GetBar());
       }
    }

     m_hitContainer->SetBit(kValid);

   return true;
}



































