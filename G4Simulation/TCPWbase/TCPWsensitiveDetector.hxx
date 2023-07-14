//my Sensitive detector

#ifndef TCPWsensitiveDetector_h
#define TCPWsensitiveDetector_h 1

/*!
 \file TCPWsensitiveDetector.hxx
 \brief TW interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCPWsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCPWsensitiveDetector(G4String);
   ~TCPWsensitiveDetector();
      
};

#endif

