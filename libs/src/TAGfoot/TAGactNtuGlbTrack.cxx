/*!
 \file
 \version $Id: TAGactNtuGlbTrack.cxx
 \brief   Implementation of TAGactNtuGlbTrack.
 */

#include "TH1F.h"

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
 : TAGactTreeReader(name),
   fpVtxVertex(p_vtxvertex),
   fpItrClus(p_itrclus),
   fpMsdClus(p_msdclus),
   fpTwPoint(p_twpoint),
   fpGlbTrack(p_glbtrack),
   fpDiGeoMap(p_geodi)
{
   AddDataOut(p_glbtrack, "TAGntuGlbTrack");
   AddDataIn(p_vtxvertex, "TAVTntuVertex");
   AddDataIn(p_itrclus, "TAITntuCluster");
   AddDataIn(p_msdclus, "TAMSDntuCluster");
   AddDataIn(p_twpoint, "TATWntuPoint");
   
   if (fgStdAloneFlag)
      SetupBranches();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGactNtuGlbTrack::~TAGactNtuGlbTrack()
{  

}

//------------------------------------------+-----------------------------------
//! Setup all branches.

void TAGactNtuGlbTrack::SetupBranches()
{
   SetupBranch(fpVtxVertex, TAVTntuVertex::GetBranchName());
   SetupBranch(fpItrClus,   TAITntuCluster::GetBranchName());
   SetupBranch(fpMsdClus,   TAMSDntuCluster::GetBranchName());
   SetupBranch(fpTwPoint,   TATWntuPoint::GetBranchName());
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
   fpGlbTrack->SetBit(kValid);
   return kTRUE;
}

