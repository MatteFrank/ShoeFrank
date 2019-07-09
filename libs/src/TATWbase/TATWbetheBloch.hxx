#ifndef _TATWbetheBloch_HXX
#define _TATWbetheBloch_HXX

#include "TAGobject.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TATWparGeo.hxx"

class TATWbetheBloch : public TAGobject {
   
public:
   TATWbetheBloch(TATWparGeo* parGeo);
   ~TATWbetheBloch();
   
   Double_t GetDeltaE(Double_t deltaX, Double_t beta, Double_t z, Double_t mass);

private:
   TATWparGeo* fpParGeo;
   Double_t    fBarDensity;
   Double_t    fBarZ;
   Double_t    fBarA;
   Double_t    fBeamZ;
   
private:
   static const Double_t fgkFactorK;
   static const Double_t fgkMassElectron;
   static const Double_t fgkIonisation;
   
   ClassDef(TATWbetheBloch,0)
};


#endif 
