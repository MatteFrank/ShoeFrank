//my Sensitive detector

#ifndef TCGbaseSensitiveDetector_h
#define TCGbaseSensitiveDetector_h 1

/*!
 \file TCGbaseSensitiveDetector.hxx
 \brief Base sensitive detector class
 
 Handle collections of hits per detector
 
 \author Ch. Finck
 */

#include "G4VSensitiveDetector.hh"
#include "TCGmcHit.hxx" //my hit class

class G4Step; //step class
class G4HCofThisEvent;
class TCGmcHit;

class TCGbaseSensitiveDetector : public G4VSensitiveDetector
{
public:
   TCGbaseSensitiveDetector(G4String);
   virtual ~TCGbaseSensitiveDetector();
   
   //! Initialize list of collection
   void Initialize(G4HCofThisEvent* HCE);
   //! End of event
   void EndOfEvent(G4HCofThisEvent* HCE);
   //! Set copy level
   void SetCopyLevel(G4int level) { fCopyLevel = level; }
   //! Process hits
   G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
   
protected:
   G4int             fCopyLevel;    ///< Copy level
   TCGmcCollections* fCollections;  ///< Hit Collections

protected:
   //! Fill hits from step
   void FillHits(G4Step* aStep, TCGmcHit* newHit);

};

#endif

