#include <stdio.h>
#include <iostream>
#include <fstream>
#include "TAMCevent.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"
#include "TAMCntuRegion.hxx"

#include "GlobalPar.hxx"

using namespace std;

ClassImp(TAMCevent);

/*-----------------------------------------------------------------*/
TAMCevent::TAMCevent(Bool_t regionFlag)
 : fRegionFlag(regionFlag),
   fEventNumber(-1),
   fTrack(new TAMCntuEve()),
   fRegion(0x0),
   fHitSTC(0x0),
   fHitBMN(0x0),
   fHitVTX(0x0),
   fHitITR(0x0),
   fHitMSD(0x0),
   fHitTW(0x0),
   fHitCAL(0x0)
{
   if (fRegionFlag)
      fRegion = new TAMCntuRegion();

    if (GlobalPar::GetPar()->IncludeST())
      fHitSTC = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeBM())
      fHitBMN = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeVT())
       fHitVTX = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeIT())
       fHitITR = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeMSD())
       fHitMSD = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeTW())
       fHitTW  = new TAMCntuHit();
   
    if (GlobalPar::GetPar()->IncludeCA())
       fHitCAL = new TAMCntuHit();
}

/*-----------------------------------------------------------------*/
Int_t TAMCevent::Clean()
{
    fTrack->Clear();
   if (fRegionFlag)
      fRegion->Clear();

    if (fHitSTC)
       fHitSTC->Clear();
   
    if (fHitBMN)
       fHitBMN->Clear();
   
    if (fHitVTX)
       fHitVTX->Clear();
   
    if (fHitITR)
       fHitITR->Clear();
   
    if (fHitMSD)
       fHitMSD->Clear();
   
    if (fHitTW)
       fHitTW->Clear();
   
    if (fHitCAL)
       fHitCAL->Clear();

    return 0;
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddPart(Int_t aTRpaid, Int_t aTRgen, Int_t aTRcharge, Int_t aTRreg,Int_t aTRbaryon, Int_t aTRdead, Int_t aTRflukid, TVector3 aTRipos,TVector3 aTRfpos, TVector3 aTRip, TVector3 aTRfp, Double_t aTRmass, Double_t aTRtof, Double_t aTRtime, Double_t aTRtrlen)
{
    fTrack->NewTrack(aTRflukid,aTRcharge,aTRgen,aTRreg,aTRbaryon,aTRdead,aTRmass,aTRpaid-1,aTRtime,aTRtof,aTRtrlen,
                     aTRipos,aTRfpos,aTRip,aTRfp,TVector3(0,0,0),TVector3(0,0,0));
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddSTC(Int_t aSTCid,
                        TVector3 aSTCinpos, TVector3 aSTCoutpos,
                        TVector3 aSTCpin, TVector3 aSTCpout,
                        Double_t aSTCde, Double_t aSTCal, Double_t aSTCtof, Int_t atrId)
{
   if (fHitSTC)
      fHitSTC->NewHit(aSTCid,0,-99,-99,aSTCinpos,aSTCoutpos,aSTCpin,aSTCpout,aSTCde,aSTCtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddBMN(Int_t aBMNid, Int_t aBMNilay, Int_t aBMNiview,
                        Int_t aBMNicell, TVector3 aBMNinpos, TVector3 aBMNoutpos,
                        TVector3 aBMNpin, TVector3 aBMNpout, Double_t aBMNde,
                        Double_t aBMNal, Double_t aBMNtof, Int_t atrId)
{
   if (fHitBMN)
      fHitBMN->NewHit(aBMNid,aBMNilay,aBMNiview,aBMNicell,aBMNinpos,aBMNoutpos,aBMNpin,aBMNpout,aBMNde,aBMNtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddVTX(Int_t aVTXid, Int_t aVTXilay,
                        TVector3 aVTXinpos, TVector3 aVTXoutpos, TVector3 aVTXpin,
                        TVector3 aVTXpout, Double_t aVTXde, Double_t aVTXal,
                        Double_t aVTXtof, Int_t atrId)
{
   if (fHitVTX)
      fHitVTX->NewHit(aVTXid,aVTXilay,-99,-99,aVTXinpos,aVTXoutpos,aVTXpin,aVTXpout,aVTXde,aVTXtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddITR(Int_t aITRid, Int_t aITRilay,
                        TVector3 aITRinpos, TVector3 aITRoutpos, TVector3 aITRpin,
                        TVector3 aITRpout, Double_t aITRde, Double_t aITRal,
                        Double_t aITRtof, Int_t atrId)
{
   if (fHitITR)
      fHitITR->NewHit(aITRid,aITRilay,-99,-99,aITRinpos,aITRoutpos,aITRpin,aITRpout,aITRde,aITRtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddMSD(Int_t aMSDid, Int_t aMSDilay, TVector3 aMSDinpos, TVector3 aMSDoutpos,
                        TVector3 aMSDpin, TVector3 aMSDpout,
                        Double_t aMSDde, Double_t aMSDal, Double_t aMSDtof, Int_t atrId)
{
   if (fHitMSD)
      fHitMSD->NewHit(aMSDid,aMSDilay,-99,-99,aMSDinpos,aMSDoutpos,aMSDpin,aMSDpout,aMSDde,aMSDtof,atrId);
}

/*-----------------------------------------------------------------*/

void TAMCevent::AddTW(Int_t aTWid, Int_t aTWibar, Int_t aTWiview,
                       TVector3 aTWinpos, TVector3 aTWoutpos, TVector3 aTWpin,
                       TVector3 aTWpout, Double_t aTWde, Double_t aTWal, Double_t aTWtof, Int_t atrId)
{
   if (fHitTW)
      fHitTW->NewHit(aTWid,aTWibar,aTWiview,-99,aTWinpos,aTWoutpos,aTWpin,aTWpout,aTWde,aTWtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddCAL(Int_t aCALid, Int_t aCALicry,
                        TVector3 aCALinpos, TVector3 aCALoutpos, TVector3 aCALpin,
                        TVector3 aCALpout, Double_t aCALde, Double_t aCALal,
                        Double_t aCALtof, Int_t atrId)
{
   if (fHitCAL)
      fHitCAL->NewHit(aCALid,aCALicry,-99,-99,aCALinpos,aCALoutpos,aCALpin,aCALpout,aCALde,aCALtof,atrId);
}

/*-----------------------------------------------------------------*/
void TAMCevent::AddCROSS(Int_t aCROSSid, Int_t aCROSSnreg, Int_t aCROSSnregold,
                          TVector3 aCROSSpos,TVector3 aCROSSp, Double_t aCROSSm,
                          Double_t aCROSSch, Double_t aCROSSt)
{
   if (fRegionFlag)
      fRegion->NewRegion(aCROSSid, aCROSSnreg, aCROSSnregold, aCROSSpos, aCROSSp, aCROSSm, aCROSSch, aCROSSt);
}

/*-----------------------------------------------------------------*/
void TAMCevent::SetBranches(TTree *RootTree){

    RootTree->Branch("EventNumber",&fEventNumber,"EventNumber/I");
   
    RootTree->Branch(fTrack->GetBranchName(),&fTrack);
    if (fRegionFlag)
       RootTree->Branch(fRegion->GetBranchName(),&fRegion);

    if (GlobalPar::GetPar()->IncludeST())
       RootTree->Branch(fHitSTC->GetStcBranchName(),&fHitSTC);
   
    if (GlobalPar::GetPar()->IncludeBM())
       RootTree->Branch(fHitBMN->GetBmBranchName(),&fHitBMN);
   
    if (GlobalPar::GetPar()->IncludeVT())
       RootTree->Branch(fHitVTX->GetVtxBranchName(),&fHitVTX);
   
    if (GlobalPar::GetPar()->IncludeIT())
       RootTree->Branch(fHitITR->GetItrBranchName(),&fHitITR);
   
    if (GlobalPar::GetPar()->IncludeMSD())
       RootTree->Branch(fHitMSD->GetMsdBranchName(),&fHitMSD);
   
    if (GlobalPar::GetPar()->IncludeTW())
       RootTree->Branch(fHitTW->GetTofBranchName(),&fHitTW);
   
    if (GlobalPar::GetPar()->IncludeCA())
       RootTree->Branch(fHitCAL->GetCalBranchName(),&fHitCAL);
}

/*-----------------------------------------------------------------*/
void TAMCevent::FindBranches(TTree *RootTree)
{
    RootTree->SetBranchAddress("EventNumber",&fEventNumber);

    RootTree->SetBranchAddress(fTrack->GetBranchName(),&fTrack);

    if (GlobalPar::GetPar()->IncludeST())
       RootTree->SetBranchAddress(fHitSTC->GetStcBranchName(),&fHitSTC);
   
    if (GlobalPar::GetPar()->IncludeBM())
       RootTree->SetBranchAddress(fHitBMN->GetBmBranchName(),&fHitBMN);
   
    if (GlobalPar::GetPar()->IncludeVT())
       RootTree->SetBranchAddress(fHitVTX->GetVtxBranchName(),&fHitVTX);
   
    if (GlobalPar::GetPar()->IncludeIT())
       RootTree->SetBranchAddress(fHitITR->GetItrBranchName(),&fHitITR);
   
    if (GlobalPar::GetPar()->IncludeMSD())
       RootTree->SetBranchAddress(fHitMSD->GetMsdBranchName(),&fHitMSD);
   
    if (GlobalPar::GetPar()->IncludeTW())
       RootTree->SetBranchAddress(fHitTW->GetTofBranchName(),&fHitTW);
   
    if (GlobalPar::GetPar()->IncludeCA())
       RootTree->SetBranchAddress(fHitCAL->GetCalBranchName(),&fHitCAL);
}


/*-----------------------------------------------------------------*/
void TAMCevent::Dump() const
{
    return;
    
}

/*-----------------------------------------------------------------*/
TAMCevent::~TAMCevent()
{
    delete fTrack;
   if (fRegionFlag)
      delete fRegion;

    if (fHitCAL)
       delete fHitSTC;
   
    if (fHitBMN)
       delete fHitBMN;
   
    if (fHitVTX)
       delete fHitVTX;
   
    if (fHitITR)
       delete fHitITR;
   
    if (fHitMSD)
       delete fHitMSD;
   
    if (fHitTW)
       delete fHitTW;
   
    if (fHitCAL)
       delete fHitCAL;
}


