#ifndef TPGSPECTRUMCALIB_H
#define TPGSPECTRUMCALIB_H

/*!
 \file TPGspectrumCalib.hxx
 \brief   Declaration of TPGspectrumCalib.
 */


#include<fstream>
#include<iostream>
#include<iomanip>
using namespace std;

#include "TH1.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "TPGspectrumCalibrator.hxx"

class TPGspectrumCalib {
   
public:
   TPGspectrumCalib();
   virtual ~TPGspectrumCalib();
   
   //! Set a point
   void SetPoint(const Int_t index, const Double_t channel, const Double_t energy) { fTabulated[index] = energy; fRaw[index] = channel; }
   void AddPoint(const Double_t channel, const Double_t energy);
   
   //! Set energy
   void SetEnergy(const Int_t index, const Double_t energy)   { fTabulated[index] = energy; }
   //! Set channel
   void SetChannel(const Int_t index, const Double_t channel) { fRaw[index] = channel;      }
   
   //! Get energy
   Double_t GetEnergy(const Int_t index)                const { return fTabulated[index];   }
   //! Get channel
   Double_t GetChannel(const Int_t index)               const { return fRaw[index];         }
   
   void Calibrate(TF1 *function) const;
   void Calibrate(const char* filename, TF1 *function);
   void Calibrate(TF1 *function, const Int_t nbpoints, const Double_t *raw, const Double_t *tabulated);
   void Calibrate(Int_t dimension);
   
   void DrawResults(const TF1 *function) const;
   
   static void CheckCalibration(TH1 *histo, const Double_t value, Double_t xmin=-1111, Double_t xmax=-1111, const Int_t color=2);
   static void CheckCalibration(TH1 *histo, const Int_t nbvalue, const Double_t *values, Double_t xmin=-1111, Double_t xmax=-1111, const Int_t color=2);
   static void CheckCalibration(TH1 *histo, const char *SourceName, Double_t xmin=-1111, Double_t xmax=-1111, const Int_t color=2);
   
protected:
   TArrayD      fTabulated;           ///< Tabulated energie vector
   TArrayD      fRaw;                 ///< Raw channel vector
   
   TPGspectrumCalibrator fCalibrator; ///< Calibrator
   
   // rootcint dictionary
   ClassDef(TPGspectrumCalib,0); ///< a TPGspectrumCalib
   
};

#endif
