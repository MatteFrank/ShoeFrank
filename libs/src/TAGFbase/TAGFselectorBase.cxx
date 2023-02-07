/*!
 \file TAGFselectorBase.cxx
 \brief Base class for track finding/selection in GenFit Global Reconstruction
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselectorBase.hxx"

/*!
 \class TAGFselectorBase
 \brief Base class for track finding/selection in GenFit Global Reconstruction

 There are already different algorithms implemented for track finding:
 - "TrueParticle": MC truth particle tracking
 - "Standard": Data-like base algorithm, no MC used
 - "Linear": Data-like algorithm for track finding w/out magnetic field -> linear track extrapolation btw detectors
 - "Backtracking": Data-like base algorithm starting from TW and going back to VT
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorBase::TAGFselectorBase()
{

	m_debug = TAGrecoManager::GetPar()->Debug();

	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	if(TAGrecoManager::GetPar()->IncludeVT())
		m_VT_geo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();

	if(TAGrecoManager::GetPar()->IncludeIT())
		m_IT_geo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();

	if(TAGrecoManager::GetPar()->IncludeMSD())
		m_MSD_geo = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();

	if(TAGrecoManager::GetPar()->IncludeTW())
		m_TW_geo = (TATWparGeo*) gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();

	m_systemsON = "";
	if( TAGrecoManager::GetPar()->KalSystems().at(0) == "all" )
	{
		if(TAGrecoManager::GetPar()->IncludeVT())	m_systemsON += "VT ";
		if(TAGrecoManager::GetPar()->IncludeIT())	m_systemsON += "IT ";
		if(TAGrecoManager::GetPar()->IncludeMSD())	m_systemsON += "MSD ";
		if(TAGrecoManager::GetPar()->IncludeTW())	m_systemsON += "TW ";
	}
	else
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
			if (i != 0)		m_systemsON += " ";
			m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
		}
	}

	m_detectors = TAGparTools::Tokenize( m_systemsON.Data() , " " );

	m_BeamEnergy = ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().Energy;

	if( m_debug > 1 )	cout << "Beam Energy::" << m_BeamEnergy << endl;

	m_eventType = 0;

	mc_eventType = 0;

	//Set default extrapolation tolerances for each detector
	m_VTtolerance = .5;
	m_ITtolerance = .5;
	m_MSDtolerance = .5;
	m_TWtolerance = 4.;
}


//! \brief Default destructor
TAGFselectorBase::~TAGFselectorBase()
{
	//Clear TrackTempMap
	for(auto it = m_trackTempMap.begin(); it != m_trackTempMap.end(); ++it)
		delete it->second;

	m_trackTempMap.clear();

	m_chargeVect->clear();

	for(auto it = m_allHitMeas->begin(); it != m_allHitMeas->end(); ++it)
	{
		for(auto itvec : it->second)
			delete itvec;
		it->second.clear();
	}
	m_allHitMeas->clear();

	for(auto it : m_trackRepVec)
		delete it;
	m_trackRepVec.clear();

	m_trackSlopeMap.clear();

	for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
		it->second.clear();
}


//! \brief Set all the needed variables for track selection
//!
//! \param[in] allHitMeas Pointer to the map containing all the measurements in GenFit format; the key is the GenFit FitPlane index
//! \param[in] chargeVect Pointer to vector of possible charges in the event (either measured from TW or MC truth)
//! \param[in] SensorIDmap Pointer to TAGFdetectorMap of the campaign
//! \param[in,out] trackCategoryMap Pointer to the map of selected tracks that have to be sent to the KFitter action
//! \param[in] measParticleMC_collection Pointer to the map containing the MC particles found in the event for each measurement; the key is the global index of the measurement
//! \param[in] IsMC boolean flag indicating if the input file is from MC or real data
//! \param[in] singleVertexCounter Pointer to variable counting events with exactly one vertex
//! \param[in] noVTtrackletEvents Pointer to variable counting events with no valid VT tracklet for track extrapolation
//! \param[in] noVTtrackletEvents Pointer to variable counting events with no valid TW point for track extrapolation
void TAGFselectorBase::SetVariables(map<int, vector<AbsMeasurement *>> *allHitMeas, vector<int> *chargeVect, TAGFdetectorMap *SensorIDmap, map<TString, Track *> *trackCategoryMap, map<int, vector<int>> *measParticleMC_collection, bool IsMC, uint *singleVertexCounter, uint *noVTtrackletEvents, uint* noTWpointEvents)
{
	m_allHitMeas = allHitMeas;
	m_chargeVect = chargeVect;
	m_SensorIDMap = SensorIDmap;
	m_trackCategoryMap = trackCategoryMap;
	m_measParticleMC_collection = measParticleMC_collection;
	m_IsMC = IsMC;
	m_singleVertexCounter = singleVertexCounter;
	m_noVTtrackletEvents = noVTtrackletEvents;
	m_noTWpointEvents = noTWpointEvents;

	if (m_IsMC)
		m_McNtuEve = (TAMCntuPart *)gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
}

//----------------------------------------------------------------------------------------------------

//! \brief Base function for track finding/selection/categorization
//!
//! This function gets called from the TAGactKFitter class
//! \return 0 if there were no errors
int TAGFselectorBase::FindTrackCandidates()
{

	CheckPlaneOccupancy();

	if (FillTrackRepVector() != 0)
		return -1;

	if( TAGrecoManager::GetPar()->EnableEventDisplay() )
		CreateDummyTrack();

	// fill m_mapTrack
	Categorize();

	return 0;
}



//----------------------------------------------------------------------------------------------------

//! \brief Check the charges seen by TW and fill the track representation vector
//!
//! \return 0 if track representation vector was filled w/out errors
int TAGFselectorBase::FillTrackRepVector()
{
	m_trackRepVec.clear();
	m_trackTempMap.clear();

	if(m_chargeVect->size() < 1)
	{
		// Error("FillTrackRepVector()", "TW charge vector has non positive length! Vec size::%ld", m_chargeVect->size());
		(*m_noTWpointEvents)++;
		return -1;
	}
	for(int i = 0; i < m_chargeVect->size(); ++i)
	{
		if ( m_debug > 0 ) cout << "TAGFselectorBase::FillTrackRepVector() -- charge: " << m_chargeVect->at(i) << "\n";

		AbsTrackRep* rep = new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope( m_chargeVect->at(i) ) );
		m_trackRepVec.push_back( rep );
	}
	return 0;
}



//! \brief Count the particles generated in the MC and visible in the acceptance of FOOT
//!
//! This function if used for Selection Efficiency evaluation
//! \return Map containing the name of MC particles seen in the event ("H", "He", "Li"...) and their number
map<string, int> TAGFselectorBase::CountParticleGenaratedAndVisible()
{

	if(m_debug > 0) cout << "TAGFselector::CountParticleGenaratedAndVisible --  Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";

	int particleCh;
	float mass;
	map<string, int> genCount_vector;
	TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();

	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		TAMCpart* particle = m_McNtuEve->GetTrack(iPart);

		particleCh = particle->GetCharge();
		mass = particle->GetMass();


		if ( particle->GetCharge() < 1 || particle->GetCharge() > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber)	continue;

		string outName, pdgName;

		switch(particleCh)	{

			case 1:	outName = "H";	break;
			case 2:	outName = "He";	break;
			case 3:	outName = "Li";	break;
			case 4:	outName = "Be";	break;
			case 5:	outName = "B";	break;
			case 6:	outName = "C";	break;
			case 7:	outName = "N";	break;
			case 8:	outName = "O";	break;
			default:
				outName = "fail";	break;
		}

		//CAREFUL HERE!! Think about the possibility of throwing an error -> skip particle for the moment
		if( outName == "fail" ) {continue;}

		pdgName = outName + int(round(mass/m_AMU));

		//CAREFUL HERE!!!!!!!!! FOOT TAGrecoManager file does not have Hydrogen and Helium isotopes!!!! Also think about throwing an error here...
		if ( !TAGrecoManager::GetPar()->Find_MCParticle( pdgName ) ) 	{
			if(m_debug > 0)  cout << "Found Particle not in MC list: " << pdgName << " num=" << iPart << "\n";
			continue;

		}

		if ( genCount_vector.find( outName ) == genCount_vector.end() ) 
			genCount_vector[ outName ] = 0;

		int foundHit = 0;
		int foundHitTW = 0;

		for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)    {
		
			//Skip plane if no hit was found
			if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()) 
				continue;
		
			//Cycle on all measurements/clusters of plane
			for(vector<AbsMeasurement*>::iterator itHit = m_allHitMeas->at(iPlane).begin(); itHit != m_allHitMeas->at(iPlane).end(); ++itHit)		{
			
				int MeasGlobId = (*itHit)->getHitId();
				int match = 0;
				for( vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasGlobId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasGlobId).end(); ++itTrackMC )	{
					
					int tmpID = ( *(itTrackMC) );
					if ( tmpID == iPart ) 
					{
						match = 1;
						if( iPlane == m_SensorIDMap->GetFitPlaneTW() )	foundHitTW++;
					}
				}


				if (match == 0)		continue;

				foundHit++;
			}
		}

		if ( foundHit < TAGrecoManager::GetPar()->MeasureN() && !foundHitTW ) 
			continue;

		genCount_vector.at( outName ) += 1;

	}

	return genCount_vector;
}



//--------------------------------------------------------------------------------------------
//! \brief Check the occupancy of all the FitPlanes in GenFit geometry and categorize the events to identify out of target fragmentations
void TAGFselectorBase::CheckPlaneOccupancy()
{

	for (auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
	{
		// RZ: Uncomment when we have an idea or the IT
		//  m_PlaneOccupancy[*itDet];
		//  if( *itDet == "IT" )
		//  	m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetPossibleITzLocal()->size());
		//  else
		//  	m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetFitPlanesN(*itDet));
		if(*itDet == "IT")
			continue;

		m_PlaneOccupancy[*itDet];
		m_PlaneOccupancy[*itDet].resize(m_SensorIDMap->GetFitPlanesN(*itDet));

		std::fill(m_PlaneOccupancy[*itDet].begin(), m_PlaneOccupancy[*itDet].end(), 0);
	}

	//Cycle on FitPlanes
	if(m_debug > 0) cout << "Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";
	for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
	{
		if(m_debug > 0) cout << "Plane::" << iPlane << "\n";

		//Skip plane if no hit was found
		if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()){continue;}

		string det = m_SensorIDMap->GetDetNameFromFitPlaneId(iPlane);
		if( det == "IT" )
		{
			continue; //RZ: IT to be studied better!
			// vector<int>* planesAtZLocal;
			// int id=0;
			// for (auto itZ : *(m_SensorIDMap->GetPossibleITzLocal()) )
			// {
			// 	planesAtZLocal = m_SensorIDMap->GetPlanesAtZLocal(itZ);
			// 	if( std::find(planesAtZLocal->begin(), planesAtZLocal->end(), iPlane) != planesAtZLocal->end() )
			// 	{
			// 		m_PlaneOccupancy[det][id] += m_allHitMeas->at(iPlane).size();
			// 		break;
			// 	}
			// 	id++;
			// }
		}
		else
		{
			int sensorId;
			if( m_SensorIDMap->GetSensorID(iPlane, &sensorId) )
				m_PlaneOccupancy[det][sensorId] = m_allHitMeas->at(iPlane).size();
		}

	} //End of loop on sensors

	//Classify the event
	if( m_PlaneOccupancy.find("TW") == m_PlaneOccupancy.end() || m_PlaneOccupancy["TW"][0] < 1 )
		m_eventType = 5; // No TW point in the event -> bad event
	else
	{
		bool needsCheck = false;
		bool needsCheckIT = false;
		bool foundIncrease = false;
		bool foundIncreaseBtwDets = false;
		map<string, bool> detCountsChange;
		map<string, bool> detCountsIncrease;
		map<string, bool> detCountsIncreaseBtwDets;
		int prev_counts, counts;

		//Modify when IT is included!!!
		if( m_detectors[0] == "IT" && m_detectors.size() > 1 )
			prev_counts = m_PlaneOccupancy[m_detectors[1]][0];
		else if( m_detectors[0] != "IT" )
			prev_counts = m_PlaneOccupancy[m_detectors[0]][0];
		else
		{
			Error("CheckPlaneOccupancy()", "Error in handling plane occupancy maps! Exiting...");
			exit(-1);
		}

		for(auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
		{

			if(*itDet != "IT")
			{
				detCountsChange[*itDet] = false;
				detCountsIncrease[*itDet] = false;
				if( itDet != m_detectors.begin() )
					detCountsIncreaseBtwDets[*itDet] = false;

				counts = m_PlaneOccupancy[*itDet][0];
				// Set flag for increase btw detectors
				if( itDet != m_detectors.begin() && prev_counts != counts )
				{
					needsCheck = true;
					if( prev_counts < counts)	detCountsIncreaseBtwDets[*itDet] = true;
				}

				for( int i = 1; i < m_PlaneOccupancy[*itDet].size(); ++i)
				{
					if( m_PlaneOccupancy[*itDet][i] != counts )
					{
						detCountsChange[*itDet] = true;

						//Set flag for increase if it happens at ANY stage
						if( m_PlaneOccupancy[*itDet][i] > counts )	detCountsIncrease[*itDet] = true;
						counts = m_PlaneOccupancy[*itDet][i];
					}
				}
				prev_counts = counts;
			}
			else
			{
				continue;
				// int counts1 = 0, counts2 = 0;
				// for( int i = 0; i < m_PlaneOccupancy[*itDet].size()/2; ++i )
				// {
				// 	// if( m_PlaneOccupancy[*itDet][2*i] != m_PlaneOccupancy[*itDet][2*i + 1] )

				// 	counts1 += m_PlaneOccupancy[*itDet][2*i];
				// 	counts2 += m_PlaneOccupancy[*itDet][2*i + 1];
				// }
				// if(counts1 != counts2) needsCheckIT = true;
			}

			foundIncrease = foundIncrease || detCountsIncrease[*itDet];
			foundIncreaseBtwDets = foundIncreaseBtwDets || detCountsIncreaseBtwDets[*itDet];
			needsCheck = needsCheck || needsCheckIT || detCountsChange[*itDet] || foundIncrease || foundIncreaseBtwDets;
		}


		if( !needsCheck )
			m_eventType = 1; //Clean event, likely all fragments reconstructable
		else
		{
			if( foundIncrease )
				m_eventType = 3; //Likely fragmentation in a detector plane
			else if ( foundIncreaseBtwDets )
				m_eventType = 4; //Likely fragmentation in air
			else
				m_eventType = 2; //Good event, likely some tracks exiting the angular acceptance
		}
	}

	//Print in debug mode
	if( m_debug > 1)// || (m_eventType != 1 && m_eventType != 5))
	{
		cout << "EVENT::" << gTAGroot->CurrentEventId().EventNumber() << "\tTYPE::" << m_eventType << endl;
		for(auto itDet = m_detectors.begin(); itDet != m_detectors.end(); ++itDet)
		{
			for( int i=0; i < m_PlaneOccupancy[*itDet].size(); ++i)
				cout << *itDet << "\tId::" << i << "\tNmeas::" << m_PlaneOccupancy[*itDet][i] << endl;
		}
	}

	///// Francesco Vascelli /////////////
	////////////////////////////////////////////////////////////////////


	if (m_IsMC){

		//creare oggetto MCpart, con get, prendere info necessarie su particelle,
		//

		TAMCntuRegion* mcNtuReg = (TAMCntuRegion*) gTAGroot->FindDataDsc("regMc","TAMCntuRegion")->Object();
		Int_t nCross  = mcNtuReg->GetRegionsN();
		mc_eventType=5;
		bool air_fragm = false;
		bool det_fragm = false;
		bool lost_fragm = false;
		bool has_tof = false;
		TAMCntuPart* mcNtu = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc","TAMCntuPart")->Object();
		int goodId;
		vector<int> frag{-1};
		vector<int> counts (21,0);

		int maxcount=-1;


		/*
		map<string, int> map;
		map["outTG"] = 0;

		map["inVTX0"] = 0;
		map["outVTX0"] = 0;
		map["inVTX1"] = 0;
		map["outVTX1"] = 0;
		map["inVTX2"] = 0;
		map["outVTX2"] = 0;
		map["inVTX3"] = 0;
		map["outVTX3"] = 0;

		map["inMSD0"] = 0;
		map["outMSD0"] = 0;
		map["inMSD1"] = 0;
		map["outMSD1"] = 0;
		map["inMSD2"] = 0;
		map["outMSD2"] = 0;
		map["inMSD3"] = 0;
		map["outMSD3"] = 0;
		map["inMSD4"] = 0;
		map["outMSD4"] = 0;
		map["inMSD5"] = 0;
		map["outMSD5"] = 0;
		*/
		
		for(int itt=0; itt<nCross; ++itt){

			

			TAMCregion* cross=mcNtuReg->GetRegion(itt); //get the itt-th crossing
			
			Int_t NewReg = cross->GetCrossN();
			Int_t OldReg = cross->GetOldCrossN();

			int trackIdx = cross->GetTrackIdx()-1;
			//TAMCpart* MCpart = mcNtu->GetTrack(trackIdx);

			if (NewReg>=81 && NewReg<=120)  //crosses TW
				has_tof=true;
				


			//if (NewReg>50 && NewReg<81)
				//count++;
	
			//fragm crosses target
			if(OldReg==50 && NewReg==2)
				counts[0]++;

			//fragm crosses VTX 
			if(OldReg==2 && (NewReg==51 || NewReg==55 || NewReg==59))
				counts[1]++;
			if((OldReg==51 || OldReg==55 || OldReg==59) && NewReg==2)
				counts[2]++;
			if(OldReg==2 && (NewReg==52 || NewReg==56 || NewReg==60))
				counts[3]++;
			if((OldReg==52 || OldReg==56 || OldReg==60) && NewReg==2)
				counts[4]++;
			if(OldReg==2 && (NewReg==53 || NewReg==57 || NewReg==61))
				counts[5]++;
			if((OldReg==53 || OldReg==57 || OldReg==61) && NewReg==2)
				counts[6]++;
			if(OldReg==2 && (NewReg==54 || NewReg==58 || NewReg==62))
				counts[7]++;
			if((OldReg==54 || OldReg==58 || OldReg==62) && NewReg==2)
				counts[8]++;

			//fragm crosses MSD
			if(OldReg==2 && (NewReg==63 || NewReg==69 || NewReg==75))
				counts[9]++;
			if((OldReg==63 || OldReg==69 || OldReg==75) && NewReg==2)
				counts[10]++;
			if(OldReg==2 && (NewReg==64 || NewReg==70 || NewReg==76))
				counts[11]++;
			if((OldReg==64 || OldReg==70 || OldReg==76) && NewReg==2)
				counts[12]++;
			if(OldReg==2 && (NewReg==65 || NewReg==71 || NewReg==77))
				counts[13]++;
			if((OldReg==65 || OldReg==71 || OldReg==77) && NewReg==2)
				counts[14]++;
			if(OldReg==2 && (NewReg==66 || NewReg==72 || NewReg==78))
				counts[15]++;
			if((OldReg==66 || OldReg==72 || OldReg==78) && NewReg==2)
				counts[16]++;
			if(OldReg==2 && (NewReg==67 || NewReg==73 || NewReg==79))
				counts[17]++;
			if((OldReg==67 || OldReg==73 || OldReg==79) && NewReg==2)
				counts[18]++;
			if(OldReg==2 && (NewReg==68 || NewReg==74 || NewReg==80))
				counts[19]++;
			if((OldReg==68 || OldReg==74 || OldReg==80) && NewReg==2)
				counts[20]++;

			//crosses air1->air2
			//if(OldReg==2 && NewReg==3)
			//	counts[21]++;

			//crosses air3->SCN
			//if(OldReg==3 && (NewReg>=81 && NewReg<=100))
			//	counts[22]++;
			
			if (std::find(frag.begin(), frag.end(), trackIdx) == frag.end()) 
			{
				frag.push_back(trackIdx);
				
			}
				
		}

		for(int j=0; j<counts.size()-1; ++j){
			if(counts[j+1]<counts[j])
			{
				lost_fragm=true;
				
			}
			//if (counts[2*j+1]>counts[2*j])
			//{
			//	air_fragm=true;
			//}
			//if (counts[2*j+2]>counts[2*j+1])
			//{
			//	det_fragm=true;
			//}
			//cout << counts[j] << endl;
		}



		for(int i=1; i<frag.size(); ++i){
			TAMCpart* MCpart = mcNtu->GetTrack(frag.at(i));
			int region = MCpart->GetRegion(); //get region where fragment originates 
			TVector3 finalPos = MCpart->GetFinalPos();
			TVector3 initPos = MCpart->GetInitPos();
	
		
			double finalZpos = finalPos[2];
			double initZpos = initPos[2];
			bool is_inside = (initZpos>0. && initZpos < 189.15);
			bool is_air = (region==2 || region==3 || region==4);
			if (region>120){ //exclude fragmentation in calorimeter 
					continue;	
			}
			if ( is_air && is_inside) //fragmentation in air	
			{
			 
				air_fragm=true;	
							
			} 
			if (region!=50 && is_inside && !is_air)//fragmentation not in target (so in detector)
			{ 
				det_fragm=true;
					
			}
		}	
	


		if (has_tof)
		{	
			
			if (lost_fragm)
			{
				
				if (det_fragm)
				{
					mc_eventType=3;
				}
				else if (air_fragm)
				{
					mc_eventType=4;
				}
				else 
					mc_eventType=2;
			}
			else
				mc_eventType=1;
		}
		
		
	}
}

int TAGFselectorBase::GetMCEventType() { return mc_eventType; }



///////////////////////////////////////////////////////////////////////////


//! \brief Get the event type seen by the Genfit selector
int TAGFselectorBase::GetEventType() { return m_eventType; }


//! \brief Fill plane occupancy histogram
void TAGFselectorBase::FillPlaneOccupancy(TH2I** h_PlaneOccupancy)
{
	int count = 0;
	for( auto it = m_detectors.begin(); it != m_detectors.end(); ++it)
	{
		if( m_PlaneOccupancy.find(*it) == m_PlaneOccupancy.end() )
			continue;
		else
		{
			for(int i=0; i < m_PlaneOccupancy[*it].size(); ++i)
			{
				h_PlaneOccupancy[0]->Fill( count, m_PlaneOccupancy[*it][i] );
				h_PlaneOccupancy[m_eventType]->Fill( count, m_PlaneOccupancy[*it][i] );
				++count;
			}
		}
	}
}


//--------------------------------------------------------------------------------------------


//! \brief Fill the final track map to process in TAGactKFitter w/ the selected tracks
void TAGFselectorBase::FillTrackCategoryMap()
{

	for(map<int, Track*>::iterator itTrack = m_trackTempMap.begin(); itTrack != m_trackTempMap.end(); ++itTrack)
	{
		// Handle the dummy track showing all the TW points -> present only when the genfit event display is on
		if(itTrack->first == -7)
		{
			(*m_trackCategoryMap)["dummy"] = new Track(*(itTrack->second));
			continue;
		}
		TString outName;
		int MeasId = itTrack->second->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
		if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" && m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
		{
			if(m_debug > 0)
				Info("FillTrackCategoryMap()", "Found track candidate (%d) with no TW point! Skipping...", itTrack->first);
			// continue;
		}

		//Add possibility to have tracks ending before TW!!!!

		int measCharge = itTrack->second->getCardinalRep()->getPDGCharge();
		int measMass;
		if( TAGrecoManager::GetPar()->PreselectStrategy() == "Linear" )
			measMass = UpdatePDG::GetPDG()->GetPdgMass( UpdatePDG::GetPDG()->GetPdgCodeMainIsotope(measCharge) );
		else
			measMass = round( itTrack->second->getCardinalRep()->getMass( (itTrack->second)->getFittedState(-1) )/m_AMU );

		switch(measCharge)
		{
			case 1:	outName = "H";	break;
			case 2:	outName = "He";	break;
			case 3:	outName = "Li";	break;
			case 4:	outName = "Be";	break;
			case 5:	outName = "B";	break;
			case 6:	outName = "C";	break;
			case 7:	outName = "N";	break;
			case 8:	outName = "O";	break;
			default:
				outName = "fail";	break;
		}

		if ( m_debug > 1 )	Info("FillTrackCategoryMap()", "Track with measured charge %d and mass %d!!", measCharge, measMass);

		if( outName == "fail" )
		{
			Info("FillTrackCategoryMap()", "Skipped Track with measured charge %d and mass %d!!", measCharge, measMass);
			continue;
		}

		outName += Form("_%d_%d", measMass, itTrack->first);
		(*m_trackCategoryMap)[outName] = new Track(*(itTrack->second));
	}
}



void TAGFselectorBase::CreateDummyTrack()
{
	int planeTW = m_SensorIDMap->GetFitPlaneTW();
	if ( m_allHitMeas->find( planeTW ) == m_allHitMeas->end() )	return;

	//If the genfit event display is switched on, create a dummy track to show all the TW points
	if( TAGrecoManager::GetPar()->EnableEventDisplay() )
	{
		Track* dummy = new Track();
		for ( vector<AbsMeasurement*>::iterator it = m_allHitMeas->at( planeTW ).begin(); it != m_allHitMeas->at( planeTW ).end(); ++it){
			AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> (*it))->clone();
				dummy->insertMeasurement( hitToAdd );
		}
		if(dummy->getNumPointsWithMeasurement()>0) m_trackTempMap[-7] = new Track(*dummy);
		delete dummy;
	}
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the possible charge of a selected track from the TW measurement
//!
//! Currently uses MC information (returns charge of most frequent particle along the track) OR the charge of the TWpoint associated to the track
//! \param[in] trackToCheck Pointer to GenFit track
//! \return Charge measured from the TW
int TAGFselectorBase::GetChargeFromTW(Track *trackToCheck)
{

	int charge = -1;

	// if( TAGrecoManager::GetPar()->PreselectStrategy() != "TrueParticle" ) //do not use MC!
	// {
	TATWpoint* twpoint = 0x0;
	if( trackToCheck->getNumPointsWithMeasurement() >= 0 )
	{
		for (int jTracking = trackToCheck->getNumPointsWithMeasurement() - 1; jTracking >= 0; --jTracking){
			if ( static_cast<genfit::PlanarMeasurement*>(trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement())->getPlaneId() != m_SensorIDMap->GetFitPlaneTW() ) continue;

			int MeasId = trackToCheck->getPointWithMeasurement(jTracking)->getRawMeasurement()->getHitId();

			twpoint = ( (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint","TATWntuPoint")->Object() )->GetPoint( m_SensorIDMap->GetHitIDFromMeasID(MeasId) ); //Find TW point associated to the track

			charge = twpoint->GetChargeZ();
			break;
		}
	}
	// }	//end of charge calculation from data

	// else	//use MC!
	// {
	// 	int MeasId = trackToCheck->getPointWithMeasurement(-1)->getRawMeasurement()->getHitId();
	// 	if(m_SensorIDMap->GetFitPlaneIDFromMeasID(MeasId) != m_SensorIDMap->GetFitPlaneTW())
	// 		return -1;

	// 	if(m_measParticleMC_collection->at(MeasId).size() == 1)
	// 	{
	// 		return m_McNtuEve->GetTrack( m_measParticleMC_collection->at(MeasId).at(0) )->GetCharge();
	// 	}
	// 	else
	// 	{
	// 		map<int,int> ChargeOccMap;

	// 		for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
	// 		{
	// 			if( m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() < 1 ||  m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() > ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber)
	// 				continue;
	// 			ChargeOccMap[ m_McNtuEve->GetTrack( *itTrackMC )->GetCharge() ] = 1;
	// 		}

	// 		for(int i = 0; i < trackToCheck->getNumPointsWithMeasurement() - 1; ++i)
	// 		{
	// 			MeasId = trackToCheck->getPointWithMeasurement(i)->getRawMeasurement()->getHitId();
	// 			for(vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasId).end(); ++itTrackMC)
	// 			{
	// 				charge = m_McNtuEve->GetTrack( *itTrackMC )->GetCharge();
	// 				if(ChargeOccMap.find(charge) == ChargeOccMap.end())
	// 					continue;

	// 				ChargeOccMap[ charge ]++;
	// 			}
	// 		}

	// 		int occ = 0;

	// 		for(map<int,int>::iterator itMap = ChargeOccMap.begin(); itMap != ChargeOccMap.end(); ++itMap)
	// 		{
	// 			if(itMap->second > occ)
	// 			{
	// 				occ = itMap->second;
	// 				charge = itMap->first;
	// 			}
	// 		}
	// 	}
	// } //end MC charge calcualtion

	return charge;
}



//----------------------------------------------------------------------------------------------------

//! \brief Extrapolate a track to a GenFit FitPlane
//!
//! \param[in] trackToFit Pointer to track to extrapolate
//! \param[in] whichPlane Index of the FitPlane where to extrapolate the track
//! \param[in] repId Index of the track representation to use for the extrapolation
//! \return Extrapolated position vector in the FitPlane local reference frame
TVector3 TAGFselectorBase::ExtrapolateToOuterTracker(Track* trackToFit, int whichPlane, int repId)
{

	//+++++++++++++++++++++++++++++++++++++++
	if(repId == -1)
		repId = trackToFit->getCardinalRepId();
	TrackPoint* tp = trackToFit->getPointWithMeasurementAndFitterInfo(-1, trackToFit->getTrackRep(repId));
	if (tp == nullptr) {
		// Error("ExtrapolateToOuterTracker()", "Track has no TrackPoint with fitterInfo");
		// exit(0);
		throw genfit::Exception("Track has no TrackPoint with fitterInfo", __LINE__, __FILE__);
	}

	if ( (static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getTrackRep(repId)))->hasForwardUpdate() ) == false) {
		Error("ExtrapolateToOuterTracker()", "TrackPoint has no forward update");
		exit(0);
	}

	//RZ: Test with last fitted state!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	KalmanFittedStateOnPlane kfTest;
	kfTest = *(static_cast<genfit::KalmanFitterInfo*>(tp->getFitterInfo(trackToFit->getTrackRep( repId )))->getForwardUpdate());
	trackToFit->getTrackRep(repId)->extrapolateToPlane(kfTest, m_SensorIDMap->GetFitPlane(whichPlane), false, false); //RZ: Local reference frame of "whichPlane"!!!

	TVector3 posi((kfTest.getState()[3]),(kfTest.getState()[4]), m_SensorIDMap->GetFitPlane(whichPlane)->getO().Z());

	return posi;
}



//----------------------------------------------------------------------------------------------------

//! \brief pre-fit requirements to be applied to EACH of the hitCollections
//!
//! FUNCTION CURRENTLY NOT USED!
bool TAGFselectorBase::PrefitRequirements(map<string, vector<AbsMeasurement*>>::iterator element)
{


	int testHitNumberLimit = 0;
	int testHit_VT = 0;
	int testHit_IT = 0;
	int testHit_MSD = 0;
	int testHit_TW = 0;

	// define the number of hits per each detector to consider to satisfy the pre-fit requirements
	if ( m_systemsON == "all" ) {
		testHit_VT = m_SensorIDMap->GetFitPlanesN("VT");
		testHit_IT = m_SensorIDMap->GetFitPlanesN("IT")/16;
		testHit_MSD = m_SensorIDMap->GetFitPlanesN("MSD")/2;
	}

  	else {
	    if ( m_systemsON.Contains("VT") )	testHit_VT = m_SensorIDMap->GetFitPlanesN("VT");
	    if ( m_systemsON.Contains("IT") )	testHit_IT = m_SensorIDMap->GetFitPlanesN("IT")/16;
	    if ( m_systemsON.Contains("MSD") )	testHit_MSD = m_SensorIDMap->GetFitPlanesN("MSD");
	    if ( m_systemsON.Contains("TW") )	testHit_TW = 1;
	}

	// num of total hits
	testHitNumberLimit = testHit_VT + testHit_IT + testHit_MSD + testHit_TW;
	if ( testHitNumberLimit == 0 ) 		
		cout << "ERROR --> TAGFselector::PrefitRequirements :: m_systemsON mode is wrong!!!" << endl, exit(0);

	// // test the total number of hits ->  speed up the test
	// if ( (int)((*element).second.size()) != testHitNumberLimit ) {
	// 	if ( m_debug > 0 )		cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : Nel=" << (int)((*element).second.size()) << "   Nexp= " << testHitNumberLimit << "\n";
	// 	return false;
	// }

	int nHitVT = 0;
	int nHitIT = 0;
	int nHitMSD = 0;
	int nHitTW = 0;

	// count the hits per each detector
	for ( vector<AbsMeasurement*>::iterator it=(*element).second.begin(); it != (*element).second.end(); it++ ) {
		int planeId = (*it)->getDetId();
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "VT") )	nHitVT++;
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "IT") )	nHitIT++;
			if ( m_SensorIDMap->IsFitPlaneInDet(planeId, "MSD") )	nHitMSD++;
			if ( planeId == m_SensorIDMap->GetFitPlaneTW() )	nHitTW++;
	}

	if ( m_debug > 0 )	cout << "nHitVT  " <<nHitVT<< " nHitIT " <<nHitIT<< " nHitMSD "<<nHitMSD<< " nHitTW "<<nHitTW<<"\n";

	// test the num of hits per each detector
	// if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD != testHit_MSD ) {

	if ( nHitVT != testHit_VT || nHitIT != testHit_IT || nHitMSD < 4 ){
	    if ( m_debug > 0 ) {
		    cout << "WARNING :: TAGFselector::PrefitRequirements  -->  number of elements different wrt the expected ones : " <<
				    "\n\n\t nVTX = " << nHitVT << "  Nexp = " << testHit_VT <<
				    "\n\n\t nITR = " << nHitIT << "  Nexp = " << testHit_IT <<
				    "\n\n\t nMSD = " << nHitMSD << "  Nexp = " << testHit_MSD <<
				    "\n\n\t nTW = " << nHitTW << "  Nexp = " << testHit_TW << "\n";
		}
		return false;
	}

	return true;
}



//! \brief Get the MC true particle information from its MC track index
//!
//! \param[in] trackId Index of the MC track
//! \param[in,out] flukaID Pointer where to save the FLUKA ID
//! \param[in,out] charge Pointer where to save the true charge of the particle
//! \param[in,out] mass Pointer where to save the true mass of the particle
//! \param[in,out] posV Pointer where to save the initial position of the particle
//! \param[in,out] momV Pointer where to save the initial momentum of the particle
void TAGFselectorBase::GetTrueParticleType(int trackid, int* flukaID, int* charge, double* mass, TVector3* posV, TVector3* momV)
{

	TAMCpart* particle = m_McNtuEve->GetTrack(trackid);

	*flukaID = particle->GetFlukaID();
	*charge  = particle->GetCharge();
	*mass    = particle->GetMass();

	*posV = particle->GetInitPos();
	*momV = particle->GetInitP();
}

//! \brief Get the name of a track
//!
//! \param[in] tr Pointer to the GenFit track
//! \return Name of the track
TString TAGFselectorBase::GetRecoTrackName(Track* tr)
{
	for(map<TString, Track*>::iterator it = m_trackCategoryMap->begin(); it != m_trackCategoryMap->end(); ++it)
	{
		if(it->second == tr)
			return it->first;
	}
	Error("GetRecoTrackName()", "Track not found in Category Map!!");
	throw -1;
}
