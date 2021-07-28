// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Mapping file for FOOT Micro Strip Detector
//

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Sensors
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Sensors:              6


//NO MORE KAPTON BUT AIR IN BETWEEN
//0.8 um di SiO2
//150 silicio
//da 1.2 um di metallizzazione ALUMINUM


// MSD
TypeName:          "MSD"
TypeNumber:        1
StripN:            640
Pitch:             0.0150
TotalSizeX:        9.815   TotalSizeY:    9.815   TotalSizeZ:    0.0200
EpiSizeX:          9.600   EpiSizeY:      9.390   EpiSizeZ:      0.0150
EpiOffsetX:        0.1075  EpiOffsetY:    0.2125  EpiOffsetZ:    0.0000
EpiMat:            "Si"
EpiMatDensity:     2.329
PixThickness:      0.00064
PixMat:            "SiO2"
PixMatDensity:     2.65
MetalThickness:    0.0001
MetalMat:          "Al"
MetalDensity:      2.6989
SupportInfo:       0


// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Detector Planes used in the run
// type 0: X
// type 1: Y
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            1
Type:              1
PositionsX:        0.0000  PositionsY:   0.2880  PositionsZ:   -5.260
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:       -90.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            2
Type:              0
PositionsX:       -0.3699  PositionsY:   0.000   PositionsZ:  -4.740
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            3
Type:              1
PositionsX:        0.0000  PositionsY:   0.2522  PositionsZ:  -0.260
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:      -90.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            4
Type:              0
PositionsX:       -0.3757  PositionsY:   0.0000  PositionsZ:   0.260
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            5
Type:              1
PositionsX:        0.0000  PositionsY:   0.2921   PositionsZ:   4.740
Tilt1:             0.0000  Tilt2:        0.000    Tilt3:      -90.000
AlignementU:       0.000
AlignementV:       0.0000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            6
Type:              0
PositionsX:       -0.3506  PositionsY:   0.000  PositionsZ:    5.260
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.0000
AlignementV:       0.000
AlignementTilt:    0.000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of Mapping File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
