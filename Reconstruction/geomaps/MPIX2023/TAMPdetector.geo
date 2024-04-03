// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for Monopix2
// (Test beam at Desy 2023)
// 
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Sensors
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Sensors:              7
Types:                2

// Monopix2
TypeName:          "MPX2"
TypeNumber:           0
PixelsNu:           512
PixelsNv:           512
PitchU:            0.003304
PitchV:            0.003304
TotalSizeX:       16916.5 TotalSizeY:   16916.5 TotalSizeZ:    0.0300
fEpiSizeX:        16916.5 EpiSizeY:     16916.5 EpiSizeZ:      0.0014
EpiOffsetX:       0.0     EpiOffsetY:   0.0     EpiOffsetZ:    0.0000
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
PixelsNu:          1024
PixelsNv:           512
PitchU:            0.002924
PitchV:            0.002688
TotalSizeX:        29941.8   TotalSizeY:   13762.6    TotalSizeZ:    0.0300
fEpiSizeX:         29941.8   EpiSizeY:     13762.6    EpiSizeZ:      0.0014
EpiOffsetX:        0.0       EpiOffsetY:   0.0        EpiOffsetZ:    0.0000
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
PositionsX:        0.0000  PositionsY:   -0.000  PositionsZ:     2.604
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
PositionsX:        0.0000  PositionsY:   -0.000  PositionsZ:    15.272
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





