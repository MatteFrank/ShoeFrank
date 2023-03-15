/*!
 \file TAGFuploader.cxx
 \brief  Class that uploads all the clusters/points in GenFit format
 \author M. Franchini and R. Zarrella
*/

#include "TAGFuploader.hxx"
#include "SpacepointMeasurement.h"
#include "TrackPoint.h"

/*!
 \class TAGFuploader
 \brief  Class that uploads all the clusters/points in GenFit format
*/


//! \brief Default constructor for the Uploader of GenFit TrackPoints.
//! 
//! The class converts clusters/points in GenFit format
//! \param[in] aSensorIDmap Pointer to the TAGFdetectorMap object that handles the GenFit geometry
TAGFuploader::TAGFuploader ( TAGFdetectorMap* aSensorIDmap, bool IsMC ) :
m_allHitMeas(0x0),
m_measParticleMC_collection(0x0),
m_NtracksMC(0),
m_IsMC(IsMC)
{

	m_SensorIDMap = aSensorIDmap;

	if(m_IsMC)
	{
		m_NtracksMC = ((TAMCntuPart*)gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object())->GetTracksN();
		m_measParticleMC_collection = new map< int, vector<int> >();
	}
	
	m_GeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

	m_debug = TAGrecoManager::GetPar()->Debug();
	// list of detectors used for kalman
	m_systemsON = "";
	if( TAGrecoManager::GetPar()->KalSystems().at(0) == "all" )
	{
		if(TAGrecoManager::GetPar()->IncludeVT())	m_systemsON += "VT ";
		if(TAGrecoManager::GetPar()->IncludeIT())	m_systemsON += "IT ";
		if(TAGrecoManager::GetPar()->IncludeMSD())	m_systemsON += "MSD ";
		if(TAGrecoManager::GetPar()->IncludeTW())	m_systemsON += "TW";
	}
	else
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
			if (i != 0)		m_systemsON += " ";
			m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
		}
	}

	switchOff_HHe = true;
}


//! \brief Default destructor
//!
//! CURRENTLY NOT USED -> CHECK. Added to have a complete documentation of the class
TAGFuploader::~TAGFuploader()
{
	if(m_allHitMeas)
	{
		for(auto it = m_allHitMeas->begin(); it != m_allHitMeas->end(); ++it)
		{
			for(auto itvec : it->second)
				delete itvec;
			it->second.clear();
		}
		m_allHitMeas->clear();
	}

	if(m_IsMC && m_measParticleMC_collection)
	{
		if( m_measParticleMC_collection->size() > 0 )
		{
			for(auto it = m_measParticleMC_collection->begin(); it != m_measParticleMC_collection->end(); ++it)
				it->second.clear();
		}
		m_measParticleMC_collection->clear();
		delete m_measParticleMC_collection;
	}
}


//----------------------------------------------------------------------------------------------------

//! \brief Upload the hits to be fitted in GenFit format from all the detectors included in the campaign
//!
//! \param[in,out] allHitMeas Map associating each GenFit plane with the vector of measurements obtained in it
//! \return 1 if everything worked properly
int TAGFuploader::TakeMeasHits4Fit(  map< int, vector<AbsMeasurement*> > &allHitMeas  ) {

	m_allHitMeas = &allHitMeas;

	if ( m_debug > 0 )		cout << "\n\n*******\tUploaderKalmanGF::PrepareData4Fit\t*******\n" << endl;

	// Vertex -  fill fitter collections
	if ( m_systemsON.Contains("VT") && TAGrecoManager::GetPar()->IncludeVT() ) {
		UploadClusVT();
		if ( m_debug > 0 )		Info( "TakeMeasHits4Fit", "Filling vertex hit collection " );
	}

	// Inner Tracker -  fill fitter collections
	if ( m_systemsON.Contains("IT") && TAGrecoManager::GetPar()->IncludeIT() ) {
		UploadClusIT();
		if ( m_debug > 0 )		Info( "TakeMeasHits4Fit", "Filling Inner Tracker hit collection ");
	}

	// MSD -  fill fitter collections
	if ( m_systemsON.Contains("MSD") && TAGrecoManager::GetPar()->IncludeMSD() ) {
		UploadClusMSD();
		if ( m_debug > 0 )		cout << endl<<endl << "Filling Strip hit collection"<<endl;
	}

	// Tof Wall-  fill fitter collections
	if ( m_systemsON.Contains("TW") && TAGrecoManager::GetPar()->IncludeTW() ) {
		UploadHitsTW();
		if ( m_debug > 0 )		cout <<endl<<endl << "Filling scintillator hit collection " << endl;
	}

	if(m_debug > 0) cout << "TAGFuploader::TakeMeasHits4Fit  ->  " << m_allHitMeas->size() << endl;

	return 1;
}





//-------------------------------------------------------------------------------------------------

//! \brief Upload the VT clusters in GenFit format
//!
//! \return Number of all the VT clusters found in the event
int TAGFuploader::UploadClusVT(){

	//cluster test
	TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus","TAVTntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAVTparGeo*)gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() )->GetSensorsN();

	// loop over VT planes
	for( int iSensor = 0; iSensor < nSensors; iSensor++){

		int nclus = vtclus->GetClustersN(iSensor);

		if (m_debug > 1) Info( "UploadClusVT()", "found %i  in sensor %i", nclus, iSensor );

		if (nclus == 0) continue;
		totClus += nclus;

		// loop over Clusters
		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)	      Info( "UploadClusVT()", "entered cycle clusVT of sensor %d", iSensor );

			TAVTcluster* clus = vtclus->GetCluster(iSensor, iClus);
			if(m_debug > 1 && iClus != clus->GetClusterIdx())
			{
				Warning("UploadClusVT()","VT INDEX MISMATCH!! Index::%d  Idx::%d", iClus, clus->GetClusterIdx());
			}
			if (!clus->IsValid())	continue; // Guardare meglio cosa significa...

			Prepare4Vertex( clus, m_SensorIDMap->GetMeasID_eventLevel( "VT", iSensor, iClus ) );

		}
	}
	return totClus;
}





//---------------------------------------------------------------------------------------------------

//! \brief Upload the IT clusters in GenFit format
//!
//! \return Number of all the IT clusters found in the event
int TAGFuploader::UploadClusIT(){

	TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() )->GetSensorsN();

	for( int iSensor = 0; iSensor < nSensors; iSensor++){

		int nclus = itclus->GetClustersN(iSensor);

		if (m_debug > 1)	std::cout << "found " << nclus << " in sensor " << iSensor << "\n";

		if (nclus == 0) continue;
		totClus += nclus;

		for( int iClus = 0; iClus < nclus; ++iClus ) {

			if (m_debug > 1)      std::cout << "entered cycle clusIT of sensor " << iSensor << std::endl;

			TAITcluster* clus = itclus->GetCluster(iSensor, iClus);
			if(iClus != clus->GetClusterIdx())
			{
				Warning("UploadClusIT()","IT INDEX MISMATCH!! Index::%d  Idx::%d", iClus, clus->GetClusterIdx());
			}
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			Prepare4InnerTracker( clus, m_SensorIDMap->GetMeasID_eventLevel( "IT", iSensor, iClus ) );

		}
	}
	return totClus;
}







//----------------------------------------------------------------------------------------------------

//! \brief Upload the MSD clusters in GenFit format
//!
//! \return Number of all the MSD clusters found in the event
int TAGFuploader::UploadClusMSD() {

	TAMSDntuCluster* msdclus = (TAMSDntuCluster*) gTAGroot->FindDataDsc("msdClus","TAMSDntuCluster")->Object();

	int totClus = 0;
	int nSensors = ( (TAMSDparGeo*) gTAGroot->FindParaDsc("msdGeo", "TAMSDparGeo")->Object() )->GetSensorsN();

	for( int iSensor = 0; iSensor < nSensors; iSensor++){
		// cout << "Sensor::" << iSensor << endl;
		int nclus = msdclus->GetClustersN(iSensor);

		if (m_debug > 1)	std::cout << "found " << nclus << " in sensor " << iSensor << "\n";

		if (nclus == 0) continue;
		totClus += nclus;

		for(Int_t iClus = 0; iClus < nclus; ++iClus){

			if (m_debug > 1)	      std::cout << "entered cycle clusMSD of sensor " << iSensor << std::endl;

			TAMSDcluster* clus = msdclus->GetCluster(iSensor, iClus);
			if(iClus != clus->GetClusterIdx())
			{
				Warning("UploadClusMSD()","MSD INDEX MISMATCH!! Index::%d  Idx::%d", iClus, clus->GetClusterIdx());

			}
			if (!clus->IsValid()) continue;		// Guardare meglio cosa significa...

			Prepare4Strip( clus, m_SensorIDMap->GetMeasID_eventLevel( "MSD", iSensor, iClus ) );

		}
	}

	//Print out MSDpoint coordinates if in debug mode
	if(m_debug > 1)
	{
		TAMSDntuPoint* msdpoint = (TAMSDntuPoint*) gTAGroot->FindDataDsc("msdPoint","TAMSDntuPoint")->Object();
		for( int iSensor = 0; iSensor < nSensors; iSensor++)
		{
			if(iSensor%2==1)
			{
				int npoint = msdpoint->GetPointsN(iSensor/2);
				std::cout << "station::" << iSensor/2 << std::endl;
				for(int j=0;j<npoint;++j)
				{
					TAMSDpoint* point = msdpoint->GetPoint(iSensor/2, j);
					std::cout << "pointId::" << j << std::endl;
					std::cout << "loc::";
					point->GetPosition().Print();
					std::cout << "posg::";
					point->GetPositionG().Print();
					std::cout << "glob::";
					m_GeoTrafo->FromMSDLocalToGlobal(point->GetPositionG()).Print();
				}
			}
		}
	}

	return totClus;
}





//----------------------------------------------------------------------------------------------------

//! \brief Upload measurement points from TOF-Wall
//!
//! \return Number of points found in the TW
int TAGFuploader::UploadHitsTW() {

	// take the ntuple object already filled
	TATWntuPoint* ntup = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();
	if ( m_debug > 0 )	cout << "\nnumber of TW points read: " << ntup->GetPointsN() << "\n";

	int totPoints = ntup->GetPointsN();

	if(totPoints < 1 && m_debug > 0)
		Warning("TAGFuploader()", "Found %d points in TW for event %d", totPoints, gTAGroot->CurrentEventId().EventNumber());

	// save hits in the collection
	for ( int iPoint = 0; iPoint < totPoints; iPoint++ ) {

		TATWpoint* point = ntup->GetPoint(iPoint);

		//HACK to prevent fake points (try)
		if ( point->GetToF() > 15 )
		{
			if(m_debug > 0) Warning("UploadHitsTW()", "Found point with high TOF:: %lf", point->GetToF());
			continue;
		}

		Prepare4TofWall( point, m_SensorIDMap->GetMeasID_eventLevel( "TW", 0, iPoint ) );

	}

	return totPoints;
}





//----------------------------------------------------------------------------------------------------

//! \brief Get the map containing the vector of MC particles for each measurement
//!
//! \return Pointer to the map GlobalMeasureId -> vector of MC particles
map< int, vector<int> >* TAGFuploader::TakeMeasParticleMC_Collection() {
 	return m_measParticleMC_collection;
}




// to be checked
int TAGFuploader::GetTWTrackFixed ( TATWpoint* point ) {

	int indexSize = point->GetMcTracksN();

	if ( indexSize == 1 )  {//in this case no pb
		int index = point->GetMcTrackIdx(0);
		if ( index != -1 ) return index;
	}


	TATWhit* rowHit = point->GetRowHit();
	TATWhit* colHit = point->GetColumnHit();

	int matching = -1;
	for (int iRow = 0; iRow < rowHit->GetMcTracksN(); ++iRow){
		for (int iCol = 0; iCol < colHit->GetMcTracksN(); ++iCol){
			if (rowHit->GetMcTrackIdx(iRow) == colHit->GetMcTrackIdx(iCol))
			matching = rowHit->GetMcTrackIdx(iRow);
		}
	}
	return matching;
}





//! \brief Get all the possible charges measured by the TOF-Wall in the event
//!
//! \param[out] chVect Pointer to vector where to store the possible charge values
void TAGFuploader::GetPossibleCharges( vector<int>* chVect, bool IsMC ) {

	if( IsMC )
	{	
		TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
		
		for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

			TAMCpart* point = m_McNtuEve->GetTrack(iPart);		
			if ( point->GetCharge() > 0 && point->GetCharge() <= ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber) {
				if ( find( chVect->begin(), chVect->end(), point->GetCharge() ) == chVect->end() ) {
					chVect->push_back( point->GetCharge() );
					if ( m_debug > 0 )		
						cout << "TAGFuploader::GetPossibleCharges  " << point->GetCharge() << "\n";
				}
			}
		}
	}
	else //data-like: get all possible charges from TW
	{
		// for(int i=1; i<= ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber; ++i)	chVect->push_back( i );
		TATWntuPoint* twPoint = (TATWntuPoint*) gTAGroot->FindDataDsc("twPoint", "TATWntuPoint")->Object();

		// save hits in the collection
		for (int iPoint = 0; iPoint < twPoint->GetPointsN(); iPoint++) {

			TATWpoint* point = twPoint->GetPoint( iPoint );
			int tmp_ch = point->GetChargeZ();

			if ( m_debug > 1 ) cout << "TAGFuploader::GetPossibleCharges  " << tmp_ch << endl;
			
			if ( tmp_ch > -1)
			{
				if ( find( chVect->begin(), chVect->end(), tmp_ch ) == chVect->end() )
					chVect->push_back( tmp_ch );	
			}
		}
	}
}






//! \brief Get the total number of particles generated in the MC event
//!
//! \return Number of particles generated in the event
int TAGFuploader::GetNumGenParticle_noFrag() {

	TAMCntuPart* m_McNtuEve = (TAMCntuPart*) gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
	
	int count = 0;
	for ( int iPart = 0; iPart < m_McNtuEve->GetTracksN(); iPart++ ) {

		TAMCpart* particle = m_McNtuEve->GetTrack(iPart);		
		if ( particle->GetCharge() > 0 && particle->GetCharge() <= ( (TAGparGeo*) gTAGroot->FindParaDsc("tgGeo", "TAGparGeo")->Object() )->GetBeamPar().AtomicNumber) {

			if ( particle->GetInitPos().z() > 1 ) continue;
			if ( particle->GetFinalPos().z() < m_GeoTrafo->FromTWLocalToGlobal(TVector3(0,0,0) ).Z() ) continue;

			count++;

		}
		
	}
	return count;
}






//----------------------------------------------------------------------------------------------------

//! \brief Upload a single VT cluster to the GenFit hit collection
//!
//! \param[in] clus Pointer to VT cluster
//! \param[in] iMeas Global measurement Id
void TAGFuploader::Prepare4Vertex( TAVTcluster* clus, int iMeas ) {

	if ( m_debug > 0 )		cout << "\nPrepare4Vertex::Entered\n";

	TMatrixDSym hitCov(3);
	TMatrixDSym planarCov(2);
	TVectorD planarCoords(2);

	// get pixel coord
	TVector3 hitPos = clus->GetPosition();

	if ( m_debug > 1 )
	{
		cout << "VT meas::" << iMeas << endl;
		TAVTparGeo* m_VT_geo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
		cout << "Sensor pos::";
		m_VT_geo->GetSensorPosition(m_SensorIDMap->GetSensorIDFromMeasID(iMeas)).Print();
		cout << "VT hit loc coords::";
		hitPos.Print();
		cout << "VT hit det coords::";
		m_VT_geo->Sensor2Detector(m_SensorIDMap->GetSensorIDFromMeasID(iMeas), hitPos).Print();
		cout << "VT hit glb coords::";
		m_GeoTrafo->FromVTLocalToGlobal( m_VT_geo->Sensor2Detector( m_SensorIDMap->GetSensorIDFromMeasID(iMeas), hitPos) ).Print();
	}

	// set hit position vector
	planarCoords(0) = hitPos.x();	planarCoords(1) = hitPos.y();  

	// set covariance matrix
	TVector3 pixReso = clus->GetPosError();    // z???

	// to be tested for SpasePoints
	// hitCov.UnitMatrix();
	// for (int j = 0; j < 3; j++){
	//   hitCov[j][j] = pixReso(j)*pixReso(j);
	// }
	// hitCov[2][2] = 0.000005;		//hardcoded

	// planar points
	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++){
	  planarCov[k][k] = pixReso(k)*pixReso(k);
	  // planarCov[k][k] = 0.001*0.001;
	}

	if( m_IsMC )
	{
		vector<int> mcParticlesInMeasuerement;
		// cout << "VT MEAS:: " << iMeas << " has N track MC = " << clus->GetMcTracksN() << endl;
		for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
			mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
		}

		(*m_measParticleMC_collection)[iMeas];
		(*m_measParticleMC_collection)[iMeas] = mcParticlesInMeasuerement;
	}


	int planeID = m_SensorIDMap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_SensorIDMap->GetDetIDFromMeasID( iMeas );
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr ); // nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	hit->setPlane( m_SensorIDMap->GetFitPlane(planeID), planeID ); 
	(*m_allHitMeas)[ planeID ].push_back(hit);


	if ( m_debug > 0 )  cout << "\nPrepare4Vertex::Exiting\n";
}







//----------------------------------------------------------------------------------------------------

//! \brief Upload a single IT cluster to the GenFit hit collection
//!
//! \param[in] clus Pointer to IT cluster
//! \param[in] iMeas Global measurement Id
void TAGFuploader::Prepare4InnerTracker( TAITcluster* clus, int iMeas ) {

	if ( m_debug > 0 )	  cout << "\nPrepare4InnerTracker::Entered\n";

	TMatrixDSym planarCov(2);
	TVectorD planarCoords(2);

	// get pixel coord
	TVector3 hitPos = clus->GetPosition();

	if ( m_debug > 0 )
	{
		TAITparGeo* m_IT_geo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object();
		cout << "IT meas::" << iMeas << endl;
		cout << "Sensor pos::";
		m_IT_geo->GetSensorPosition(m_SensorIDMap->GetSensorIDFromMeasID(iMeas)).Print();
		cout << "IT hit loc coords::";
		hitPos.Print();
		cout << "IT hit det coords::";
		m_IT_geo->Sensor2Detector(m_SensorIDMap->GetSensorIDFromMeasID(iMeas), hitPos).Print();
		cout << "IT hit glb coords::";
		m_GeoTrafo->FromITLocalToGlobal( m_IT_geo->Sensor2Detector( m_SensorIDMap->GetSensorIDFromMeasID(iMeas), hitPos) ).Print();
	}


	// set hit position vector
	planarCoords(0) = hitPos.x();
	planarCoords(1) = hitPos.y();

	// set covariance matrix
	TVector3 pixReso = clus->GetPosError();
	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++){
	  planarCov[k][k] = pixReso(k)*pixReso(k);
	}

	if( m_IsMC )
	{
		vector<int> mcParticlesInMeasuerement;
		for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
			mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
		}

		(*m_measParticleMC_collection)[iMeas];
		(*m_measParticleMC_collection)[iMeas] = mcParticlesInMeasuerement;
	}

	int sensorID = m_SensorIDMap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_SensorIDMap->GetDetIDFromMeasID( iMeas );
	// nullptr e' un TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane( m_SensorIDMap->GetFitPlane(sensorID), sensorID );	 

	(*m_allHitMeas)[ sensorID ].push_back(hit);

	if ( m_debug > 0 )			cout << "\nPrepare4InnerTracker::Exiting\n";

}







//----------------------------------------------------------------------------------------------------

//! \brief Upload a single MSD cluster to the GenFit hit collection
//!
//! \param[in] clus Pointer to MSD cluster
//! \param[in] iMeas Global measurement Id
void TAGFuploader::Prepare4Strip( TAMSDcluster* clus, int iMeas ) {

	if ( m_debug > 0 )	 cout << "\nPrepare4Strip::Entered\n";

	TMatrixDSym planarCov(1);
	TVectorD planarCoords(1);


	// get pixel local coord
	TVector3 hitPos = clus->GetPosition();

	if ( m_debug > 1 )
	{
		TAMSDparGeo* m_MSD_geo = (TAMSDparGeo*) gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object();
		cout << "MSD Meas::" << iMeas << "\t View::" << clus->GetPlaneView() <<  endl;
		cout << "Sensor pos::";
		m_MSD_geo->GetSensorPosition(m_SensorIDMap->GetSensorIDFromMeasID(iMeas)).Print();
		cout << "MSD hit loc coords::";
		hitPos.Print();
		cout << "MSD hit loc frame hit coord::" << clus->GetPositionF() << endl;
		cout << "MSD hit det coords::";
		m_MSD_geo->Sensor2Detector(m_SensorIDMap->GetSensorIDFromMeasID(iMeas), hitPos).Print();
		cout << "MSD PositionG::";
		clus->GetPositionG().Print();
		cout << "MSD hit glb coords::";
		m_GeoTrafo->FromMSDLocalToGlobal( clus->GetPositionG() ).Print();
	}

	//check the view, 0 ->X, 1->Y
	double pixReso = 0;
	bool isYView = false;

	//RZ: WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//MSD detector coordinates are all in the X value of the position vector!!!! So, an X strip will give the local pos in the X value. A Y strip will rotate the Y measurement in local coords and return it in the X when looking at detector coordinates.

	planarCoords(0) = clus->GetPositionF();
	pixReso = clus->GetPosErrorF();
	if ( clus->GetPlaneView() == 1 )
		isYView = true;

	// set covariance matrix
	planarCov.UnitMatrix();
	for (int k = 0; k < 1; k++) {
		planarCov[k][k] = pixReso*pixReso;
	}

	if( m_IsMC )
	{
		vector<int> mcParticlesInMeasuerement;
		for (int nPart = 0; nPart < clus->GetMcTracksN(); nPart++) {
				mcParticlesInMeasuerement.push_back( clus->GetMcTrackIdx( nPart ) );
		}

		(*m_measParticleMC_collection)[iMeas];
		(*m_measParticleMC_collection)[iMeas] = mcParticlesInMeasuerement;
	}

	int sensorID = m_SensorIDMap->GetFitPlaneIDFromMeasID( iMeas );
	int detId = m_SensorIDMap->GetDetIDFromMeasID( iMeas );
	// nullptr is a TrackPoint(fitTrack). Leave like this otherwise it gives memory leak problems!!!!
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	// if (isYView) hit->setStripV();
	if (isYView) hit->setYview();
	hit->setPlane( m_SensorIDMap->GetFitPlane(sensorID), sensorID );
	if( m_debug > 1 )
	{
		cout << "toLab::";
		(m_SensorIDMap->GetFitPlane(sensorID)->toLab(TVector2(clus->GetPositionF(),0))).Print(); 
	}

	(*m_allHitMeas)[ sensorID ].push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4Strip::Exiting\n";

}







//----------------------------------------------------------------------------------------------------

//! \brief Upload a single TW point to the GenFit hit collection
//!
//! \param[in] point Pointer to TW point
//! \param[in] iMeas Global measurement Id
void TAGFuploader::Prepare4TofWall( TATWpoint* point, int iMeas) {

	if ( m_debug > 0 )	  cout << "\nPrepare4TofWall::Entered\n";

	TMatrixDSym planarCov(2);
	TVectorD planarCoords(2);

	// get point local coords
	TVector3 hitPos = point->GetPosition();

	if ( m_debug > 1 )
	{
		cout << "TW hit loc coords::";
		hitPos.Print();
		cout << "TW glob hit coords::";
		// TATWparGeo *m_TW_geo = (TATWparGeo *)gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object();
		m_GeoTrafo->FromTWLocalToGlobal(point->GetPosition()).Print();
	}

	// set point position vector
	planarCoords(0) = hitPos.x();
	planarCoords(1) = hitPos.y();
	TVector3 pixReso = point->GetPosErrorG();

	if (m_debug > 1)
	pixReso.Print();

	planarCov.UnitMatrix();
	for (int k = 0; k < 2; k++)
	{
	planarCov[k][k] = pixReso(k)*pixReso(k);
	if(m_debug > 1) 
		cout << pixReso(k)*pixReso(k) << endl;
	}

	if( m_IsMC )
	{
		vector<int> mcParticlesInMeasuerement;
		// cout << "TW MEAS:: " << iMeas << " has N track MC = " << point->GetMcTracksN() << endl;
		for (int nPart = 0; nPart < point->GetMcTracksN(); nPart++) {
			mcParticlesInMeasuerement.push_back( point->GetMcTrackIdx( nPart ) );
		}

		(*m_measParticleMC_collection)[iMeas];
		(*m_measParticleMC_collection)[iMeas] = mcParticlesInMeasuerement;
	}

	int sensorID = m_SensorIDMap->GetFitPlaneTW();
	int detId = m_SensorIDMap->GetDetIDFromMeasID( iMeas);
	PlanarMeasurement* hit = new PlanarMeasurement(planarCoords, planarCov, detId, iMeas, nullptr );
	hit->setPlane(m_SensorIDMap->GetFitPlane(sensorID), sensorID);      
	(*m_allHitMeas)[ sensorID ].push_back(hit);

	if ( m_debug > 0 )	  cout << "\nPrepare4TofWall::Exiting\n";

}




