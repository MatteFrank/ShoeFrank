#ifndef _TAGactNtuGlbTrack_HXX
#define _TAGactNtuGlbTrack_HXX
/*!
 \file
 \version $Id: TAGactNtuGlbTrack.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
 \brief   Declaration of TAGactNtuGlbTrack.
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"

#include "TATOEact.hxx"

class TH1F;
class TTree;
class TAGactTreeReader;

class TAGntuPoint;
class TAGgeoTrafo;

class TAGactNtuGlbTrack : public TAGaction {
public:
   
   explicit  TAGactNtuGlbTrack(const char* name       = 0,
                               TAGdataDsc* p_vtxclus  = 0,
                               TAGdataDsc* p_vtxtrack = 0,
                               TAGdataDsc* p_itrclus  = 0,
                               TAGdataDsc* p_msdclus  = 0,
                               TAGdataDsc* p_twpoint  = 0,
                               TAGdataDsc* p_glbtrack = 0,
                               TAGparaDsc* p_geoG     = 0,
                               TAGparaDsc* p_geodi    = 0,
                               TAGparaDsc* p_geoVtx   = 0,
                               TAGparaDsc* p_geoItr   = 0,
                               TAGparaDsc* p_geoMsd   = 0,
                               TAGparaDsc* p_geoTof   = 0,
                               TADIgeoField* field = nullptr);
   
   virtual  ~TAGactNtuGlbTrack();
   
   //! Action
   Bool_t    Action();
   
   //! Base creation of histogram
   void      CreateHistogram();
   
   //! Set up branches
   void      SetupBranches();
   
   //! Set up branches
   void      SetupBranches(TTree* tree);

   //! Open File
   void      Open(TString name);
   
   //! Close File
   void      Close();
   
   //! Get tree in standalone mode
   TTree*    GetTree();
    
private:
    TATOEbaseAct* SetupAction() const;
    TADIgeoField*        GetFootField()      const { return fField;  }
    
private:
   TAGgeoTrafo*      fpFootGeo;        // geo trafo
   TAGdataDsc*       fpVtxVertex;		// Vertex
    TAGdataDsc*      fpVtxClus;
   TAGdataDsc*       fpItrClus;		   // ITR clusters
   TAGdataDsc*       fpMsdClus;		   // MSD clusters
   TAGdataDsc*       fpTwPoint;		   // TOF points, should be cluster
   TAGdataDsc*       fpGlbTrack;		   // global tracks
   TAGparaDsc*       fpGGeoMap;
   TAGparaDsc*       fpDiGeoMap;       // par geo for dipole
   TAGparaDsc*       fpVtxGeoMap;      // par geo for vertex
   TAGparaDsc*       fpItrGeoMap;      // par geo for inner tracker
   TAGparaDsc*       fpMsdGeoMap;      // par geo for MSD
   TAGparaDsc*       fpTofGeoMap;      // par geo for ToF
   TADIgeoField*     fField;
   
//   TAGntuPoint*      fpNtuPoint;       // tmp containers of all points
   TAGactTreeReader* fActEvtReader;    // tree reader, atand alone mode only
   
   TATOEbaseAct*     fActTOE;
    
   TH1F*             fpHisMass;

public:
   //! Disable/Enable stand alone reading
   static void DisableStdAlone()   { fgStdAloneFlag = false;  }
   static void EnableStdAlone()    { fgStdAloneFlag = true;   }
   static Bool_t GetStdAloneFlag() { return fgStdAloneFlag;   }
   
private:
   static Bool_t     fgStdAloneFlag;    // flag for standalone (read from root file using MC interface)
   
   ClassDef(TAGactNtuGlbTrack,0)
};

#endif
