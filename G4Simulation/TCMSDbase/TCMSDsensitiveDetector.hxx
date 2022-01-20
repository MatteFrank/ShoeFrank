#ifndef TCMSDSsensitiveDetector_h
#define TCSDSsensitiveDetector_h 1

/*!
 \file TCMSDsensitiveDetector.hxx
 \brief MSD interface for base sensitive detector class
 
 \author Ch. Finck
 */

#include "TCGbaseSensitiveDetector.hxx"

class TCMSDsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCMSDsensitiveDetector(G4String);
   ~TCMSDsensitiveDetector();
};

#endif

