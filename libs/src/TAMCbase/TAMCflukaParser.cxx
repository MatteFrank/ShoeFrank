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
//! Tracks
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
TAMCntuTrack* TAMCflukaParser::GetTracks(EVENT_STRUCT* evStr, TAGdataDsc* p_ntutrack)
{
  TAMCntuTrack* p_nturaw = (TAMCntuTrack*) p_ntutrack->Object();
  
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
