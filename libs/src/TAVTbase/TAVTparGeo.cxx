/*!
  \file
  \version $Id: TAVTparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAVTparGeo.
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TEveGeoShapeExtract.h"
#include "TEveTrans.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"

#include "foot_geo.h"
#include "GlobalPar.hxx"



//_____________________________________________________________________________
TAVTparGeo::TAVTparGeo() {

    m_nPassiveLayersPerBoard_z = 13;
    m_volumeCount = -1;
    m_passiveCount = -1;
    m_setW_0number = 2;

    m_debug = GlobalPar::GetPar()->Debug();

    // fill m_materialOrder, m_materialThick, m_materialType
    InitMaterial();

};



//_____________________________________________________________________________
void TAVTparGeo::InitMaterial() {

    m_materialOrder = {     "VTX_MEDIUM",
                            "VTX_MEDIUM"
                             };

    m_passiveMaterial = {};


    for ( unsigned int i=0; i<m_materialOrder.size(); i++ ) {
        if( m_materialOrder[i] == "VTX_MEDIUM" ){
            m_materialThick[ m_materialOrder[i] ] = VTX_THICK;
            m_materialType[ m_materialOrder[i] ] = VTX_MEDIUM;
        }
    }
}




//_____________________________________________________________________________
//  copy constructor
TAVTparGeo::TAVTparGeo( TAVTparGeo* original ) :

    m_rotation(original->m_rotation),
    m_origin(original->m_origin),  // current position
    m_center(original->m_center),  // current position
    m_dimension(original->m_dimension),

    m_nSensors (original->m_nSensors),

    m_materialOrder(original->m_materialOrder),

    m_materialThick(original->m_materialThick),
    m_materialType(original->m_materialType),

    m_siliconSensorThick_Lz(original->m_siliconSensorThick_Lz),
    m_layerDistance_samePair(original->m_layerDistance_samePair),
    m_layerDistance_interPair(original->m_layerDistance_interPair),

    m_nPixel_X(original->m_nPixel_X),
    m_nPixel_Y(original->m_nPixel_Y)         
    {

        SensorMatrix m_sensorMatrix = original->m_sensorMatrix;
}





//_____________________________________________________________________________
void TAVTparGeo::InitGeo()  {

    if ( GlobalPar::GetPar()->Debug() > 0 )     cout << "\n\nTAVTparGeo::InitGeo" << endl<< endl;

    m_origin = TVector3(0,0,0);                         // center in local coord.
    m_center = TVector3(VTX_X, VTX_Y, VTX_Z);           // center in global coord.
    
    m_nSensors = TVector3( 1, 1, VTX_NLAY );            // set number of sensors


//---------------------------------------------------------------------
//     Find DETECTOR dimension
//---------------------------------------------------------------------

    m_layerDistance_samePair  = VTX_LAYDIST1;          // from center to center
    m_layerDistance_interPair = VTX_LAYDIST2;          // from center to center
    m_siliconSensorThick_Lz = VTX_THICK;    // ONLY silicon

    // set detector dimension
    double length_Lz = m_siliconSensorThick_Lz + (VTX_NLAY/2)*m_layerDistance_samePair + (-1+VTX_NLAY/2)*m_layerDistance_interPair; // from edge to edge
    m_dimension = TVector3( VTX_WIDTH + 2*VTX_XDEAD , 2*VTX_HEIGHT - VTX_SENSE_HEIGHT, length_Lz );


//---------------------------------------------------------------------
//     Init SENSOR geometry
//---------------------------------------------------------------------
    if ( GlobalPar::GetPar()->Debug() > 0 ) cout << " Init SENSOR geometry " << endl;

    double pixelDistance = 0;  

    // evaluate sensor dimension 
    // double sensor_Width_Lx = VTX_SENSE_WIDTH;
    // double sensor_Height_Ly = VTX_SENSE_HEIGHT;
    // double sensor_Length_Lz = m_siliconSensorThick_Lz;
    TVector3 sensorDimension = TVector3( VTX_SENSE_WIDTH, VTX_SENSE_HEIGHT, m_siliconSensorThick_Lz );
    TVector3 passiveSiDimension = TVector3( VTX_WIDTH, VTX_HEIGHT, m_siliconSensorThick_Lz );

    // pixels per sensors
    m_nPixel_X = VTX_XPIX;
    m_nPixel_Y = VTX_YPIX;
    m_pitchX = VTX_DX;
    m_pitchY = VTX_DY;  


    // fill sensor matrix
    double sensor_newZ = m_origin.Z() - m_dimension.z()/2 +0.5*m_siliconSensorThick_Lz;
    m_sensorMatrix.resize( m_nSensors.Z() );
    for (int k=0; k<m_nSensors.Z(); k++) {
        if ( k!=0 )     // increment the layer distance Z, distance not uniform
            sensor_newZ += ( k%2 != 0 ? m_layerDistance_samePair : m_layerDistance_interPair );
        m_sensorMatrix[k].resize( m_nSensors.X() );
        for (int i=0; i<m_nSensors.X(); i++) {
            double sensor_newX = m_origin.X();  
            m_sensorMatrix[k][i].resize( m_nSensors.Y() );
            for (int j=0; j<m_nSensors.Y(); j++) {
                double sensor_newY = m_origin.Y();

                m_sensorMatrix[k][i][j] = new IronPlate();

                m_volumeCount++;
                stringstream ss_bodySensorName; ss_bodySensorName << "vtxs" << m_volumeCount;
                stringstream ss_regionSensorName; ss_regionSensorName << "VTXS" << m_volumeCount;
                m_sensorMatrix[k][i][j]->SetMaterial( m_materialType[ "VTX_MEDIUM" ], "VTX_MEDIUM", ss_bodySensorName.str(), ss_regionSensorName.str(), m_volumeCount );

                m_sensorMatrix[k][i][j]->SetSensor(
                        TVector3( sensor_newX, sensor_newY, sensor_newZ ),  // sensor center
                        TVector3( sensorDimension.x(), sensorDimension.y(), sensorDimension.z() ),    // sensor dimension
                        m_nPixel_X, m_nPixel_Y,
                        m_pitchX, m_pitchY, m_siliconSensorThick_Lz,
                        pixelDistance, pixelDistance, 0, //layerDistance,
                        TVector3(0,0,0)
                 );

                if ( GlobalPar::GetPar()->Debug() > 0 ) cout << "sensor center ",    TVector3( sensor_newX, sensor_newY, sensor_newZ ).Print();
            }
        }
    }

    m_rotation = new TRotation();
    // m_rotation->SetYEulerAngles( m_tilt_eulerAngle.x(), m_tilt_eulerAngle.y(), m_tilt_eulerAngle.z() );
    m_rotation->SetYEulerAngles( 0,0,0 );

//---------------------------------------------------------------------
//     Init passive materials geometry
//---------------------------------------------------------------------
    if ( GlobalPar::GetPar()->Debug() > 0 ) cout << "Init passive materials geometry" << endl;

    double passiveSi_Z = m_origin.Z() - m_dimension.z()/2 +0.5*m_siliconSensorThick_Lz;
    m_passiveMatrix.resize( m_nSensors.Z() );
    for (int k=0; k<m_nSensors.Z(); k++) {
        if ( k!=0 )     // increment the layer distance Z, distance not uniform
            passiveSi_Z += ( k%2 != 0 ? m_layerDistance_samePair : m_layerDistance_interPair );
        m_passiveMatrix[k].resize( m_nSensors.X() );
        for (int i=0; i<m_nSensors.X(); i++) {
            double passiveSi_X = m_origin.X() + sensorDimension.x()/2 + VTX_XDEAD - passiveSiDimension.x()/2;
            passiveSi_X *= ( i>1 ? -1 : 1 );   // commenta!
            m_passiveMatrix[k][i].resize( m_nSensors.Y() );
            for (int j=0; j<m_nSensors.Y(); j++) {
                double passiveSi_Y = m_origin.Y() + passiveSiDimension.y()/2 - sensorDimension.y()/2;
                passiveSi_Y *= ( j%2 == 0 ? -1 : 1 );   // commenta!

                m_passiveCount++;
                stringstream ss_bodyPassiveName;      ss_bodyPassiveName << "vtxp" << m_passiveCount;
                stringstream ss_regionPassiveName;      ss_regionPassiveName << "VTXP" << m_passiveCount;


                m_passiveMatrix[k][i][j] = new FootBox( TVector3( passiveSi_X, passiveSi_Y, passiveSi_Z ),          // position passive
                                                TVector3( passiveSiDimension.x(), passiveSiDimension.y(), passiveSiDimension.z() ),   // dimension passive
                                                (string)VTX_MEDIUM,                         // name of the material
                                                "VTX_MEDIUM",                       // name of the material-region in the foot_geo.h
                                                ss_bodyPassiveName.str(),                  // FLUKA body name
                                                ss_regionPassiveName.str(),          // FLUKA region name
                                                m_volumeCount                       // volume ID num
                                                );


                if ( GlobalPar::GetPar()->Debug() > 0 ) cout << "passive center ",    TVector3( passiveSi_X, passiveSi_Y, passiveSi_Z ).Print();

            }
        }
    }


    // create the universe volume
    if ( GlobalPar::GetPar()->geoROOT() ) {
        m_universe = gGeoManager->MakeBox("VTXuniverse",gGeoManager->GetMedium("AIR"),m_dimension.x()/2,m_dimension.y()/2,m_dimension.z()/2); //top è scatola che conterrà tutto (dimensioni in cm)
        m_universe->SetLineColor(kGreen);
        m_universe->SetVisibility(kTRUE);
        gGeoManager->SetTopVisible(10);
    }

//---------------------------------------------------------------------
//     Build passive materials in ROOT and FLUKA
//---------------------------------------------------------------------
    if ( GlobalPar::GetPar()->Debug() > 0 ) cout << "Build passive materials in ROOT and FLUKA" << endl;

    // int sensor_k = 0;
    // cout << m_passiveMatrix.size()<<endl;
    // for ( PassiveMatrix::iterator itX = m_passiveMatrix.begin(); itX != m_passiveMatrix.end(); itX++ ) {
    //     cout << (*itX).size()<<endl;
    //     int sensor_i = 0;
    //     for ( PassivePlane::iterator itY = (*itX).begin(); itY != (*itX).end(); itY++ ) {
    //         cout << (*itY).size()<<endl;
    //         int sensor_j = 0;
    //         for ( PassiveLine::iterator itZ = (*itY).begin(); itZ != (*itY).end(); itZ++ ) {

    for ( unsigned int k=0; k<m_nSensors.Z(); k++ ) {
        for ( unsigned int j=0; j<m_nSensors.Y(); j++ ) {
            for ( unsigned int i=0; i<m_nSensors.X(); i++ ) {    
                
                //ROOT addNode
                if ( GlobalPar::GetPar()->geoROOT() )    {
                    if ( !gGeoManager->GetVolume( m_passiveMatrix[k][i][j]->GetMaterialRegionName().c_str() ) )       cout << "ERROR >> FootBox::AddNodeToUniverse  -->  volume not defined: "<< m_passiveMatrix[k][i][j]->GetMaterialRegionName() << endl;

                    TVector3 globalCoord = m_passiveMatrix[k][i][j]->GetPosition();
                    Local2Global(&globalCoord);
                    m_universe->AddNode( gGeoManager->GetVolume( m_passiveMatrix[k][i][j]->GetMaterialRegionName().c_str() ), 
                                        m_passiveMatrix[k][i][j]->GetNodeID() , 
                                        new TGeoCombiTrans( globalCoord.x(), globalCoord.y(), globalCoord.z(), 
                                        new TGeoRotation("null,",0,0,0) ) );
                    if ( GlobalPar::GetPar()->Debug() > 0 ) cout <<"\t"<<m_passiveMatrix[k][i][j]->GetMaterialRegionName()<<"  ", globalCoord.Print();
                }

                // boidies
                if ( GlobalPar::GetPar()->geoFLUKA() ) {

                    TVector3 minCoord = TVector3( m_passiveMatrix[k][i][j]->GetMinCoord().x(), m_passiveMatrix[k][i][j]->GetMinCoord().y(), m_passiveMatrix[k][i][j]->GetMinCoord().z() );
                    TVector3 maxCoord = TVector3( m_passiveMatrix[k][i][j]->GetMaxCoord().x(), m_passiveMatrix[k][i][j]->GetMaxCoord().y(), m_passiveMatrix[k][i][j]->GetMaxCoord().z() );
                    // minCoord.Print();
                    // maxCoord.Print();
                    Local2Global( &minCoord );
                    Local2Global( &maxCoord );
                    // minCoord.Print();
                    // maxCoord.Print();
		    
                    stringstream ss;    
                    ss << setiosflags(ios::fixed) << setprecision(6);
                    ss <<  "RPP " << m_passiveMatrix[k][i][j]->GetBodyName() <<  "     " 
                                << minCoord.x() << " " << maxCoord.x() << " "
                                << minCoord.y() << " " << maxCoord.y() << " "
                                << minCoord.z() << " " << maxCoord.z() << endl;
                    // cout << "Build passive materials in ROOT and FLUKA " << ss.str() <<  endl;
                    m_bodyPrintOut  [ m_passiveMatrix[k][i][j]->GetMaterialName() ].push_back( ss.str() );
                    m_bodyName      [ m_passiveMatrix[k][i][j]->GetMaterialName() ].push_back( m_passiveMatrix[k][i][j]->GetBodyName() );

                    // regions
                    stringstream ssr;    ssr << setw(13) << setfill( ' ' ) << std::left << m_passiveMatrix[k][i][j]->GetRegionName()
                                            << "5 " << m_passiveMatrix[k][i][j]->GetBodyName() << " - " << 
                                            m_sensorMatrix[k][i][j]->GetBodyName() << endl;
                    // cout << "Build passive materials in ROOT and FLUKA " << ssr.str() <<  endl;
                    m_regionPrintOut[ m_passiveMatrix[k][i][j]->GetMaterialName() ].push_back( ssr.str() );
                    m_regionName    [ m_passiveMatrix[k][i][j]->GetMaterialName() ].push_back( m_passiveMatrix[k][i][j]->GetRegionName() );
                    if ( genfit::FieldManager::getInstance()->getFieldVal( TVector3( minCoord ) ).Mag() == 0 && genfit::FieldManager::getInstance()->getFieldVal( TVector3( maxCoord ) ).Mag() == 0 )
                        m_magneticRegion[ m_passiveMatrix[k][i][j]->GetRegionName() ] = 0;
                    else 
                        m_magneticRegion[ m_passiveMatrix[k][i][j]->GetRegionName() ] = 1;

                }

                // sensor_j++;
            }
            // sensor_i++;
        }
        // sensor_k++;
    }


//---------------------------------------------------------------------
//     Build sensor materials in ROOT and FLUKA
//---------------------------------------------------------------------
    if ( GlobalPar::GetPar()->Debug() > 0 ) cout << "Build sensor materials in ROOT and FLUKA" << endl;

    // for ( SensorMatrix::iterator itX = m_sensorMatrix.begin(); itX != m_sensorMatrix.end(); itX++ ) {
    //     for ( SensorPlane::iterator itY = (*itX).begin(); itY != (*itX).end(); itY++ ) {
    //         for ( SensorLine::iterator itZ = (*itY).begin(); itZ != (*itY).end(); itZ++ ) {

    for ( unsigned int k=0; k<m_nSensors.Z(); k++ ) {
        for ( unsigned int j=0; j<m_nSensors.Y(); j++ ) {
            for ( unsigned int i=0; i<m_nSensors.X(); i++ ) {    

                //ROOT addNode
                if ( GlobalPar::GetPar()->geoROOT() )   {
                    if ( !gGeoManager->GetVolume( m_sensorMatrix[k][i][j]->GetMaterialRegionName().c_str() ) )       cout << "ERROR >> FootBox::AddNodeToUniverse  -->  volume not defined: "<< m_sensorMatrix[k][i][j]->GetMaterialRegionName() << endl;

                    TVector3 globalCoord = m_sensorMatrix[k][i][j]->GetCenter();
                    Local2Global(&globalCoord);
                    m_universe->AddNode( gGeoManager->GetVolume( m_sensorMatrix[k][i][j]->GetMaterialRegionName().c_str() ), 
                                        m_sensorMatrix[k][i][j]->GetNodeID() , 
                                        new TGeoCombiTrans( globalCoord.x(), globalCoord.y(), globalCoord.z(), 
                                        new TGeoRotation("null,",0,0,0) ) );
                    if ( GlobalPar::GetPar()->Debug() > 0 ) cout <<"\t"<<m_sensorMatrix[k][i][j]->GetMaterialRegionName()<<"  ", globalCoord.Print();
                }

                    // boidies
                if ( GlobalPar::GetPar()->geoFLUKA() ) {
                    
                    TVector3 minCoord = TVector3( m_sensorMatrix[k][i][j]->GetMinCoord().x(), m_sensorMatrix[k][i][j]->GetMinCoord().y(), m_sensorMatrix[k][i][j]->GetMinCoord().z() );
                    TVector3 maxCoord = TVector3( m_sensorMatrix[k][i][j]->GetMaxCoord().x(), m_sensorMatrix[k][i][j]->GetMaxCoord().y(), m_sensorMatrix[k][i][j]->GetMaxCoord().z() );
                    Local2Global( &minCoord );
                    Local2Global( &maxCoord );

		    if ( k==0 && j==0 && i==0 ) m_xmin = minCoord.x();
		    else{
		      if ( m_xmin != minCoord.x()){
    			cout << "Error in VTX xmin coord " << m_xmin
    			     << "  " << minCoord.x() << endl;
		      }
		    }
							    
		    if ( k==0 && j==0 && i==0 ) m_ymin = minCoord.y();
		    else{
                if ( m_ymin != minCoord.y()){
                    cout << "Error in VTX ymin coord" << m_ymin
                         << "  " << minCoord.y() << endl;
                }
		    }

                    stringstream ss;
                    ss << setiosflags(ios::fixed) << setprecision(6);
                    ss <<  "RPP " << m_sensorMatrix[k][i][j]->GetBodyName() <<  "     " 
                                << minCoord.x() << " " << maxCoord.x() << " "
                                << minCoord.y() << " " << maxCoord.y() << " "
                                << minCoord.z() << " " << maxCoord.z() << endl;
                    
                    m_bodyPrintOut  [ m_sensorMatrix[k][i][j]->GetMaterialName() ].push_back( ss.str() );
                    // m_bodyName      [ m_sensorMatrix[k][i][j]->GetMaterialName() ].push_back( m_sensorMatrix[k][i][j]->GetBodyName() );

                    // regions
                    stringstream ssr;
                    ssr << setw(13) << setfill( ' ' ) << std::left << m_sensorMatrix[k][i][j]->GetRegionName()
                        << "5 " << m_sensorMatrix[k][i][j]->GetBodyName() << endl;
                        
                    m_regionPrintOut[ m_sensorMatrix[k][i][j]->GetMaterialName() ].push_back( ssr.str() );
                    m_regionName    [ m_sensorMatrix[k][i][j]->GetMaterialName() ].push_back( m_sensorMatrix[k][i][j]->GetRegionName() );
                    if ( genfit::FieldManager::getInstance()->getFieldVal( TVector3( minCoord ) ).Mag() == 0 && genfit::FieldManager::getInstance()->getFieldVal( TVector3( maxCoord ) ).Mag() == 0 )
                        m_magneticRegion[ m_sensorMatrix[k][i][j]->GetRegionName() ] = 0;
                    else 
                        m_magneticRegion[ m_sensorMatrix[k][i][j]->GetRegionName() ] = 1;
                }


            }
        }
    } 

}



//_____________________________________________________________________________
TVector3 TAVTparGeo::GetPixelPos_sensorFrame( int layer, int col, int row )  { return m_sensorMatrix[layer][0][0]->GetPosition_local( col, row );  }



//_____________________________________________________________________________
TVector3 TAVTparGeo::GetPixelPos_detectorFrame( int layer, int col, int row )  { return m_sensorMatrix[layer][0][0]->GetPosition( col, row );  }



//_____________________________________________________________________________
TVector3 TAVTparGeo::GetPixelPos_footFrame( int layer, int col, int row )  {
    TVector3 pos = GetPixelPos_detectorFrame( layer, col, row );
    Local2Global(&pos);
    return pos;
}



//_____________________________________________________________________________
float TAVTparGeo::GetColumnCenter_sensorFrame( int col )  { return GetPixelPos_sensorFrame( 0, col, 0 ).x(); }
float TAVTparGeo::GetColumnCenter_detectorFrame( int layer, int col )  { return GetPixelPos_detectorFrame( layer, col, 0 ).x(); }
float TAVTparGeo::GetColumnCenter_footFrame( int layer, int col ) { return GetPixelPos_footFrame( layer, col, 0 ).x(); }
//_____________________________________________________________________________
float TAVTparGeo::GetRowCenter_sensorFrame( int row )     { return GetPixelPos_sensorFrame( 0, 0, row ).y();   }
float TAVTparGeo::GetRowCenter_detectorFrame( int layer, int row )     { return GetPixelPos_detectorFrame( layer, 0, row ).y();   }
float TAVTparGeo::GetRowCenter_footFrame( int layer, int row )    { return GetPixelPos_footFrame( layer, 0, row ).y();     }


//_____________________________________________________________________________
Float_t TAVTparGeo::GetPositionU(Int_t column) const {      // GetColumnCenter_sensorFrame
   return ((2*column - m_pitchX + 1 ) * m_pitchX)/2 ;
 }

//_____________________________________________________________________________
Float_t TAVTparGeo::GetPositionV(Int_t line) const{         //TAVTparGeo::GetRowCenter_sensorFrame(
   return -((2*line - m_pitchY + 1 ) * m_pitchY)/2 ;   
}






//_____________________________________________________________________________
void TAVTparGeo::Detector2Sensor_frame( int sensorID, TVector3* coord ) {
    m_sensorMatrix[sensorID][0][0]->Global2Local( coord );
}

//_____________________________________________________________________________
void TAVTparGeo::Sensor2Detector_frame( int sensorID, TVector3* coord ) {
    m_sensorMatrix[sensorID][0][0]->Local2Global( coord );
}

//_____________________________________________________________________________
void TAVTparGeo::Global2Local( TVector3* glob ) {
    glob->Transform( GetRotationToLocal() );
    *glob = *glob - m_center;
}

//_____________________________________________________________________________
void TAVTparGeo::Global2Local_TranslationOnly( TVector3* glob ) {
    *glob = *glob - m_center;
}

//_____________________________________________________________________________
void TAVTparGeo::Global2Local_RotationOnly( TVector3* glob ) {
    glob->Transform( GetRotationToLocal() );
}

//_____________________________________________________________________________
void TAVTparGeo::Local2Global( TVector3* loc ) {
    loc->Transform( GetRotationToGlobal() );
    *loc = *loc + m_center;
}

//_____________________________________________________________________________
void TAVTparGeo::Local2Global_TranslationOnly( TVector3* loc ) {
    *loc = *loc + m_center;
}

//_____________________________________________________________________________
void TAVTparGeo::Local2Global_RotationOnly( TVector3* loc ) {
    loc->Transform( GetRotationToGlobal() );
}





//_____________________________________________________________________________
TGeoVolume* TAVTparGeo::GetVolume() {

    if ( !GlobalPar::GetPar()->geoROOT() ) 
        cout << "ERROR << TAVTparGeo::GetVolume()  -->  Calling this function without enabling the correct parameter in the param file.\n", exit(0);

    return m_universe;
}





//_____________________________________________________________________________
string TAVTparGeo::PrintBodies() {

    if ( !GlobalPar::GetPar()->geoFLUKA() ) 
        cout << "ERROR << TAVTparGeo::PrintBodies()  -->  Calling this function without enabling the corrct parameter in the param file.\n", exit(0);

    stringstream outstr;
    outstr << "* ***Vertex" << endl;

    // loop in order of the material alfabeth
    for ( map<string, vector<string> >::iterator itMat = m_bodyPrintOut.begin(); itMat != m_bodyPrintOut.end(); itMat++ ) {
        // loop over all body of the same material
        for ( vector<string>::iterator itBody = (*itMat).second.begin(); itBody != (*itMat).second.end(); itBody++ ) {
            outstr << (*itBody);
            if (m_debug > 3)    cout << (*itBody);
        }        
    }
    return outstr.str();
}





//_____________________________________________________________________________
string TAVTparGeo::PrintRegions() {

    if ( !GlobalPar::GetPar()->geoFLUKA() ) 
        cout << "ERROR << TAVTparGeo::PrintRegions()  -->  Calling this function without enabling the corrct parameter in the param file.\n", exit(0);

    stringstream outstr;
    outstr << "* ***Vertex" << endl;

  // loop in order of the material alfabeth
    for ( map<string, vector<string> >::iterator itMat = m_regionPrintOut.begin(); itMat != m_regionPrintOut.end(); itMat++ ) {
        // loop over all body of the same material
        for ( vector<string>::iterator itRegion = (*itMat).second.begin(); itRegion != (*itMat).second.end(); itRegion++ ) {
            outstr << (*itRegion);
            if (m_debug > 3)    cout << (*itRegion);
        }        
    }
    return outstr.str();
}




string TAVTparGeo::PrintSubtractBodiesFromAir() {

    if ( !GlobalPar::GetPar()->geoFLUKA() ) 
        cout << "ERROR << TAVTparGeo::PrintSubtractMaterialFromAir()  -->  Calling this function without enabling the correct parameter in the param file.\n", exit(0);


    stringstream outstr;
    // loop in order of the material alfabeth
    for ( map<string, vector<string> >::iterator itMat = m_bodyName.begin(); itMat != m_bodyName.end(); itMat++ ) {
        // loop over all region of the same material
        for ( vector<string>::iterator itRegion = (*itMat).second.begin(); itRegion != (*itMat).second.end(); itRegion++ ) {
            outstr << " -" << (*itRegion);
        }        
    }
    return outstr.str();

}





//_____________________________________________________________________________
string TAVTparGeo::PrintAssignMaterial() {

    if ( !GlobalPar::GetPar()->geoFLUKA() ) 
        cout << "ERROR << TAVTparGeo::PrintAssignMaterial()  -->  Calling this function without enabling the correct parameter in the param file.\n", exit(0);


    // loop in order of the material alfabeth
    stringstream outstr; 
    for ( map<string, vector<string> >::iterator itMat = m_regionName.begin(); itMat != m_regionName.end(); itMat++ ) {

        // check dimension greater than 0
        if ( (*itMat).second.size() == 0 ) {
            cout << "ERROR << TAVTparGeo::PrintAssignMaterial  ::  "<<endl, exit(0);
        }

        // take the first region
        string firstReg = (*itMat).second.at(0);
        // take the last region
        string lastReg = "";
        if ( (*itMat).second.size() != 1 ) 
            lastReg = (*itMat).second.at( (*itMat).second.size()-1 );

        // build output string 
        outstr  << setw(10) << setfill( ' ' ) << std::left << "ASSIGNMA" 
                << setw(10) << setfill( ' ' ) << std::right << (*itMat).first 
                << setw(10) << setfill( ' ' ) << std::right << firstReg 
                << setw(10) << setfill( ' ' ) << std::right << lastReg;
                       
        
        // multiple region condition 
        if ( (*itMat).second.size() != 1 ) {
            outstr << setw(10) << setfill( ' ' ) << std::right  << 1 ;
        }
        else {
            outstr << setw(10) << setfill( ' ' ) << std::right  << " ";
        }


        // region in the magnetic filed condition
        bool isMag = true;
        for (int i=0; i<(*itMat).second.size(); i++) {
            if ( m_magneticRegion[ (*itMat).second.at(i) ] == 0 ) {
                isMag = false;
                break;
            }
        }
        if ( isMag )
            outstr << setw(10) << setfill( ' ' ) << std::right  << 1 ;
        else 
            outstr << setw(10) << setfill( ' ' ) << std::right  << " " ;
        
        outstr << endl;

        // DEBUG
        if (m_debug > 0)    cout << outstr.str();

    }

    return outstr.str();

}



//_____________________________________________________________________________
string TAVTparGeo::PrintParameters() {
  
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);

  string precision = "D+00";

  outstr << "c     VERTEX PARAMETERS " << endl;
  outstr << endl;    
  
  map<string, int> intp;
  intp["xpixVTX"] = m_nPixel_X;
  intp["ypixVTX"] = m_nPixel_Y;
  intp["nlayVTX"] = m_nSensors.Z();
  for (auto i : intp){
    outstr << "      integer " << i.first << endl;
    outstr << "      parameter (" << i.first << " = " << i.second << ")" << endl;
    // outstr << endl;    
  }
  
  map<string, double> doublep;
  doublep["dxVTX"] = VTX_DX;
  doublep["dyVTX"] = VTX_DY;
  doublep["xminVTX"] = m_xmin;
  doublep["yminVTX"] = m_ymin;
  for (auto i : doublep){
    outstr << "      double precision " << i.first << endl;
    outstr << "      parameter (" << i.first << " = " << i.second << precision << ")" << endl;
    // outstr << endl;    
  }
  outstr << endl;    
  
  return outstr.str();

}






// **************** VERTEXING      *******************************************************


// //_____________________________________________________________________________
// TGeoVolume* TAVTparGeo::BuildVertex(const char* basemoduleName, const char *vertexName)
// {
//    TGeoVolume* vertex = 0x0;

//    for(Int_t iSensor = 0; iSensor < GetSensorsN(); iSensor++) {
//    TGeoHMatrix* hm = GetTransfo(iSensor);
//    vertex = TAVTparGeo::AddVertexModule(hm, basemoduleName, vertexName);
//    }

//    return vertex;
// }

// //_____________________________________________________________________________
// TGeoVolume* TAVTparGeo::AddVertexModule(TGeoHMatrix* hm, const char* basemoduleName, const char *vertexName)
// {
//    if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
//    new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
//    }

//    TGeoVolume* vertex = gGeoManager->FindVolumeFast(vertexName);
//    if ( vertex == 0x0 ) {
//    Int_t nSensors = GetSensorsN();

//    Float_t posZ1 = (*GetPosition(0))(2)*0.9;
//    Float_t posZ2 = (*GetPosition(nSensors-1))(2)*1.1;

//    TGeoMedium   *med;
//    TGeoMaterial *mat;
//    if ( (mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("Vacuum")) == 0x0 )
//       mat = new TGeoMaterial("Vacuum",0,0,0);
//    if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("Vacuum")) == 0x0 )
//       med = new TGeoMedium("Vacuum",1,mat);
//    vertex = gGeoManager->MakeBox(vertexName, med, fHeight/2., fHeight/2., (posZ2-posZ1)/2.); // volume corresponding to vertex
//    }

//    // create module
//    TGeoMaterial* matMod;
//    TGeoMedium*   medMod;

//    if ( (matMod = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("Si")) == 0x0 )
//    matMod = new TGeoMaterial("Si", 28.09, 14, 2.3);
//    if ( (medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("Si")) == 0x0 )
//    medMod = new TGeoMedium("Si",2,matMod);

//    TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fWidth/2, fHeight/2, fThick/2.);

//    Int_t nbModule = 0;

//    TGeoVolume *vertexMod = new TGeoVolume(Form("%s_Vertex",basemoduleName),box, medMod);
//    vertexMod->SetLineColor(kAzure-5);
//    vertexMod->SetTransparency( TAGgeoTrafo::GetDefaultTransp());

//    TObjArray* list = vertex->GetNodes();
//    if (list) {
//    for (Int_t i = 0; i < list->GetEntries(); ++i) {
//       TGeoVolume* vol = (TGeoVolume*)list->At(i);
//       if (vol) {
//          TString name(vol->GetName());
//          if ( name.Contains(Form("%s_Vertex",basemoduleName)) )
//             nbModule++;
//       }
//    }
//    }

//    vertex->AddNode(vertexMod, nbModule, new TGeoHMatrix(*hm));
//    return vertex;
// }


// //_____________________________________________________________________________
// TEveGeoShapeExtract* TAVTparGeo::BuildExtractVertex(const char* basemoduleName, const char *vertexName)
// {
//    Int_t nSensors = GetSensorsN();
//    Float_t posZ1 = (*GetPosition(0))(2)*0.9;
//    Float_t posZ2 = (*GetPosition(nSensors-1))(2)*1.1;
//    TGeoBBox *box = new TGeoBBox(vertexName, 11000,11000,(posZ2-posZ1)/2.);

//    TEveGeoShapeExtract* vertexExtract = new TEveGeoShapeExtract(vertexName);
//    vertexExtract->SetShape(box);
//    Float_t color[] = {0, 0, 0, 0};
//    vertexExtract->SetRGBA(color);

//    for(Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
//    TGeoHMatrix* hm = GetTransfo(iSensor);
//    TEveGeoShapeExtract* vertexMod = AddExtractVertexModule(hm, basemoduleName, vertexName);
//    vertexExtract->AddElement(vertexMod);
//    }

//    return vertexExtract;
// }

// //_____________________________________________________________________________
// TEveGeoShapeExtract* TAVTparGeo::AddExtractVertexModule(TGeoHMatrix* hm, const char* basemoduleName, const char *vertexName)
// {
//    // create module
//    static Int_t nbModule = 0;

//    TGeoBBox *box = new TGeoBBox(Form("%s_%d",basemoduleName, nbModule), fWidth/2, fHeight/2, fThick/2.);

//    TEveTrans eveTrans;
//    eveTrans.SetFrom(*hm);
//    TColor* color = gROOT->GetColor(kAzure-5);
//    Float_t rgba[4];
//    color->GetRGB(rgba[0], rgba[1], rgba[2]);
//    rgba[3] = TAGgeoTrafo::GetDefaultTransp()/100.;

//    TEveGeoShapeExtract* vertexModExtract = new TEveGeoShapeExtract(Form("%s_%d",basemoduleName, nbModule++));
//    vertexModExtract->SetShape(box);
//    vertexModExtract->SetTrans(eveTrans.Array());
//    vertexModExtract->SetRGBA(rgba);

//    return vertexModExtract;
// }

// //_____________________________________________________________________________
// TEveGeoShapeExtract* TAVTparGeo::AddExtractTarget(const Float_t dx, const Float_t dy, const Float_t dz, const char *targetName)
// {
//    TGeoBBox* box = new TGeoBBox(targetName, dx, dy, dz);

//    TColor* color = gROOT->GetColor(19);
//    Float_t rgba[4];
//    color->GetRGB(rgba[0], rgba[1], rgba[2]);
//    rgba[3] = TAGgeoTrafo::GetDefaultTransp()/100.;

//    TEveGeoShapeExtract* target = new TEveGeoShapeExtract(targetName);
//    target->SetShape(box);
//    target->SetRGBA(rgba);

//    return target;

// }

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TAVTparGeo::Clear(Option_t*)
{
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAVTparGeo::ToStream(ostream& os, Option_t*) const
{
//  os << "TAVTparGeo " << GetName() << endl;
//  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
//     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}
