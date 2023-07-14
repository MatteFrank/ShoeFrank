//my Sensitive detector

#ifndef TCCEsensitiveDetector_h
#define TCCEsensitiveDetector_h 1

/*!
 \file TCCEsensitiveDetector.hxx
 \brief TW interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCCEsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCCEsensitiveDetector(G4String);
   ~TCCEsensitiveDetector();
      
};

#endif

