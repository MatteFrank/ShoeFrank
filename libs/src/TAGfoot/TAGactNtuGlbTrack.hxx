#ifndef _TAGactNtuGlbTrack_HXX
#define _TAGactNtuGlbTrack_HXX
/*!
 \file TAGactNtuGlbTrack.hxx
 \brief   Declaration of TAGactNtuGlbTrack.
 \author A. Sécher
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
   
   explicit  TAGactNtuGlbTrack(const char* name        = 0,
                               TAGdataDsc* p_vtxclus   = 0,
                               TAGdataDsc* p_vtxtrack  = 0,
                               TAGdataDsc* p_vtxvertex = 0,
                               TAGdataDsc* p_itrclus   = 0,
                               TAGdataDsc* p_msdclus   = 0,
                               TAGdataDsc* p_msdpoint  = 0,
                               TAGdataDsc* p_twpoint   = 0,
                               TAGdataDsc* p_glbtrack  = 0,
                               TAGparaDsc* p_geoG      = 0,
                               TAGparaDsc* p_geodi     = 0,
                               TAGparaDsc* p_geoVtx    = 0,
                               TAGparaDsc* p_geoItr    = 0,
                               TAGparaDsc* p_geoMsd    = 0,
                               TAGparaDsc* p_geoTof    = 0,
                               TADIgeoField* field     = nullptr);
   
   virtual  ~TAGactNtuGlbTrack();
   
   // Action
   Bool_t    Action();
   
   // Base creation of histogram
   void      CreateHistogram();
  
   //! return Global track container
   TAGntuGlbTrack* GetTrackContainer() {return static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() );}
   
   //! output from TOE action
   void Output(){ fActTOE->Output(); }
   
   // Write histogram
   void WriteHistogram();
    
private:
   // Set up action
   TATOEbaseAct* SetupAction() const;
   //! Get field
   TADIgeoField*        GetFootField()      const { return fField;  }
    
private:
   TAGgeoTrafo*      fpFootGeo;        ///< geo trafo
   TAGdataDsc*       fpVtxVertex;		///< VTX Vertex
   TAGdataDsc*       fpVtxTrack;       ///< VTX tracks
   TAGdataDsc*       fpVtxClus;        ///< VTX clusters
   TAGdataDsc*       fpItrClus;		   ///< ITR clusters
   TAGdataDsc*       fpMsdClus;		   ///< MSD clusters
   TAGdataDsc*       fpMsdPoint;       ///< MSD clusters
   TAGdataDsc*       fpTwPoint;		   ///< TOF points, should be cluster
   TAGdataDsc*       fpGlbTrack;		   ///< global tracks
   TAGparaDsc*       fpGGeoMap;        ///< par geo for beam/target
   TAGparaDsc*       fpDiGeoMap;       ///< par geo for dipole
   TAGparaDsc*       fpVtxGeoMap;      ///< par geo for vertex
   TAGparaDsc*       fpItrGeoMap;      ///< par geo for inner tracker
   TAGparaDsc*       fpMsdGeoMap;      ///< par geo for MSD
   TAGparaDsc*       fpTofGeoMap;      ///< par geo for ToF
   TADIgeoField*     fField;           ///< Masgnetic field
   
   TAGactTreeReader* fActEvtReader;    ///< tree reader, stand alone mode only
   
   TATOEbaseAct*     fActTOE;          ///< TOE action
    
   TH1F*             fpHisMass;        ///< mass histogram
   
   ClassDef(TAGactNtuGlbTrack,0)  ///< TAGactNtuGlbTrack
};

#endif
