// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for target and beam
//

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Beam info
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
BeamSize:            0.48
BeamShape:         "Gaussian"
BeamEnergy:          0.2   //! GeV/u
BeamAtomicMass:       12   //! A Beam
BeamAtomicNumber:     6    //! Z Beam
BeamMaterial:        "C"   //! Beam Material
BeamPartNumber:       1    // particles in Beam
BeamPosX:            0.0000   BeamPosY:     0.0000    BeamPosZ:      -50.0
BeamSpreadX:         0.0000   BeamSpreadY:  0.0000    BeamSpread:      0.0
BeamDiv:             0.0000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Target info (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TargetShape:       "cubic"
TargetSizeX:        5.0   TargetSizeY:    5.0    TargetSizeZ:  0.5
TargetMaterial:     "C"
TargetAtomicMass:   12.0107
TargetDensity:      1.83
TargetExc:         78.0e-6


//TargetMaterial:   "Polyethy"
//TargetAtomicMass:   28.0534
//TargetDensity:      0.94
//TargetExc:         61.0e-6

TargetInserts:        0

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of geometry File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
