//my Sensitive detector

#ifndef TCSTsensitiveDetector_h
#define TCSTsensitiveDetector_h 1
/*!
 \file TCSTsensitiveDetector.hxx
 \brief STC interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCSTsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCSTsensitiveDetector(G4String);
   ~TCSTsensitiveDetector();
   
};

#endif

