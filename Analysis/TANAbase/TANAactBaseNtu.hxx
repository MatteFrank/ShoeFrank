#ifndef _TANAactBaseNtu_HXX
#define _TANAactBaseNtu_HXX
/*!
 \file TANAactBaseNtu.hxx
 \brief   Declaration of TANAactBaseNtu.
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TTree.h"

#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGaction.hxx"

class TANAactBaseNtu : public TAGaction {
   
public:
   explicit  TANAactBaseNtu(const char* name         = 0,
                            TAGdataDsc* p_irntutrack = 0,
                            TAGparaDsc* p_geomap     = 0,
                            TTree*      p_tree       = 0);

   virtual ~TANAactBaseNtu();
   
   // Create histograms
   virtual  void    CreateHistogram() { return;      }
   
   // Action
   virtual  Bool_t  Action()          { return true; }

protected:
   TString GetParticleNameFromCharge(int ch);

private:
   TAGdataDsc*     fpNtuTrack;          ///< input global tracks
   TAGgeoTrafo*    fpFootGeo;           ///< First geometry transformer
   TAGparaDsc*     fpGeoMapG;           ///< geometry para dsc
   TTree*          fpTree;              ///< tree

   Float_t         fBeamEnergyTarget;   ///< Beam energy at target
   Float_t         fBeamEnergy;         ///< Beam energy
   Float_t         fBeamA;              ///< Beam atomic mass
   Float_t         fBeamZ;              ///< Beam atomic number
   
   ClassDef(TANAactBaseNtu, 0)
};

#endif
