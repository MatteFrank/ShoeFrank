//my Sensitive detector

#ifndef TCPLsensitiveDetector_h
#define TCPLsensitiveDetector_h 1
/*!
 \file TCPLsensitiveDetector.hxx
 \brief STC interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCPLsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCPLsensitiveDetector(G4String);
   ~TCPLsensitiveDetector();
   
};

#endif

