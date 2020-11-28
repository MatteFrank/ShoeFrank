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
 \brief NTuplizer for MC events. **
 */

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCflukaParser::TAMCflukaParser(EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrck , TAGdataDsc* p_ntuhit)
: TAGobject(),
  fpEvtStr(evStr),
  fpNtuMCtrack(p_ntutrck),
  fpNtuMChit(p_ntuhit)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCflukaParser::~TAMCflukaParser()
{
}

//------------------------------------------+-----------------------------------
//! ST hits
TAMCntuHit* TAMCflukaParser::GetStcHits()
{
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->STCn; i++) {
    
    Int_t id       = 0;
    Int_t trackIdx = fpEvtStr->STCid[i];
    
    TVector3 ipos( fpEvtStr->STCxin[i], fpEvtStr->STCyin[i], fpEvtStr->STCzin[i]);
    TVector3 fpos( fpEvtStr->STCxout[i], fpEvtStr->STCyout[i], fpEvtStr->STCzout[i]);
    TVector3 imom( fpEvtStr->STCpxin[i], fpEvtStr->STCpyin[i], fpEvtStr->STCpzin[i]);
    TVector3 fmom( fpEvtStr->STCpxout[i], fpEvtStr->STCpyout[i], fpEvtStr->STCpzout[i]);
    
    p_nturaw->NewHit(trackIdx, id, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->STCde[i], fpEvtStr->STCtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! BM hits
TAMCntuHit* TAMCflukaParser::GetBmHits()
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->BMNn; i++) {
    
    Int_t layer    = fpEvtStr->BMNilay[i];
    Int_t view     = fpEvtStr->BMNicell[i];
    Int_t cellId   = fpEvtStr->BMNicell[i];
    Int_t trackIdx = fpEvtStr->BMNid[i];
    
    TVector3 ipos( fpEvtStr->BMNxin[i], fpEvtStr->BMNyin[i], fpEvtStr->BMNzin[i]);
    TVector3 fpos( fpEvtStr->BMNxout[i], fpEvtStr->BMNyout[i], fpEvtStr->BMNzout[i]);
    TVector3 imom( fpEvtStr->BMNpxin[i], fpEvtStr->BMNpyin[i], fpEvtStr->BMNpzin[i]);
    TVector3 fmom( fpEvtStr->BMNpxout[i],fpEvtStr->BMNpyout[i],fpEvtStr->BMNpzout[i]);
    
    p_nturaw->NewHit(trackIdx, layer, view, cellId, ipos, fpos, imom, fmom, fpEvtStr->BMNde[i], fpEvtStr->BMNtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! VTX hits
TAMCntuHit* TAMCflukaParser::GetVtxHits()
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->VTXn; i++) {
    
    Int_t sensorId = fpEvtStr->VTXilay[i];
    Int_t trackIdx = fpEvtStr->VTXid[i];
    
    TVector3 ipos( fpEvtStr->VTXxin[i], fpEvtStr->VTXyin[i], fpEvtStr->VTXzin[i]);
    TVector3 imom( fpEvtStr->VTXpxin[i], fpEvtStr->VTXpyin[i], fpEvtStr->VTXpzin[i]);
    TVector3 fpos( fpEvtStr->VTXxout[i], fpEvtStr->VTXyout[i], fpEvtStr->VTXzout[i]);
    TVector3 fmom( fpEvtStr->VTXpxout[i], fpEvtStr->VTXpyout[i], fpEvtStr->VTXpzout[i]);
    
    p_nturaw->NewHit(trackIdx, sensorId, -99, -99,ipos, fpos, imom, fmom, fpEvtStr->VTXde[i], fpEvtStr->VTXtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! ITR hits
TAMCntuHit* TAMCflukaParser::GetItrHits()
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->ITRn; i++) {
    
    Int_t sensorId = fpEvtStr->ITRisens[i];
    Int_t trackIdx = fpEvtStr->ITRid[i];
    
    TVector3 ipos( fpEvtStr->ITRxin[i], fpEvtStr->ITRyin[i], fpEvtStr->ITRzin[i]);
    TVector3 fpos( fpEvtStr->ITRxout[i], fpEvtStr->ITRyout[i], fpEvtStr->ITRzout[i]);
    TVector3 imom( fpEvtStr->ITRpxin[i], fpEvtStr->ITRpyin[i], fpEvtStr->ITRpzin[i]);
    TVector3 fmom( fpEvtStr->ITRpxout[i],fpEvtStr->ITRpyout[i],fpEvtStr->ITRpzout[i]);
    
    p_nturaw->NewHit(trackIdx, sensorId, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->ITRde[i], fpEvtStr->ITRtim[i],0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! MSD hits
TAMCntuHit* TAMCflukaParser::GetMsdHits()
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->MSDn; i++) {
    
    Int_t layer    = fpEvtStr->MSDilay[i];
    Int_t trackIdx = fpEvtStr->MSDid[i];
    
    TVector3 ipos( fpEvtStr->MSDxin[i], fpEvtStr->MSDyin[i], fpEvtStr->MSDzin[i]);
    TVector3 fpos( fpEvtStr->MSDxout[i], fpEvtStr->MSDyout[i], fpEvtStr->MSDzout[i]);
    TVector3 imom( fpEvtStr->MSDpxin[i], fpEvtStr->MSDpyin[i], fpEvtStr->MSDpzin[i]);
    TVector3 fmom( fpEvtStr->MSDpxout[i], fpEvtStr->MSDpyout[i], fpEvtStr->MSDpzout[i]);
    
    p_nturaw->NewHit(trackIdx, layer, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->MSDde[i], fpEvtStr->MSDtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! TOF hits
TAMCntuHit* TAMCflukaParser::GetTofHits()
{
  
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->SCNn; i++) {
    
    Int_t trackIdx = fpEvtStr->SCNid[i];
    Int_t barId    = fpEvtStr->SCNibar[i];
    Int_t view     = fpEvtStr->SCNiview[i];
    Float_t edep   = fpEvtStr->SCNde[i]*TAGgeoTrafo::GevToMev();
    Float_t time   = fpEvtStr->SCNtim[i]*TAGgeoTrafo::SecToNs();
    
    TVector3 ipos( fpEvtStr->SCNxin[i], fpEvtStr->SCNyin[i], fpEvtStr->SCNzin[i]);
    TVector3 fpos( fpEvtStr->SCNxout[i], fpEvtStr->SCNyout[i], fpEvtStr->SCNzout[i]);
    TVector3 imom( fpEvtStr->SCNpxin[i], fpEvtStr->SCNpyin[i], fpEvtStr->SCNpzin[i]);
    TVector3 fmom( fpEvtStr->SCNpxout[i], fpEvtStr->SCNpyout[i], fpEvtStr->SCNpzout[i]);
    
    p_nturaw->NewHit(trackIdx, barId, view, -99, ipos, fpos, imom, fmom, edep, time, 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! CAL hits
TAMCntuHit* TAMCflukaParser::GetCalHits()
{
  TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMChit->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->CALn; i++) {
    
    Int_t crystalId = fpEvtStr->CALicry[i];
    Int_t trackIdx  = fpEvtStr->CALid[i];
    
    TVector3 ipos( fpEvtStr->CALxin[i], fpEvtStr->CALyin[i], fpEvtStr->CALzin[i]);
    TVector3 fpos( fpEvtStr->CALxout[i], fpEvtStr->CALyout[i], fpEvtStr->CALzout[i]);
    TVector3 imom( fpEvtStr->CALpxin[i], fpEvtStr->CALpyin[i], fpEvtStr->CALpzin[i]);
    TVector3 fmom( fpEvtStr->CALpxout[i],fpEvtStr->CALpyout[i],fpEvtStr->CALpzout[i]);
    
    p_nturaw->NewHit(trackIdx, crystalId, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->CALde[i], fpEvtStr->CALtim[i], 0);
  }
  
  return p_nturaw;
}

//------------------------------------------+-----------------------------------
//! CAL hits
TAMCntuEve* TAMCflukaParser::GetTracks()
{
  TAMCntuEve* p_nturaw = (TAMCntuEve*) fpNtuMCtrack->Object();
  
  p_nturaw->Clear();
  
  for (Int_t i = 0; i < fpEvtStr->TRn; i++) {
    
    Double_t i_mass = fpEvtStr->TRmass[i];
    Int_t i_id = fpEvtStr->TRfid[i];
    Int_t i_mid = fpEvtStr->TRpaid[i];  //mother id
    Int_t i_chg = fpEvtStr->TRcha[i];
    Int_t i_reg = fpEvtStr->TRreg[i];
    Int_t i_bar = fpEvtStr->TRbar[i];
    Int_t i_dead = fpEvtStr->TRdead[i];
    Int_t i_moth = i_mid-1;
    Int_t i_type = -999;//useless for now
    Double_t i_time = fpEvtStr->TRtime[i];
    Double_t i_tof = fpEvtStr->TRtof[i];
    Double_t i_trlen = fpEvtStr->TRtrlen[i];
    
    TVector3 ipos = TVector3(fpEvtStr->TRix[i],fpEvtStr->TRiy[i],fpEvtStr->TRiz[i]);
    TVector3 fpos = TVector3(fpEvtStr->TRfx[i],fpEvtStr->TRfy[i],fpEvtStr->TRfz[i]);
    TVector3 ip = TVector3(fpEvtStr->TRipx[i],fpEvtStr->TRipy[i],fpEvtStr->TRipz[i]);
    TVector3 fp = TVector3(fpEvtStr->TRfpx[i],fpEvtStr->TRfpy[i],fpEvtStr->TRfpz[i]);
    //decide if propagate also final momentum
    
    TVector3 mothip = TVector3 (0,0,0);
    if(i_moth>=0) mothip = TVector3(fpEvtStr->TRipx[i_moth],fpEvtStr->TRipy[i_moth],fpEvtStr->TRipz[i_moth]);
    TVector3 mothfp = TVector3 (0,0,0);
    if(i_moth>=0) mothfp = TVector3(fpEvtStr->TRfpx[i_moth],fpEvtStr->TRfpy[i_moth],fpEvtStr->TRfpz[i_moth]);
    
    Int_t i_pileup = 0; // VM added 17/11/13 for pileup
    
    p_nturaw->NewTrack(i_id,i_chg,i_type,i_reg,i_bar,i_dead,i_mass,i_moth,i_time,i_tof,i_trlen,ipos,fpos,ip,fp,mothip,mothfp,i_pileup);
  }
  
  return p_nturaw;
}
