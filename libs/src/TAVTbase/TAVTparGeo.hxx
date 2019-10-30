#ifndef _TAVTparGeo_HXX
#define _TAVTparGeo_HXX
/*!
  \version $Id: TAVTparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  
    Fully revised in 2017 by Matteo Franchini franchinim@bo.infn.it

    Three reference frames are possible and all the transformation from one to another 
    are defined in this class:
        - sensor frame
        - detector frame
        - FOOT frame

    All the coordinates are in cm and in the detector reference frame, i.e. the center
    is the center of the detector.

*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TEveGeoShapeExtract.h"

#include "TObject.h"

#include "TAVTparTools.hxx"

#include "IronPlate.hxx"
#include "FootBox.hxx"
#include "GlobalPar.hxx"
#include "FootField.hxx"

#include <FieldManager.h>


class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAVTparGeo : public TAVTparTools {


typedef vector< vector< vector< IronPlate* > > > SensorMatrix;
typedef vector< vector< IronPlate* > > SensorPlane;
typedef vector< IronPlate* > SensorLine;

typedef vector< vector< vector< FootBox* > > > PassiveMatrix;
typedef vector< vector< FootBox* > > PassivePlane;
typedef vector< FootBox* > PassiveLine;

public:

    TAVTparGeo();
    TAVTparGeo( TAVTparGeo* original );
    virtual ~TAVTparGeo() {
      // sensor matrix cleaning
      for ( SensorMatrix::iterator itX = m_sensorMatrix.begin(); itX != m_sensorMatrix.end(); itX++ ) {
        for ( SensorPlane::iterator itY = (*itX).begin(); itY != (*itX).end(); itY++ ) {
            for ( SensorLine::iterator itZ = (*itY).begin(); itZ != (*itY).end(); itZ++ ) {
                delete (*itZ);
            }
            (*itY).clear();
        }
        (*itX).clear();
      }
      m_sensorMatrix.clear();
    };

    void InitGeo();
    void InitMaterial();

    // to be removed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Float_t GetPositionU(Int_t column)         const;
    Float_t GetPositionV(Int_t line)           const;


    TVector3 GetSensorPosition( int sensorID )   { return m_sensorMatrix[sensorID][0][0]->GetCenter(); };

    //    it should be changed accordingly with the simulation choice when more than one sensors will be used
    // TVector3 GetPixelPos_Global( int layer, int col, int row );
    // TVector3 GetPixelPos_Local( int layer, int col, int row );

    // new
    TVector3 GetPixelPos_sensorFrame( int layer, int col, int row );
    TVector3 GetPixelPos_detectorFrame( int layer, int col, int row );
    TVector3 GetPixelPos_footFrame( int layer, int col, int row );

    // float GetColumnCenter_Local ( int layer, int col);
    // float GetRowCenter_Local ( int layer, int row);
    // float GetColumnCenter_Global (int layer, int col);
    // float GetRowCenter_Global ( int layer, int row);

// new
    float GetColumnCenter_sensorFrame ( int col);
    float GetColumnCenter_detectorFrame ( int layer, int col);
    float GetColumnCenter_footFrame ( int layer, int col);
    // new
    float GetRowCenter_sensorFrame ( int row);
    float GetRowCenter_detectorFrame ( int layer, int row);
    float GetRowCenter_footFrame ( int layer, int row);


    //! Transform point from the foot global reference frame
    //! to the detector local reference frame.  Transformation between detector and sensor are managed by IronPlate
    void Detector2Sensor_frame( int sensorID, TVector3* coord );
    void Sensor2Detector_frame( int sensorID, TVector3* coord );

    // foot to detector
    void Global2Local( TVector3* glob );
    void Global2Local_TranslationOnly( TVector3* glob );
    void Global2Local_RotationOnly( TVector3* glob );

    // detector to foot
    void Local2Global( TVector3* loc );
    void Local2Global_TranslationOnly( TVector3* loc );
    void Local2Global_RotationOnly( TVector3* loc );

    TRotation GetRotationToGlobal() { return *m_rotation; };
    TRotation GetRotationToLocal() { return m_rotation->Inverse(); };


    //  Return Vertex center coord. in the foot global frame
    TVector3 GetCenter() { return m_center; };

    // Return Vertex full dimension.
    TVector3 GetDimension() { return m_dimension; };

    double GetSingleSensorThickness() { return m_siliconSensorThick_Lz; };

    int GetNPixelX() { return m_nPixel_X; };
    int GetNPixelY() { return m_nPixel_Y; };
    int GetNLayers() { return m_nSensors.Z(); };
    
    int GetNSensors()    { return m_nSensors.X()*m_nSensors.Y()*m_nSensors.Z(); };  // return tot number of sensors
    
    // Return a vector with the number of sensors along the cartesian directions 
    TVector3        GetNumberOfSensorAlongDirections() { return m_nSensors; }; 
    
    // define the agloritm to map the sensor with a single variable. For VTX is too easy :).
    int GetSensorID( int layer, int col, int row )    { return layer; };
    int GetLayerFromSensorID( int sensID )            { return sensID; };

    // pixel dimension
    double GetPitchX()  { return m_pitchX; };
    double GetPitchY()  { return m_pitchY; };
    
    // function for the FRUKA geometry creation
    string PrintBodies();
    string PrintRegions();
    string PrintAssignMaterial();
    string PrintSubtractBodiesFromAir();
    string PrintParameters();

    // Return the ROOT volume of the entire detector
    TGeoVolume*     GetVolume();

    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;


private:

    SensorMatrix m_sensorMatrix;
    PassiveMatrix m_passiveMatrix;
    TRotation* m_rotation;

    TGeoVolume* m_universe;

    TVector3  m_origin;  // current position in local coord.
    TVector3  m_center;  // current position in global coord.
    TVector3  m_dimension;

    int m_volumeCount;
    int m_passiveCount;

    TVector3 m_nSensors;

    vector<string> m_materialOrder;
    vector<string> m_passiveMaterial;

    map<string, double> m_materialThick;
    map<string, string> m_materialType;

    map<string, vector<string> > m_regionPrintOut;
    map<string, vector<string> > m_bodyPrintOut;
    map<string, vector<string> > m_regionName;
    map<string, vector<string> > m_bodyName;
    map<string, int > m_magneticRegion;

    int m_nPassiveLayersPerBoard_z;
    double m_passiveMaterialThick;
    double m_siliconSensorThick_Lz;
    double m_layerDistance_samePair;
    double m_layerDistance_interPair;

    int m_debug;
    int m_setW_0number;

    int m_nPixel_X;
    int m_nPixel_Y;
    double m_pitchX;
    double m_pitchY;

    double m_xmin, m_ymin;



////////////////////////////////////////////////////////////////////////////////



   // //! Add CMOS module geometry to world
   // TGeoVolume* AddVertexModule(TGeoHMatrix* hm, const char* basemoduleName = "Module", const char *name = "Vertex");

   // //! Add CMOS module geometry to world
   // TEveGeoShapeExtract* AddExtractVertexModule(TGeoHMatrix* hm, const char* basemoduleName = "Module", const char *name = "Vertex");

   // //! Add Target
   // TGeoVolume* AddTarget(const Float_t dx = fgTargetSize/2., const Float_t dy = fgTargetSize/2.,
			// 			 const Float_t dz = fgTargetWidth/2., const char *targetName = "Target");

   // //! Add Extract Target
   // TEveGeoShapeExtract* AddExtractTarget(const Float_t dx = fgTargetSize/2., const Float_t dy = fgTargetSize/2.,
			// 							 const Float_t dz = fgTargetWidth/2., const char *targetName = "Target");

   // //! Build Vertex
   // TGeoVolume* BuildVertex(const char* basemoduleName = "Module", const char *name = "Vertex");

   // //! Build Extract Vertex
   // TEveGeoShapeExtract* BuildExtractVertex(const char* basemoduleName = "Module", const char *name = "Vertex");

   // //! Get Sensor parameter
   // SensorParameter_t& GetSensorPar(Int_t idx){return fSensorParameter[idx];}


   ClassDef(TAVTparGeo,1)
};

#endif
