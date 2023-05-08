/*!
 \file TAGFselectorBack.cxx
 \brief  Class for track finding/selection in GenFit Global Reconstruction
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselectorBack.hxx"

/*!
 \class TAGFselectorBack
 \brief Class for backward track finding/selection in GenFit Global Reconstruction
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorBack::TAGFselectorBack() : TAGFselectorBase()
{
}



//----------------------------------------------------------------------------------------------------

//! \brief Main function of backtracking algorithm
void TAGFselectorBack::Categorize( ) {

	if (!TAGrecoManager::GetPar()->IncludeTW() || !TAGrecoManager::GetPar()->IncludeMSD())
	{
		Error("Categorize_Backtracking()", "TW and MSD are needed for backtracking!");
		exit(0);
	}

	if (m_debug > 1)
		Info("Categorize_Backtracking()", "Backtracking START!!");

	BackTracklets();

	if (m_debug > 1)
		Info("Categorize_Backtracking()", "BackTracklets created!");

	if (TAGrecoManager::GetPar()->IncludeIT())
	{
		if (m_debug > 1)
			Info("Categorize_Backtracking()", "Start of IT cycle!");
		CategorizeIT_back();
		if (m_debug > 1)
			Info("Categorize_Backtracking()", "End of IT cycle!");
	}

	if (TAGrecoManager::GetPar()->IncludeVT())
	{
		if (m_debug > 1)
			Info("Categorize_Backtracking()", "Start of VT cycle!");
		CategorizeVT_back();
		if (m_debug > 1)
			Info("Categorize_Backtracking()", "End of VT cycle!");
	}

	FillTrackCategoryMap();
}


//! \brief Find and categorize the possible back-tracklets in the event from TW-MSD point association
//!
//! This step uses a combinatorial approach to determine the possible track candiadtes of the events from TW and MSD points
void TAGFselectorBack::BackTracklets()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > 0)
			cout << "TAGFselectorBack::BackTracklets() -- no measurement found in TW layer for event " << gTAGroot->CurrentEventId().EventNumber() << "! Skipping...\n";
		return;
	}

	KalmanFitter* m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);
	int TrackCounter=0;
	// Cycle on TW points
	for (vector<AbsMeasurement*>::iterator itTW = m_allHitMeas->at( planeTW ).begin(); itTW != m_allHitMeas->at( planeTW ).end(); ++itTW) 
	{
		//LOCAL COORDS!!!!
		float xTW = (*itTW)->getRawHitCoords()(0);
		float yTW = (*itTW)->getRawHitCoords()(1);
		float zTW = 0;// m_SensorIDMap->GetFitPlane(m_SensorIDMap->GetFitPlaneTW())->getO().Z();
		TVector3 globPosTW = m_GeoTrafo->FromTWLocalToGlobal(TVector3(xTW, yTW, zTW));
		int MSDPlane1 = m_SensorIDMap->GetMinFitPlane("MSD");
		int MSDPlane2 = MSDPlane1+1;
		float xMSD = -100., yMSD = -100.;
		float zMSD = m_SensorIDMap->GetFitPlane(MSDPlane1)->getO().Z();

		//Cycle on the first 2 planes of MSD
		for(vector<AbsMeasurement*>::iterator itMSD1 = m_allHitMeas->at(MSDPlane1).begin(); itMSD1 != m_allHitMeas->at(MSDPlane1).end(); ++itMSD1)
		{
			bool isMSD1y = static_cast<PlanarMeasurement*>(*itMSD1)->getYview();

			if( isMSD1y )
				yMSD = (m_SensorIDMap->GetFitPlane(MSDPlane1)->toLab(TVector2((*itMSD1)->getRawHitCoords()(0), 0))).Y();
			// yMSD = m_GeoTrafo->FromMSDLocalToGlobal(TVector3(0, (*itMSD1)->getRawHitCoords()(0), 0)).Y(); //RZ: Check for local coordinates!!!!
			else
				xMSD = (m_SensorIDMap->GetFitPlane(MSDPlane1)->toLab(TVector2((*itMSD1)->getRawHitCoords()(0), 0))).X();
				// xMSD = m_GeoTrafo->FromMSDLocalToGlobal(TVector3((*itMSD1)->getRawHitCoords()(0), 0, 0)).X();

			for(vector<AbsMeasurement*>::iterator itMSD2 = m_allHitMeas->at(MSDPlane2).begin(); itMSD2 != m_allHitMeas->at(MSDPlane2).end(); ++itMSD2)
			{
				bool isMSD2y = static_cast<PlanarMeasurement*>(*itMSD2)->getYview();

				if( isMSD1y == isMSD2y )
					Warning("BackTracklets()", "MSD clusters have the same strip direction!!");


				if( isMSD2y )
					yMSD = (m_SensorIDMap->GetFitPlane(MSDPlane2)->toLab(TVector2((*itMSD2)->getRawHitCoords()(0), 0))).Y();
				else
					xMSD = (m_SensorIDMap->GetFitPlane(MSDPlane2)->toLab(TVector2((*itMSD2)->getRawHitCoords()(0), 0))).X();

				Track* testTrack = new Track();
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itMSD1)->clone() );
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itMSD2)->clone() );
				testTrack->insertMeasurement( static_cast<genfit::PlanarMeasurement*>(*itTW)->clone() );

				int Z_Hypo = GetChargeFromTW(testTrack);
				double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
				int A_Hypo = round(mass_Hypo/m_AMU);
				testTrack->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( Z_Hypo )));
				
				TVector3 pos(xMSD, yMSD, zMSD*0.99);
				TVector3 mom((globPosTW.x() - xMSD)/(globPosTW.z() - zMSD), (globPosTW.y() - yMSD)/(globPosTW.z() -zMSD) ,1);
				if(m_debug > 1)
				{
					cout << "BACKTRACKLET CANDIDATE::" << (*itTW)->getHitId() << "\t" << (*itMSD1)->getHitId() << "\t" << (*itMSD2)->getHitId() << endl;
					cout << Z_Hypo << "\t" << A_Hypo << endl;
					cout << "Pos::"; pos.Print();
					cout << "Mom::"; mom.Print();
				}

				// if(mom.Theta() > angularCoverage)
				// if(mom.Phi() !compatible w/ x,y coordinates quadrant)


				for ( int MSDnPlane = MSDPlane2+1; MSDnPlane <= m_SensorIDMap->GetMaxFitPlane("MSD"); MSDnPlane++ )
				{
					TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal( pos + mom*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
					
					if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) ) //RZ: should be ok since X,Y local coordinates of MSD are currently in the detector frame
						continue;

					int indexOfMinY = -1;
					int count = 0;
					double distanceInY = m_MSDtolerance;

					//RZ: TO BE CHECKED!! ADDED TO AVOID ERRORS
					// Bool_t areLightFragments = false;
					// if (areLightFragments) distanceInY = m_MSDtolerance*2;
					// loop all absMeas in the found IT plane

					if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
						if(m_debug > 0) cout << "TAGFselectorBack::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
						continue;
					}

					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){

						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != MSDnPlane )	cout << "TAGFselectorBack::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

						//RZ: CHECK -> AVOID ERRORS
						double distanceFromHit;
						// string strip;

						if ( ! static_cast<PlanarMeasurement*>(*it)->getYview() )
							distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
						else
							distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));

						// find hit at minimum distance
						if ( distanceFromHit < distanceInY ){
							distanceInY = distanceFromHit;
							indexOfMinY = count;
						}
						count++;
					}

					// insert measurementi in GF Track
					if (indexOfMinY != -1){

						AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(MSDnPlane).at(indexOfMinY)))->clone();
						testTrack->insertMeasurement( hitToAdd, testTrack->getNumPointsWithMeasurement()-1 );
					}

				} // end loop MSD planes

				// cout << "Found testTrack with " << testTrack->getNumPointsWithMeasurement() << " points" << endl;

				if( testTrack->getNumPointsWithMeasurement() < 5 )
				{
					delete testTrack;
					continue;
				}

				try
				{
					mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo )*0.95);
					testTrack->setStateSeed(pos, mom);

					m_fitter_extrapolation->processTrackWithRep(testTrack, testTrack->getCardinalRep() );

					StateOnPlane tempState = testTrack->getFittedState(-1);

					// cout << "TW::"; tempState.getPos().Print();

					TVector2 TWcoords(xTW, yTW);

					// cout << "dist::" << (tempState.getPos().XYvector() - TWcoords).Mod() << endl;

					if( (tempState.getPos().XYvector() - TWcoords).Mod() > 3 )
					{
						if( m_debug > 0 )
							Info("BackTracklets()", "Found wrong MSD-TW point association! Removing track...");
						delete testTrack;
						continue;
					}

					// tempState = testTrack->getFittedState(0);

					// testTrack->getCardinalRep()->extrapolateToPoint( tempState, TVector3(0,0,0));
					
					// //Check if 
					// cout << "TGT::"; tempState.getPos().Print();
					m_trackTempMap[TrackCounter] = testTrack;
					TrackCounter++;
				}
				catch(genfit::Exception& e)
				{
					std::cerr << e.what() << '\n';
					std::cerr << "Exception for backward tracklet fitting! Skipping..." << std::endl;
					continue;
				}
				
			}
		}

	}
	delete m_fitter_extrapolation;

	return;
}


//! \brief Track selection at the IT level
//!
//! Set-up but currently not implemented
void TAGFselectorBack::CategorizeIT_back()
{
	// cout << "IT back" << endl;
	return;
}


//! \brief Track selection at the VT level
void TAGFselectorBack::CategorizeVT_back()
{
	// cout << "VT back start" << endl;

	int maxVTplane = m_SensorIDMap->GetMaxFitPlane("VT");
	int minVTplane = m_SensorIDMap->GetMinFitPlane("VT");

	KalmanFitter *m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);

	for(auto itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		itTrack->second->reverseTrack();
		m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());

		for(int VTplane = maxVTplane; VTplane != minVTplane-1; VTplane--)
		{
			TVector3 guessOnVT = ExtrapolateToOuterTracker(itTrack->second, VTplane);
			if (!m_SensorIDMap->GetFitPlane(VTplane)->isInActive(guessOnVT.x(), guessOnVT.y())) // RZ: should be ok since X,Y local coordinates of MSD are currently in the detector frame
				continue;

			int indexOfMinDist = -1;
			int count = 0;
			double minDist = m_VTtolerance;

			if (m_allHitMeas->find(VTplane) == m_allHitMeas->end())
			{
				if (m_debug > 0)
					Info("CategorizeVT_back()", "No measurement found in VTplane %d", VTplane);
				continue;
			}

			for (vector<AbsMeasurement *>::iterator it = m_allHitMeas->at(VTplane).begin(); it != m_allHitMeas->at(VTplane).end(); ++it)
			{
				// cout << "VT plane::" << VTplane << endl;
				if (m_SensorIDMap->GetFitPlaneIDFromMeasID((*it)->getHitId()) != VTplane)
					cout << "TAGFselectorBack::Categorize_dataLike() --> ERROR VT" << endl, exit(0);

				// RZ: CHECK -> AVOID ERRORS
				double distanceFromHit = TMath::Sqrt( pow(guessOnVT.x() - (*it)->getRawHitCoords()(0), 2) + pow(guessOnVT.y() - (*it)->getRawHitCoords()(1), 2) );

				// find hit at minimum distance
				if (distanceFromHit < minDist)
				{
					minDist = distanceFromHit;
					indexOfMinDist = count;
				}
				count++;
			}

			// insert measurementi in GF Track
			if (indexOfMinDist != -1)
			{
				// cout << "Found point!!" << endl;
				AbsMeasurement *hitToAdd = (static_cast<genfit::PlanarMeasurement *>(m_allHitMeas->at(VTplane).at(indexOfMinDist)))->clone();
				(itTrack->second)->insertMeasurement(hitToAdd);
			}
		}
		(itTrack->second)->reverseTrack();
		m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());
	}

	// cout << "VT back end" << endl;
	delete m_fitter_extrapolation;
	return;
}