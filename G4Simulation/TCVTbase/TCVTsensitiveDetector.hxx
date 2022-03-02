//my Sensitive detector

#ifndef TCVTsensitiveDetector_h
#define TCVTsensitiveDetector_h 1

/*!
 \file TCVTsensitiveDetector.hxx
 \brief VTX interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCVTsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCVTsensitiveDetector(G4String);
   ~TCVTsensitiveDetector();

};

#endif

