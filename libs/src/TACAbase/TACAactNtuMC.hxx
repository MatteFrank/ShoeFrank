#ifndef _TACAactNtuMC_HXX
#define _TACAactNtuMC_HXX
/*!
  \file
  \version $Id: TACAactNtuMC.hxx,v 1.4 2003/06/09 18:17:14 mueller Exp $
  \brief   Declaration of TACAactNtuMC.
*/
/*------------------------------------------+---------------------------------*/

#include "Evento.hxx"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TACAdigitizer.hxx"
#include "TACAparGeo.hxx"
#include "TAGgeoTrafo.hxx"

#include "TH1F.h"
#include "TH2F.h"

class TACAactNtuMC : public TAGaction {

  protected:
    TObjArray*         fListOfParticles;

  public:
    explicit        TACAactNtuMC(const char* name     = 0,
                                 TAGdataDsc* p_datraw = 0,
                                 TAGparaDsc* p_geomap = 0,
                                 EVENT_STRUCT* evStr  = 0);
   
    virtual         ~TACAactNtuMC();

    virtual         Bool_t  Action();

    void           CreateHistogram();

    ClassDef(TACAactNtuMC,0)

  private:
    TAGparaDsc*     fpGeoMap;		    // geometry para dsc
    TAGdataDsc*     fpNtuMC;		    // output data dsc
    TACAparGeo*     parGeo;   
    TAGgeoTrafo*    geoTrafo;
    TACAdigitizer*  fDigitizer;     // cluster size digitizer
    EVENT_STRUCT*   fpEvtStr;

    TH1F* fpHisDeTot;
    TH1F* fpHisDeTotMc;
    TH1F* fpHisDeNeutron;
    TH1F* fpHisMass;
    TH1F* fpHisTime;
    TH1F* fpHisDeIonSpectrum[8];
    TH1F* fpHisDeIon[8];
    TH1F* fpHisEnPerCry[300];

    TH2F* fpHisCryHitVsEnDep;
    TH2F* fpHisCryHitVsZ;
    TH2F* fpHisEnDepVsZ;
    TH2F* fpHisHitMapXY;
    TH2F* fpHisHitMapZYin;
    TH2F* fpHisHitMapZYout;
    TH2I* fpHisParticleVsRegion;
   
private:
   void           CreateDigitizer();

};


struct EnergyDep {
  EnergyDep(int crystalId, float energyDep) : crystalId(crystalId), energyDep(energyDep) {}
  int crystalId;
  float energyDep;
};

class ParticleEnergyDep : public TObject {
  public:  

    ParticleEnergyDep(int partId, int i) : 
       partId(partId), iRelease(i) {}
    int partId;                                  // particle ID
    int iRelease;                                // index in EvnStr ---> i (the number of the energy deposition)
    std::vector<EnergyDep> energyDeps;           // vector of pairs of: (cryID - endep)

    void addEnergyDep(int cryId, float energyDep) {
       this->energyDeps.push_back(EnergyDep(cryId, energyDep));
    }

    //Sum the energy blocks of one particle (trackID)
    float getTotalEnergyDep() {
       float totalEnergyDep(0);
       for (std::vector<EnergyDep>::iterator it = this->energyDeps.begin(); it != this->energyDeps.end(); ++it) {
          totalEnergyDep += (*it).energyDep;
       }
       return totalEnergyDep;
    }

    std::set<int> getUniqueCryIds() {
       std::set<int> uniqueCryIds;
       for (std::vector<EnergyDep>::const_iterator it = this->energyDeps.begin(); it != this->energyDeps.end(); it++) {
          uniqueCryIds.insert((*it).crystalId);
       }
       return uniqueCryIds;
    }

    float getNCrystals() {
      return this->getUniqueCryIds().size();
    }
    ClassDef(ParticleEnergyDep,0)
};


#endif
