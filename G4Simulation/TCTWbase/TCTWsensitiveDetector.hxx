//my Sensitive detector

#ifndef TCTWsensitiveDetector_h
#define TCTWsensitiveDetector_h 1

/*!
 \file TCTWsensitiveDetector.hxx
 \brief TW interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCTWsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCTWsensitiveDetector(G4String);
   ~TCTWsensitiveDetector();
      
};

#endif

