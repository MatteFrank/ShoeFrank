/*!
 \file TAGFgeometryConstructor.hxx
 \brief  Header for GenFit geometry constructor class
 \author R. Zarrella
*/

#ifndef TAGFGEOMETRYCONSTRUCTOR_H
#define TAGFGEOMETRYCONSTRUCTOR_H

#include "TObject.h"

#include "TAGroot.hxx"
#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoMaterial.h>

#include <Track.h>
#include <Exception.h>
#include <FieldManager.h>
#include <StateOnPlane.h>
#include <TrackPoint.h>
#include <SpacepointMeasurement.h>
#include <MaterialEffects.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>
#include "DetPlane.h"
#include "AbsFinitePlane.h"
#include "RectangularFinitePlane.h"

#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"
#include "TAGFdetectorMap.hxx"

#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TADIparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TAGgeoTrafo.hxx"

using namespace std;
using namespace genfit;

class TAGFgeometryConstructor : public TObject
{
	TASTparGeo* m_ST_geo;								///< Pointer to ST par Geo
	TABMparGeo* m_BM_geo;								///< Pointer to BM par Geo
	TAGparGeo* m_TG_geo;								///< Pointer to TG par Geo
	TADIparGeo* m_DI_geo;								///< Pointer to DI par Geo
	TAVTparGeo* m_VT_geo;								///< Pointer to VT par Geo
	TAITparGeo* m_IT_geo;								///< Pointer to IT par Geo
	TAMSDparGeo* m_MSD_geo;								///< Pointer to MSD par Geo
	TATWparGeo* m_TW_geo;								///< Pointer to TW par Geo
	TACAparGeo* m_CA_geo;								///< Pointer to CA par Geo
	TGeoVolume* m_TopVolume;							///< Top volume of geometry
	TAGgeoTrafo* m_GeoTrafo;							///< GeoTrafo object

	TAGFdetectorMap* m_SensorIDMap;						///< Pointer to TAGFdetectorMap object
	TString *m_systemsON;								///< Pointer to string containing the detectors included in the kalman tracking

public:
	TAGFgeometryConstructor(TAGFdetectorMap* sensorIdMap, TString* systemsOn);
	virtual ~TAGFgeometryConstructor();

	void IncludeDetectors();
	void CreateGeometry();

	//! \brief Get the pointer to the ST par Geo object
	TASTparGeo*		GetSTparGeo()	{return m_ST_geo;}
	//! \brief Get the pointer to the BM par Geo object
	TABMparGeo*		GetBMparGeo()	{return m_BM_geo;}
	//! \brief Get the pointer to the target par Geo object
	TAGparGeo*		GetGparGeo()	{return m_TG_geo;}
	//! \brief Get the pointer to the magnet par Geo object
	TADIparGeo*		GetDIparGeo()	{return m_DI_geo;}
	//! \brief Get the pointer to the VT par Geo object
	TAVTparGeo*		GetVTparGeo()	{return m_VT_geo;}
	//! \brief Get the pointer to the IT par Geo object
	TAITparGeo*		GetITparGeo()	{return m_IT_geo;}
	//! \brief Get the pointer to the MSD par Geo object
	TAMSDparGeo*	GetMSDparGeo()	{return m_MSD_geo;}
	//! \brief Get the pointer to the TW par Geo object
	TATWparGeo*		GetTWparGeo()	{return m_TW_geo;}
	//! \brief Get the pointer to the CA par Geo object
	TACAparGeo*		GetCAparGeo()	{return m_CA_geo;}
	//! \brief Get the pointer to top Volume of the GenFit geometry
	TGeoVolume*		GeteoVolume()	{return m_TopVolume;}
	//! \brief Get the pointer to the object handling reference frame transformations btw detectors
	TAGgeoTrafo*	GetGgeoTrafo()	{return m_GeoTrafo;}

	ClassDef(TAGFgeometryConstructor,0);
};

#endif