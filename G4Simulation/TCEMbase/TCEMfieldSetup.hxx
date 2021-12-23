//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TCEMfieldSetup_h
#define TCEMfieldSetup_h 1

/*!
 \file TCEMfieldSetup.hxx
 \brief  Field map setup
 
 \author Ch. Finck
 */

#include "G4MagneticField.hh"
#include "TCEMfield.hxx"

#include "TVector3.h"

class G4FieldManager;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;
class TCEMfieldSetupMessenger;
class G4MagneticField;

class TCEMfieldSetup
{
public:
   TCEMfieldSetup(TCEMfield* field);
  virtual ~TCEMfieldSetup();

   //! Set setpper type
  void SetStepperType(Int_t i) { fStepperType = i; }
   //! Set minimum step
  void SetMinStep(Double_t s)  { fMinStep = s;     }
  
   //! Set stepper
  void SetStepper();
   //! Initialise
  void Initialize();    //  Set parameters and call method below
   //! Create stepper
  void CreateStepperAndChordFinder();
   //! Return Field manager
   G4FieldManager*  GetFieldManager() { return fFieldManager; }

protected:
  G4FieldManager*         fFieldManager;   ///< Field manager
  G4ChordFinder*          fChordFinder;    ///< Chord finder
  G4Mag_UsualEqRhs*       fEquation;       ///< Mouvement equation
  TCEMfield*              fMagneticField;  ///< Magnetic field map

  G4MagIntegratorStepper* fStepper;        ///< Stepper
  Int_t                   fStepperType;    ///< Flag to chose stepper type

  Double_t                fMinStep;        ///< Minimum step
 
};

#endif
