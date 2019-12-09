#ifndef _TAMSDparGeo_HXX
#define _TAMSDparGeo_HXX
/*!
  \file
  \version $Id: TAMSDparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TAMSDparGeo.
*/
/*------------------------------------------+---------------------------------*/


#include <map>
#include <sstream>

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGmaterials.hxx"
#include "TAVTbaseParGeo.hxx"

class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAMSDparGeo : public TAVTbaseParGeo {


public:
  TAMSDparGeo();
  virtual ~TAMSDparGeo() {};
  
  // Define materials
   void DefineMaterial();
  
  //! Add SSSD module geometry to strip detector
  TGeoVolume* AddModule(const char* basemoduleName = "SSSD", const char *name = "MSD");
   
  //! Build MSD
  TGeoVolume* BuildMultiStripDetector(const char* basemoduleName = "Module", const char *name = "MSD");
   
  //! Reading from file
  Bool_t      FromFile(const TString& name = "");
  
  // to keep interace for compilation
  string      PrintParameters();
  string      PrintRotations();
  string      PrintBodies();
  string      PrintRegions();
  string      PrintAssignMaterial(TAGmaterials *Material);
  string      PrintSubtractBodiesFromAir();

  int StripN;
  float EpiSizeX;
  float EpiSizeY;
  float EpiSizeZ;
  float stripSizeX;
  float stripSizeY;

  map<int, float> x_strip;
  map<int, float> y_strip;

  int GetStrip( int view, float k ) { 
  	if (view == 0 ) {
	  	for ( map< int, float >::iterator stripx = x_strip.begin(); stripx != x_strip.end(); stripx++ ) 
	  		if ( k < (*stripx).second + stripSizeX*0.5 )	return (*stripx).first;
  	}
  	else {
  		for ( map< int, float >::iterator stripx = y_strip.begin(); stripx != y_strip.end(); stripx++ ) 
	  		if ( k < (*stripx).second + stripSizeY*0.5 )	return (*stripx).first;
  	}
  	return -1;
  };
  float GetPosition (int view,  int s ) { 
  	if (view == 0 )		return x_strip[s];
  	else 		 		return y_strip[s]; 
  };

private:
  map<string, vector<string> > m_regionName;
  map<string, vector<string> > m_bodyName;
  // map<string, vector<string> > m_regionPrintOut;
  map<string, vector<string> > m_bodyPrintOut;
  vector<string> vMetalBody, vModBody, vStripBody;
  vector<string> vMetalRegion, vModRegion, vStripRegion;
  
  Int_t      fStripN;          // Number of strips
  int 		 m_nLayer;
  Float_t    fPitch;           // Pitch value
  
  TVector3   fMetalSize;        // Sensitive size of metallization
  Float_t    fMetalThickness;   // Thickness of metallization
  TString    fMetalMat;         // Material of metallization
  Float_t    fMetalDensity;     // density of metallization
  
private:
  static const TString fgkBaseName;   // MSD base name
  static const TString fgkDefParaName;
  
public:
  static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
  static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

  Float_t GetPosition(Int_t strip) const;

  //! Get number of strip
  int GetNLayers()  				{ return m_nLayer; };
  Int_t GetNStrip()                  const { return fStripN;       }
  Int_t GetPitch()                   const { return fPitch;        }

  
  ClassDef(TAMSDparGeo,1)
};

#endif
