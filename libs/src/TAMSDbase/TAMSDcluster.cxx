////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDcluster.hxx"
#include "TAMSDhit.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TF1.h"

#include <math.h>

ClassImp(TAMSDcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster()
:  TAGcluster(),
   fPositionF(0.),
   fPosErrorF(0),
   fPosErrorCorr(0),
   fCurPosition(0,0,0),
   fCog(0),
   fPlaneView(-1),
   fEnergyLoss(0),
   fEnergyLossCorr(0),
   fEtaValue(0),
   fEtaFastValue(0)
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
   fPositionF(cluster.fPositionF),
   fPosErrorF(cluster.fPosErrorF),
   fPosErrorCorr(cluster.fPosErrorCorr),
   fCog(cluster.fCog),
   fCurPosition(cluster.fCurPosition),
   fPlaneView(cluster.fPlaneView),
   fEnergyLoss(cluster.fEnergyLoss),
   fEnergyLossCorr(cluster.fEnergyLossCorr),
   fEtaValue(cluster.fEtaValue),
   fEtaFastValue(cluster.fEtaFastValue)
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
   
   fElementsN = fListOfStrips->GetEntries();
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   
   if (GetPlaneView() == 0)
      fPosError2.SetXYZ(fPosErrorF, 0, 0.01);
   else
      fPosError2.SetXYZ(0, fPosErrorF, 0.01);
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
void TAMSDcluster::SetPositionF(Float_t pos)
{
   fPositionF = pos;
   if (GetPlaneView() == 0)
      fCurPosition.SetXYZ(fPositionF, 0, 0);
   else
      fCurPosition.SetXYZ(0, fPositionF, 0);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPosErrorF(Float_t pos)
{
   fPosErrorF = pos;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetCog(Float_t pos)
{
   fCog = pos;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPlaneView(Int_t v)
{
   fPlaneView = v;
   fDeviceType = TAGgeoTrafo::GetDeviceType(TAMSDparGeo::GetBaseName()) + fPlaneView;
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
Float_t TAMSDcluster::ComputeEtaFast(Float_t cog)
{
   Double_t fractpart, intpart;
   fractpart = modf (cog , &intpart);

   fEtaFastValue = fractpart;
   return fractpart;
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::ComputeEta()
{
   Int_t nstrips = fListOfStrips->GetEntries();

   if (nstrips == 1)
      return 1.0;

   Float_t eta;
   TAMSDhit* strip;
   Float_t max_adc = -1;
   Int_t max_pos = 0;

   //cout << "Cluster has " << fListOfStrips->GetEntries() << " strips" << endl;
   for (int i = 0; i < nstrips; i++)
   {  
      strip = (TAMSDhit*)fListOfStrips->At(i);
      // cout << "\tStrip " << i << " has value " << strip->GetEnergyLoss() << endl;

      if(strip->GetEnergyLoss() > max_adc)
      {
         max_adc = strip->GetEnergyLoss();
         max_pos = i;
      }
   }

   if (max_pos == 0)
   {
      eta = ((TAMSDhit*)fListOfStrips->At(0))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(0))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(1))->GetEnergyLoss() );
   } 
   else if(max_pos == nstrips - 1)
   {
      eta = ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() );
   } 
   else
   {
      if( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() >  ((TAMSDhit*)fListOfStrips->At(max_pos + 1))->GetEnergyLoss() )
      {
         eta = ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() );
      }
      else
      {
         eta = ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos + 1))->GetEnergyLoss() );
      }
   }

   fEtaValue = eta;
   
   return eta;
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::GetAddress() const
{
   const TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(0);
   return strip->GetPosition();
}


