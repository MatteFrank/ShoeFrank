// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Mapping file for QAPIVI Vertex Detector
// (Test beam at Catania 2015)
// 2*M18
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Sensors
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Sensors:              7
Types:                2

// Monopix2
TypeName:          "MPX2"
TypeNumber:           0
PixelsNu:           960
PixelsNv:           928
PitchU:            20.7
PitchV:            20.7
TotalSizeX:       22710   TotalSizeY:   23220   TotalSizeZ:    50
fEpiSizeX:        22710   EpiSizeY:     20220   EpiSizeZ:      14
EpiOffsetX:       0.0356   EpiOffsetY:   0.32800  EpiOffsetZ:    0.0000
EpiMat:            "Si"
EpiMatDensity:    2.329
EpiMatExc:        174.5e-6
PixThickness:     0.00064
PixMat:          "SiO2/Al"
PixMatDensities: "2.65/2.70"
PixMatProp:      "0.89/0.11"
PixMatDensity:      2.3

// Adenium
TypeName:          "Adenium"
TypeNumber:           1
PixelsNu:           512
PixelsNv:           512
PitchU:            10.0
PitchV:            10.0
TotalSizeX:        5130   TotalSizeY:   5130    TotalSizeZ:    50
fEpiSizeX:         5120   EpiSizeY:     5120    EpiSizeZ:      14
EpiOffsetX:       0.0356   EpiOffsetY:   0.32800  EpiOffsetZ:    0.0000
EpiMat:            "Si"
EpiMatDensity:    2.329
EpiMatExc:        174.5e-6
PixThickness:     0.00064
PixMat:          "SiO2/Al"
PixMatDensities: "2.65/2.70"
PixMatProp:      "0.89/0.11"
PixMatDensity:      2.3


// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Detector Planes used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 1  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:           1
Type:             1
PositionsX:       0.000  PositionsY:     0.000  PositionsZ:     0
Tilt1:            0.000  Tilt2:          0.000   Tilt3:         0.000
AlignementU:      0.00
AlignementV:      0.00
AlignementTilt:   0.00

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 2  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            2
Type:              1
PositionsX:        0.0437  PositionsY:   -0.000  PositionsZ:     2.604
Tilt1:             0.0000  Tilt2:         0.000   Tilt3:         0.000
AlignementU:       0.00
AlignementV:       0.00
AlignementTilt:    0.00

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 3  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:           3
Type:             1
PositionsX:       0.000  PositionsY:     0.000  PositionsZ:     5.203
Tilt1:            0.000  Tilt2:          0.000   Tilt3:         0.000
AlignementU:      0.00
AlignementV:      0.00
AlignementTilt:   0.00

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 4  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:           4
Type:             0
PositionsX:       0.000  PositionsY:     0.000  PositionsZ:     8.269
Tilt1:            0.000  Tilt2:          0.000   Tilt3:         0.000
AlignementU:      0.00
AlignementV:      0.00
AlignementTilt:   0.00

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 5  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            5
Type:              1
PositionsX:        0.000  PositionsY:     0.000  PositionsZ:    12.657
Tilt1:             0.000  Tilt2:          0.000   Tilt3:         0.000
AlignementU:      0.00
AlignementV:      0.00
AlignementTilt:   0.00


// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 6  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            6
Type:              1
PositionsX:        0.0437  PositionsY:   -0.000  PositionsZ:    15.272
Tilt1:             0.0000  Tilt2:         0.000   Tilt3:         0.000
AlignementU:       0.00
AlignementV:       0.00
AlignementTilt:    0.00

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Plane 7  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:           7
Type:             1
PositionsX:       0.000  PositionsY:     0.000  PositionsZ:    17.898
Tilt1:            0.000  Tilt2:          0.000   Tilt3:         0.000
AlignementU:      0.00
AlignementV:      0.00
AlignementTilt:   0.00


// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// End of Mapping File
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-





