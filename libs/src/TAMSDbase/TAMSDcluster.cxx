////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDcluster.hxx"
#include "TAMSDntuHit.hxx"

ClassImp(TAMSDcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster()
:  TAGcluster(),
   fPosition(0.),
   fPosError(0),
   fCurPosition(0,0,0),
fPositionG(0., 0., 0.),
fPosErrorG(0., 0., 0.),
   fPlaneNumber(10),
   fPlaneView(-1),
   fIsValid(false)
{
   // TAMSDcluster constructor
   SetupClones();
   fMcTrackIdx.Reset();
   fMcTrackMap.clear();
}

//______________________________________________________________________________
//
void TAMSDcluster::SetupClones()
{
   fListOfStrips = new TClonesArray("TAMSDntuHit");
   fListOfStrips->SetOwner(true);
}

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster(const TAMSDcluster& cluster)
:  TAGcluster(),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fCurPosition(cluster.fCurPosition),
   fPositionG(cluster.fPositionG),
   fPosErrorG(cluster.fPosErrorG),
   fPlaneNumber(cluster.fPlaneNumber),
   fPlaneView(cluster.fPlaneView),
   fIsValid(cluster.fIsValid),
   fMcTrackIdx(cluster.fMcTrackIdx)
{
   fListOfStrips = (TClonesArray*)cluster.fListOfStrips->Clone();
}

//______________________________________________________________________________
//  
TAMSDcluster::~TAMSDcluster()
{ 
   // TAMSDcluster default destructor
   delete fListOfStrips;
}


//______________________________________________________________________________
//  
void TAMSDcluster::AddStrip(TAMSDntuHit* strip)
{
   for (Int_t k = 0; k < strip->GetMcTracksN(); ++k) {
      Int_t idx = strip->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &StripArray = *fListOfStrips;
   new(StripArray[StripArray.GetEntriesFast()]) TAMSDntuHit(*strip);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionG(TVector3& posGlo)
{
   fPositionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   
   if (fPlaneView == 0)
      fPosErrorG.SetXYZ(fPosition, 0, 0.01);
   else
      fPosErrorG.SetXYZ(0, fPosition, 0.01);   
}

//______________________________________________________________________________
//
TAMSDntuHit* TAMSDcluster::GetStrip(Int_t idx)
{
   if (idx >=0 && idx < fListOfStrips->GetEntries())
      return (TAMSDntuHit*)fListOfStrips->At(idx);
   else
      return 0x0;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPosition(Float_t pos)
{
   fPosition = pos;
   if (fPlaneView == 0)
      fCurPosition.SetXYZ(fPosition, 0, 0);
   else
      fCurPosition.SetXYZ(0, fPosition, 0);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPosError(Float_t pos)
{
   fPosError = pos;
   if (fPlaneView == 0)
    fCurPosition.SetXYZ(fPosError, 0, 0);
   else
     fCurPosition.SetXYZ(0, fPosError, 0);
}


//______________________________________________________________________________
//
Float_t TAMSDcluster::Distance(TAMSDcluster *aClus) {
   // Return the distance between this clusters and the pointed cluster
   // regardless of the plane
   
   TVector3 clusPosition( aClus->GetPositionG() );
   
   // Now compute the distance beetween the two hits
   clusPosition -= (GetPositionG());
   
   // Insure that z position is 0 for 2D length computation
   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
   
   return clusPosition.Mag();
}

//______________________________________________________________________________
//
void TAMSDcluster::AddMcTrackIdx(Int_t trackIdx)
{
   if (fMcTrackMap[trackIdx] == 0) {
      fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
      fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackIdx;
      fMcTrackMap[trackIdx] = 1;
   }
}
