#ifndef _TANAactPtReso_HXX
#define _TANAactPtReso_HXX
/*!
 \file TANAactPtReso.hxx
 \brief   Declaration of TANAactPtReso
 \author R. Zarrella
 */
/*------------------------------------------+---------------------------------*/

#include "TANAactBaseNtu.hxx"

class TANAactPtReso : public TANAactBaseNtu {
   
public:
   explicit  TANAactPtReso(const char* name         = 0,
                            TAGdataDsc* p_irntutrack = 0,
                            TAGparaDsc* p_geomap     = 0);

   virtual ~TANAactPtReso();
   
   // Create histograms
   virtual  void    CreateHistogram();
   
   // Action
   virtual  Bool_t  Action();

private:
   TAGdataDsc*     fpNtuTrack;          ///< input global tracks
   TAGgeoTrafo*    fpFootGeo;           ///< First geometry transformer
   TAGparaDsc*     fpGeoMapG;           ///< geometry para dsc

   Float_t         fBeamEnergyTarget;   ///< Beam energy at target
   Float_t         fBeamEnergy;         ///< Beam energy
   Float_t         fBeamA;              ///< Beam atomic mass
   Float_t         fBeamZ;              ///< Beam atomic number
   
   ClassDef(TANAactPtReso, 0)
};

#endif
