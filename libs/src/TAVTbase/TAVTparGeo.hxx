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
   static const TString fgkDefParaName; ///< default parameter name

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
   // Fluka material assignment
   virtual string PrintAssignMaterial(TAGmaterials *Material);
   // Fluka air substraction
   virtual string PrintSubtractBodiesFromAir();
   
protected:
   vector<string> fvEpiBody;   //! Fluka epitaxial body
   vector<string> fvModBody;   //! Fluka module body
   vector<string> fvPixBody;   //! Fluka pixel body
   vector<string> fvEpiRegion; //! Fluka epitaxial region
   vector<string> fvModRegion; //! Fluka module region
   vector<string> fvPixRegion; //! Fluka pixel region

public:
   //! Get base name
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   //! Get default parameter name
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

   ClassDef(TAVTparGeo,1)
};

#endif
