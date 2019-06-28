/*!
 \file
 \brief   Implementation of TAVTactNtuVertexL. 
 */
#include "TClonesArray.h"

#include "TMath.h"
#include "TH2F.h"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactBaseNtuVertex.hxx"
#include "TArrayI.h"


/*!
 \class TAVTactNtuVertex
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactNtuVertexPD);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuVertexPD::TAVTactNtuVertexPD(const char* name, 
                                       TAGdataDsc* pNtuTrack, TAGdataDsc* pNtuVertex,
                                       TAGparaDsc* pConfig, TAGparaDsc* pGeoMap, TAGparaDsc* pGeoMapG, TAGdataDsc* pBmTrack)
: TAVTactBaseNtuVertex(name, pNtuTrack, pNtuVertex, pConfig, pGeoMap, pGeoMapG, pBmTrack),
  fStepZ(0.0010),
  fImpactParameterCut(0.025),
  fProbabilityCut(0.1)
{
   fErr.SetXYZ(0.005, 0.005, 0.005); //errore in X, Y, Z
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuVertexPD::~TAVTactNtuVertexPD()
{    
}

//_________________________________________________
//! Compute Vertex method

Bool_t TAVTactNtuVertexPD::ComputeVertex()
{
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    Int_t nTracks          = ntuTrack->GetTracksN();
    fTrack = nTracks;
    
    fProbValuesMax.Set(nTracks*nTracks);
    fVvalues.Set(nTracks*nTracks);
    fFlagValidity.Set(nTracks*nTracks);
    
    fProbValuesMax.Reset(-1);
    fRValuesMax.clear();
    fFlagValidity.Reset(-1);
   
    fNotValidTrack.Set(nTracks);
    fNotValidTrack.Reset(-1);
   
   //Initialise Map
    for (Int_t i = 0; i < nTracks-1; ++i){
       
        for(Int_t j = i+1; j < nTracks; ++j){
       
            TAVTtrack* track0 = ntuTrack->GetTrack(i);
        
            TAVTtrack* track1 = ntuTrack->GetTrack(j);
            SearchMaxProduct (track0, track1, i, j);
            
            if(fProbValuesMax[i*fTrack + j] < fProbabilityCut){
                fFlagValidity[i*fTrack + j] = 0;
                fFlagValidity[j*fTrack + i] = 0; //symmetric

            } else {
                fFlagValidity[i*fTrack + j] = 1;
                fFlagValidity[j*fTrack + i] = 1;
            } 
        }
                       
    }
    
    Int_t countTrki =0;
    Int_t countNotValid = 0;
    Bool_t OK = false;
   
    for(Int_t ii = 0; ii < nTracks;++ii){
        countTrki = 0;
        for(Int_t jj = 0; jj < nTracks; ++jj){
                       
            if(fFlagValidity[ii*fTrack + jj] == 0){
               countTrki++;
               if(countTrki == nTracks-1){ 
                    fNotValidTrack[ii] = 1;
                    OK =  SetNotValidVertex(ii);
                    countNotValid++;
               }
            }
        }
    }
                
            
    TVector3 maxVij(0,0,0);
    if(countNotValid ==nTracks) 
	   return OK;
    else 
        maxVij = ComputeMaxVMaxIMaxJ();
    
     for(Int_t z =0; z<3; ++z)
        fVtxPos[z] = fRValuesMax[maxVij[1]*fTrack + maxVij[2]][z];
    
    if(fgCheckPileUp == false)
        ImpactParameterAdjustement();
    
    OK = SetVertex();
   
    return OK;
}


//___________________________________________
//!Search max Value

void TAVTactNtuVertexPD::SearchMaxProduct(TAVTtrack* linei, TAVTtrack* linej, Int_t i, Int_t j)
{
    Double_t fi = 0;
    Double_t fj = 0;    
    TVector3 vertexPoint (0,0,0);
    
    TVector3 maxPosition (0,0,0);
    
    Double_t actualProb = 0;
    Double_t maxProb = -10e+10;
    //loop su z per trovare il massimo 
    for(Double_t iz = fMinZ; iz < fMaxZ; iz += fStepZ){
        vertexPoint = ComputeVertexPoint(linei, linej, iz); //Gli passo le tracce
        fi = ComputeProbabilityForSingleTrack(linei, vertexPoint);
        fj = ComputeProbabilityForSingleTrack(linej, vertexPoint);
        actualProb = fi*fj;
        if(actualProb > maxProb){
            maxProb = actualProb;
            maxPosition = vertexPoint;
        }
       
    }
   
   //ritornare i valori messi in una struttura
    fProbValuesMax[i*fTrack + j] = maxProb;
    fRValuesMax[i*fTrack + j] = maxPosition;
}


//----------------------------------------------
//!Calcola il punto di vertice

TVector3 TAVTactNtuVertexPD::ComputeVertexPoint(TAVTtrack* line0, TAVTtrack* line1, Double_t zVal)
{
    //Calcolare la media delle due tracce
    TVector3 result;
    Double_t x = (line0->GetPoint(zVal).X() + line1->GetPoint(zVal).X())/2;
    Double_t y = (line0->GetPoint(zVal).Y() + line1->GetPoint(zVal).Y())/2;
    result.SetXYZ(x,y,zVal);
    return result;

}

//----------------------------------------------
//!Calcola il probabilitá

Double_t TAVTactNtuVertexPD::ComputeProbabilityForSingleTrack(TAVTtrack* lin0, TVector3 v)
{
    //prendo le posizioni della tracia in esame
   
    TVector3 r = ComputeMinimumPointDistance(lin0, v);
    Double_t result = 0;
    for(Int_t q = 0; q<3; ++q)
        result += (r[q]-v[q])*(r[q]-v[q])/(fErr[q]*fErr[q]);

    result = TMath::Exp(-0.5*result);
   
    return result;

}

//______________________________________________________
//!Routin che calcola la minima distanza della retta dal punto

TVector3 TAVTactNtuVertexPD::ComputeMinimumPointDistance(TAVTtrack* l, TVector3 vt)
{
    Double_t z = vt[2];
    
    TVector3 A  (l->GetPoint(z).X(), l->GetPoint(z).Y(),z);
    
    Double_t DZ = 1;
    TVector3 cosDir = l->GetSlopeZ()*DZ;//Direzione della retta
    Double_t cosDirMod = cosDir.Mag();
    cosDir *= 1/cosDirMod; //ho normalizzato quindi dovrebbe essere 1
    TVector3 AP = vt-A;
    TVector3 result;
    Double_t tPar = cosDir.Dot(AP);
   
    for(Int_t i= 0; i<3; ++i)
        result[i] = A[i] + tPar*cosDir[i];
    
    return result;
    
}

//-------------------------------------------------------
//!Compute V(r)

Double_t TAVTactNtuVertexPD::ComputeV (TVector3 rpos)
{
    //Calcolo V sono com le Valide
    Double_t firstMember =0;
    Double_t secondMember = 0;
    
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
   // Int_t nTracks          = ntuTrack->GetTracksN();
   
    //Loop sulle tracce 
    for (Int_t i = 0; i<fTrack; ++i){
        TAVTtrack* track1 = ntuTrack->GetTrack(i);
        if (fNotValidTrack[i] == -1){ //se sono valide
           // cout<<"Metto la traccia "<<i<<endl;
            Double_t fi = ComputeProbabilityForSingleTrack(track1, rpos);
            firstMember += fi;
            secondMember += fi*fi;
        }
    }
      
    Double_t result = firstMember + (secondMember/firstMember);
   
    return result;   
}

//_________________________________________________
//! Set Vertex information after vertexing

Bool_t TAVTactNtuVertexPD::SetVertex()
{
    TAVTntuVertex* ntuVertex = (TAVTntuVertex*)fpNtuVertex->Object();
   
    TAVTvertex* vtx = new TAVTvertex();
    vtx->SetVertexValidity(1);
    vtx->SetVertexPosition(fVtxPos);

   if (ValidHistogram()) {
	  fpHisPosZ->Fill(fVtxPos.Z());
	  fpHisPosXY->Fill(fVtxPos.X(), fVtxPos.Y());
   }
   
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    Int_t nTracks = ntuTrack->GetTracksN();
    for(Int_t q = 0; q < nTracks; ++q ) {
       if(fNotValidTrack[q] == 1) continue;
        TAVTtrack* track0 = ntuTrack->GetTrack(q);
        track0->SetValidity(1);
	    track0->SetVertexZ(fVtxPos.Z());
        vtx->AddTrack(track0);
    }
    
   if (vtx->GetTracksN() > 0) 
	  ntuVertex->NewVertex(*vtx);
   
    delete vtx;
    return true;
}

//_________________________________________________
//! SetImpact Parameter Adjustement

void TAVTactNtuVertexPD::ImpactParameterAdjustement()
{
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    
    Double_t distance =0;
    TVector3 minPoint(0,0,0);
   
    Bool_t res;
    for(Int_t q= 0; q<fTrack; ++q) {
        distance = 0;
        if(fNotValidTrack[q] == 1) continue;
        TAVTtrack* track1 = ntuTrack->GetTrack(q);
        minPoint = track1->Intersection(fVtxPos[2]);
               
        for(Int_t pq =0; pq<3; ++pq)
            distance += (minPoint[pq] - fVtxPos[pq])*(minPoint[pq] - fVtxPos[pq]);
        
        distance = TMath::Sqrt(distance);

        if(distance>fImpactParameterCut) {//500 micron - Setto la traccia non valid
            fNotValidTrack[q] = 1;
            res = SetNotValidVertex(q);
        }
     }
}


//---------------------------------------------------
//!Compute max V

TVector3 TAVTactNtuVertexPD::ComputeMaxVMaxIMaxJ()
{
    TVector3 returnValue (-10e-10,-1,-1);
    Double_t actualVvalues =0;
    
    for(Int_t  p = 0; p< fTrack-1; ++p){//loop sulle tracce per trovare il max V
        
        for(Int_t q = p+1; q<fTrack; ++q ) {
            if (fNotValidTrack[q] == 1 || fNotValidTrack[p] == 1) continue;
            actualVvalues = ComputeV(fRValuesMax[p*fTrack + q]); //GLi passo la lista delle non valide 
            if(actualVvalues>returnValue[0]){
                returnValue[0] = actualVvalues;
                returnValue[1] = p;
                returnValue[2] = q;
            }
        }
    }
   
    return returnValue;
}
