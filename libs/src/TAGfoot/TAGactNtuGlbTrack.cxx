/*!
 \file
 \version $Id: TAGactNtuGlbTrack.cxx
 \brief   Implementation of TAGactNtuGlbTrack.
 */

#include "TH1F.h"
#include "TVector3.h"

#include "TAGactTreeReader.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAVTntuVertex.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntupoint.hxx"

#include "TAGactNtuGlbTrack.hxx"


/*!
 \class TAGactNtuGlbTrack TAGactNtuGlbTrack.hxx "TAGactNtuGlbTrack.hxx"
 \brief  Read back detector clusters to reconstruct global tracks**
 */

Bool_t  TAGactNtuGlbTrack::fgStdAloneFlag = false;

ClassImp(TAGactNtuGlbTrack)

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactNtuGlbTrack::TAGactNtuGlbTrack(const char* name, TAGdataDsc* p_vtxvertex, TAGdataDsc* p_itrclus, TAGdataDsc* p_msdclus,
                                     TAGdataDsc* p_twpoint, TAGdataDsc* p_glbtrack, TAGparaDsc* p_geodi)
 : TAGaction(name, "TAGactNtuGlbTrack - Global Tracker"),
   fpVtxVertex(p_vtxvertex),
   fpItrClus(p_itrclus),
   fpMsdClus(p_msdclus),
   fpTwPoint(p_twpoint),
   fpGlbTrack(p_glbtrack),
   fpDiGeoMap(p_geodi),
   fActEvtReader(0x0)
{
   AddDataOut(p_glbtrack, "TAGntuGlbTrack");
   if (GlobalPar::GetPar()->IncludeVertex())
      AddDataIn(p_vtxvertex, "TAVTntuVertex");
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      AddDataIn(p_itrclus, "TAITntuCluster");
   
   if (GlobalPar::GetPar()->IncludeMSD())
      AddDataIn(p_msdclus, "TAMSDntuCluster");
   
   if(GlobalPar::GetPar()->IncludeTW())
      AddDataIn(p_twpoint, "TATWntuPoint");
   
   if (fgStdAloneFlag)
      SetupBranches();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuGlbTrack::~TAGactNtuGlbTrack()
{  
   if (fActEvtReader)
      delete fActEvtReader;
}

//------------------------------------------+-----------------------------------
//! Setup all branches.
void TAGactNtuGlbTrack::SetupBranches()
{
   fActEvtReader = new TAGactTreeReader("evtReader");
   
   if (GlobalPar::GetPar()->IncludeVertex())
      fActEvtReader->SetupBranch(fpVtxVertex, TAVTntuVertex::GetBranchName());
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      fActEvtReader->SetupBranch(fpItrClus,   TAITntuCluster::GetBranchName());
   
   if (GlobalPar::GetPar()->IncludeMSD())
     fActEvtReader->SetupBranch(fpMsdClus,   TAMSDntuCluster::GetBranchName());
   
   if(GlobalPar::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpTwPoint,   TATWntuPoint::GetBranchName());
}

//__________________________________________________________
// ! Open file
void TAGactNtuGlbTrack::Open(TString name)
{
   if (fgStdAloneFlag)
      fActEvtReader->Open(name.Data());
   else
      Error("OpenFile", "Not in stand alone mode");
}

//__________________________________________________________
//! Close file
void TAGactNtuGlbTrack::Close()
{
   fActEvtReader->Close();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactNtuGlbTrack::CreateHistogram()
{
   DeleteHistogram();
   fpHisMass = new TH1F("glbMass", "Mass Distribution", 200, 0, 16);
   
   AddHistogram(fpHisMass);
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuGlbTrack::Action()
{
   if(GlobalPar::GetPar()->IncludeTW()) {
      TATWntuPoint* pNtuPoint  = (TATWntuPoint*) fpTwPoint->Object();
      for (Int_t i = 0; i < pNtuPoint->GetPointN(); ++i) {
         TATWpoint* point = pNtuPoint->GetPoint(i);
         TVector3 pos = point->GetPosition();
         pos.Print();
      }
   }
   
   fpGlbTrack->SetBit(kValid);
   return kTRUE;
}

