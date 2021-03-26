#ifndef _TAVTparGeo_HXX
#define _TAVTparGeo_HXX
/*!
  \file
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
   static const TString fgkBaseName;   // Vtx base name
   static const TString fgkDefParaName;

public:
   TAVTparGeo();
    virtual ~TAVTparGeo();
  
   // Define materials
   virtual void  DefineMaterial();

   //! Add CMOS module geometry to vertex
   TGeoVolume* AddModule(const char* basemoduleName = "Module", const char *name = "Vertex");
      
   //! Add CMOS module geometry to vertex
   TGeoVolume* BuildBoard(const char* basemoduleName = "M28Board", const char *name = "M28Epi");
   
   //! Build Vertex
   TGeoVolume* BuildVertex( const char *name = "Vertex", const char* basemoduleName = "M28", Bool_t board = false);
   
   //crossing regions (n = layer number; 0<=n<=3)
   Int_t          GetRegEpitaxial(Int_t n);
   Int_t          GetRegModule(Int_t n);
   Int_t          GetRegPixel(Int_t n);

   // to print fluka files
   virtual string PrintParameters();
   virtual string PrintRotations();
   virtual string PrintBodies();
   virtual string PrintRegions();
   virtual string PrintAssignMaterial(TAGmaterials *Material);
   virtual string PrintSubtractBodiesFromAir();
   
protected:
   vector<string> fvEpiBody;   //!
   vector<string> fvModBody;   //!
   vector<string> fvPixBody;   //!
   vector<string> fvEpiRegion; //!
   vector<string> fvModRegion; //!
   vector<string> fvPixRegion; //!

public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

   ClassDef(TAVTparGeo,1)
};

#endif
