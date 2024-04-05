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
TotalSizeX:        10.25   TotalSizeY:    10.36   TotalSizeZ:    0.0152
EpiSizeX:          9.600   EpiSizeY:      9.600   EpiSizeZ:      0.0150
EpiOffsetX:        0.270   EpiOffsetY:    0.380   EpiOffsetZ:    0.0000
EpiMat:            "Si"
EpiMatDensity:     2.329
PixThickness:      0.00064
PixMat:            "SiO2"
PixMatDensity:     2.65
MetalThickness:    0.0001
MetalMat:          "Al"
MetalDensity:      2.6989
//
SupportInfo:       2
//
PCBoardThickness:  0.16
PCBoardMat:        "Epoxy/Eg"
PCBoardDensity:    1.85
PCBSizeX:          16.5  PCBSizeY:     13.66  PCBSizeZ:      0.16
PCBOffsetX:        1.37  PCBOffsetY:     0.0  PCBOffsetZ:    0.0
PCBdHoleSizeX:	   9.2   PCBdHoleSizeY:  9.6  PCBdHoleSizeZ: 0.16
BoxMat:            "Al"
BoxDensity:        2.7
BoxOutSizeX:	   24.0  BoxOutSizeY:   24.00 BoxOutSizeZ:    1.8
BoxInSizeX:	   22.0  BoxInSizeY:    22.00 BoxInSizeZ:     1.0
BoxOffX:	    3.0  BoxOffY:       -3.00 BoxOffZ:        0.00
BoxHoleSizeX:	   10.2  BoxHoleSizeY:  10.2  BoxHoleSizeZ:   0.40
BoxHoleOffX:	    0.0  BoxHoleOffY:    0.0  BoxHoleOffZ:    0.0

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Detector Planes used in the run
// type 0: X
// type 1: Y
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            1
Type:              0
PositionsX:        0.0000  PositionsY:   0.0000  PositionsZ:  -3.4461
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            2
Type:              1
PositionsX:        0.0000  PositionsY:   0.000   PositionsZ:  -2.7539
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:       90.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            3
Type:              0
PositionsX:        0.0000  PositionsY:   0.000  PositionsZ:   -0.3461
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            4
Type:              1
PositionsX:        0.0000  PositionsY:   0.0000  PositionsZ:   0.3461
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:       90.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3-X  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            5
Type:              0
PositionsX:        0.0000  PositionsY:   0.000   PositionsZ:   2.7539
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3-Y  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            6
Type:              1
PositionsX:        0.0000  PositionsY:   0.000  PositionsZ:    3.4461
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:       90.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of Mapping File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
