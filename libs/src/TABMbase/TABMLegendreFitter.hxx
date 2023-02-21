#ifndef _TABMLegendreFitter_HXX
#define _TABMLegendreFitter_HXX
/*!
  \file
  \brief   Declaration of TABMactNtuTrack.
*/
/*------------------------------------------+---------------------------------*/
#include "TAGnamed.hxx"
#include "TH2F.h"
class TABMparConf;
class TABMntuHit;
class TABMhit;



class TABMLegendreFitter : public TAGnamed {
public:

  explicit TABMLegendreFitter(const char * name=0, TABMparConf* p_bmcon=0);
  
  virtual ~TABMLegendreFitter();

  //! To stream
  virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
  
  // make all the analysis from here!
  Int_t doLegendreFit(TABMntuHit* p_nturaw, Int_t trackNum, Int_t nowView);

  // to save the Legendre histogram
  void SaveLegpol();

  // Getters and NO Setters!
  Float_t GetBestMValue() const {return bestMValue;};
  Float_t GetBestQValue() const {return bestQValue;};
  Float_t GetMBinWidth() const {return mBinWidth;};
  Float_t GetRBinWidth() const {return qBinWidth;};
  Int_t GetMRebins() const { return mRebins;};
  Int_t GetQRebins() const { return qRebins;};

protected:

  //fill the fLegPolSum for the legendre polynomy
  void ChargeLegendrePoly();

  //find in fLegPolSum the highest bin with the first estimate of the track parameters
  void FindLegendreBestValues();

  //???
  Int_t CheckAssHits(const Float_t asshiterror, const Float_t minMerr);

  //adopted inside checkasshits to check if a hit is associated to the track or not
  Int_t CheckPossibleHits(Float_t yvalue, Float_t residual, Float_t resolution, 
                          Int_t hitnum, TABMhit* p_hit, Int_t wireplane[]);

private:

  TABMntuHit* fpnturaw;
  Int_t    fTrackNum;
  Int_t    fNowView;

  // Legendre workbench:
  TH2F*    fLegPolSum;
  Int_t    fNxBins;
  Float_t* fpM;
  Float_t* fpSqmqp1;

  // Legendre results
  Int_t    fBestMBin;
  Float_t  bestMValue;
  Float_t  mBinWidth;
  Int_t    mRebins;
  Int_t    fBestQBin;
  Float_t  bestQValue;
  Float_t  qBinWidth;
  Int_t    qRebins;

  TABMparConf* fpbmcon;
   
  ClassDef(TABMLegendreFitter,0)
};
 
#endif
