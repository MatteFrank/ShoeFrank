/*!
 \file TAGFgeometryConstructor.cxx
 \brief  File for GenFit geometry construction - Global reconstruction with kalman
 \author R. Zarrella
*/

#include "TAGFgeometryConstructor.hxx"

/*!
 \class TAGFgeometryConstructor
 \brief Class that handles the construction of the FOOT geometry in GenFit-compliant form for the Global Reconstruction algorithm
*/


//! Default constructor
TAGFgeometryConstructor::TAGFgeometryConstructor(TAGFdetectorMap* sensorIdMap, TString* systemsOn)
{
	m_SensorIDMap = sensorIdMap;
	m_systemsON = systemsOn;

	gGeoManager->ClearPhysicalNodes();
}


//! Default destructor
TAGFgeometryConstructor::~TAGFgeometryConstructor()
{
	if(m_SensorIDMap)
		delete m_SensorIDMap;
}


//! \brief Check and print which detectors included and/or used in the kalman
void TAGFgeometryConstructor::IncludeDetectors()
{
	// check kalman detectors set in param file are correct
	if (TAGrecoManager::GetPar()->KalSystems().size() == 0)
		Error("IncludeDetectors()", "KalSystems parameter not set properly! Size is 0."), exit(42);

	else if ( !(TAGrecoManager::GetPar()->KalSystems().size() == 1 && TAGrecoManager::GetPar()->KalSystems().at(0) == "all") )
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ )
		{
			if ( !m_SensorIDMap->IsDetectorInMap( TAGrecoManager::GetPar()->KalSystems().at(i) ) )
				Error("IncludeDetectors()", "KalSystems parameter not set properly! Detector '%s' not found in global map.", TAGrecoManager::GetPar()->KalSystems().at(i).c_str()), exit(42);
		}
	}

	// list of detectors used for kalman
	*m_systemsON = "";
	if( TAGrecoManager::GetPar()->KalSystems().at(0) == "all" )
	{
		if(TAGrecoManager::GetPar()->IncludeVT())	*m_systemsON += "VT ";
		if(TAGrecoManager::GetPar()->IncludeIT())	*m_systemsON += "IT ";
		if(TAGrecoManager::GetPar()->IncludeMSD())	*m_systemsON += "MSD ";
		if(TAGrecoManager::GetPar()->IncludeTW())	*m_systemsON += "TW";
	}
	else
	{
		for (unsigned int i=0; i<TAGrecoManager::GetPar()->KalSystems().size(); i++ ) {
			if (i != 0)		*m_systemsON += " ";
			*m_systemsON += TAGrecoManager::GetPar()->KalSystems().at(i);
		}
	}
	if ( FootDebugLevel(2) )	cout << "TAGFgeometryConstructor::IncludeDetectors() -- Detector systems for Kalman:  " << *m_systemsON << endl;

	// print-out of the particle hypothesis used for the fit
	cout << "TAGactKFitter::IncludeDetectors() -- TAGrecoManager::GetPar()->MCParticles()";
	for (unsigned int i=0; i<TAGrecoManager::GetPar()->MCParticles().size(); i++ ) {
		cout << "   " << TAGrecoManager::GetPar()->MCParticles().at(i);
	}
	cout << endl;
}


//! \brief Create the FOOT geometry and declare the detectors in GenFit format
void TAGFgeometryConstructor::CreateGeometry()
{

	if( FootDebugLevel(2) )	cout << "TAGFgeometryConstructor::CreateGeometry() -- START" << endl;

	// take geometry objects
	if (TAGrecoManager::GetPar()->IncludeST())
		m_ST_geo = static_cast<TASTparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TASTparGeo"))->Object() );

	if (TAGrecoManager::GetPar()->IncludeBM())
		m_BM_geo = static_cast<TABMparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TABMparGeo"))->Object() );

	if (TAGrecoManager::GetPar()->IncludeTG())
		m_TG_geo = static_cast<TAGparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"))->Object() );

	if (TAGrecoManager::GetPar()->IncludeDI())
		m_DI_geo = static_cast<TADIparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TADIparGeo"))->Object() );

	if ( TAGrecoManager::GetPar()->IncludeVT() )
		m_VT_geo = static_cast<TAVTparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TAVTparGeo"))->Object() );

	if ( TAGrecoManager::GetPar()->IncludeIT() )
		m_IT_geo = static_cast<TAITparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TAITparGeo"))->Object() );

	if ( TAGrecoManager::GetPar()->IncludeMSD() )
		m_MSD_geo = static_cast<TAMSDparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TAMSDparGeo"))->Object() );

	if ( TAGrecoManager::GetPar()->IncludeTW() )
		m_TW_geo = static_cast<TATWparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TATWparGeo"))->Object() );

	if (TAGrecoManager::GetPar()->IncludeCA())
		m_CA_geo = static_cast<TACAparGeo*> ( gTAGroot->FindParaDsc(FootParaDscName("TACAparGeo"))->Object() );


  m_GeoTrafo = static_cast<TAGgeoTrafo*> ( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()) );


	//set the stage for TGeoManagerInterface class of GenFit
	// this is the World volume
	TGeoMedium* med = gGeoManager->GetMedium("AIR");
	m_TopVolume = gGeoManager->MakeBox("World",med, 300., 300., 300.);
	m_TopVolume->SetInvisible();
	gGeoManager->SetTopVolume(m_TopVolume);

	int indexOfPlane = 0;

	// ST
	if (TAGrecoManager::GetPar()->IncludeST()) {
		TGeoVolume* stVol = m_ST_geo->BuildStartCounter();
		stVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TASTparGeo"));
		m_TopVolume->AddNode(stVol, 1, transfo);
	}

	// BM
	if (TAGrecoManager::GetPar()->IncludeBM()) {
		TGeoVolume* bmVol = m_BM_geo->BuildBeamMonitor();
		bmVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TABMparGeo"));
		m_TopVolume->AddNode(bmVol, 2, transfo);
	}


	// target
	if (TAGrecoManager::GetPar()->IncludeTG()) {
		TGeoVolume* tgVol = m_TG_geo->BuildTarget();
		tgVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TAGparGeo::GetBaseName());
		m_TopVolume->AddNode(tgVol, 3, transfo);

		genfit::AbsFinitePlane* targetArea;
		TVector3 TGsize = m_TG_geo->GetTargetPar().Size;
		TVector3 origin_( m_GeoTrafo->FromTGLocalToGlobal(m_TG_geo->GetTargetPar().Position) );

		if(m_TG_geo->GetTargetPar().Shape == "cubic")
			targetArea = new RectangularFinitePlane(-TGsize.x()/2, TGsize.x()/2, -TGsize.y()/2, TGsize.y()/2);
		else //RZ: Decide on how to handle this if we have different shapes!!
			targetArea = new RectangularFinitePlane(-20,20,-20,20);

		genfit::SharedPlanePtr targetPlane(new genfit::DetPlane(origin_, TVector3(0,0,1), targetArea));
		genfit::SharedPlanePtr targetAirInterfacePlane(new genfit::DetPlane(origin_ + TVector3(0,0,TGsize.z()/2), TVector3(0,0,1), targetArea->clone()));

		TVector3 U(1.,0,0);
		TVector3 V(0,1.,0);
		targetPlane->setUV(U,V);
		m_SensorIDMap->AddFitPlane(-42, targetPlane);
		m_SensorIDMap->AddFitPlaneIDToDet(-42, "TG");

		targetAirInterfacePlane->setUV(U,V);
		m_SensorIDMap->AddFitPlane(-17, targetAirInterfacePlane);
		m_SensorIDMap->AddFitPlaneIDToDet(-17, "TG");

	}

	// Vertex
	if (TAGrecoManager::GetPar()->IncludeVT()) {
		TGeoVolume* vtVol  = m_VT_geo->BuildVertex(m_VT_geo->GetBaseName(), "M28", true);
		vtVol->SetLineColor(kRed+1);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TAVTparGeo"));
		m_TopVolume->AddNode(vtVol, 4, transfo);

		if(m_systemsON->Contains("VT"))
		{
			for ( int i = 0; i < m_VT_geo->GetSensorsN(); ++i ) {
 				int signOffset = m_VT_geo->GetSensorPar(i).IsReverseY ? -1 : 1;
 				TVector3 EpiOffset = signOffset*TVector3(0,0,-m_VT_geo->GetTotalSize().Z()/2 + m_VT_geo->GetPixThickness() + m_VT_geo->GetEpiSize().Z()/2);
 				TVector3 origin_(m_GeoTrafo->FromVTLocalToGlobal(m_VT_geo->GetSensorPosition(i) + EpiOffset) );

				// RZ, note to self: Careful w/ coordinates here: the "IsInActive" functions uses exactly the coordinates give to define the active area, either they are local or global!! BE CONSISTENT AND RE-CHECK EVERYTHING
				float xMin, xMax, yMin, yMax;
				xMin = /*m_VT_geo->GetEpiOffset().X()*/ - m_VT_geo->GetEpiSize().X()/2;
				xMax = /*m_VT_geo->GetEpiOffset().X()*/ + m_VT_geo->GetEpiSize().X()/2;
				yMin = /*m_VT_geo->GetEpiOffset().Y()*/ - m_VT_geo->GetEpiSize().Y()/2;
				yMax = /*m_VT_geo->GetEpiOffset().Y()*/ + m_VT_geo->GetEpiSize().Y()/2;
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane(xMin, xMax, yMin, yMax);
				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, trafoNorm, activeArea));

				//Set versors
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, U));
				TVector3 trafoV = m_GeoTrafo->VecFromVTLocalToGlobal(m_VT_geo->Sensor2DetectorVect(i, V));
				// Some debug print-outs for geometry
				if(FootDebugLevel(2))
				{
					cout << "VT sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
					cout << "Z versor::"; trafoNorm.Print();
				}
				detectorplane->setUV(trafoU, trafoV);
				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "VT");
				++indexOfPlane;
			}
		}
	}

	// Magnet
	if (TAGrecoManager::GetPar()->IncludeDI()) {
		TGeoVolume* diVol = m_DI_geo->BuildMagnet();
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(TADIparGeo::GetBaseName());
		m_TopVolume->AddNode(diVol, 5, transfo);
	}

	// IT
	if (TAGrecoManager::GetPar()->IncludeIT()) {
		TGeoVolume* itVol  = m_IT_geo->BuildInnerTracker(m_IT_geo->GetBaseName(), "Module", true);
		itVol->SetLineColor(kRed);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TAITparGeo"));
		m_TopVolume->AddNode(itVol, 6, transfo);

		if( m_systemsON->Contains("IT") )
		{
			for ( int i = 0; i < m_IT_geo->GetSensorsN(); i++ ) {
				// int signOffsetY = m_IT_geo->GetSensorPar(i).IsReverseY ? -1 : 1;
				// int signOffsetX = m_IT_geo->GetSensorPar(i).IsReverseX ? -1 : 1;
 				TVector3 EpiOffset = TVector3(0,0,-m_IT_geo->GetTotalSize().Z()/2 + m_IT_geo->GetPixThickness() + m_IT_geo->GetEpiSize().Z()/2);
 
 				TVector3 origin_(m_GeoTrafo->FromITLocalToGlobal(m_IT_geo->GetSensorPosition(i) + EpiOffset) );

				float xMin, xMax, yMin, yMax;
				xMin = /*m_IT_geo->GetEpiOffset().X()*/ - m_IT_geo->GetEpiSize().X()/2;
				xMax = /*m_IT_geo->GetEpiOffset().X()*/ + m_IT_geo->GetEpiSize().X()/2;
				yMin = /*m_IT_geo->GetEpiOffset().Y()*/ - m_IT_geo->GetEpiSize().Y()/2;
				yMax = /*m_IT_geo->GetEpiOffset().Y()*/ + m_IT_geo->GetEpiSize().Y()/2;

				// This make all the 32 IT sensors
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane( xMin, xMax, yMin, yMax );
				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::SharedPlanePtr detectorplane (new genfit::DetPlane( origin_, trafoNorm, activeArea));

				// Set versors
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, U));
				TVector3 trafoV = m_GeoTrafo->VecFromITLocalToGlobal(m_IT_geo->Sensor2DetectorVect(i, V));
				detectorplane->setUV(trafoU, trafoV);

				m_SensorIDMap->AddPlane_Zorder( origin_.Z(), indexOfPlane );
				m_SensorIDMap->AddPlane_ZorderLocal( m_IT_geo->GetSensorPosition(i).Z(), indexOfPlane );

				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "IT");
				++indexOfPlane;

				// Some debug print-outs for geometry
				if(FootDebugLevel(2))
				{
					cout << "IT plane::" << indexOfPlane << "\tZ::" << origin_.Z() << endl;
					cout << "IT sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
				}
			}
		}
	}

	// MSD
	if (TAGrecoManager::GetPar()->IncludeMSD()) {
		TGeoVolume* msdVol = m_MSD_geo->BuildMicroStripDetector();
		msdVol->SetLineColor(kViolet);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TAMSDparGeo"));
		m_TopVolume->AddNode(msdVol, 7, transfo);

		if( m_systemsON->Contains("MSD") )
		{
			for ( int i = 0; i < m_MSD_geo->GetSensorsN(); i++ ) {
				int signOffset = i%2 == 1 ? -1 : 1;
				TVector3 EpiOffset = signOffset*TVector3(0,0,-m_MSD_geo->GetTotalSize().Z()/2 + m_MSD_geo->GetMetalThickness() + m_MSD_geo->GetEpiSize().Z()/2);

				TVector3 origin_( m_GeoTrafo->FromMSDLocalToGlobal(m_MSD_geo->GetSensorPosition(i) + EpiOffset ));

				float xMin = /*m_MSD_geo->GetEpiOffset().x()*/ - m_MSD_geo->GetEpiSize().x()/2;
				float xMax = /*m_MSD_geo->GetEpiOffset().x()*/ + m_MSD_geo->GetEpiSize().x()/2;
				float yMin = /*m_MSD_geo->GetEpiOffset().y()*/ - m_MSD_geo->GetEpiSize().y()/2;
				float yMax = /*m_MSD_geo->GetEpiOffset().y()*/ + m_MSD_geo->GetEpiSize().y()/2;

				TVector3 normal_versor = TVector3(0,0,1);
				TVector3 trafoNorm = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i, normal_versor));
				genfit::AbsFinitePlane* activeArea = new RectangularFinitePlane( xMin, xMax, yMin, yMax );
				genfit::SharedPlanePtr detectorplane ( new genfit::DetPlane( origin_, trafoNorm, activeArea) );

				// Set versors -> MSD still needs some fixes maybe
				TVector3 U(1.,0,0);
				TVector3 V(0,1.,0);
				TVector3 trafoU = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i,U));
				TVector3 trafoV = m_GeoTrafo->VecFromMSDLocalToGlobal(m_MSD_geo->Sensor2DetectorVect(i,V));
				// detectorplane->setUV(U, V);
				detectorplane->setUV(trafoU, trafoV);

				m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
				m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "MSD");
				m_SensorIDMap->SetMSDsensorView(i, m_MSD_geo->GetSensorPar(i).TypeIdx);
				++indexOfPlane;

				// Some debug print-outs for geometry
				if(FootDebugLevel(2))
				{
					cout << "MSD sensor::" << i << endl;
					cout << "origin::"; origin_.Print();
					cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
					cout << "U::"; U.Print();
					cout << "V::"; V.Print();
					cout << "trafoU::"; trafoU.Print();
					cout << "trafoV::"; trafoV.Print();
					cout << "trafoNorm::"; trafoNorm.Print();
					cout << "SensorType::"<< m_MSD_geo->GetSensorPar(i).TypeIdx << endl;
				}
			}
		}
	}

	// TW -> RZ: APPLY ROTATIONS TO TW AT SOME POINT
	if (TAGrecoManager::GetPar()->IncludeTW()) {
		TGeoVolume* twVol = m_TW_geo->BuildTofWall();
		twVol->SetLineColor(kBlue);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TATWparGeo"));
		m_TopVolume->AddNode(twVol, 8, transfo);


		if( m_systemsON->Contains("TW") )
		{
			TVector3 origin_( m_GeoTrafo->FromTWLocalToGlobal(m_TW_geo->GetLayerPosition(1)));
			genfit::SharedPlanePtr detectorplane (new genfit::DetPlane(origin_, TVector3(0,0,1)));

			//RZ, note to self: Set the active area of the TW!!!

			// Set versors -> maybe trafo versors not needed
			TVector3 U(1.,0,0);
			TVector3 V(0,1.,0);
			// TVector3 trafoU = m_TW_geo->Detector2SensorVect(i, u);
			// TVector3 trafoV = m_TW_geo->Detector2SensorVect(i, v);
			// detectorplane->setU(trafoU);
			// detectorplane->setV(trafoV);
			detectorplane->setU(U);
			detectorplane->setV(V);

			m_SensorIDMap->AddFitPlane(indexOfPlane, detectorplane);
			m_SensorIDMap->AddFitPlaneIDToDet(indexOfPlane, "TW");
			++indexOfPlane;

			// Some debug print-outs for geometry
			if(FootDebugLevel(2))
			{
				cout << "TW geometry" << endl;
				cout << "origin::"; origin_.Print();
				// cout << "Boundaries::\tx=["<< xMin << "," << xMax << "]\ty=[" << yMin << "," << yMax << "]\n";
				cout << "U::"; U.Print();
				cout << "V::"; V.Print();
			}
		}
	}

	// CA
	if (TAGrecoManager::GetPar()->IncludeCA()) {
		TGeoVolume* caVol = m_CA_geo->BuildCalorimeter();
		caVol->SetLineColor(kBlack);
		TGeoCombiTrans* transfo = m_GeoTrafo->GetCombiTrafo(FootBaseName("TACAparGeo"));
		m_TopVolume->AddNode(caVol, 9, transfo);
	}

	if(FootDebugLevel(2))	cout << "TAGactKFitter::CreateGeometry() -- STOP\n";

}