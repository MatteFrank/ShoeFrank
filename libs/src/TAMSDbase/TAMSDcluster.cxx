////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDcluster.hxx"
#include "TAMSDhit.hxx"

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
}

//______________________________________________________________________________
//
void TAMSDcluster::SetupClones()
{
   fListOfStrips = new TClonesArray("TAMSDhit");
   fListOfStrips->SetOwner(true);
}

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster(const TAMSDcluster& cluster)
:  TAGcluster(cluster),
   fPosition(cluster.fPosition),
   fPosError(cluster.fPosError),
   fCurPosition(cluster.fCurPosition),
   fPositionG(cluster.fPositionG),
   fPosErrorG(cluster.fPosErrorG),
   fPlaneNumber(cluster.fPlaneNumber),
   fPlaneView(cluster.fPlaneView),
   fIsValid(cluster.fIsValid)
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
void TAMSDcluster::AddStrip(TAMSDhit* strip)
{
   for (Int_t k = 0; k < strip->GetMcTracksN(); ++k) {
      Int_t idx = strip->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &StripArray = *fListOfStrips;
   new(StripArray[StripArray.GetEntriesFast()]) TAMSDhit(*strip);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionG(TVector3& posGlo)
{
   fPositionG.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   
   if (fPlaneView == 0)
      fPosErrorG.SetXYZ(fPosError, 0, 0.01);
   else
      fPosErrorG.SetXYZ(0, fPosError, 0.01);
}

//______________________________________________________________________________
//
TAMSDhit* TAMSDcluster::GetStrip(Int_t idx)
{
   if (idx >=0 && idx < fListOfStrips->GetEntries())
      return (TAMSDhit*)fListOfStrips->At(idx);
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

