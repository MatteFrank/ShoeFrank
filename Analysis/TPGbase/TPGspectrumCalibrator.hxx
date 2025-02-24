#ifndef TPGSPECTRUMCALIBRATOR_H
#define TPGSPECTRUMCALIBRATOR_H

/*!
 \file TPGspectrumCalibrator.hxx
 \brief   Declaration of TPGspectrumCalibrator.
 */

// root's includes
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TAxis.h>
#include <TF1.h>

class TPGspectrumCalibrator
{
public:
   TPGspectrumCalibrator();
   virtual ~TPGspectrumCalibrator(); // destructor
   void Calibrate(const TH1*, const TF1*, const Option_t *AxisOption="x");
  
   // perform calibration of a TH2 with 2 TF1
   void Calibrate(const TH2*, const TF1*, const TF1*);
   
   //! perform calibration of a TH3 with 3 TF1
   void Calibrate(const TH3*, const TF1*, const TF1*, const TF1*);
   void Calibrate(const TH3*, const TF1*, const TF1*, const Option_t *AxisOption="xy");
   
   // perform calibration of a TH3 with 2 TF1
   void Calibrate(const TH1*, const Double_t a, const Double_t b, const Option_t *AxisOption="x");
   void Calibrate(const TH1*, const Double_t a, const Double_t b, const Double_t c, const Option_t *AxisOption="x");
   // perform calibration of a TH2 with 6 coefficients
   void Calibrate(const TH2*, const Double_t ax, const Double_t bx, const Double_t cx,
                  const Double_t ay, const Double_t by, const Double_t cy);
   // perform calibration of a TH2 with 4 coefficients
   void Calibrate(const TH2*, const Double_t ax, const Double_t bx,
                  const Double_t ay, const Double_t by);
   // perform calibration of a TH3 with 9 coefficients
   void Calibrate(const TH3*, const Double_t ax, const Double_t bx, const Double_t cx, const Double_t ay,
                  const Double_t by, const Double_t cy, const Double_t az, const Double_t bz, const Double_t cz);
   // perform calibration of a TH3 with 6 coefficients
   void Calibrate(const TH3*, const Double_t ax, const Double_t bx, const Double_t ay,
                  const Double_t by, const Double_t az, const Double_t bz);
   // perform calibration of a TH3 with 6 coefficients (i.e. 2 axis)
   void Calibrate(const TH3*, const Double_t a1, const Double_t b1, const Double_t c1, const Double_t a2,
                  const Double_t b2, const Double_t c2, const Option_t *AxisOption);
   // perform calibration of a TH3 with 4 coefficients (i.e. 2 axis)
   void Calibrate(const TH3*, const Double_t a1, const Double_t b1, const Double_t a2,
                  const Double_t b2, const Option_t *AxisOption);
   
   // return the calibrated histogram
   TH1* GetHistoCal() const;
   
   // draw the calibrated histogram in the current pad with same range of the raw histogram
   void DrawResult(const Option_t *opt="") const ;
   
private:
   TAxis *fXRawAxis; // X raw axis
   TAxis *fYRawAxis; // Y raw axis
   TAxis *fZRawAxis; // Z raw axis
   TH1   *fHistoCal; // Calibrated Histo
   
   // set a new raw histogram - the old calibrated histogram is deleted
   void Set(const TH1 *);
   void CalibOneAxis(const TF1 *, const Option_t *);
   void CalibOneAxis(const Double_t, const Double_t, const Option_t *);
   void CalibOneAxis(const Double_t, const Double_t, const Double_t, const Option_t *);
   void Invert();
   
   //! rootcint dictionary
   ClassDef(TPGspectrumCalibrator,0); // a TPGspectrumCalibrator
};

#endif



