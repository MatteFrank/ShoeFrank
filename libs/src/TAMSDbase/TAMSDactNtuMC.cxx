/*!
  \file
  \version $Id: TAMSDactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAMSDactNtuMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"

#include "TAMSDntuRaw.hxx"
#include "TAMSD_ContainerPoint.hxx"



#include "TAMSDactNtuMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAMSDactNtuMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAMSDactNtuMC);

Float_t TAMSDactNtuMC::fgSigmaNoiseLevel = -1.;
Int_t   TAMSDactNtuMC::fgMcNoiseId       = -99;

//------------------------------------------+-----------------------------------
//
TAMSDactNtuMC::TAMSDactNtuMC(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
:  TAGaction(name, "TAMSDactNtuMC - NTuplize MSD MC data"),
   fpNtuRaw(pNtuRaw),
   fpGeoMap(pGeoMap),
   fpEvtStr(evStr),
   fNoisyStripsN(0)
{
	AddDataOut(pNtuRaw, "TAMSDntuRaw");
	AddPara(pGeoMap, "TAMSDparGeo");

   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuMC::CreateHistogram()
{
  
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Multi Strip Detector";
  
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStrip[i] = new TH1F(Form("%sMcStripl%d", prefix.Data(), i+1), Form("%s - MC # strip per clusters for sensor %d", titleDev.Data(), i+1), 100, 0., 100.);
    AddHistogram(fpHisStrip[i]);
  }
  
  fpHisStripTot = new TH1F(Form("%sMcStripTot", prefix.Data()), Form("%s - MC # total strips per clusters", titleDev.Data()), 100, 0., 100.);
  AddHistogram(fpHisStripTot);
  
  fpHisDeTot = new TH1F(Form("%sMcDeTot", prefix.Data()), Form("%s - MC total energy loss", titleDev.Data()), 1000, 0., 10000.);
  AddHistogram(fpHisDeTot);
  
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisDeSensor[i] = new TH1F(Form("%sMcDe%d", prefix.Data(), i+1), Form("%s - MC energy loss for sensor %d", titleDev.Data(), i+1), 1000, 0., 10000.);
    AddHistogram(fpHisDeSensor[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStripMap[i]  = new TH1F(Form("%sMcStripMap%d", prefix.Data(), i+1) , Form("%s - MC strip map for sensor %d", titleDev.Data(), i+1),
				 pGeoMap->GetNStrip(), 0, pGeoMap->GetNStrip());
    AddHistogram(fpHisStripMap[i]);
    
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisPosMap[i] =  new TH1F(Form("%sMcPosMap%d", prefix.Data(), i+1), Form("%s - MC position map for sensor %d", titleDev.Data(), i+1),
			       pGeoMap->GetNStrip(), -pGeoMap->GetPitch()/2.*pGeoMap->GetNStrip(), pGeoMap->GetPitch()/2.*pGeoMap->GetNStrip());
    AddHistogram(fpHisPosMap[i]);
    
  }
   
  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Create histogram
void TAMSDactNtuMC::CreateDigitizer()
{
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
   fDigitizer = new TAMSDdigitizer(pGeoMap);
   if (fgSigmaNoiseLevel > 0)
      ComputeNoiseLevel();
}


//------------------------------------------+-----------------------------------
//! Action.
bool TAMSDactNtuMC::Action()
{
	if ( GlobalPar::GetPar()->Debug()> 0 )     
	  Info("TAMSDactNtuMC::Action()", "start  -->  VTn : %d  ", fpEvtStr->MSDn);

	TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
	TAMSDntuRaw* containerHit = (TAMSDntuRaw*) fpNtuRaw->Object();
	containerHit->Clear();
   
	// Loop over all MC hits
	for (Int_t i = 0; i < fpEvtStr->MSDn; i++) {
		if ( GlobalPar::GetPar()->Debug()> 0 )     cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;

		Int_t sensorId = fpEvtStr->MSDilay[i];

		// Digitizing
      if (ValidHistogram()) {
         fpHisDeTot->Fill(fpEvtStr->MSDde[i]*TAGgeoTrafo::GevToKev());
         fpHisDeSensor[sensorId]->Fill(fpEvtStr->MSDde[i]*TAGgeoTrafo::GevToKev());
      }
            
      // Go to sensor frame
      TVector3 posIn(fpEvtStr->MSDxin[i], fpEvtStr->MSDyin[i], fpEvtStr->MSDzin[i]);
      TVector3 posOut(fpEvtStr->MSDxout[i], fpEvtStr->MSDyout[i], fpEvtStr->MSDzout[i]);
      posIn  = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      // if (!fDigitizer->Process(fpEvtStr->MSDde[i], posIn[0], posIn[1], posIn[2], posOut[2])) continue;
		FillStrips(sensorId, i);
		
		if (ValidHistogram()) {
         Int_t stripsN = fDigitizer->GetMap().size();
         fpHisStrip[sensorId]->Fill(stripsN);
         fpHisStripTot->Fill(stripsN);
		}
   }


if ( GlobalPar::GetPar()->Debug() > 0 )	cout<< endl<< endl << "Hits - " << endl;
    // container of points
	TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "Geo - " << endl;
    TAMSD_ContainerPoint* containerPoint = (TAMSD_ContainerPoint*) gTAGroot->FindDataDsc("containerPoint", "TAMSD_ContainerPoint")->Object();
    if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "Container - " << endl;
    bool xyOrder = true; 

    float posMSDplane;
    int iLayer = -1;
    

    // loop over layers, to consider sensor column and row. 
    // count sensors two by tow
    for (int iLayer=0; iLayer < 6; iLayer += 2 ) {  
    // for (int iLayer=0; iLayer < containerHit->GetNSensors(); iLayer+2 ) {  
if ( GlobalPar::GetPar()->Debug() > 0 )	cout << endl<< "Layer - " << iLayer << endl;

		if (iLayer == 0	|| iLayer == 1)  	posMSDplane = -5;
		else if (iLayer == 2 || iLayer == 3)  	posMSDplane = 0;
		else if (iLayer == 4 || iLayer == 5)  	posMSDplane = 5;

        // built the points from the hits
        for (int iCol=0; iCol < containerHit->GetStripsN( iLayer ); iCol++) {  // col loop
            if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "iCol - " << iCol << endl;
            // FOR TEST, REMOVE
            // if ( !containerHit->GetStrip( 0, iCol )->IsColumn() )       cout <<"TAMSDactNtuMC::Action() Col"<<endl, exit(0);
            
            TAMSDntuHit* colHit = containerHit->GetStrip( iLayer, iCol );
           	if ( colHit->GetView() == 0 ) 	xyOrder = true;
           	else 							xyOrder = false;
           	if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "View - " << colHit->GetView() << endl;
            for (int iRow=0; iRow < containerHit->GetStripsN( iLayer+1 ); iRow++) {  // row loop 
                if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "iRow - " << iRow << endl;
                // FOR TEST, REMOVE
                // if ( !containerHit->GetStrip( 1, iRow )->IsRow() )        cout <<"TAMSDactNtuMC::Action() Row"<<endl, exit(0);
                TAMSDntuHit* rowHit = containerHit->GetStrip( iLayer+1, iRow );

                if ( !(rowHit->GetView() == 1 && xyOrder) ) 	cout << "ERROR" << endl;

                // TVector3 rowPos = rowHit->GetHitPosition_detector();
                // TVector3 colPos = colHit->GetHitPosition_detector();
                // TVector3 position ( colHit->GetHitCoordinate_detectorFrame().x(), rowHit->GetHitCoordinate_detectorFrame().y(), 
                //                     ( colHit->GetHitCoordinate_detectorFrame().z() + rowHit->GetHitCoordinate_detectorFrame().z() )/2 );

                // float pointX = 
                // float pointX = 
                // float pointZ = rowHit->GetHitPosition_detector().z() ;

                TVector3 rowPos = 	( 	geoTrafo->FromMSDLocalToGlobal( TVector3( 	colHit->GetPosition(), 
																					rowHit->GetPosition(),
																					posMSDplane ) 
																		)
									);
                 if ( GlobalPar::GetPar()->Debug() > 0 )	{cout << "Position - ";     rowPos.Print(); }
                // containerPoint->NewPoint( iCol, colHit, iRow, rowHit );

                // TAMSD_Point* point = containerPoint->NewPoint( iCol iRow, rowPos );

                // one point for each layer (half sensor number)
                int pointLayer = trunc( iLayer/2 );
                TAMSD_Point* point = containerPoint->NewPoint( pointLayer, iRow, iCol, rowPos );
                if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "Point - " << pointLayer << endl;
                // point->SetColumnBar( iRow, rowHit );    // set pos and mom inside
                // point->SetRowBar( iCol, colHit );       // set pos and mom inside

                int colGenParticleID = colHit->GetMcIndex(0);
                int rowGenParticleID = rowHit->GetMcIndex(0);

                point->SetGeneratedParticle( colGenParticleID, rowGenParticleID );
if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "Gen - " << colGenParticleID << endl;
                // point->FindTrueGhost(...);
                // point->SetMCPosition( MCpos );
                // point->SetMCMomentum( MCmom );

                if ( GlobalPar::GetPar()->Debug() > 0 )     cout << "Point "<<iCol<<" "<<iRow<<" -> Col: "<<point->GetColumn()<<" row: "<<point->GetRow()<<endl;

                // ControlPlotsRepository::GetControlObject( "MSDcontrol" )->SetTW_HitPoint( "MSD___Point", point->GetColumn(), point->GetRow() );
            

            }
        }

    }



   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}


//------------------------------------------+-----------------------------------
void TAMSDactNtuMC::FillStrips(Int_t sensorId, Int_t hitId )
{
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  TAMSDntuRaw* pNtuRaw = (TAMSDntuRaw*) fpNtuRaw->Object();
  
  // Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  // map<int, double> digiMap = fDigitizer->GetMap();
  

	float possss;
	float view;
	if ( sensorId % 2 == 0 ) { //X
		 possss = fpEvtStr->MSDxin[hitId];
		 view = 0;

	}
	else {
		possss = fpEvtStr->MSDyin[hitId];
		view = 1;
	}

	int stripId = pGeoMap->GetStrip( view, possss );

	TAMSDntuHit* strip = (TAMSDntuHit*)pNtuRaw->NewStrip(sensorId, view, stripId);


	double pos = pGeoMap->GetPosition(view, stripId);

	strip->SetPosition(pos);
	if ( GlobalPar::GetPar()->Debug() > 0 )	cout << "SetPos - " << pos << "   -- stripID = " << stripId << endl;
	
	Int_t genPartID = fpEvtStr->MSDid[hitId] - 1;
	strip->AddMcTrackIdx(genPartID, hitId);
	
	// if (ValidHistogram()) {
	// fpHisStripMap[sensorId]->Fill(stripId);
	// fpHisPosMap[sensorId]->Fill(pos);
 //     }
 




  // // fill strips from map
  // int count = 0;
  // for ( map< int, double >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {
    
  //   if ( digiMap[it->first] > 0 ) {
		// count++;
		// int stripId = it->first;

		// TAMSDntuHit* strip = (TAMSDntuHit*)pNtuRaw->NewStrip(sensorId, digiMap[it->first], view, stripId);

		// Int_t genPartID = fpEvtStr->MSDid[hitId] - 1;
		// strip->AddMcTrackIdx(genPartID, hitId);

		// if ( GlobalPar::GetPar()->Debug()> 0 )
		// printf("strip %d\n", stripId);

		// double pos = pGeoMap->GetPosition(stripId);

		// strip->SetPosition(pos);
		// cout << "SetPos - " << pos << "   -- stripID = " << stripId << endl;

		// if (ValidHistogram()) {
		// fpHisStripMap[sensorId]->Fill(stripId);
		// fpHisPosMap[sensorId]->Fill(pos);
  //     }
  //   }
  // }
}

//___________________________________
void TAMSDactNtuMC::FillNoise()
{
   // TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   // for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
   //    FillNoise(i);
   // }
}

//___________________________________
void TAMSDactNtuMC::FillNoise(Int_t sensorId)
{
  // TAMSDntuRaw* pNtuRaw = (TAMSDntuRaw*) fpNtuRaw->Object();
  // TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  
  // Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  // Int_t stripsN = gRandom->Uniform(0, fNoisyStripsN);
  // for (Int_t i = 0; i < stripsN; ++i) {
  //   Int_t stripId  = gRandom->Uniform(0,fDigitizer->GetStripsN());
  //   TAMSDntuHit* strip = pNtuRaw->NewStrip(sensorId, 1., view, stripId);
  //   strip->AddMcTrackIdx(fgMcNoiseId);
  // }
}

// --------------------------------------------------------------------------------------
void TAMSDactNtuMC::ComputeNoiseLevel()
{
  // computing number of noise pixels (sigma level) from gaussian
  TF1* f = new TF1("f", "gaus", -10, 10);
  f->SetParameters(1,0,1);
  Float_t fraction = 0;
  
  if (fgSigmaNoiseLevel > 0) {
    fraction = f->Integral(-fgSigmaNoiseLevel, fgSigmaNoiseLevel)/TMath::Sqrt(2*TMath::Pi());
    fNoisyStripsN = TMath::Nint(fDigitizer->GetStripsN()*(1.-fraction));
  }
  
  if (GlobalPar::GetPar()->Debug())
      printf("Number of noise pixels %d\n", fNoisyStripsN);
  
  delete f;
}
