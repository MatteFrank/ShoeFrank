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
	m_VTtolerance = .5;
	m_ITtolerance = .5;
	m_MSDtolerance = .5;
	m_TWtolerance = 4.;
}


//----------------------------------------------------------------------------------------------------

//! \brief Main function of backtracking algorithm
void TAGFselectorBack::Categorize( ) {
	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "Backtracking START!!");

	if (! m_systemsON.Contains("TW") || ! m_systemsON.Contains("MSD"))
	{
		Error("Categorize_Backtracking()", "TW and MSD are needed for backtracking!");
		exit(42);
	}

	BackTracklets();

	if ( m_systemsON.Contains("IT"))
		CategorizeIT_back();

	if ( m_systemsON.Contains("VT"))
		CategorizeVT_back();

	FillTrackCategoryMap();
}


//! \brief Find and categorize the possible back-tracklets in the event from TW-MSD point association
//!
//! This step uses a combinatorial approach to determine the possible track candiadtes of the events from TW and MSD points
void TAGFselectorBack::BackTracklets()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(FootDebugLevel(1))
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
		float zTW = 0;
		TVector3 globPosTW = m_GeoTrafo->FromTWLocalToGlobal(TVector3(xTW, yTW, zTW));

		//Get central couple of MSD planes in order to start back tracklet creation from there
		int MSDPlane1 = m_SensorIDMap->GetMinFitPlane("MSD") + ((m_SensorIDMap->GetMaxFitPlane("MSD") - m_SensorIDMap->GetMinFitPlane("MSD"))/4)*2;
		int MSDPlane2 = MSDPlane1+1;
		float xMSD = -100., yMSD = -100.;
		float zMSD = m_SensorIDMap->GetFitPlane(MSDPlane1)->getO().Z();

		if( m_allHitMeas->find(MSDPlane1) == m_allHitMeas->end() )
		{
			Warning("BackTracklets()", "No MSD clusters in first plane!!");
			return;
		}

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

			if( m_allHitMeas->find(MSDPlane2) == m_allHitMeas->end() )
			{
				Warning("BackTracklets()", "No MSD clusters in second plane!!");
				return;
			}

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
				if(FootDebugLevel(2))
				{
					cout << "BACKTRACKLET CANDIDATE::" << (*itTW)->getHitId() << "\t" << (*itMSD1)->getHitId() << "\t" << (*itMSD2)->getHitId() << endl;
					cout << Z_Hypo << "\t" << A_Hypo << endl;
					cout << "Pos::"; pos.Print();
					cout << "Mom::"; mom.Print();
				}

				// Extrapolate to other planes
				for ( int MSDnPlane = m_SensorIDMap->GetMinFitPlane("MSD"); MSDnPlane <= m_SensorIDMap->GetMaxFitPlane("MSD"); MSDnPlane++ )
				{
					//Skip the seed planes
					if( MSDnPlane == MSDPlane1|| MSDnPlane == MSDPlane2 )
						continue;

					//Skip if no measurment is found in the plane
					if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
						if(FootDebugLevel(1))
							cout << "TAGFselectorBack::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
						continue;
					}

					//Extrapolate to current MSD plane and move to local coordinates
					TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal( pos + mom*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
					int iSensor;
					m_SensorIDMap->GetSensorID(MSDnPlane, &iSensor);
					guessOnMSD = m_MSD_geo->Detector2Sensor(iSensor, guessOnMSD);

					int indexOfMinDist = -1;
					int count = 0;
					double minDist = m_MSDtolerance;

					for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){

						if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != MSDnPlane )	cout << "TAGFselectorBack::Categorize() --> ERROR MSD" <<endl, exit(42);

						// Check distance of hit from extrapolation: works w/ 1st coordinate because its local
						double distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
						if ( FootDebugLevel(1) )
						{
							float guess = static_cast<PlanarMeasurement*>(*it)->getYview() ? guessOnMSD.Y() : guessOnMSD.X();
							
							cout << "Hit::" << (*it)->getHitId() << " " << ListMCparticlesOfHit((*it)->getHitId()) << "\tguess::" << guessOnMSD.X() << "\trawHitCoords::" << (*it)->getRawHitCoords()(0) << "\tdist::" << distanceFromHit<<endl;
						}

						// find hit at minimum distance
						if ( distanceFromHit < minDist ){
							minDist = distanceFromHit;
							indexOfMinDist = count;
						}
						count++;
					}

					// insert measurement in GF Track
					if (indexOfMinDist != -1){

						AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(MSDnPlane).at(indexOfMinDist)))->clone();
						testTrack->insertMeasurement( hitToAdd, testTrack->getNumPointsWithMeasurement()-1 );

						//Fill extrapolation distance histos
						if( h_extrapDist.size() > 0 )
						{
							int iCoord = static_cast<PlanarMeasurement*>(hitToAdd)->getYview() ? 1 : 0;
							std::pair<string, std::pair<int, int>> sensId = make_pair("MSD",make_pair(iSensor,iCoord));
							h_extrapDist[sensId]->Fill(guessOnMSD.X() - hitToAdd->getRawHitCoords()(0));
						}
					}
				} // end loop MSD planes


				//Check for a minimum number of clusters in the track candidate
				if( testTrack->getNumPointsWithMeasurement() < 5 )
				{
					if ( FootDebugLevel(1) )
						Info("BackTracklets()", "Too few points in back-tracklet candidate");
					delete testTrack;
					continue;
				}

				//Fit the track candidate for backward extrapolation in next steps
				try
				{
					int pointID = m_SensorIDMap->GetHitIDFromMeasID(testTrack->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId());
					float TOF = ( (TATWntuPoint*) gTAGroot->FindDataDsc(FootActionDscName("TATWntuPoint"))->Object() )->GetPoint( pointID )->GetMeanTof();
					float var = m_BeamEnergy/m_AMU;
					float beam_speed = TAGgeoTrafo::GetLightVelocity()*TMath::Sqrt(var*(var + 2))/(var + 1);
					TOF -= (m_GeoTrafo->GetTGCenter().Z()-m_GeoTrafo->GetSTCenter().Z())/beam_speed;
					float beta = (m_GeoTrafo->GetTWCenter().Z() - m_GeoTrafo->GetTGCenter().Z())/(TOF*TAGgeoTrafo::GetLightVelocity());
					mom.SetMag(mass_Hypo*beta/TMath::Sqrt(1 - pow(beta,2)));
					if( FootDebugLevel(2) )
						cout << "Track::"<< TrackCounter << "\tp::" << mom.Mag() << endl;
					
					testTrack->setStateSeed(pos, mom);
					m_fitter_extrapolation->processTrackWithRep(testTrack, testTrack->getCardinalRep() );
					if( testTrack->getFitStatus(testTrack->getCardinalRep())->getNFailedPoints() != 0 )
						throw genfit::Exception("Fit failed for backtracklet!", __LINE__, __FILE__);

					TVector3 TWguessGlb = testTrack->getFittedState(-1).getPos();
					TVector2 TWguessLoc = m_SensorIDMap->GetFitPlane( m_SensorIDMap->GetFitPlaneTW() )->LabToPlane( TWguessGlb );
					TVector2 TWcoords(xTW, yTW);

					if( FootDebugLevel(2) )
					{
						cout << "TWguess::" ;TWguessLoc.Print();
						cout << "TWmeas::"  ;TWcoords.Print();
					}

					if( (TWguessLoc - TWcoords).Mod() > 3 )
					{
						if( FootDebugLevel(1) )
							Info("BackTracklets()", "Found wrong MSD-TW point association! Removing track...");
						delete testTrack;
						continue;
					}

					// //Check if track falls inside TG... Is it that precise at this stage???
					// tempState = testTrack->getFittedState(0);
					// testTrack->getCardinalRep()->extrapolateToPoint( tempState, TVector3(0,0,0));
					// cout << "TGT::"; tempState.getPos().Print();

					m_trackTempMap[TrackCounter] = testTrack;
					TrackCounter++;
				}
				catch(genfit::Exception& e)
				{
					if( FootDebugLevel(2) )
					{
						std::cerr << e.what() << '\n';
						std::cerr << "Exception for backward tracklet fitting! Skipping..." << std::endl;
					}
					continue;
				}
			} //end loop on MSD plane2
		} //end loop on MSD plane1
	} //end loop on TW points
	delete m_fitter_extrapolation;

	if( m_IsMC && FootDebugLevel(1) )
	{
		cout << "End of TW-MSD tracking -> found these tracks\n";
		PrintCurrentTracksMC();
	}

	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "BackTracklets created!");
}


//! \brief Track selection at the IT level
void TAGFselectorBack::CategorizeIT_back()
{
	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "Start of IT cycle!");
	KalmanFitter *m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);

	// same index if VTX_tracklets (for one vertex..)
	vector<int> tracksToRemove;
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		if( FootDebugLevel(2) )
			cout << "Track::" << itTrack->first << endl;

		m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());
		int ITpointsAdded = 0;
		
		//Get some parameters for IT FitPlanes
		vector<float>* allZinIT = m_SensorIDMap->GetPossibleITzLocal();
		
		for ( int iz = allZinIT->size()-1; iz >= 0; --iz )
		{
			vector<int>* planesAtZ  = m_SensorIDMap->GetPlanesAtZLocal( allZinIT->at(iz) );

			int sensorMatch;
			double minDistFromHit = m_ITtolerance;
			double distanceInY;
			double distanceInX;
			TVector3 guessOnIT;
			TVector3 momGuessOnIT;
			int indexOfMinY = -1.;
			Int_t sensorId;
			for ( vector<int>::iterator iPlane = planesAtZ->begin(); iPlane != planesAtZ->end(); ++iPlane )
			{
				if( !m_SensorIDMap->GetSensorID(*iPlane, &sensorId) )
					Error("CategorizeIT_back()", "Sensor not found for Genfit plane %d!", *iPlane), exit(42);

				if ( m_allHitMeas->find( *iPlane ) == m_allHitMeas->end() )   {
					if( FootDebugLevel(2) )
						Info("CategorizeIT_back()", "Plane %d of IT has no clusters. Skipping...", *iPlane);
					continue;
				}

				TVector3 momGuessOnIT_dummy;
				guessOnIT = ExtrapolateToOuterTracker(itTrack->second, *iPlane, momGuessOnIT_dummy, true);
				if( FootDebugLevel(2) )
				{
					cout<<"PosGuess::";guessOnIT.Print();
					cout<<"MomGuess::";momGuessOnIT_dummy.Print();
				}

				//RZ: IsInActive???? Here the extrapolation could be slightly outside plane and still be right!!
				// if ( !m_SensorIDMap->GetFitPlane( *iPlane )->isInActiveY( guessOnIT.Y() ) )
				// {
				// 	if( FootDebugLevel(2) )
				// 		cout << "Extrapolation to IT not in active region of sensor " << *iPlane << endl;
				// 	continue;
				// }

				int count = 0;
				for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( *iPlane ).begin(); it != m_allHitMeas->at( *iPlane ).end(); ++it)
				{
					double distanceFromHit = TMath::Sqrt( pow(guessOnIT.X() - (*it)->getRawHitCoords()(0),2) + pow(guessOnIT.Y() - (*it)->getRawHitCoords()(1),2) );
					if( FootDebugLevel(2) )
						cout << "Hit::" << (*it)->getHitId() << " " << ListMCparticlesOfHit( (*it)->getHitId() ) << "\tguessX::" << guessOnIT.X() << "\trawHitX::" << (*it)->getRawHitCoords()(0)  << "\tdistX::" << fabs(guessOnIT.X() - (*it)->getRawHitCoords()(0)) << "\tguessY::" << guessOnIT.Y() << "\trawHitY::" << (*it)->getRawHitCoords()(1)  << "\tdistY::" << fabs(guessOnIT.Y() - (*it)->getRawHitCoords()(1)) << "\tp::" << momGuessOnIT_dummy.Mag() <<endl;

					// find hit at minimum distance
					if ( distanceFromHit < minDistFromHit )
					{
						distanceInY = fabs(guessOnIT.Y() - (*it)->getRawHitCoords()(1));
						distanceInX = fabs(guessOnIT.X() - (*it)->getRawHitCoords()(0));
						minDistFromHit = distanceFromHit;
						indexOfMinY = count;
						sensorMatch = *iPlane;
						momGuessOnIT = momGuessOnIT_dummy;
					}
					count++;
				}
			}	// end loop on IT planes

			//Insert measurement in GF track if found!
			if (indexOfMinY != -1)// && distanceInX < 1.)
			{
				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( m_allHitMeas->at(sensorMatch).at(indexOfMinY) ))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd, 0);

				//Fill extrapolation distance histos
				if( h_extrapDist.size() > 0 )
				{
					int iSensor;
					m_SensorIDMap->GetSensorID(sensorMatch, &iSensor);
					std::pair<string, std::pair<int, int>> sensId = make_pair("IT",make_pair(iSensor,0));
					h_extrapDist[sensId]->Fill(guessOnIT.X() - hitToAdd->getRawHitCoords()(0));
					sensId = make_pair("IT",make_pair(iSensor,1));
					h_extrapDist[sensId]->Fill(guessOnIT.Y() - hitToAdd->getRawHitCoords()(1));
				}

				guessOnIT = m_SensorIDMap->GetFitPlane(sensorMatch)->toLab( TVector2((hitToAdd)->getRawHitCoords()(0), (hitToAdd)->getRawHitCoords()(1)) );
				guessOnIT.SetZ(guessOnIT.Z()*0.99);

				if( FootDebugLevel(2) )
				{
					cout << "****SEED****"<<endl;
					guessOnIT.Print(); momGuessOnIT.Print();
				}
				(itTrack->second)->setStateSeed(guessOnIT,momGuessOnIT);
				m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());
			
				if( itTrack->second->getFitStatus(itTrack->second->getCardinalRep())->getNFailedPoints() != 0 )
				{
					tracksToRemove.push_back(itTrack->first);
					break;
				}
				ITpointsAdded++;
			}

		} // end loop over z

		//Save tracks w/ no point in IT in order to delete them
		if(ITpointsAdded == 0 && std::find(tracksToRemove.begin(), tracksToRemove.end(), itTrack->first) == tracksToRemove.end())
			tracksToRemove.push_back(itTrack->first);
	}	// end loop on GF Track candidates


	//Delete tracks that did not find any point in the IT
	for ( auto trackId : tracksToRemove )
	{
		delete m_trackTempMap[trackId];
		m_trackTempMap.erase(trackId);
	}

	if( m_IsMC && FootDebugLevel(1) )
	{
		cout << "End of IT tracking -> found these tracks\n";
		PrintCurrentTracksMC();
	}

	delete m_fitter_extrapolation;
	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "End of IT cycle!");
}


//! \brief Track selection at the VT level
void TAGFselectorBack::CategorizeVT_back()
{
	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "Start of VT cycle!");

	int maxVTplane = m_SensorIDMap->GetMaxFitPlane("VT");
	int minVTplane = m_SensorIDMap->GetMinFitPlane("VT");

	KalmanFitter *m_fitter_extrapolation = new KalmanFitter(1);
	m_fitter_extrapolation->setMaxIterations(1);

	vector<int> tracksToRemove;
	for(auto itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());
		if( FootDebugLevel(2) )
			cout << "Track::" << itTrack->first << endl;

		for(int VTplane = maxVTplane; VTplane != minVTplane-1; VTplane--)
		{
			//Skip if there is no measurement
			if (m_allHitMeas->find(VTplane) == m_allHitMeas->end())
			{
				if (FootDebugLevel(1))
					Info("CategorizeVT_back()", "No measurement found in VTplane %d", VTplane);
				continue;
			}

			// Extrapolate
			TVector3 momGuessOnVT;
			TVector3 guessOnVT = ExtrapolateToOuterTracker(itTrack->second, VTplane, momGuessOnVT, true);

			int indexOfMinDist = -1;
			int count = 0;
			double minDist = m_VTtolerance;

			for (vector<AbsMeasurement *>::iterator it = m_allHitMeas->at(VTplane).begin(); it != m_allHitMeas->at(VTplane).end(); ++it)
			{
				if (m_SensorIDMap->GetFitPlaneIDFromMeasID((*it)->getHitId()) != VTplane)
					cout << "TAGFselectorBack::Categorize() --> ERROR VT" << endl, exit(42);

				// RZ: On VT, distance on non-bending plane (Y) works best apparently!
				// double distanceFromHit = TMath::Sqrt( pow(guessOnVT.x() - (*it)->getRawHitCoords()(0), 2) + pow(guessOnVT.y() - (*it)->getRawHitCoords()(1), 2) );
				double distanceFromHit = fabs(guessOnVT.y() - (*it)->getRawHitCoords()(1));

				if( FootDebugLevel(2) )
					cout << "Hit::" << (*it)->getHitId() << " " << ListMCparticlesOfHit( (*it)->getHitId() ) << "\tguessX::" << guessOnVT.X() << "\trawHitX::" << (*it)->getRawHitCoords()(0) << "\tdistX::" << fabs(guessOnVT.X() - (*it)->getRawHitCoords()(0)) << "\tguessY::" << guessOnVT.Y() << "\trawHitY::" << (*it)->getRawHitCoords()(1) << "\tdistY::" << fabs(guessOnVT.Y() - (*it)->getRawHitCoords()(1)) << "\tdist::" << distanceFromHit<<endl;

				// find hit at minimum distance
				if (distanceFromHit < minDist)
				{
					minDist = distanceFromHit;
					indexOfMinDist = count;
				}
				count++;
			}

			// insert measurement in GF Track
			if (indexOfMinDist != -1)
			{
				// cout << "Found point!!" << endl;
				AbsMeasurement *hitToAdd = (static_cast<genfit::PlanarMeasurement *>(m_allHitMeas->at(VTplane).at(indexOfMinDist)))->clone();
				(itTrack->second)->insertMeasurement(hitToAdd, 0);

				//Fill extrapolation distance histos
				if( h_extrapDist.size() > 0 )
				{
					int iSensor;
					m_SensorIDMap->GetSensorID(VTplane, &iSensor);
					std::pair<string, std::pair<int, int>> sensId = make_pair("VT",make_pair(iSensor,0));
					h_extrapDist[sensId]->Fill(guessOnVT.X() - hitToAdd->getRawHitCoords()(0));
					sensId = make_pair("VT",make_pair(iSensor,1));
					h_extrapDist[sensId]->Fill(guessOnVT.Y() - hitToAdd->getRawHitCoords()(1));
				}

				//Re-set the track seed w/ new point
				guessOnVT = m_SensorIDMap->GetFitPlane(VTplane)->toLab( TVector2((hitToAdd)->getRawHitCoords()(0), (hitToAdd)->getRawHitCoords()(1)) );
				guessOnVT.SetZ(guessOnVT.Z()*0.99);

				// cout << "****SEED****"<<endl;
				// guessOnVT.Print(); momGuessOnVT.Print();
				(itTrack->second)->setStateSeed(guessOnVT,momGuessOnVT);
				m_fitter_extrapolation->processTrackWithRep(itTrack->second, (itTrack->second)->getCardinalRep());
				if( itTrack->second->getFitStatus(itTrack->second->getCardinalRep())->getNFailedPoints() != 0 )
				{
					tracksToRemove.push_back(itTrack->first);
					break;
				}

			}
		}
		// (itTrack->second)->reverseTrack();
	}

	delete m_fitter_extrapolation;

	if( m_IsMC && FootDebugLevel(1) )
	{
		cout << "End of VT tracking -> found these tracks\n";
		PrintCurrentTracksMC();
	}

	if (FootDebugLevel(2))	Info("Categorize_Backtracking()", "End of VT cycle!");
}
