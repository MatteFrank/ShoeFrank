// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Mapping file for FOOT Inner Tracker Detector
//

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Sensors
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Sensors:              32

// M28
TypeName:          "M28"
TypeNumber:           0
PixelsNx:           960
PixelsNy:           928
PitchX:           0.00207
PitchY:           0.00207
TotalSizeX:       2.0240   TotalSizeY:   2.27100  TotalSizeZ:    0.0050
EpiSizeX:         1.9872   EpiSizeY:     1.92096  EpiSizeZ:      0.0014
EpiOffsetX:       0.0356   EpiOffsetY:   0.32800  EpiOffsetZ:    0.0000
EpiMat:            "Si"
EpiMatDensity:    2.329
EpiMatExc:        174.5e-6
PixThickness:     0.00064
PixMat:          "SiO2/Al"
PixMatDensities: "2.65/2.70"
PixMatProp:      "0.89/0.11"
PixMatDensity:      2.3

SupportInfo:         1
FoamSizeX:       15.634   FoamSizeY:     2.871  FoamSizeZ:      0.226
FoamOffsetX:      3.908   FoamOffsetY:   0.531  FoamOffsetZ:    0.0000

FoamThickness:     0.2
FoamMat:           "SiC/AIR"
FoamMatDensities: "3.22/1.29e-3"
FoamMatProp:      "0.04/0.96"
FoamMatDensity:    0.1288

KaptonThickness:    0.0025
KaptonMat:        "Kapton"
KaptonMatDensity:   1.42

EpoxyThickness:     0.001
EpoxyMat:         "Epoxy"
EpoxyMatDensity:   1.18

AlThickness:       0.001
AlMat:             "Al"
AlMatDensity:     2.6989

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Detector Planes used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            1
Type:              0
PositionsX:        3.0534  PositionsY:   -2.4314  PositionsZ:  -0.6155
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            2
Type:              0
PositionsX:        1.0294  PositionsY:   -2.4314  PositionsZ:  -0.6155
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            3
Type:              0
PositionsX:       -0.9946  PositionsY:   -2.4314  PositionsZ:  -0.6155
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 4  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            4
Type:              0
PositionsX:       -3.0186  PositionsY:   -2.4314  PositionsZ:  -0.6155
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 5  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            5
Type:              0
PositionsX:        3.0534  PositionsY:   -0.8105  PositionsZ:   0.3845
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 6  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            6
Type:              0
PositionsX:        1.0294  PositionsY:   -0.8105  PositionsZ:   0.3845
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 7  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            7
Type:              0
PositionsX:       -0.9946  PositionsY:   -0.8105  PositionsZ:   0.3845
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 8  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            8
Type:              0
PositionsX:       -3.0186  PositionsY:   -0.8105  PositionsZ:   0.3845
Tilt1:             0.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 9  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            9
Type:              0
PositionsX:        3.0186  PositionsY:    0.8105  PositionsZ:  -0.6155
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 10  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            10
Type:              0
PositionsX:        0.9946  PositionsY:    0.8105  PositionsZ:  -0.6155
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 11  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            11
Type:              0
PositionsX:       -1.0294  PositionsY:    0.8105  PositionsZ:  -0.6155
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 12  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            12
Type:              0
PositionsX:       -3.0534  PositionsY:    0.8105  PositionsZ:  -0.6155
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 13  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            13
Type:              0
PositionsX:        3.0186  PositionsY:    2.4314  PositionsZ:   0.3845
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 14  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            14
Type:              0
PositionsX:        0.9946  PositionsY:    2.4314  PositionsZ:   0.3845
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 15  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            15
Type:              0
PositionsX:       -1.0294  PositionsY:    2.4314  PositionsZ:   0.3845
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 16  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            16
Type:              0
PositionsX:       -3.0534  PositionsY:    2.4314  PositionsZ:   0.3845
Tilt1:           180.0000  Tilt2:         0.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 17  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            17
Type:              0
PositionsX:       -3.0534  PositionsY:   -2.4314  PositionsZ:  -0.3845
Tilt1:             0.0000  Tilt2:       180.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 18  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            18
Type:              0
PositionsX:       -1.0294  PositionsY:   -2.4314  PositionsZ:  -0.3845
Tilt1:             0.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 19  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            19
Type:              0
PositionsX:        0.9946  PositionsY:   -2.4314  PositionsZ:  -0.3845
Tilt1:             0.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 20  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            20
Type:              0
PositionsX:        3.0186  PositionsY:   -2.4314  PositionsZ:  -0.3845
Tilt1:             0.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 21  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            21
Type:              0
PositionsX:       -3.0534  PositionsY:   -0.8105  PositionsZ:   0.6155
Tilt1:             0.0000  Tilt2:       180.000   Tilt3:         0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 22  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            22
Type:              0
PositionsX:       -1.0294  PositionsY:   -0.8105  PositionsZ:   0.6155
Tilt1:             0.0000  Tilt2:       180.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 23  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            23
Type:              0
PositionsX:        0.9946  PositionsY:   -0.8105  PositionsZ:   0.6155
Tilt1:             0.0000  Tilt2:       180.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 24  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            24
Type:              0
PositionsX:        3.0186  PositionsY:   -0.8105  PositionsZ:   0.6155
Tilt1:             0.0000  Tilt2:       180.0000   Tilt3:       0.00
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 25  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            25
Type:              0
PositionsX:       -3.0186  PositionsY:    0.8105  PositionsZ:  -0.3845
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 26  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            26
Type:              0
PositionsX:       -0.9946  PositionsY:    0.8105  PositionsZ:  -0.3845
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 27  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            27
Type:              0
PositionsX:        1.0294  PositionsY:    0.8105  PositionsZ:  -0.3845
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 28  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            28
Type:              0
PositionsX:        3.0534  PositionsY:    0.8105  PositionsZ:  -0.3845
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000



// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 29  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            29
Type:              0
PositionsX:       -3.0186  PositionsY:    2.4314  PositionsZ:   0.6155
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 30  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            30
Type:              0
PositionsX:       -0.9946  PositionsY:    2.4314  PositionsZ:   0.6155
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 31  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            31
Type:              0
PositionsX:        1.0294  PositionsY:    2.4314  PositionsZ:   0.6155
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 32  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            32
Type:              0
PositionsX:        3.0534  PositionsY:    2.4314  PositionsZ:   0.6155
Tilt1:           180.0000  Tilt2:       180.000   Tilt3:        0.000
AlignementU:       0.000
AlignementV:       0.000
AlignementTilt:    0.000


// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of Mapping File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
