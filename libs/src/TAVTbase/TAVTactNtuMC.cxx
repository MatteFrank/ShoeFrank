/*!
  \file
  \version $Id: TAVTactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAVTactNtuMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAVTparGeo.hxx"
#include "TAVTparMap.hxx"
#include "TAVTparConf.hxx"

#include "TAVTntuRaw.hxx"


#include "TAVTactNtuMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAVTactNtuMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAVTactNtuMC);


Bool_t  TAVTactNtuMC::fgPileup          = true;
Float_t TAVTactNtuMC::fgPoissonPar      = 0.736; // ajust for FIRST
Int_t   TAVTactNtuMC::fgPileupEventsN   = 10;
Float_t TAVTactNtuMC::fgSigmaNoiseLevel = -1.;
Int_t   TAVTactNtuMC::fgMcNoiseId       = -99;


//------------------------------------------+-----------------------------------
//
TAVTactNtuMC::TAVTactNtuMC(const char* name,
			   TAGdataDsc* pNtuRaw, 
			   TAGparaDsc* pGeoMap,
			   TAGparaDsc* pParMap,
			   EVENT_STRUCT* evStr)
  : TAGaction(name, "TAVTactNtuMC - NTuplize Vertex raw data"),
	fpNtuRaw(pNtuRaw),
	fpGeoMap(pGeoMap),
	fpParMap(pParMap),
	fpEvtStr(evStr),
	// fDigitizer(new TAVTdigitizerE(pGeoMap)),
	fNoisyPixelsN(0),
	fDebugLevel(0)
{
	AddDataOut(pNtuRaw, "TAVTntuRaw");
	AddPara(pGeoMap, "TAVTparGeo");
	AddPara(pParMap, "TAVTparMap");

	fDigitizer = new TAVTdigitizerE(pGeoMap);

	fpHisPoisson = (TH1F*)gDirectory->FindObject("vtPoisson");
	if (fpHisPoisson == 0x0) {
	   
		Double_t tot = 0.;
		Double_t par = fgPoissonPar;

		for (Int_t i = 1; i < 10; ++i) {
			tot += TMath::PoissonI(i, par);
		}

		fpHisPoisson = new TH1F("vtPoisson", "Poisson", 12, -0.5, 11.5);

		for (Int_t i = 1; i < 10; ++i) {
			Float_t val = TMath::PoissonI(i, par)/tot*100.;
			fpHisPoisson->Fill(i, val);
		}
	}

}






//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactNtuMC::CreateHistogram() {

	DeleteHistogram();
	
	// TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
	
	// for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	//    fpHisPixel[i] = new TH1F(Form("vtMcPixel%d", i+1), Form("Vertex - MC # pixels per clusters for sensor %d", i+1), 100, 0., 100.);
	//    AddHistogram(fpHisPixel[i]);
	// }
	
	
	// for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	//    if (TAVTparConf::IsMapHistOn()) {
	//       fpHisPixelMap[i]  = new TH2F(Form("vtMcPixelMap%d", i+1) , Form("Vertex - pixel map for sensor %d", i+1),
	//                                    pGeoMap->GetNPixelX(), 0, pGeoMap->GetNPixelX(),
	//                                    pGeoMap->GetNPixelY(), 0, pGeoMap->GetNPixelY());
	//       fpHisPixelMap[i]->SetStats(kFALSE);
	//       AddHistogram(fpHisPixelMap[i]);
	//    }
	// }
	
	// for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	//    if (TAVTparConf::IsMapHistOn()) {
	//       fpHisPosMap[i] =  new TH2F(Form("vtMcPosMap%d", i+1), Form("Vertex - position map for sensor %d", i+1),
	//                                  pGeoMap->GetNPixelX(), -pGeoMap->GetPitchX()/2*pGeoMap->GetNPixelX(), pGeoMap->GetPitchX()/2*pGeoMap->GetNPixelX(),
	//                                  pGeoMap->GetNPixelY(), -pGeoMap->GetPitchY()/2*pGeoMap->GetNPixelY(), pGeoMap->GetPitchY()/2*pGeoMap->GetNPixelY());
	//       fpHisPosMap[i]->SetStats(kFALSE);
	//       AddHistogram(fpHisPosMap[i]);
	//    }
	   
	// }
	
	SetValidHistogram(kTRUE);
}





//------------------------------------------+-----------------------------------
//! Action.
bool TAVTactNtuMC::Action() {
  
	if ( GlobalPar::GetPar()->Debug() > 0 )     
	  cout <<endl<< "TAVTactNtuMC::Action() start  -->  VTn :  " << fpEvtStr->VTXn << endl;

	TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();     
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
	pNtuRaw->Clear();

	static Int_t storedEvents = 0;
	std::vector<RawMcHit_t> storedEvtInfo;
	RawMcHit_t mcHit;

	vector<int> blackList;
	// Loop over all MC hits
	for (Int_t i = 0; i < fpEvtStr->VTXn; i++) {
		if ( GlobalPar::GetPar()->Debug() > 0 )     cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;

		// !!  in ntuple, the row and col start from 0  !!!
		int myTrow, myTcol;
		myTrow = fpEvtStr->VTXirow[i];
		myTcol = fpEvtStr->VTXicol[i];
		int sensorId = pGeoMap->GetSensorID( fpEvtStr->VTXilay[i], myTcol, myTrow );
		

	 

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		////////  "DECLUSTER" HACK waiting for clustering   ///////////////////////////////////////
        // remove all the hits adiacent to the current one


		// check if the current event is in the blacklist. if so skip.
		bool skip = false;
		for ( int bl = 0; bl<blackList.size(); bl++ ) {
			if ( blackList.at(bl) == i ) {
			  skip = true;
			  continue;
			}
		}
		if ( skip )            continue;   // next event


		// DECLUSTER
		  bool decluster = false;
		  for ( int j = i+1; j < fpEvtStr->VTXn; j++) {   // other hit loop

			// same sensor .....
			bool decluster_inner = false;
			for ( int k = -1; k <= 1; k++ ) {
			  for ( int h = -1; h <= 1; h++ ) {
				if   ( myTrow == fpEvtStr->VTXirow[j]+k && myTcol == fpEvtStr->VTXicol[j]+h )   {
				  decluster_inner = true;
				  break;
				}
			  }
			  if ( decluster_inner )    break;
			}

			if ( decluster_inner ) {
			   blackList.push_back( j );
			   decluster = true;
			 }

		  }
		  if ( decluster )   {
			blackList.push_back( i );
			// continue;  // next event
		  }
			
		  // DECLUSTER end

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	   
		// Generated particle ID 
		int genPartID = fpEvtStr->VTXid[i] - 1;
		TAVTntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., myTrow, myTcol, "mc_hit", i, genPartID );
		
		// // Digitization test
		// for ( int io=-10; io<10; io++ ) {
		//     if ( myTrow+io >= pGeoMap->GetNPixelY() ||  myTrow+io < 0 ) continue;
		//     for ( int jo=-10; jo<10; jo++ ) {
		//     	if ( myTcol+jo >= pGeoMap->GetNPixelX() ||  myTcol+jo < 0 ) continue;
		//         pNtuRaw->NewPixel(sensorId, 1., myTrow+io, myTcol+jo, "mc_cluster", pixel );
		//     }
		// }

		// used for pileup ...
		if (fgPileup && storedEvents <= fgPileupEventsN) {
			mcHit.id  = sensorId;
			mcHit.de  = fpEvtStr->VTXde[i];
			mcHit.x   = fpEvtStr->VTXpxin[i];
			mcHit.y   = fpEvtStr->VTXpyin[i];
			mcHit.zi  = fpEvtStr->VTXzin[i];
			mcHit.zo  = fpEvtStr->VTXpxout[i];
			storedEvtInfo.push_back(mcHit);
		}




// ************  fDigitizer  ****************************************************************************************

		// gives all empty
		if (!fDigitizer->Process(fpEvtStr->VTXde[i], fpEvtStr->VTXpxin[i], fpEvtStr->VTXpyin[i], fpEvtStr->VTXzin[i], fpEvtStr->VTXpxout[i])) continue;     // when false?
		FillPixels( pixel, sensorId, i );
		
// ****************************************************************************************************

		// fill MC info
		SetMCinfo( pixel, i );


		if (ValidHistogram()) {
			Int_t pixelsN = fDigitizer->GetPixelsN();
			fpHisPixel[sensorId]->Fill(pixelsN);
		}


   }
   blackList.clear();

   // PILEUP

   // if (fgPileup && storedEvents <= fgPileupEventsN) {
   //     fStoredEvents.push_back(storedEvtInfo);
   //     storedEvents++;
   //  }
	
   //  if (fgPileup && storedEvents >= fgPileupEventsN)
   //     GeneratePileup();


   //  if(fDebugLevel) {
   //     std::vector<RawMcHit_t> mcInfo;
   //     if (fgPileup && storedEvents <= fgPileupEventsN) {
   //        for (Int_t i = 0; i < fStoredEvents.size(); ++i) {
   //           printf("Event %d\n", i);
   //           mcInfo = fStoredEvents[i];
   //           for (Int_t j = 0; j < mcInfo.size(); ++j) {
   //              RawMcHit_t hit = mcInfo[j];
   //              printf("id %d de %.4f x %.4f y %.4f\n", hit.id, hit.de, hit.x, hit.y);
   //           }
   //        }
   //     }
   //  }

   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}





//------------------------------------------+-----------------------------------
void TAVTactNtuMC::FillPixels ( TAVTntuHit* originatingHit, int sensorId, int hitId ) {
   
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
 
	map<int, int> digiMap = fDigitizer->GetMap();
	int nPixelX = fDigitizer->GetNPixelX();
 
	// fill pixels from map
	for ( map< int, int >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {

	   if ( digiMap[it->first] == 1 ) {

			int line = it->first / nPixelX;
			int col  = it->first % nPixelX;


			TAVTntuHit* pixel = (TAVTntuHit*)pNtuRaw->NewPixel(sensorId, 1., line, col, "mc_cluster", originatingHit);
			
			if ( GlobalPar::GetPar()->Debug() > 0 )
				printf("line %d col %d\n", line, col);

			// sensor frame
			double v = pGeoMap->GetPositionV(line);
			double u = pGeoMap->GetPositionU(col);

			if (ValidHistogram()) {
				fpHisPixelMap[sensorId]->Fill(line, col);
				fpHisPosMap[sensorId]->Fill(u, v);
			}
		}
   }
}





//------------------------------------------+-----------------------------------
void TAVTactNtuMC::FillPixels ( int sensorId, int hitId ) {
   
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
 
	map<int, int> digiMap = fDigitizer->GetMap();
	int nPixelX = fDigitizer->GetNPixelX();
 
	// fill pixels from map
	for ( map< int, int >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {

	   if ( digiMap[it->first] == 1 ) {

			int line = it->first / nPixelX;
			int col  = it->first % nPixelX;


			TAVTntuHit* pixel = (TAVTntuHit*)pNtuRaw->NewPixel(sensorId, 1., line, col, "pileup");


			if ( GlobalPar::GetPar()->Debug() > 0 )
				printf("line %d col %d\n", line, col);

			double v = pGeoMap->GetPositionV(line);
			double u = pGeoMap->GetPositionU(col);

			if (ValidHistogram()) {
				fpHisPixelMap[sensorId]->Fill(line, col);
				fpHisPosMap[sensorId]->Fill(u, v);
			}
		}
   }
}






// --------------------------------------------------------------------------------------
void TAVTactNtuMC::ComputeNoiseLevel() {

	// computing number of noise pixels (sigma level) from gaussian
	TF1* f = new TF1("f", "gaus", -10, 10);
	f->SetParameters(1,0,1);
	Float_t fraction = 0;
	
	if (fgSigmaNoiseLevel > 0) {
	   fraction = f->Integral(-fgSigmaNoiseLevel, fgSigmaNoiseLevel)/TMath::Sqrt(2*TMath::Pi());
	   fNoisyPixelsN = TMath::Nint(fDigitizer->GetNPixelX()*fDigitizer->GetNPixelY()*(1.-fraction));
	}
	
	if (fDebugLevel)
	   printf("Number of noise pixels %d\n", fNoisyPixelsN);
	
	delete f;
}






//___________________________________
void TAVTactNtuMC::FillNoise()
{
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
	   FillNoise(i);
	}
}




//___________________________________
void TAVTactNtuMC::FillNoise(Int_t sensorId) {

	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetNPixelX());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetNPixelY());
	   // TAVTntuHit* pixel = (TAVTntuHit*)pNtuRaw->NewPixel(sensorId, 1., line, col);
	   TAVTntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col, "mc_noise" );
	   double v = pGeoMap->GetPositionV(line);
	   double u = pGeoMap->GetPositionU(col);
	   TVector3 pos(v,u,0);
	   // pixel->SetPosition(pos);     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	   // pixel->SetMCid(fgMcNoiseId);
	}
}




//------------------------------------------+-----------------------------------
void TAVTactNtuMC::SetMCinfo(TAVTntuHit* pixel, int hitId) {
	
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	int genPartID = fpEvtStr->VTXid[hitId] - 1;

	// check true particle ID linked to the hit is in the correct range
	if ( genPartID < 0 || genPartID > fpEvtStr->TRn-1 ) {
	   cout << "TAVTactNtuMC::Action :: ERROR >> wrong generate particle ID: "<< genPartID << " nPart= " << fpEvtStr->TRn << endl;
	   exit(0);
	}

	// take only hits linked to specific particle
	// if ( fpEvtStr->TRfid[genPartID] != -6 || fpEvtStr->TRcha[genPartID] != 2 )    continue;
	// cout << "ALPHA found! "  << endl;
	// if ( fpEvtStr->TRfid[genPartID] != -2 || fpEvtStr->TRcha[genPartID] != 3 )    continue;
	// cout << "LITIUM found! "  << endl;
	// if ( fpEvtStr->TRfid[genPartID] != -2 || fpEvtStr->TRcha[genPartID] != 6 )    continue;
	// if ( fpEvtStr->TRfid[genPartID] != -2 || fpEvtStr->TRcha[genPartID] != 6 ||
	//               fpEvtStr->TRmass[genPartID] < 10 ||  fpEvtStr->TRmass[genPartID] > 10.5 )    continue;
	// if ( GlobalPar::GetPar()->Debug() > 0 )     cout << "CARBONIUM found! "  << endl;

	if ( GlobalPar::GetPar()->Debug() > 0 )     {
		cout << "Part type: " << fpEvtStr->TRfid[genPartID] << " and charge: " << fpEvtStr->TRcha[genPartID] << endl;
		// double momentum = sqrt( fpEvtStr->TRipx[genPartID]*fpEvtStr->TRipx[genPartID] +
		//                   fpEvtStr->TRipy[genPartID]*fpEvtStr->TRipy[genPartID] +
		//                   fpEvtStr->TRipz[genPartID]*fpEvtStr->TRipz[genPartID] );
		// cout << "\t\t\tmomentum: " << momentum << endl;
		cout << "Generated Position: " << fpEvtStr->TRix[genPartID] <<" "<<fpEvtStr->TRiy[genPartID]<<" "<<fpEvtStr->TRiz[genPartID] << endl;
		cout << "Generated Momentum: " << fpEvtStr->TRipx[genPartID] <<" "<<fpEvtStr->TRipy[genPartID]<<" "<<fpEvtStr->TRipz[genPartID] << endl;
	}


	// global coordinates
	TVector3 MCpos = TVector3((fpEvtStr->VTXxin[hitId]  + fpEvtStr->VTXxout[hitId])/2,  (fpEvtStr->VTXyin[hitId]  + fpEvtStr->VTXyout[hitId])/2,  (fpEvtStr->VTXzin[hitId]  + fpEvtStr->VTXzout[hitId])/2);
	TVector3 MCmom = TVector3((fpEvtStr->VTXpxin[hitId] + fpEvtStr->VTXpxout[hitId])/2, (fpEvtStr->VTXpyin[hitId] + fpEvtStr->VTXpyout[hitId])/2, (fpEvtStr->VTXpzin[hitId] + fpEvtStr->VTXpzout[hitId])/2);    
	
	if ( GlobalPar::GetPar()->Debug() > 0 )     {
	   cout << "Vertex pixel hit n: " << hitId << ". Col " << pixel->GetPixelColumn() << " row "<< pixel->GetPixelLine() << endl;
	   cout << "\tGlobal kinematic: \n\t\tPos:\t";
	   MCpos.Print();
	   cout << "\t\tMom:\t";
	   MCmom.Print();
	}
	
	
	pixel->SetMCPosition(MCpos);   // set in local coord (transformation in Hit)
	pixel->SetMCMomentum(MCmom);   // set in local coord
	pixel->SetEneLoss(fpEvtStr->VTXde[hitId]);  
	
	// store generated particle info
	pixel->SetGeneratedParticleInfo( genPartID, fpEvtStr->TRfid[genPartID], fpEvtStr->TRcha[genPartID],
									fpEvtStr->TRbar[genPartID], fpEvtStr->TRmass[genPartID],
									TVector3(fpEvtStr->TRix[genPartID], fpEvtStr->TRiy[genPartID], fpEvtStr->TRiz[genPartID]),
									TVector3(fpEvtStr->TRipx[genPartID], fpEvtStr->TRipy[genPartID], fpEvtStr->TRipz[genPartID]) );
}





//------------------------------------------+-----------------------------------
void  TAVTactNtuMC::GeneratePileup()
{
	Int_t pileupEvents = TMath::Nint(fpHisPoisson->GetRandom())-1;

	
	// form pileup events number pull out randomly the stored events
	std::vector<int> rarray;

	for (Int_t i = 0; i < fgPileupEventsN; ++i) {
	   if (i > pileupEvents-1)
		  rarray.push_back(0);
	   else
		  rarray.push_back(1);
	}
	
	std::random_shuffle (rarray.begin(), rarray.end(), TAVTdigitizer::GetRandom);

	std::vector<RawMcHit_t> mcInfo;

	for (Int_t p = 0; p < fgPileupEventsN; ++p) {
	   
	   if (rarray[p] == 0) continue;
	   
	   mcInfo = fStoredEvents[p];
	   
	   for (Int_t j = 0; j < mcInfo.size(); ++j) {
		  RawMcHit_t hit = mcInfo[j];
		  
		  if (!fDigitizer->Process(hit.de, hit.x, hit.y, hit.zi, hit.zo)) continue;
		  FillPixels( hit.id, -1); // !!!!!!!!!!!!!!!!!!!!!
	   }
	}
}













































