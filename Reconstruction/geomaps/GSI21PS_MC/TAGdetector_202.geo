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
BeamAtomicMass:       16   //! A Beam
BeamAtomicNumber:     8    //! Z Beam
BeamMaterial:        "O"   //! Beam Material
BeamPartNumber:       1    // particles in Beam
BeamPosX:          0.146955   BeamPosY:  -0.055152    BeamPosZ:     -104.45
BeamSpreadX:        -0.7104   BeamSpreadY: -0.5527    BeamSpread:      0.0
BeamDiv:             0.0000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Target info (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TargetShape:       "cubic"
TargetSizeX:        5.0   TargetSizeY:    5.0    TargetSizeZ:  1.0
//TargetMaterial:     "C"
//TargetAtomicMass:   12.0107
//TargetDensity:      1.83
//TargetExc:         78.0e-6

TargetMaterial:   "AIR"
TargetAtomicMass:   15.54
TargetDensity:      0.00129
TargetExc:         92.11e-6

TargetInserts:        0

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of geometry File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
