/*!
 \file
 \version $Id: TAMCflukaParser.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCflukaParser.
 */

#include "TAMCntuHit.hxx"
#include "TAMCflukaParser.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCflukaParser TAMCflukaParser.hxx "TAMCflukaParser.hxx"
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
TAMCntuHit* TAMCflukaParser::GetTofHits(EVENT_STRUCT* evStr, TAGdataDsc* p_ntuhit)
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
//! CAL hits
TAMCntuEve* TAMCflukaParser::GetTracks(EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrack)
{
  TAMCntuEve* p_nturaw = (TAMCntuEve*) p_ntutrack->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < evStr->TRn; i++) {
    
    Double_t i_mass = evStr->TRmass[i];
    Int_t i_id = evStr->TRfid[i];
    Int_t i_mid = evStr->TRpaid[i];  //mother id
    Int_t i_chg = evStr->TRcha[i];
    Int_t i_reg = evStr->TRreg[i];
    Int_t i_bar = evStr->TRbar[i];
    Int_t i_dead = evStr->TRdead[i];
    Int_t i_moth = i_mid-1;
    Int_t i_type = -999;//useless for now
    Double_t i_time = evStr->TRtime[i];
    Double_t i_tof = evStr->TRtof[i];
    Double_t i_trlen = evStr->TRtrlen[i];
    
    TVector3 ipos = TVector3(evStr->TRix[i],evStr->TRiy[i],evStr->TRiz[i]);
    TVector3 fpos = TVector3(evStr->TRfx[i],evStr->TRfy[i],evStr->TRfz[i]);
    TVector3 ip = TVector3(evStr->TRipx[i],evStr->TRipy[i],evStr->TRipz[i]);
    TVector3 fp = TVector3(evStr->TRfpx[i],evStr->TRfpy[i],evStr->TRfpz[i]);
    //decide if propagate also final momentum
    
    TVector3 mothip = TVector3 (0,0,0);
    if(i_moth>=0) mothip = TVector3(evStr->TRipx[i_moth],evStr->TRipy[i_moth],evStr->TRipz[i_moth]);
    TVector3 mothfp = TVector3 (0,0,0);
    if(i_moth>=0) mothfp = TVector3(evStr->TRfpx[i_moth],evStr->TRfpy[i_moth],evStr->TRfpz[i_moth]);
    
    Int_t i_pileup = 0; // VM added 17/11/13 for pileup
    
    p_nturaw->NewTrack(i_id,i_chg,i_type,i_reg,i_bar,i_dead,i_mass,i_moth,i_time,i_tof,i_trlen,ipos,fpos,ip,fp,mothip,mothfp,i_pileup);
  }
  
  return p_nturaw;
}
