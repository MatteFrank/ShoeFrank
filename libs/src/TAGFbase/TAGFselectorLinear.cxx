/*!
 \file TAGFselectorLinear.cxx
 \brief  Class for track finding/selection in GenFit Global Reconstruction w/ linear forward extrapolation
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselectorLinear.hxx"

/*!
 \class TAGFselectorLinear
 \brief Class for track finding/selection in GenFit Global Reconstruction w/ linear forward extrapolation
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorLinear::TAGFselectorLinear() : TAGFselectorStandard()
{}


//----------------------------------------------------------------------------------------------------

//! \brief Base function for track finding/selection/categorization for linear data-like selection
void TAGFselectorLinear::Categorize( ) {

	if( m_debug > 1 ) cout << "******* START OF VT CYCLE *********\n";

	if(!TAGrecoManager::GetPar()->IncludeVT() || !m_systemsON.Contains("VT"))
	{
		Error("Categorize_dataLike()", "Linear selection algorithm currently not supported without Vertex!");
		throw -1;
	}
	else
		CategorizeVT();

	if( m_debug > 1 ) cout << "******** END OF VT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF IT CYCLE *********\n";
	
	if( m_systemsON.Contains("IT") )
		CategorizeIT();
	
	if( m_debug > 1 ) cout << "******** END OF IT CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF MSD CYCLE *********\n";
	
	if( m_systemsON.Contains("MSD") )
		CategorizeMSD();
	
	if( m_debug > 1 ) cout << "******** END OF MSD CYCLE **********\n";

	if( m_debug > 1 ) cout << "******* START OF TW CYCLE *********\n";
	
	if( m_systemsON.Contains("TW") )
		CategorizeTW();
	
	if( m_debug > 1 ) cout << "******** END OF TW CYCLE **********\n";

	double dPVal = 1.E-3; // convergence criterion
	KalmanFitter* preFitter = new KalmanFitter(1, dPVal);

	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		int Z_Hypo = GetChargeFromTW(itTrack->second);
		if( Z_Hypo == -1 )
		{
			Z_Hypo = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber;
			itTrack->second->addTrackRep(new RKTrackRep(UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( Z_Hypo )));
		}
		else
		{
			for ( int nRep=0; nRep < m_trackRepVec.size(); nRep++)
				if( m_trackRepVec.at(nRep)->getPDGCharge() == Z_Hypo )	(itTrack->second)->addTrackRep( m_trackRepVec.at( nRep )->clone() );
		}

		//Get first track measurement and use it to set the track seed
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int SensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3(firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(SensorId, pos ) ); //Move position to global coords

		pos = pos - m_trackSlopeMap[itTrack->first]*pos.Z();

		TVector3 mom = m_trackSlopeMap[itTrack->first];
		double mass_Hypo = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(Z_Hypo) );
		int A_Hypo = round(mass_Hypo/m_AMU);

		mom.SetMag(TMath::Sqrt( pow(m_BeamEnergy*A_Hypo,2) + 2*mass_Hypo*m_BeamEnergy*A_Hypo ));
		itTrack->second->setStateSeed(pos, mom);

		// preFitter->processTrackWithRep( itTrack->second, itTrack->second->getCardinalRep() );
	}
	delete preFitter;

	FillTrackCategoryMap();
}



//! \brief Track selection at the MSD when no magnetic field is present ("Linear")
//!
//! This step is performed through a linear extrapolation at the MSD
void TAGFselectorLinear::CategorizeMSD()
{
	int findMSD;

	// Extrapolate to MSD
	// same index if VTX_tracklets (for one vertex..)
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end();) {

		findMSD=0;

		int maxMSDdetPlane = m_SensorIDMap->GetMaxFitPlane("MSD");
		int minMSDdetPlane = m_SensorIDMap->GetMinFitPlane("MSD");

		//RZ: SET STATE SEED
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3( firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, pos) );
		
		if(m_debug > 0)
		{
			cout << "***POS SEED***\nVTX: "; pos.Print();
		}

		for ( int MSDnPlane = minMSDdetPlane; MSDnPlane <= maxMSDdetPlane; MSDnPlane++ ) {

			int sensorId;
			if( !m_SensorIDMap->GetSensorID(MSDnPlane, &sensorId) )
			{
				Error("CategorizeIT()", "Sensor not found for Genfit plane %d!", MSDnPlane);
				throw -1;
			}
			//Skip if no measurement found
			if ( m_allHitMeas->find( MSDnPlane ) == m_allHitMeas->end() ) {
				if(m_debug > 0) cout << "TAGFselectorLinear::CategorizeMSD() -- no measurement found in MSDnPlane "<< MSDnPlane<<"\n";
				continue;
			}

			TVector3 guessOnMSD = m_GeoTrafo->FromGlobalToMSDLocal(pos + m_trackSlopeMap[itTrack->first]*(m_SensorIDMap->GetFitPlane(MSDnPlane)->getO().Z() - pos.Z()));
			guessOnMSD = m_MSD_geo->Detector2Sensor( sensorId,  guessOnMSD );
			
			if( !m_SensorIDMap->GetFitPlane(MSDnPlane)->isInActive( guessOnMSD.x(), guessOnMSD.y() ) )
				continue;

			int indexOfMinY = -1;
			int count = 0;
			double minDistFromHit = m_MSDtolerance;
			int sensorMatch = MSDnPlane;


			for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( MSDnPlane ).begin(); it != m_allHitMeas->at( MSDnPlane ).end(); ++it){

				if ( m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != sensorMatch )	cout << "TAGFselectorLinear::Categorize_dataLike() --> ERROR MSD" <<endl, exit(0);

				//RZ: CHECK -> AVOID ERRORS
				double distanceFromHit;
				string strip;

				if ( ! static_cast<PlanarMeasurement*>(*it)->getStripV() )
				{
					distanceFromHit = fabs(guessOnMSD.X() - (*it)->getRawHitCoords()(0));
					strip = "X";
				}
				else
				{
					distanceFromHit = fabs(guessOnMSD.Y() - (*it)->getRawHitCoords()(0));
					strip = "Y";
				}

				// find hit at minimum distance
				if ( distanceFromHit < minDistFromHit ){
					if(m_debug > 0) cout << "MSDcheck\tPlane::" << sensorMatch << "\tTrack::" << itTrack->first << "\tdistanceFromHit::" << distanceFromHit << "\tStrip::" << strip << "\n";
					minDistFromHit = distanceFromHit;
					indexOfMinY = count;
				}
				count++;
			}

			// insert measurementi in GF Track
			if (indexOfMinY != -1){

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(sensorMatch).at(indexOfMinY)))->clone();
				(itTrack->second)->insertMeasurement( hitToAdd );
				findMSD++;
			}

		} // end loop MSD planes

		++itTrack;

	}// end loop on GF Track candidates

	return;
}



//! \brief TW track selection when no magnetic field is present ("Linear")
//!
//! This step uses a linear extrapolation at the TW
void TAGFselectorLinear::CategorizeTW()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	//RZ -> See if this check can be done outside this cycle... it seems a much more general skip
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() ) {
		if(m_debug > 0) cout << "TAGFselectorLinear::CategorizeTW() -- no measurement found in TW layer\n";
		return;
	}

	// Extrapolate to TW
	for (map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); itTrack++) 
	{
		PlanarMeasurement* firstTrackMeas = static_cast<genfit::PlanarMeasurement*> (itTrack->second->getPointWithMeasurement(0)->getRawMeasurement());
		int VTsensorId = m_SensorIDMap->GetSensorIDFromMeasID( firstTrackMeas->getHitId() );
		TVector3 pos = TVector3( firstTrackMeas->getRawHitCoords()(0), firstTrackMeas->getRawHitCoords()(1), 0);
		pos = m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector(VTsensorId, pos) );

		TVector3 guessOnTW =  m_GeoTrafo->FromGlobalToTWLocal( pos + m_trackSlopeMap[itTrack->first]*(m_SensorIDMap->GetFitPlane(planeTW)->getO().Z() - pos.Z()) );
		// guessOnTW = m_TW_geo->Detector2Sensor( 0, guessOnTW );

		if( m_debug > 1) cout << "guessOnTW " << guessOnTW.X() << "  " << guessOnTW.Y() << "\n";

		//calculate distance TW point
		double TWdistance = m_TWtolerance;
		int indexOfMin = -1;
		int count = 0;

		double distInX, distInY;

		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){

			if (  m_SensorIDMap->GetFitPlaneIDFromMeasID( (*it)->getHitId() ) != planeTW )
				cout << "TAGFselectorLinear::Categorize_dataLike() --> ERROR TW" <<endl, exit(0);

			double distanceFromHit = sqrt( ( guessOnTW.X() - (*it)->getRawHitCoords()(0) )*( guessOnTW.X() - (*it)->getRawHitCoords()(0) ) +
					( guessOnTW.Y() - (*it)->getRawHitCoords()(1) )*( guessOnTW.Y() - (*it)->getRawHitCoords()(1) ) );
			
			if( m_debug > 0) cout << "measurement: " << (*it)->getRawHitCoords()(0) << "   " << (*it)->getRawHitCoords()(1)<< "\n";

			if ( distanceFromHit < TWdistance )	{
				distInX = guessOnTW.X() - (*it)->getRawHitCoords()(0);
				distInY = guessOnTW.Y() - (*it)->getRawHitCoords()(1);
				TWdistance = distanceFromHit;
				indexOfMin = count;
			}

			count++;
		}	// end of TW hits

		if (indexOfMin != -1)	{
			AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (m_allHitMeas->at(planeTW).at(indexOfMin)))->clone();
			(itTrack->second)->insertMeasurement( hitToAdd );
		}
	}

	return;
}
