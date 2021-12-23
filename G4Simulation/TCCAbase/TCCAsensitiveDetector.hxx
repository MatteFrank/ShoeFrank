//my Sensitive detector

#ifndef TCCAsensitiveDetector_h
#define TCCAsensitiveDetector_h 1

/*!
\file TCCAsensitiveDetector.hxx
\brief  Sensitive detector of CAL

\author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "TCGbaseSensitiveDetector.hxx"

class TCCAsensitiveDetector : public TCGbaseSensitiveDetector
{
public:
   TCCAsensitiveDetector(G4String);
   ~TCCAsensitiveDetector();
   
};

#endif

