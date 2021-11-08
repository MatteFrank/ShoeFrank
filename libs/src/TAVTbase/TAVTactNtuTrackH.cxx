
#include "TClonesArray.h"
#include "TF1.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVector2.h"
#include <map>


//TAGroot
#include "TAGroot.hxx"

//BM
#include "TABMntuTrack.hxx"

//First
#include "TAGgeoTrafo.hxx"

#include "TAVTdatRaw.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuTrackH.hxx"



ClassImp(TAVTactNtuTrackH);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactNtuTrackH::TAVTactNtuTrackH(const char* name, 
								   TAGdataDsc* pNtuClus, TAGdataDsc* pNtuTrack, TAGparaDsc* pConfig, 
								   TAGparaDsc* pGeoMap, TAGparaDsc* pCalib, TAGdataDsc* pBMntuTrack)
: TAVTactBaseNtuTrack(name, pNtuClus, pNtuTrack, pConfig, pGeoMap, pCalib, pBMntuTrack),
  fListOfVertices(new TList())
{
   AddDataIn(pNtuClus,   "TAVTntuCluster");
   AddDataOut(pNtuTrack, "TAVTntuTrack");
   
   TAVTbaseParGeo* geoMap  = (TAVTbaseParGeo*) fpGeoMap->Object();

   fpHisHoughXZ = new TH2F("vtHoughXZ", "Hough Transform XZ space",534,-1.6,1.6,
						   140,-geoMap->GetPitchU()*geoMap->GetPixelsNu()/2., geoMap->GetPitchU()*geoMap->GetPixelsNu()/2.);
   fpHisHoughXZ->SetStats(kFALSE);
   
   fpHisHoughYZ = new TH2F("vtHoughYZ", "Hough Transform YZ space",400,-1.6,1.6,
						   140,-geoMap->GetPitchU()*geoMap->GetPixelsNu()/2., geoMap->GetPitchU()*geoMap->GetPixelsNu()/2.);
   fpHisHoughYZ->SetStats(kFALSE);
   
   fListOfVertices->SetOwner(true);
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuTrackH::~TAVTactNtuTrackH()
{
   delete fListOfVertices;
   delete fpHisHoughYZ;
   delete fpHisHoughYZ;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackH::FindTiltedTracks()
{
	TAVTntuCluster* pNtuClus  = (TAVTntuCluster*) fpNtuClus->Object();
	TAVTntuTrack*   pNtuTrack = (TAVTntuTrack*)   fpNtuTrack->Object();
	TAVTbaseParGeo* pGeoMap   = (TAVTbaseParGeo*) fpGeoMap->Object();
	
	TAVTtrack* track = 0x0;
	Int_t nPlane   = pGeoMap->GetSensorsN()-1;
	
	
	vector<Int_t> vecListClus,vecListClusTrack;
	map<int, vector<int> > MapXZ;
	map<int, vector<int> > MapYZ;
	map<int, vector<int> > MapListOfBinsXZ,MapListOfBinsYZ;
	map<int,int > MapPlane;
	
	fpHisHoughXZ->Reset();
	fpHisHoughYZ->Reset();
	Double_t SlopeStep = 0.006; 
	
	// Fill XZ Histo for vertices 
	TList* ListVtx = GetListOfVertices();
	if (ListVtx->GetEntries() > 0) {
		for (Int_t ivtx = 0; ivtx < ListVtx->GetEntries(); ++ivtx) {
			Int_t compteurClus = 9000+ivtx;  // Vertices are in "plane 9"
			TVector3* posVtx = (TVector3*)ListVtx->At(ivtx);
			Double_t InterceptX; 
			for (Double_t Slope = -1.6; Slope <= 1.6; Slope = Slope + SlopeStep) {
				InterceptX = -Slope*posVtx->Z() + posVtx->X();
				if ( InterceptX < fpHisHoughXZ->GetYaxis()->GetXmax() && InterceptX > fpHisHoughXZ->GetYaxis()->GetXmin()){
					fpHisHoughXZ->Fill(Slope,InterceptX);
					MapXZ[fpHisHoughXZ->GetXaxis()->GetNbins()*fpHisHoughXZ->GetYaxis()->FindBin(InterceptX)+fpHisHoughXZ->GetXaxis()->FindBin(Slope)].push_back(compteurClus);
				}
			}
		}
	}
	//Fill XZ Histo for all clusters 
	for (Int_t iPl = nPlane; iPl >=0; --iPl) { // loop on all planes	
		TClonesArray* ListClus = pNtuClus->GetListOfClusters(iPl);
		Int_t nClusters    = pNtuClus->GetClustersN(iPl);
		
		if ( nClusters == 0) continue;
		if ( nClusters > 0){	//Remove clusters too close to another one in same plane
			for( Int_t iClus = 0; iClus < nClusters - 1; ++iClus) {
				TAVTcluster* iCluster = (TAVTcluster*)ListClus->At(iClus);
				if (iCluster->GetPixelsN() <= 1) continue;
				if (iCluster->GetFound()) continue;
				for( Int_t jClus = iClus+1; jClus < nClusters; ++jClus) {
					TAVTcluster* jCluster = (TAVTcluster*)ListClus->At(jClus);
					if (TMath::Abs(iCluster->GetPositionG().X()-jCluster->GetPositionG().X()) < fpHisHoughXZ->GetYaxis()->GetBinWidth(1)/*SlopeStep*iCluster->GetPositionG().Z()*/ 
						&& TMath::Abs(iCluster->GetPositionG().Y()-jCluster->GetPositionG().Y()) < fpHisHoughXZ->GetYaxis()->GetBinWidth(1)/*SlopeStep*iCluster->GetPositionG().Z()*/)
						jCluster->SetFound();
				}
			}
		}
		for( Int_t iClus = 0; iClus < nClusters; ++iClus) { // loop on clusters of each plane
			Int_t compteurClus = iPl*1000+iClus;
			TAVTcluster* Cluster = (TAVTcluster*)ListClus->At(iClus);
			if (Cluster->GetPixelsN() <= 1) continue;
			if (Cluster->GetFound()) continue;
			Double_t InterceptX; 
			for (Double_t Slope = -1.6; Slope <= 1.6; Slope = Slope + SlopeStep) {
				InterceptX = -Slope*Cluster->GetPositionG().Z() + Cluster->GetPositionG().X();
				if ( InterceptX < fpHisHoughXZ->GetYaxis()->GetXmax() && InterceptX > fpHisHoughXZ->GetYaxis()->GetXmin()){
					fpHisHoughXZ->Fill(Slope,InterceptX);
					MapXZ[fpHisHoughXZ->GetXaxis()->GetNbins()*fpHisHoughXZ->GetYaxis()->FindBin(InterceptX)+fpHisHoughXZ->GetXaxis()->FindBin(Slope)].push_back(compteurClus);
				}
			}
		}
	}
	//Sort bins' content of XZ Histogram
	Int_t ContentmaxXZ = 0;
	for (Int_t binm = 0; binm < fpHisHoughXZ->GetXaxis()->GetNbins(); ++binm) {
		for (Int_t binc = 0; binc < fpHisHoughXZ->GetYaxis()->GetNbins(); ++binc) {
			Double_t Content = fpHisHoughXZ->GetBinContent(binm,binc);
			if ( Content >= fRequiredClusters){
				MapListOfBinsXZ[Content].push_back(fpHisHoughXZ->GetXaxis()->GetNbins()*binc+binm);
				if (Content > ContentmaxXZ)
					ContentmaxXZ = Content;
			}
			else if (Content < 2)								//Clean histogram
				fpHisHoughXZ->SetBinContent(binm,binc,0);
		}
	}
	
	if (ContentmaxXZ < fRequiredClusters) return false;
	// Start Hough transform in YZ space
	SlopeStep = 0.008;
	for (Int_t iNumberClus = ContentmaxXZ; iNumberClus >= fRequiredClusters; --iNumberClus) { // Loop on bins' content
		if (MapListOfBinsXZ.count(iNumberClus)){
			vector<int> vectMapListOfBinsXZ = MapListOfBinsXZ.find(iNumberClus)->second;
			for (UInt_t ibin = 0; ibin < vectMapListOfBinsXZ.size(); ++ibin) {	// Make separate Hough Transform for each identified bin in XZ Histo
				Int_t binc = int(vectMapListOfBinsXZ[ibin]/fpHisHoughXZ->GetXaxis()->GetNbins());
				Int_t binm = vectMapListOfBinsXZ[ibin] - fpHisHoughXZ->GetXaxis()->GetNbins()*binc;
				if (!MapXZ.count(vectMapListOfBinsXZ[ibin]) || fpHisHoughXZ->GetBinContent(binm,binc) < 2)
					continue;
				vector<int> vectMapXZ = MapXZ.find(vectMapListOfBinsXZ[ibin])->second;
				//Remove bin XZ if number planes < 3
				Int_t nPlanes = vectMapXZ.size();
				for (unsigned int iVecMap = 0; iVecMap < vectMapXZ.size()-1 ; ++iVecMap) {
					if (int(vectMapXZ[iVecMap+1]/1000) == int(vectMapXZ[iVecMap]/1000))
						--nPlanes;
				}
				if (nPlanes < fRequiredClusters) continue;
				for (unsigned int iVecMap = 0; iVecMap < vectMapXZ.size() ; ++iVecMap) {
					if (int(vectMapXZ[iVecMap]/1000) == 9) { // Case of vertices
						TVector3* posVtx = (TVector3*)ListVtx->At(vectMapXZ[iVecMap]%1000);
						Double_t InterceptY; 
						for (Double_t Slope = -1.6; Slope <= 1.6; Slope = Slope + SlopeStep) {
							InterceptY = -Slope*posVtx->Z() + posVtx->Y();
							if ( InterceptY < fpHisHoughYZ->GetYaxis()->GetXmax() && InterceptY > fpHisHoughYZ->GetYaxis()->GetXmin()){
								fpHisHoughYZ->Fill(Slope,InterceptY);
								MapYZ[fpHisHoughYZ->GetXaxis()->GetNbins()*fpHisHoughYZ->GetYaxis()->FindBin(InterceptY)+fpHisHoughYZ->GetXaxis()->FindBin(Slope)].push_back(vectMapXZ[iVecMap]);
							}
						}
					}
					else {	
						TClonesArray* ListClus = pNtuClus->GetListOfClusters(int(vectMapXZ[iVecMap]/1000));
						TAVTcluster* Cluster = (TAVTcluster*)ListClus->At(vectMapXZ[iVecMap]%1000);
						if (Cluster->GetFoundXZ()) continue;
						Double_t InterceptY;
						for (Double_t Slope = -1.6; Slope <= 1.6; Slope = Slope + SlopeStep) {
							InterceptY = -Slope*Cluster->GetPositionG().Z() + Cluster->GetPositionG().Y();
							if ( InterceptY < fpHisHoughYZ->GetYaxis()->GetXmax() && InterceptY > fpHisHoughYZ->GetYaxis()->GetXmin()){
								fpHisHoughYZ->Fill(Slope,InterceptY);
								MapYZ[fpHisHoughYZ->GetXaxis()->GetNbins()*fpHisHoughYZ->GetYaxis()->FindBin(InterceptY)+fpHisHoughYZ->GetXaxis()->FindBin(Slope)].push_back(vectMapXZ[iVecMap]);
							}
						}
						vecListClus.push_back(vectMapXZ[iVecMap]); 
						Cluster->SetFoundXZ();
					}
				}
				//Sort bins' content of YZ Histogram
				Int_t ContentmaxYZ = 0;
				for (Int_t binm = 0; binm < fpHisHoughYZ->GetXaxis()->GetNbins(); ++binm) {
					for (Int_t binc = 0; binc < fpHisHoughYZ->GetYaxis()->GetNbins(); ++binc) {
						Double_t Content = fpHisHoughYZ->GetBinContent(binm,binc);
						if ( Content >= fRequiredClusters){
							MapListOfBinsYZ[Content].push_back(fpHisHoughYZ->GetXaxis()->GetNbins()*binc+binm);
							if (Content > ContentmaxYZ)
								ContentmaxYZ = Content;
						}
					}
				}
				// Test all potential tracks from YZ Histogram
				if (ContentmaxYZ >= fRequiredClusters) {
					for (Int_t iNumberClusYZ = ContentmaxYZ; iNumberClusYZ >= fRequiredClusters; --iNumberClusYZ) {
						if (MapListOfBinsYZ.count(iNumberClusYZ)){
							vector<int> vectMapListOfBinsYZ = MapListOfBinsYZ.find(iNumberClusYZ)->second;
							for (UInt_t ibinYZ = 0; ibinYZ < vectMapListOfBinsYZ.size(); ++ibinYZ) {
								track = new TAVTtrack();
								Int_t nHitPlane =0;	
								vector<int> vectMapYZ = MapYZ.find(vectMapListOfBinsYZ[ibinYZ])->second;
								for (unsigned int iVecMap = 0; iVecMap < vectMapYZ.size() ; ++iVecMap) {
									if (int(vectMapYZ[iVecMap]/1000) == 9) { // Case of vertices
										if (!MapPlane.count(int(vectMapYZ[iVecMap]/1000))){
											MapPlane[int(vectMapYZ[iVecMap]/1000)] = 1;
											++nHitPlane;
										}
									}
									else {
										TClonesArray* ListClus = pNtuClus->GetListOfClusters(int(vectMapYZ[iVecMap]/1000));
										TAVTcluster* Cluster = (TAVTcluster*)ListClus->At(vectMapYZ[iVecMap]%1000);
										if (Cluster->GetFoundYZ()) continue;
										else {
											track->AddCluster(Cluster);
											Cluster->SetFoundYZ();
											vecListClusTrack.push_back(vectMapYZ[iVecMap]);
											if (!MapPlane.count(int(vectMapYZ[iVecMap]/1000))){
												MapPlane[int(vectMapYZ[iVecMap]/1000)] = 1;
												++nHitPlane;
											}
										}
									}
								}
								if (MapPlane.count(9) && nHitPlane >= fRequiredClusters){ 
									UpdateParam(track);
									track->SetNumber(pNtuTrack->GetTracksN());
									track->MakeChiSquare();
									track->SetType(1);
									pNtuTrack->NewTrack(*track);
									if (ValidHistogram()) 
										FillHistogramm(track);
									
									for (UInt_t iclusTr = 0; iclusTr < vecListClusTrack.size(); ++iclusTr) {
										if (int(vecListClusTrack[iclusTr]/1000) == 9) continue;
										TClonesArray* iListTr = pNtuClus->GetListOfClusters(int(vecListClusTrack[iclusTr]/1000));
										TAVTcluster* iClusterTr = (TAVTcluster*)iListTr->At(vecListClusTrack[iclusTr]%1000);
										iClusterTr->SetFound();
									}
								}
								
								else if (AppyCuts(track) && nHitPlane >= fRequiredClusters) {
									UpdateParam(track);
									track->SetNumber(pNtuTrack->GetTracksN());
									track->MakeChiSquare();
									track->SetType(1);
									pNtuTrack->NewTrack(*track);						
									if (ValidHistogram()) {
										FillHistogramm(track);
									}
									for (UInt_t iclusTr = 0; iclusTr < vecListClusTrack.size(); ++iclusTr) {
										if (int(vecListClusTrack[iclusTr]/1000) == 9) continue;
										TClonesArray* iListTr = pNtuClus->GetListOfClusters(int(vecListClusTrack[iclusTr]/1000));
										TAVTcluster* iClusterTr = (TAVTcluster*)iListTr->At(vecListClusTrack[iclusTr]%1000);
										iClusterTr->SetFound();
									}
								}
								else {					
									for (Int_t nclus = 0; nclus < track->GetClustersN(); ++nclus) {
										TClonesArray* ListClus = pNtuClus->GetListOfClusters(track->GetCluster(nclus)->GetPlaneNumber());
										TAVTcluster* iCluster = (TAVTcluster*)ListClus->At(track->GetCluster(nclus)->GetTrackIdx());
										iCluster->SetFoundYZ(kFALSE);
									}
								}
								MapPlane.clear();
								nHitPlane = 0;
								delete track;
							}	
						}
					} // End loop on potential tracks
				}
				for (UInt_t iclus = 0; iclus < vecListClus.size(); ++iclus) {
					if (int(vecListClus[iclus]/1000) == 9) continue;
					TClonesArray* iList = pNtuClus->GetListOfClusters(int(vecListClus[iclus]/1000));
					TAVTcluster* iCluster = (TAVTcluster*)iList->At(vecListClus[iclus]%1000);
					iCluster->SetFoundXZ(kFALSE);
				}
				vecListClus.clear();
				fpHisHoughYZ->Reset();
				MapYZ.clear();
				MapListOfBinsYZ.clear();
			} // End loop on YZ Hough transforms
		}
	} // End loop on bins' content
	
	if (ListVtx->GetEntries() > 0)  // Reset List of vertices 
		ListVtx->Delete();
	
	return true;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackH::IsGoodCandidate(TAVTtrack* track)
{
   TVector3 vec = track->Intersection(0.);
   if (TMath::Abs(vec.X()) > 10000 || TMath::Abs(vec.Y()) > 10000)
	  return false;
   
   return true;
}

//_____________________________________________________________________________
//  
Bool_t TAVTactNtuTrackH::FindVertices()
{
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   Int_t nTracks = pNtuTrack->GetTracksN();
   TList* ListVertex = GetListOfVertices();
   ListVertex->Delete();
   if (nTracks > 1) {
	  for (Int_t i = 0; i < nTracks; ++i) {
		 for (Int_t j = i+1; j < nTracks; ++j) {
			TAVTtrack* track0 = pNtuTrack->GetTrack(i);
			TAVTtrack* track1 = pNtuTrack->GetTrack(j);
			TVector2 pos = track0->DistanceMin(track1);
			if (TMath::Abs(pos.X()) < TAVTbaseParGeo::GetTargetWidth()/2. && pos.Y() < fSearchClusDistance) {
			   TVector3* vec = new TVector3(0.,0.,0.);
			   vec->SetXYZ(line.GetSlopeZ()(0)*pos.X() + line.GetOrigin()(0),track0->GetSlopeZ()(1)*pos.X() + track0->GetOrigin()(1),pos.X());
			   TVector3* veclast = (TVector3*)ListVertex->Last();
			   if (!veclast){
				  ListVertex->Add(vec);
			   }
			   else {
				  Bool_t sameVtx = kFALSE;
				  for (Int_t ilist = 0; ilist < ListVertex->GetEntries(); ++ilist) {
					 TVector3* ivec = (TVector3*)ListVertex->At(ilist);
					 if (TMath::Abs(vec->X()-ivec->X()) < fSearchClusDistance && TMath::Abs(vec->Y()-ivec->Y()) < fSearchClusDistance)
						sameVtx = kTRUE;
				  }
				  if (!sameVtx)
					 ListVertex->Add(vec);
			   }
			}
		 }
	  } 
   }
   
   if (ListVertex->GetEntries() == 0)
	  return false;
   else 
	  return true;
}






