/*!
 \file TAGFselectorTrue.cxx
 \brief  Class for track finding/selection in GenFit Global Reconstruction based on MC truth info
 \author R. Zarrella and M. Franchini
*/

#include "TAGFselectorTrue.hxx"

/*!
 \class TAGFselectorTrue
 \brief Class for track finding/selection in GenFit Global Reconstruction that uses MC truth information
*/


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
TAGFselectorTrue::TAGFselectorTrue() : TAGFselectorBase()
{}



//----------------------------------------------------------------------------------------------------

//! \brief Main function of MC track selection algorithm
void TAGFselectorTrue::Categorize( ) {

	//Categorize events using MC truth
	int flukaID, charge;
	double mass;

	if(m_debug > 0)
	{
		cout << "MC particles:\n\n";
		for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
		{
			for(int i=0; i < (it->second).size(); ++i)
			{
				cout << "GlobId::" << it->first << "\tId::" << i << "\tTrackId::" << it->second.at(i) << "\n";
			}
		}
	}

	//Cycle on FitPlanes
	if(m_debug > 1) cout << "Cycle on planes\t"  << m_SensorIDMap->GetFitPlanesN() << "\n";
	for(int iPlane = 0; iPlane < m_SensorIDMap->GetFitPlanesN(); ++iPlane)
	{

		if(m_debug > 1) cout << "Plane::" << iPlane << "\n";
		
		//Skip plane if no hit was found
		if(m_allHitMeas->find(iPlane) == m_allHitMeas->end()){continue;}

		if(m_debug > 0) cout << "Cycle on hits\n";

		int foundHit = 0;
	
		//Cycle on all measurements/clusters of plane
		for(vector<AbsMeasurement*>::iterator itHit = m_allHitMeas->at(iPlane).begin(); itHit != m_allHitMeas->at(iPlane).end(); ++itHit)
		{
			int MeasGlobId = (*itHit)->getHitId();

			//Cycle on all the MC particles that created that cluster/measurement
			if(m_debug > 1) cout << "Cycle on MC particles with GlobId::" << MeasGlobId << "\n";


			TVector3 posV;		//global coord [cm]
			TVector3 momV;	//GeV //considering that fragments have same velocity of beam this should be changed accordingly
			posV = TVector3(0,0,0);
			for( vector<int>::iterator itTrackMC = m_measParticleMC_collection->at(MeasGlobId).begin(); itTrackMC != m_measParticleMC_collection->at(MeasGlobId).end(); ++itTrackMC )
			{
				if(*itTrackMC == -666)
				{
					if(m_debug > 0)
						cout << "Pile-up particle from VT!" << endl;
					continue;
				}
				GetTrueParticleType(*itTrackMC, &flukaID, &charge, &mass, &posV, &momV );

				TString outName, pdgName;

				switch(charge)
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

				//CAREFUL HERE!! Think about the possibility of throwing an error -> skip particle for the moment
				if( outName == "fail" ) {continue;}

				pdgName = outName + int(round(mass/m_AMU));

				//CAREFUL HERE!!!!!!!!! FOOT TAGrecoManager file does not have Hydrogen and Helium isotopes!!!! Also think about throwing an error here...
				if ( !TAGrecoManager::GetPar()->Find_MCParticle( pdgName.Data() ) ) 
				{
					if(m_debug > 0) cout << "Found Particle not in MC list: " << pdgName << "\n";
					continue;
				}

				//Set a unique ID for the particle that will be used in the map of Genfit tracks to fit
				outName += Form("_%d_%d", int(round(mass/m_AMU)), *itTrackMC);

				if ( m_debug > 0 ) cout << "\tSelected Category: " << outName << "  flukaID=" << flukaID << "  partID="<< *itTrackMC << "  charge="<<charge << "  mass="<<mass<< "\n";

				if(m_trackCategoryMap->find(outName) == m_trackCategoryMap->end())
				{
					if(m_debug > 0) {
						cout << "Found new particle!! " << outName << "\n";
						posV.Print();
						momV.Print();
					}
					(*m_trackCategoryMap)[outName] = new Track();
					posV.SetMag(posV.Mag()*0.99);
					m_trackCategoryMap->at(outName)->setStateSeed(posV, momV);

					m_trackCategoryMap->at(outName)->addTrackRep( new RKTrackRep( UpdatePDG::GetPDG()->GetPdgCode( pdgName.Data() ) ) );
				}

				AbsMeasurement* hitToAdd = (static_cast<genfit::PlanarMeasurement*> ( *itHit ) )->clone() ;

				m_trackCategoryMap->at(outName)->insertMeasurement( hitToAdd );

				foundHit++;
			
			} //End of loop on MC particles per cluster/measurement

		} //End of loop on measurements per sensor
	
	} //End of loop on sensors
}