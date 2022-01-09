//my Sensitive detector

#ifndef TCITsensitiveDetector_h
#define TCITsensitiveDetector_h 1

/*!
 \file TCITsensitiveDetector.hxx
 \brief ITR interface for base sensitive detector class
 
 \author Ch. Finck
 */


#include "TCGbaseSensitiveDetector.hxx"

class TCITsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCITsensitiveDetector(G4String);
   ~TCITsensitiveDetector();

};

#endif

