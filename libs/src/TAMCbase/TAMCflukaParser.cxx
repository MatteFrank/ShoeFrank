/*!
 \file TAMCflukaParser.cxx
 \brief   Implementation of TAMCflukaParser.
 */

#include "TAMCntuHit.hxx"
#include "TAMCflukaParser.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCflukaParser 
 \brief Parser for Fluka MC events. **
 */

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCflukaParser::TAMCflukaParser()
: TAGobject()
{
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCflukaParser::~TAMCflukaParser()
{
}

//------------------------------------------+-----------------------------------
//! ST hits
TAMCntuHit* TAMCflukaParser::GetStcHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->STCn; i++) {
    
    Int_t id       = 0;
    Int_t trackIdx = evStr->STCid[i];
    
    TVector3 ipos( evStr->STCxin[i], evStr->STCyin[i], evStr->STCzin[i]);
    TVector3 fpos( evStr->STCxout[i], evStr->STCyout[i], evStr->STCzout[i]);
    TVector3 imom( evStr->STCpxin[i], evStr->STCpyin[i], evStr->STCpzin[i]);
    TVector3 fmom( evStr->STCpxout[i], evStr->STCpyout[i], evStr->STCpzout[i]);
    
    p_nturaw->NewHit(trackIdx, id, -99, -99, ipos, fpos, imom, fmom, evStr->STCde[i], evStr->STCtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! BM hits
TAMCntuHit* TAMCflukaParser::GetBmHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->BMNn; i++) {
    
    Int_t layer    = evStr->BMNilay[i];
    Int_t view     = evStr->BMNiview[i];
    Int_t cellId   = evStr->BMNicell[i];
    Int_t trackIdx = evStr->BMNid[i];
    
    TVector3 ipos( evStr->BMNxin[i], evStr->BMNyin[i], evStr->BMNzin[i]);
    TVector3 fpos( evStr->BMNxout[i], evStr->BMNyout[i], evStr->BMNzout[i]);
    TVector3 imom( evStr->BMNpxin[i], evStr->BMNpyin[i], evStr->BMNpzin[i]);
    TVector3 fmom( evStr->BMNpxout[i],evStr->BMNpyout[i],evStr->BMNpzout[i]);
    
    p_nturaw->NewHit(trackIdx, layer, view, cellId, ipos, fpos, imom, fmom, evStr->BMNde[i], evStr->BMNtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! VTX hits
TAMCntuHit* TAMCflukaParser::GetVtxHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->VTXn; i++) {
    
    Int_t sensorId = evStr->VTXilay[i];
    Int_t trackIdx = evStr->VTXid[i];
    
    TVector3 ipos( evStr->VTXxin[i], evStr->VTXyin[i], evStr->VTXzin[i]);
    TVector3 imom( evStr->VTXpxin[i], evStr->VTXpyin[i], evStr->VTXpzin[i]);
    TVector3 fpos( evStr->VTXxout[i], evStr->VTXyout[i], evStr->VTXzout[i]);
    TVector3 fmom( evStr->VTXpxout[i], evStr->VTXpyout[i], evStr->VTXpzout[i]);
    
    p_nturaw->NewHit(trackIdx, sensorId, -99, -99,ipos, fpos, imom, fmom, evStr->VTXde[i], evStr->VTXtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! ITR hits
TAMCntuHit* TAMCflukaParser::GetItrHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->ITRn; i++) {
    
    Int_t sensorId = evStr->ITRisens[i];
    Int_t trackIdx = evStr->ITRid[i];
    
    TVector3 ipos( evStr->ITRxin[i], evStr->ITRyin[i], evStr->ITRzin[i]);
    TVector3 fpos( evStr->ITRxout[i], evStr->ITRyout[i], evStr->ITRzout[i]);
    TVector3 imom( evStr->ITRpxin[i], evStr->ITRpyin[i], evStr->ITRpzin[i]);
    TVector3 fmom( evStr->ITRpxout[i],evStr->ITRpyout[i],evStr->ITRpzout[i]);
    
    p_nturaw->NewHit(trackIdx, sensorId, -99, -99, ipos, fpos, imom, fmom, evStr->ITRde[i], evStr->ITRtim[i],0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! MSD hits
TAMCntuHit* TAMCflukaParser::GetMsdHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->MSDn; i++) {
    
    Int_t layer    = evStr->MSDilay[i];
    Int_t trackIdx = evStr->MSDid[i];
    
    TVector3 ipos( evStr->MSDxin[i], evStr->MSDyin[i], evStr->MSDzin[i]);
    TVector3 fpos( evStr->MSDxout[i], evStr->MSDyout[i], evStr->MSDzout[i]);
    TVector3 imom( evStr->MSDpxin[i], evStr->MSDpyin[i], evStr->MSDpzin[i]);
    TVector3 fmom( evStr->MSDpxout[i], evStr->MSDpyout[i], evStr->MSDpzout[i]);
    
    p_nturaw->NewHit(trackIdx, layer, -99, -99, ipos, fpos, imom, fmom, evStr->MSDde[i], evStr->MSDtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! TOF hits
TAMCntuHit* TAMCflukaParser::GetTwHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->SCNn; i++) {
    
    Int_t trackIdx = evStr->SCNid[i];
    Int_t barId    = evStr->SCNibar[i];
    Int_t view     = evStr->SCNiview[i];
    Float_t edep   = evStr->SCNde[i];
    Float_t time   = evStr->SCNtim[i];
    
    TVector3 ipos( evStr->SCNxin[i], evStr->SCNyin[i], evStr->SCNzin[i]);
    TVector3 fpos( evStr->SCNxout[i], evStr->SCNyout[i], evStr->SCNzout[i]);
    TVector3 imom( evStr->SCNpxin[i], evStr->SCNpyin[i], evStr->SCNpzin[i]);
    TVector3 fmom( evStr->SCNpxout[i], evStr->SCNpyout[i], evStr->SCNpzout[i]);
    
    p_nturaw->NewHit(trackIdx, barId, view, -99, ipos, fpos, imom, fmom, edep, time, 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! CAL hits
TAMCntuHit* TAMCflukaParser::GetCalHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
{
  TAMCntuHit* p_nturaw = (TAMCntuHit*) p_ntuhit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->CALn; i++) {
    
    Int_t crystalId = evStr->CALicry[i];
    Int_t trackIdx  = evStr->CALid[i];
    
    TVector3 ipos( evStr->CALxin[i], evStr->CALyin[i], evStr->CALzin[i]);
    TVector3 fpos( evStr->CALxout[i], evStr->CALyout[i], evStr->CALzout[i]);
    TVector3 imom( evStr->CALpxin[i], evStr->CALpyin[i], evStr->CALpzin[i]);
    TVector3 fmom( evStr->CALpxout[i],evStr->CALpyout[i],evStr->CALpzout[i]);
    
    p_nturaw->NewHit(trackIdx, crystalId, -99, -99, ipos, fpos, imom, fmom, evStr->CALde[i], evStr->CALtim[i], 0);
  }
  
  return p_nturaw;
}


//------------------------------------------+-----------------------------------
//! events
TAMCntuEvent* TAMCflukaParser::GetEvents(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuevt)
{
  TAMCntuEvent* p_nturaw = (TAMCntuEvent*) p_ntuevt->Object();  
  p_nturaw->Clear();

  p_nturaw->SetEventNumber(evStr->EventNumber);
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! Crossing regions
TAMCntuRegion* TAMCflukaParser::GetRegions(EVENT_STRUCT* evStr, TAGdataDsc* p_ntureg)
{
  TAMCntuRegion* p_nturaw = (TAMCntuRegion*) p_ntureg->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->CROSSn; i++) {
    
    Int_t   id      = evStr->CROSSid[i];
    Int_t   nreg    = evStr->CROSSnreg[i];  //mother id
    Int_t   nregold = evStr->CROSSnregold[i];
    Double_t mass   = evStr->CROSSm[i];
    Double_t charge = evStr->CROSSch[i];
    Double_t time   = evStr->CROSSt[i];
    
    TVector3 pos = TVector3(evStr->CROSSx[i],  evStr->CROSSy[i],  evStr->CROSSz[i]);
    TVector3 p   = TVector3(evStr->CROSSpx[i], evStr->CROSSpy[i], evStr->CROSSpz[i]);
    
    p_nturaw->NewRegion(id, nreg, nregold, pos, p, mass, charge, time);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! Tracks
TAMCntuPart* TAMCflukaParser::GetTracks(EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrack)
{
  TAMCntuPart* p_nturaw = (TAMCntuPart*) p_ntutrack->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->TRn; i++) {
    
    Double_t i_mass = evStr->TRmass[i];
    Int_t i_id = evStr->TRfid[i];
    Int_t i_mid = evStr->TRpaid[i];  //mother id
    Int_t i_chg = evStr->TRcha[i];
    Int_t i_reg = evStr->TRreg[i];
    Int_t i_bar = evStr->TRbar[i];
    Int_t i_dead = evStr->TRdead[i];
    Int_t i_type = evStr->TRgen[i];
    Double_t i_time = evStr->TRtime[i];
    Double_t i_tof = evStr->TRtof[i];
    Double_t i_trlen = evStr->TRtrlen[i];
    
    TVector3 ipos = TVector3(evStr->TRix[i],evStr->TRiy[i],evStr->TRiz[i]);
    TVector3 fpos = TVector3(evStr->TRfx[i],evStr->TRfy[i],evStr->TRfz[i]);
    TVector3 ip = TVector3(evStr->TRipx[i],evStr->TRipy[i],evStr->TRipz[i]);
    TVector3 fp = TVector3(evStr->TRfpx[i],evStr->TRfpy[i],evStr->TRfpz[i]);
    
    p_nturaw->NewTrack(i_id, i_chg, i_type, i_reg, i_bar, i_dead, i_mass, i_mid, i_time, i_tof, i_trlen, ipos, fpos, ip, fp);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! Find branches
void TAMCflukaParser::FindBranches(TTree* rootTree, EVENT_STRUCT* eve)
{
  rootTree->SetBranchAddress("EventNumber",&(eve->EventNumber));
  rootTree->SetBranchAddress("TRn",&(eve->TRn));
  rootTree->SetBranchAddress("TRpaid",&(eve->TRpaid));
  rootTree->SetBranchAddress("TRgen",&(eve->TRgen));
  rootTree->SetBranchAddress("TRcha",&(eve->TRcha));
  rootTree->SetBranchAddress("TRreg",&(eve->TRreg));
  rootTree->SetBranchAddress("TRbar",&(eve->TRbar));
  rootTree->SetBranchAddress("TRdead",&(eve->TRdead));
  rootTree->SetBranchAddress("TRfid",&(eve->TRfid));
  rootTree->SetBranchAddress("TRix",&(eve->TRix));
  rootTree->SetBranchAddress("TRiy",&(eve->TRiy));
  rootTree->SetBranchAddress("TRiz",&(eve->TRiz));
  rootTree->SetBranchAddress("TRfx",&(eve->TRfx));
  rootTree->SetBranchAddress("TRfy",&(eve->TRfy));
  rootTree->SetBranchAddress("TRfz",&(eve->TRfz));
  rootTree->SetBranchAddress("TRipx",&(eve->TRipx));
  rootTree->SetBranchAddress("TRipy",&(eve->TRipy));
  rootTree->SetBranchAddress("TRipz",&(eve->TRipz));
  rootTree->SetBranchAddress("TRfpx",&(eve->TRfpx));
  rootTree->SetBranchAddress("TRfpy",&(eve->TRfpy));
  rootTree->SetBranchAddress("TRfpz",&(eve->TRfpz));
  rootTree->SetBranchAddress("TRmass",&(eve->TRmass));
  rootTree->SetBranchAddress("TRtime",&(eve->TRtime));
  rootTree->SetBranchAddress("TRtof",&(eve->TRtof));
  rootTree->SetBranchAddress("TRtrlen",&(eve->TRtrlen));
  
  rootTree->SetBranchAddress("STCn",&(eve->STCn));
  rootTree->SetBranchAddress("STCid",&(eve->STCid));
  rootTree->SetBranchAddress("STCxin",&(eve->STCxin));
  rootTree->SetBranchAddress("STCyin",&(eve->STCyin));
  rootTree->SetBranchAddress("STCzin",&(eve->STCzin));
  rootTree->SetBranchAddress("STCpxin",&(eve->STCpxin));
  rootTree->SetBranchAddress("STCpyin",&(eve->STCpyin));
  rootTree->SetBranchAddress("STCpzin",&(eve->STCpzin));
  rootTree->SetBranchAddress("STCxout",&(eve->STCxout));
  rootTree->SetBranchAddress("STCyout",&(eve->STCyout));
  rootTree->SetBranchAddress("STCzout",&(eve->STCzout));
  rootTree->SetBranchAddress("STCpxout",&(eve->STCpxout));
  rootTree->SetBranchAddress("STCpyout",&(eve->STCpyout));
  rootTree->SetBranchAddress("STCpzout",&(eve->STCpzout));
  rootTree->SetBranchAddress("STCde",&(eve->STCde));
  rootTree->SetBranchAddress("STCal",&(eve->STCal));
  rootTree->SetBranchAddress("STCtim",&(eve->STCtim));
  
  rootTree->SetBranchAddress("BMNn",&(eve->BMNn));
  rootTree->SetBranchAddress("BMNid",&(eve->BMNid));
  rootTree->SetBranchAddress("BMNilay",&(eve->BMNilay));
  rootTree->SetBranchAddress("BMNiview",&(eve->BMNiview));
  rootTree->SetBranchAddress("BMNicell",&(eve->BMNicell));
  rootTree->SetBranchAddress("BMNxin",&(eve->BMNxin));
  rootTree->SetBranchAddress("BMNyin",&(eve->BMNyin));
  rootTree->SetBranchAddress("BMNzin",&(eve->BMNzin));
  rootTree->SetBranchAddress("BMNpxin",&(eve->BMNpxin));
  rootTree->SetBranchAddress("BMNpyin",&(eve->BMNpyin));
  rootTree->SetBranchAddress("BMNpzin",&(eve->BMNpzin));
  rootTree->SetBranchAddress("BMNxout",&(eve->BMNxout));
  rootTree->SetBranchAddress("BMNyout",&(eve->BMNyout));
  rootTree->SetBranchAddress("BMNzout",&(eve->BMNzout));
  rootTree->SetBranchAddress("BMNpxout",&(eve->BMNpxout));
  rootTree->SetBranchAddress("BMNpyout",&(eve->BMNpyout));
  rootTree->SetBranchAddress("BMNpzout",&(eve->BMNpzout));
  rootTree->SetBranchAddress("BMNde",&(eve->BMNde));
  rootTree->SetBranchAddress("BMNal",&(eve->BMNal));
  rootTree->SetBranchAddress("BMNtim",&(eve->BMNtim));
  
  rootTree->SetBranchAddress("VTXn",&(eve->VTXn));
  rootTree->SetBranchAddress("VTXid",&(eve->VTXid));
  rootTree->SetBranchAddress("VTXilay",&(eve->VTXilay));
  rootTree->SetBranchAddress("VTXxin",&(eve->VTXxin));
  rootTree->SetBranchAddress("VTXyin",&(eve->VTXyin));
  rootTree->SetBranchAddress("VTXzin",&(eve->VTXzin));
  rootTree->SetBranchAddress("VTXpxin",&(eve->VTXpxin));
  rootTree->SetBranchAddress("VTXpyin",&(eve->VTXpyin));
  rootTree->SetBranchAddress("VTXpzin",&(eve->VTXpzin));
  rootTree->SetBranchAddress("VTXxout",&(eve->VTXxout));
  rootTree->SetBranchAddress("VTXyout",&(eve->VTXyout));
  rootTree->SetBranchAddress("VTXzout",&(eve->VTXzout));
  rootTree->SetBranchAddress("VTXpxout",&(eve->VTXpxout));
  rootTree->SetBranchAddress("VTXpyout",&(eve->VTXpyout));
  rootTree->SetBranchAddress("VTXpzout",&(eve->VTXpzout));
  rootTree->SetBranchAddress("VTXde",&(eve->VTXde));
  rootTree->SetBranchAddress("VTXal",&(eve->VTXal));
  rootTree->SetBranchAddress("VTXtim",&(eve->VTXtim));
  
  rootTree->SetBranchAddress("ITRn",&(eve->ITRn));
  rootTree->SetBranchAddress("ITRid",&(eve->ITRid));
  rootTree->SetBranchAddress("ITRisens",&(eve->ITRisens));
  rootTree->SetBranchAddress("ITRxin",&(eve->ITRxin));
  rootTree->SetBranchAddress("ITRyin",&(eve->ITRyin));
  rootTree->SetBranchAddress("ITRzin",&(eve->ITRzin));
  rootTree->SetBranchAddress("ITRpxin",&(eve->ITRpxin));
  rootTree->SetBranchAddress("ITRpyin",&(eve->ITRpyin));
  rootTree->SetBranchAddress("ITRpzin",&(eve->ITRpzin));
  rootTree->SetBranchAddress("ITRxout",&(eve->ITRxout));
  rootTree->SetBranchAddress("ITRyout",&(eve->ITRyout));
  rootTree->SetBranchAddress("ITRzout",&(eve->ITRzout));
  rootTree->SetBranchAddress("ITRpxout",&(eve->ITRpxout));
  rootTree->SetBranchAddress("ITRpyout",&(eve->ITRpyout));
  rootTree->SetBranchAddress("ITRpzout",&(eve->ITRpzout));
  rootTree->SetBranchAddress("ITRde",&(eve->ITRde));
  rootTree->SetBranchAddress("ITRal",&(eve->ITRal));
  rootTree->SetBranchAddress("ITRtim",&(eve->ITRtim));
  
  rootTree->SetBranchAddress("MSDn",&(eve->MSDn));
  rootTree->SetBranchAddress("MSDid",&(eve->MSDid));
  rootTree->SetBranchAddress("MSDilay",&(eve->MSDilay));
  rootTree->SetBranchAddress("MSDxin",&(eve->MSDxin));
  rootTree->SetBranchAddress("MSDyin",&(eve->MSDyin));
  rootTree->SetBranchAddress("MSDzin",&(eve->MSDzin));
  rootTree->SetBranchAddress("MSDpxin",&(eve->MSDpxin));
  rootTree->SetBranchAddress("MSDpyin",&(eve->MSDpyin));
  rootTree->SetBranchAddress("MSDpzin",&(eve->MSDpzin));
  rootTree->SetBranchAddress("MSDxout",&(eve->MSDxout));
  rootTree->SetBranchAddress("MSDyout",&(eve->MSDyout));
  rootTree->SetBranchAddress("MSDzout",&(eve->MSDzout));
  rootTree->SetBranchAddress("MSDpxout",&(eve->MSDpxout));
  rootTree->SetBranchAddress("MSDpyout",&(eve->MSDpyout));
  rootTree->SetBranchAddress("MSDpzout",&(eve->MSDpzout));
  rootTree->SetBranchAddress("MSDde",&(eve->MSDde));
  rootTree->SetBranchAddress("MSDal",&(eve->MSDal));
  rootTree->SetBranchAddress("MSDtim",&(eve->MSDtim));
  
  rootTree->SetBranchAddress("SCNn",&(eve->SCNn));
  rootTree->SetBranchAddress("SCNid",&(eve->SCNid));
  rootTree->SetBranchAddress("SCNibar",&(eve->SCNibar));
  rootTree->SetBranchAddress("SCNiview",&(eve->SCNiview));
  rootTree->SetBranchAddress("SCNxin",&(eve->SCNxin));
  rootTree->SetBranchAddress("SCNyin",&(eve->SCNyin));
  rootTree->SetBranchAddress("SCNzin",&(eve->SCNzin));
  rootTree->SetBranchAddress("SCNpxin",&(eve->SCNpxin));
  rootTree->SetBranchAddress("SCNpyin",&(eve->SCNpyin));
  rootTree->SetBranchAddress("SCNpzin",&(eve->SCNpzin));
  rootTree->SetBranchAddress("SCNxout",&(eve->SCNxout));
  rootTree->SetBranchAddress("SCNyout",&(eve->SCNyout));
  rootTree->SetBranchAddress("SCNzout",&(eve->SCNzout));
  rootTree->SetBranchAddress("SCNpxout",&(eve->SCNpxout));
  rootTree->SetBranchAddress("SCNpyout",&(eve->SCNpyout));
  rootTree->SetBranchAddress("SCNpzout",&(eve->SCNpzout));
  rootTree->SetBranchAddress("SCNde",&(eve->SCNde));
  rootTree->SetBranchAddress("SCNal",&(eve->SCNal));
  rootTree->SetBranchAddress("SCNtim",&(eve->SCNtim));
  
  rootTree->SetBranchAddress("CALn",&(eve->CALn));
  rootTree->SetBranchAddress("CALid",&(eve->CALid));
  rootTree->SetBranchAddress("CALicry",&(eve->CALicry));
  rootTree->SetBranchAddress("CALxin",&(eve->CALxin));
  rootTree->SetBranchAddress("CALyin",&(eve->CALyin));
  rootTree->SetBranchAddress("CALzin",&(eve->CALzin));
  rootTree->SetBranchAddress("CALpxin",&(eve->CALpxin));
  rootTree->SetBranchAddress("CALpyin",&(eve->CALpyin));
  rootTree->SetBranchAddress("CALpzin",&(eve->CALpzin));
  rootTree->SetBranchAddress("CALxout",&(eve->CALxout));
  rootTree->SetBranchAddress("CALyout",&(eve->CALyout));
  rootTree->SetBranchAddress("CALzout",&(eve->CALzout));
  rootTree->SetBranchAddress("CALpxout",&(eve->CALpxout));
  rootTree->SetBranchAddress("CALpyout",&(eve->CALpyout));
  rootTree->SetBranchAddress("CALpzout",&(eve->CALpzout));
  rootTree->SetBranchAddress("CALde",&(eve->CALde));
  rootTree->SetBranchAddress("CALal",&(eve->CALal));
  rootTree->SetBranchAddress("CALtim",&(eve->CALtim));
  
  rootTree->SetBranchAddress("CROSSn",&(eve->CROSSn));
  rootTree->SetBranchAddress("CROSSid",&(eve->CROSSid));
  rootTree->SetBranchAddress("CROSSnreg",&(eve->CROSSnreg));
  rootTree->SetBranchAddress("CROSSnregold",&(eve->CROSSnregold));
  rootTree->SetBranchAddress("CROSSx",&(eve->CROSSx));
  rootTree->SetBranchAddress("CROSSy",&(eve->CROSSy));
  rootTree->SetBranchAddress("CROSSz",&(eve->CROSSz));
  rootTree->SetBranchAddress("CROSSpx",&(eve->CROSSpx));
  rootTree->SetBranchAddress("CROSSpy",&(eve->CROSSpy));
  rootTree->SetBranchAddress("CROSSpz",&(eve->CROSSpz));
  rootTree->SetBranchAddress("CROSSm",&(eve->CROSSm));
  rootTree->SetBranchAddress("CROSSch",&(eve->CROSSch));
  rootTree->SetBranchAddress("CROSSt",&(eve->CROSSt));
}

//------------------------------------------+-----------------------------------
//! Create branches
void TAMCflukaParser::CreateBranches(TTree* rootTree, EVENT_STRUCT* eve, Bool_t reg)
{
   rootTree->Branch("EventNumber",&eve->EventNumber,"EventNumber/I");
   rootTree->Branch("TRn",&eve->TRn,"TRn/I");
   rootTree->Branch("TRpaid",&eve->TRpaid,"TRpaid[TRn]/I");
   rootTree->Branch("TRgen",&eve->TRgen,"TRgen[TRn]/I");
   rootTree->Branch("TRcha",&eve->TRcha,"TRcha[TRn]/I");
   rootTree->Branch("TRreg",&eve->TRreg,"TRreg[TRn]/I");
   rootTree->Branch("TRbar",&eve->TRbar,"TRbar[TRn]/I");
   rootTree->Branch("TRdead",&eve->TRdead,"TRdead[TRn]/I");
   rootTree->Branch("TRfid",&eve->TRfid,"TRfid[TRn]/I");
   rootTree->Branch("TRix",&eve->TRix,"TRix[TRn]/D");
   rootTree->Branch("TRiy",&eve->TRiy,"TRiy[TRn]/D");
   rootTree->Branch("TRiz",&eve->TRiz,"TRiz[TRn]/D");
   rootTree->Branch("TRfx",&eve->TRfx,"TRfx[TRn]/D");
   rootTree->Branch("TRfy",&eve->TRfy,"TRfy[TRn]/D");
   rootTree->Branch("TRfz",&eve->TRfz,"TRfz[TRn]/D");
   rootTree->Branch("TRipx",&eve->TRipx,"TRipx[TRn]/D");
   rootTree->Branch("TRipy",&eve->TRipy,"TRipy[TRn]/D");
   rootTree->Branch("TRipz",&eve->TRipz,"TRipz[TRn]/D");
   rootTree->Branch("TRfpx",&eve->TRfpx,"TRfpx[TRn]/D");
   rootTree->Branch("TRfpy",&eve->TRfpy,"TRfpy[TRn]/D");
   rootTree->Branch("TRfpz",&eve->TRfpz,"TRfpz[TRn]/D");
   rootTree->Branch("TRmass",&eve->TRmass,"TRmass[TRn]/D");
   rootTree->Branch("TRtime",&eve->TRtime,"TRtime[TRn]/D");
   rootTree->Branch("TRtof",&eve->TRtof,"TRtof[TRn]/D");
   rootTree->Branch("TRtrlen",&eve->TRtrlen,"TRtrlen[TRn]/D");
   
   rootTree->Branch("STCn",&eve->STCn,"STCn/I");
   rootTree->Branch("STCid",&eve->STCid,"STCid[STCn]/I");
   rootTree->Branch("STCxin",&eve->STCxin,"STCxin[STCn]/D");
   rootTree->Branch("STCyin",&eve->STCyin,"STCyin[STCn]/D");
   rootTree->Branch("STCzin",&eve->STCzin,"STCzin[STCn]/D");
   rootTree->Branch("STCpxin",&eve->STCpxin,"STCpxin[STCn]/D");
   rootTree->Branch("STCpyin",&eve->STCpyin,"STCpyin[STCn]/D");
   rootTree->Branch("STCpzin",&eve->STCpzin,"STCpzin[STCn]/D");
   rootTree->Branch("STCxout",&eve->STCxout,"STCxout[STCn]/D");
   rootTree->Branch("STCyout",&eve->STCyout,"STCyout[STCn]/D");
   rootTree->Branch("STCzout",&eve->STCzout,"STCzout[STCn]/D");
   rootTree->Branch("STCpxout",&eve->STCpxout,"STCpxout[STCn]/D");
   rootTree->Branch("STCpyout",&eve->STCpyout,"STCpyout[STCn]/D");
   rootTree->Branch("STCpzout",&eve->STCpzout,"STCpzout[STCn]/D");
   rootTree->Branch("STCde",&eve->STCde,"STCde[STCn]/D");
   rootTree->Branch("STCal",&eve->STCal,"STCal[STCn]/D");
   rootTree->Branch("STCtim",&eve->STCtim,"STCtim[STCn]/D");
   
   rootTree->Branch("BMNn",&eve->BMNn,"BMNn/I");
   rootTree->Branch("BMNid",&eve->BMNid,"BMNid[BMNn]/I");
   rootTree->Branch("BMNilay",&eve->BMNilay,"BMNilay[BMNn]/I");
   rootTree->Branch("BMNiview",&eve->BMNiview,"BMNiview[BMNn]/I");
   rootTree->Branch("BMNicell",&eve->BMNicell,"BMNicell[BMNn]/I");
   rootTree->Branch("BMNxin",&eve->BMNxin,"BMNxin[BMNn]/D");
   rootTree->Branch("BMNyin",&eve->BMNyin,"BMNyin[BMNn]/D");
   rootTree->Branch("BMNzin",&eve->BMNzin,"BMNzin[BMNn]/D");
   rootTree->Branch("BMNpxin",&eve->BMNpxin,"BMNpxin[BMNn]/D");
   rootTree->Branch("BMNpyin",&eve->BMNpyin,"BMNpyin[BMNn]/D");
   rootTree->Branch("BMNpzin",&eve->BMNpzin,"BMNpzin[BMNn]/D");
   rootTree->Branch("BMNxout",&eve->BMNxout,"BMNxout[BMNn]/D");
   rootTree->Branch("BMNyout",&eve->BMNyout,"BMNyout[BMNn]/D");
   rootTree->Branch("BMNzout",&eve->BMNzout,"BMNzout[BMNn]/D");
   rootTree->Branch("BMNpxout",&eve->BMNpxout,"BMNpxout[BMNn]/D");
   rootTree->Branch("BMNpyout",&eve->BMNpyout,"BMNpyout[BMNn]/D");
   rootTree->Branch("BMNpzout",&eve->BMNpzout,"BMNpzout[BMNn]/D");
   rootTree->Branch("BMNde",&eve->BMNde,"BMNde[BMNn]/D");
   rootTree->Branch("BMNal",&eve->BMNal,"BMNal[BMNn]/D");
   rootTree->Branch("BMNtim",&eve->BMNtim,"BMNtim[BMNn]/D");
   
   rootTree->Branch("VTXn",&eve->VTXn,"VTXn/I");
   rootTree->Branch("VTXid",&eve->VTXid,"VTXid[VTXn]/I");
   rootTree->Branch("VTXilay",&eve->VTXilay,"VTXilay[VTXn]/I");
   rootTree->Branch("VTXxin",&eve->VTXxin,"VTXxin[VTXn]/D");
   rootTree->Branch("VTXyin",&eve->VTXyin,"VTXyin[VTXn]/D");
   rootTree->Branch("VTXzin",&eve->VTXzin,"VTXzin[VTXn]/D");
   rootTree->Branch("VTXpxin",&eve->VTXpxin,"VTXpxin[VTXn]/D");
   rootTree->Branch("VTXpyin",&eve->VTXpyin,"VTXpyin[VTXn]/D");
   rootTree->Branch("VTXpzin",&eve->VTXpzin,"VTXpzin[VTXn]/D");
   rootTree->Branch("VTXxout",&eve->VTXxout,"VTXxout[VTXn]/D");
   rootTree->Branch("VTXyout",&eve->VTXyout,"VTXyout[VTXn]/D");
   rootTree->Branch("VTXzout",&eve->VTXzout,"VTXzout[VTXn]/D");
   rootTree->Branch("VTXpxout",&eve->VTXpxout,"VTXpxout[VTXn]/D");
   rootTree->Branch("VTXpyout",&eve->VTXpyout,"VTXpyout[VTXn]/D");
   rootTree->Branch("VTXpzout",&eve->VTXpzout,"VTXpzout[VTXn]/D");
   rootTree->Branch("VTXde",&eve->VTXde,"VTXde[VTXn]/D");
   rootTree->Branch("VTXal",&eve->VTXal,"VTXal[VTXn]/D");
   rootTree->Branch("VTXtim",&eve->VTXtim,"VTXVTXtim[VTXn]/D");
   
   rootTree->Branch("ITRn",&eve->ITRn,"ITRn/I");
   rootTree->Branch("ITRid",&eve->ITRid,"ITRid[ITRn]/I");
   rootTree->Branch("ITRisens",&eve->ITRisens,"ITRisens[ITRn]/I");
   rootTree->Branch("ITRxin",&eve->ITRxin,"ITRxin[ITRn]/D");
   rootTree->Branch("ITRyin",&eve->ITRyin,"ITRyin[ITRn]/D");
   rootTree->Branch("ITRzin",&eve->ITRzin,"ITRzin[ITRn]/D");
   rootTree->Branch("ITRpxin",&eve->ITRpxin,"ITRpxin[ITRn]/D");
   rootTree->Branch("ITRpyin",&eve->ITRpyin,"ITRpyin[ITRn]/D");
   rootTree->Branch("ITRpzin",&eve->ITRpzin,"ITRpzin[ITRn]/D");
   rootTree->Branch("ITRxout",&eve->ITRxout,"ITRxout[ITRn]/D");
   rootTree->Branch("ITRyout",&eve->ITRyout,"ITRyout[ITRn]/D");
   rootTree->Branch("ITRzout",&eve->ITRzout,"ITRzout[ITRn]/D");
   rootTree->Branch("ITRpxout",&eve->ITRpxout,"ITRpxout[ITRn]/D");
   rootTree->Branch("ITRpyout",&eve->ITRpyout,"ITRpyout[ITRn]/D");
   rootTree->Branch("ITRpzout",&eve->ITRpzout,"ITRpzout[ITRn]/D");
   rootTree->Branch("ITRde",&eve->ITRde,"ITRde[ITRn]/D");
   rootTree->Branch("ITRal",&eve->ITRal,"ITRal[ITRn]/D");
   rootTree->Branch("ITRtim",&eve->ITRtim,"ITRITRtim[ITRn]/D");
   
   rootTree->Branch("MSDn",&eve->MSDn,"MSDn/I");
   rootTree->Branch("MSDid",&eve->MSDid,"MSDid[MSDn]/I");
   rootTree->Branch("MSDilay",&eve->MSDilay,"MSDilay[MSDn]/I");
   rootTree->Branch("MSDxin",&eve->MSDxin,"MSDxin[MSDn]/D");
   rootTree->Branch("MSDyin",&eve->MSDyin,"MSDyin[MSDn]/D");
   rootTree->Branch("MSDzin",&eve->MSDzin,"MSDzin[MSDn]/D");
   rootTree->Branch("MSDpxin",&eve->MSDpxin,"MSDpxin[MSDn]/D");
   rootTree->Branch("MSDpyin",&eve->MSDpyin,"MSDpyin[MSDn]/D");
   rootTree->Branch("MSDpzin",&eve->MSDpzin,"MSDpzin[MSDn]/D");
   rootTree->Branch("MSDxout",&eve->MSDxout,"MSDxout[MSDn]/D");
   rootTree->Branch("MSDyout",&eve->MSDyout,"MSDyout[MSDn]/D");
   rootTree->Branch("MSDzout",&eve->MSDzout,"MSDzout[MSDn]/D");
   rootTree->Branch("MSDpxout",&eve->MSDpxout,"MSDpxout[MSDn]/D");
   rootTree->Branch("MSDpyout",&eve->MSDpyout,"MSDpyout[MSDn]/D");
   rootTree->Branch("MSDpzout",&eve->MSDpzout,"MSDpzout[MSDn]/D");
   rootTree->Branch("MSDde",&eve->MSDde,"MSDde[MSDn]/D");
   rootTree->Branch("MSDal",&eve->MSDal,"MSDal[MSDn]/D");
   rootTree->Branch("MSDtim",&eve->MSDtim,"MSDtim[MSDn]/D");
   
   rootTree->Branch("SCNn",&eve->SCNn,"SCNn/I");
   rootTree->Branch("SCNid",&eve->SCNid,"SCNid[SCNn]/I");
   rootTree->Branch("SCNibar",&eve->SCNibar,"SCNibar[SCNn]/I");
   rootTree->Branch("SCNiview",&eve->SCNiview,"SCNiview[SCNn]/I");
   rootTree->Branch("SCNxin",&eve->SCNxin,"SCNxin[SCNn]/D");
   rootTree->Branch("SCNyin",&eve->SCNyin,"SCNyin[SCNn]/D");
   rootTree->Branch("SCNzin",&eve->SCNzin,"SCNzin[SCNn]/D");
   rootTree->Branch("SCNpxin",&eve->SCNpxin,"SCNpxin[SCNn]/D");
   rootTree->Branch("SCNpyin",&eve->SCNpyin,"SCNpyin[SCNn]/D");
   rootTree->Branch("SCNpzin",&eve->SCNpzin,"SCNpzin[SCNn]/D");
   rootTree->Branch("SCNxout",&eve->SCNxout,"SCNxout[SCNn]/D");
   rootTree->Branch("SCNyout",&eve->SCNyout,"SCNyout[SCNn]/D");
   rootTree->Branch("SCNzout",&eve->SCNzout,"SCNzout[SCNn]/D");
   rootTree->Branch("SCNpxout",&eve->SCNpxout,"SCNpxout[SCNn]/D");
   rootTree->Branch("SCNpyout",&eve->SCNpyout,"SCNpyout[SCNn]/D");
   rootTree->Branch("SCNpzout",&eve->SCNpzout,"SCNpzout[SCNn]/D");
   rootTree->Branch("SCNde",&eve->SCNde,"SCNde[SCNn]/D");
   rootTree->Branch("SCNal",&eve->SCNal,"SCNal[SCNn]/D");
   rootTree->Branch("SCNtim",&eve->SCNtim,"SCNtim[SCNn]/D");
   
   rootTree->Branch("CALn",&eve->CALn,"CALn/I");
   rootTree->Branch("CALid",&eve->CALid,"CALid[CALn]/I");
   rootTree->Branch("CALicry",&eve->CALicry,"CALicry[CALn]/I");
   rootTree->Branch("CALxin",&eve->CALxin,"CALxin[CALn]/D");
   rootTree->Branch("CALyin",&eve->CALyin,"CALyin[CALn]/D");
   rootTree->Branch("CALzin",&eve->CALzin,"CALzin[CALn]/D");
   rootTree->Branch("CALpxin",&eve->CALpxin,"CALpxin[CALn]/D");
   rootTree->Branch("CALpyin",&eve->CALpyin,"CALpyin[CALn]/D");
   rootTree->Branch("CALpzin",&eve->CALpzin,"CALpzin[CALn]/D");
   rootTree->Branch("CALxout",&eve->CALxout,"CALxout[CALn]/D");
   rootTree->Branch("CALyout",&eve->CALyout,"CALyout[CALn]/D");
   rootTree->Branch("CALzout",&eve->CALzout,"CALzout[CALn]/D");
   rootTree->Branch("CALpxout",&eve->CALpxout,"CALpxout[CALn]/D");
   rootTree->Branch("CALpyout",&eve->CALpyout,"CALpyout[CALn]/D");
   rootTree->Branch("CALpzout",&eve->CALpzout,"CALpzout[CALn]/D");
   rootTree->Branch("CALde",&eve->CALde,"CALde[CALn]/D");
   rootTree->Branch("CALal",&eve->CALal,"CALal[CALn]/D");
   rootTree->Branch("CALtim",&eve->CALtim,"CALtim[CALn]/D");
   
   if (reg) {
      rootTree->Branch("CROSSn",&eve->CROSSn,"CROSSn/I");
      rootTree->Branch("CROSSid",&eve->CROSSid,"CROSSid[CROSSn]/I");
      rootTree->Branch("CROSSnreg",&eve->CROSSnreg,"CROSSnreg[CROSSn]/I");
      rootTree->Branch("CROSSnregold",&eve->CROSSnregold,"CROSSnregold[CROSSn]/I");
      rootTree->Branch("CROSSx",&eve->CROSSx,"CROSSx[CROSSn]/D");
      rootTree->Branch("CROSSy",&eve->CROSSy,"CROSSy[CROSSn]/D");
      rootTree->Branch("CROSSz",&eve->CROSSz,"CROSSz[CROSSn]/D");
      rootTree->Branch("CROSSpx",&eve->CROSSpx,"CROSSpx[CROSSn]/D");
      rootTree->Branch("CROSSpy",&eve->CROSSpy,"CROSSpy[CROSSn]/D");
      rootTree->Branch("CROSSpz",&eve->CROSSpz,"CROSSpz[CROSSn]/D");
      rootTree->Branch("CROSSm",&eve->CROSSm,"CROSSm[CROSSn]/D");
      rootTree->Branch("CROSSch",&eve->CROSSch,"CROSSch[CROSSn]/D");
      rootTree->Branch("CROSSt",&eve->CROSSt,"CROSSt[CROSSn]/D");
   }
}

//------------------------------------------+-----------------------------------
//! Reset Event
Bool_t TAMCflukaParser::ResetEvent(EVENT_STRUCT* eve)
{
   for(int kk=0;kk<eve->TRn;kk++){
      eve->TRpaid[kk]  = 0;
      eve->TRgen[kk]   = 0;
      eve->TRcha[kk]   = -100;
      eve->TRreg[kk]   = 0;
      eve->TRbar[kk]   = 0;
      eve->TRdead[kk]  = 0;
      eve->TRfid[kk]   = -100;
      eve->TRix[kk]    = 0.;
      eve->TRiy[kk]    = 0.;
      eve->TRiz[kk]    = 0.;
      eve->TRfx[kk]    = 0.;
      eve->TRfy[kk]    = 0.;
      eve->TRfz[kk]    = 0.;
      eve->TRipx[kk]   = 0.;
      eve->TRipy[kk]   = 0.;
      eve->TRipz[kk]   = 0.;
      eve->TRfpx[kk]   = 0.;
      eve->TRfpy[kk]   = 0.;
      eve->TRfpz[kk]   = 0.;
      eve->TRmass[kk]  = 0.;
      eve->TRtime[kk]  = 0.;
      eve->TRtof[kk]   = 0.;
      eve->TRtrlen[kk] = 0.;
   }
   
   for(int kk=0;kk<eve->STCn;kk++){
      eve->STCid[kk]    = 0;
      eve->STCxin[kk]   = 0.;
      eve->STCyin[kk]   = 0.;
      eve->STCzin[kk]   = 0.;
      eve->STCxout[kk]  = 0.;
      eve->STCyout[kk]  = 0.;
      eve->STCzout[kk]  = 0.;
      eve->STCpxin[kk]  = 0.;
      eve->STCpyin[kk]  = 0.;
      eve->STCpzin[kk]  = 0.;
      eve->STCpxout[kk] = 0.;
      eve->STCpyout[kk] = 0.;
      eve->STCpzout[kk] = 0.;
      eve->STCde[kk]    = 0.;
      eve->STCal[kk]    = 0.;
      eve->STCtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->BMNn;kk++){
      eve->BMNid[kk]    = 0;
      eve->BMNilay[kk]  = 0;
      eve->BMNiview[kk] = 0;
      eve->BMNicell[kk] = 0;
      eve->BMNxin[kk]   = 0.;
      eve->BMNyin[kk]   = 0.;
      eve->BMNzin[kk]   = 0.;
      eve->BMNxout[kk]  = 0.;
      eve->BMNyout[kk]  = 0.;
      eve->BMNzout[kk]  = 0.;
      eve->BMNpxin[kk]  = 0.;
      eve->BMNpyin[kk]  = 0.;
      eve->BMNpzin[kk]  = 0.;
      eve->BMNpxout[kk] = 0.;
      eve->BMNpyout[kk] = 0.;
      eve->BMNpzout[kk] = 0.;
      eve->BMNde[kk]    = 0.;
      eve->BMNal[kk]    = 0.;
      eve->BMNtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->VTXn;kk++){
      eve->VTXid[kk]    = 0;
      eve->VTXilay[kk]  = 0;
      eve->VTXxin[kk]   = 0.;
      eve->VTXyin[kk]   = 0.;
      eve->VTXzin[kk]   = 0.;
      eve->VTXxout[kk]  = 0.;
      eve->VTXyout[kk]  = 0.;
      eve->VTXzout[kk]  = 0.;
      eve->VTXpxin[kk]  = 0.;
      eve->VTXpyin[kk]  = 0.;
      eve->VTXpzin[kk]  = 0.;
      eve->VTXpxout[kk] = 0.;
      eve->VTXpyout[kk] = 0.;
      eve->VTXpzout[kk] = 0.;
      eve->VTXde[kk]    = 0.;
      eve->VTXal[kk]    = 0.;
      eve->VTXtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->ITRn;kk++){
      eve->ITRid[kk]    = 0;
      eve->ITRisens[kk]  = 0;
      eve->ITRxin[kk]   = 0.;
      eve->ITRyin[kk]   = 0.;
      eve->ITRzin[kk]   = 0.;
      eve->ITRxout[kk]  = 0.;
      eve->ITRyout[kk]  = 0.;
      eve->ITRzout[kk]  = 0.;
      eve->ITRpxin[kk]  = 0.;
      eve->ITRpyin[kk]  = 0.;
      eve->ITRpzin[kk]  = 0.;
      eve->ITRpxout[kk] = 0.;
      eve->ITRpyout[kk] = 0.;
      eve->ITRpzout[kk] = 0.;
      eve->ITRde[kk]    = 0.;
      eve->ITRal[kk]    = 0.;
      eve->ITRtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->MSDn;kk++){
      eve->MSDid[kk]    = 0;
      eve->MSDilay[kk]  = 0;
      eve->MSDxin[kk]   = 0.;
      eve->MSDyin[kk]   = 0.;
      eve->MSDzin[kk]   = 0.;
      eve->MSDxout[kk]  = 0.;
      eve->MSDyout[kk]  = 0.;
      eve->MSDzout[kk]  = 0.;
      eve->MSDpxin[kk]  = 0.;
      eve->MSDpyin[kk]  = 0.;
      eve->MSDpzin[kk]  = 0.;
      eve->MSDpxout[kk] = 0.;
      eve->MSDpyout[kk] = 0.;
      eve->MSDpzout[kk] = 0.;
      eve->MSDde[kk]    = 0.;
      eve->MSDal[kk]    = 0.;
      eve->MSDtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->SCNn;kk++){
      eve->SCNid[kk]     = 0;
      eve->SCNibar[kk] = 0;
      eve->SCNiview[kk]  = 0;
      eve->SCNxin[kk]    = 0.;
      eve->SCNyin[kk]    = 0.;
      eve->SCNzin[kk]    = 0.;
      eve->SCNxout[kk]   = 0.;
      eve->SCNyout[kk]   = 0.;
      eve->SCNzout[kk]   = 0.;
      eve->SCNpxin[kk]   = 0.;
      eve->SCNpyin[kk]   = 0.;
      eve->SCNpzin[kk]   = 0.;
      eve->SCNpxout[kk]  = 0.;
      eve->SCNpyout[kk]  = 0.;
      eve->SCNpzout[kk]  = 0.;
      eve->SCNde[kk]     = 0.;
      eve->SCNal[kk]     = 0.;
      eve->SCNtim[kk]    = 0.;
   }
   
   for(int kk=0;kk<eve->CALn;kk++){
      eve->CALid[kk]    = 0;
      eve->CALicry[kk]  = 0;
      eve->CALxin[kk]   = 0.;
      eve->CALyin[kk]   = 0.;
      eve->CALzin[kk]   = 0.;
      eve->CALxout[kk]  = 0.;
      eve->CALyout[kk]  = 0.;
      eve->CALzout[kk]  = 0.;
      eve->CALpxin[kk]  = 0.;
      eve->CALpyin[kk]  = 0.;
      eve->CALpzin[kk]  = 0.;
      eve->CALpxout[kk] = 0.;
      eve->CALpyout[kk] = 0.;
      eve->CALpzout[kk] = 0.;
      eve->CALde[kk]    = 0.;
      eve->CALal[kk]    = 0.;
      eve->CALtim[kk]   = 0.;
   }
   
   for(int kk=0;kk<eve->CROSSn;kk++){
      eve->CROSSid[kk]      = 0;
      eve->CROSSnreg[kk]    = 0;
      eve->CROSSnregold[kk] = 0;
      eve->CROSSx[kk]       = 0.;
      eve->CROSSy[kk]       = 0.;
      eve->CROSSz[kk]       = 0.;
      eve->CROSSpx[kk]      = 0.;
      eve->CROSSpy[kk]      = 0.;
      eve->CROSSpz[kk]      = 0.;
      eve->CROSSm[kk]       = 0.;
      eve->CROSSch[kk]      = 0.;
      eve->CROSSt[kk]       = 0.;
   }
   
   eve->EventNumber = 0;
   eve->TRn       = 0;
   eve->STCn      = 0;
   eve->VTXn      = 0;
   eve->ITRn      = 0;
   eve->BMNn      = 0;
   eve->MSDn      = 0;
   eve->SCNn      = 0;
   eve->CALn      = 0;
   eve->CROSSn    = 0;
   
   return 0;
}
