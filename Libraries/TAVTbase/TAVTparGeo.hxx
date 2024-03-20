#ifndef _TAVTparGeo_HXX
#define _TAVTparGeo_HXX
/*!
  \file TAVTparGeo.hxx
  \brief   Declaration of TAVTparGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include <sstream>

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAVTbaseParGeo.hxx"
#include "TAGmaterials.hxx"

//##############################################################################

class TAVTparGeo : public TAVTbaseParGeo {
      

protected:
   static const TString fgkBaseName;    ///< Vtx base name
   static const Int_t   fgkDefSensorsN;   ///< default number of sensors

public:
   TAVTparGeo();
    virtual ~TAVTparGeo();
  
   // Define materials
   virtual void  DefineMaterial();

   // Add CMOS module geometry to vertex
   TGeoVolume* AddModule(const char* basemoduleName = "Module", const char *name = "Vertex");
      
   // Add CMOS module geometry to vertex
   TGeoVolume* BuildBoard(const char* basemoduleName = "M28Board", const char *name = "M28Epi");
   
   // Build Vertex
   TGeoVolume* BuildVertex( const char *name = GetBaseName(), const char* basemoduleName = "M28", Bool_t board = false);
   
   //crossing regions (n = layer number; 0<=n<=3)
   // Get expitaxial region
   Int_t          GetRegEpitaxial(Int_t n);
   // Get module region
   Int_t          GetRegModule(Int_t n);
   // Get pixel region
   Int_t          GetRegPixel(Int_t n);

   // to print fluka files
   // Fluka parameters
   virtual string PrintParameters();
   // Fluka rorations
   virtual string PrintRotations();
   // Fluka bodies
   virtual string PrintBodies();
   // Fluka regions
   virtual string PrintRegions();
   // Fluka passive regions
   virtual string PrintPassiveRegions();
   // Fluka material assignment
   virtual string PrintAssignMaterial(TAGmaterials *Material);
   // Fluka Transport settings
   virtual string PrintVTPhysics();
   // Fluka air substraction
   virtual string PrintSubtractBodiesFromAir();
   
protected:
   //! Fluka epitaxial body
   vector<string> fvEpiBody;   //! Fluka epitaxial body
   //! Fluka module body
   vector<string> fvModBody;   //! Fluka module body
   //! Fluka pixel body
   vector<string> fvPixBody;   //! Fluka pixel body
   //! Fluka epitaxial region
   vector<string> fvEpiRegion; //! Fluka epitaxial region
    //! Fluka module region
   vector<string> fvModRegion; //! Fluka module region
   //! Fluka pixel region
   vector<string> fvPixRegion; //! Fluka pixel region
   //! Fluka passive body
   vector<string> fvPassBody;   //! Fluka passive body
   //! Fluka passive region
   vector<string> fvPassRegion;   //! Fluka passive region

public:
   //! Get base name
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   //! Get default number of sensors
   static Int_t         GetDefSensorsN() { return fgkDefSensorsN;        }

   ClassDef(TAVTparGeo,1)
};

#endif
