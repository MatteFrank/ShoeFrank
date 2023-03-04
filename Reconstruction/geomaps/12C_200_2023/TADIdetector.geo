// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for FOOT magnets
//

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the magnets
// Types 0 const, 1 function, 2 map file
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Magnets:     2
Type:        2
MapName:     "./data/SigmaPhi_FOOT2023.table"

MapMeshSize: 0.5
MagLimLoX:  -5.5     MagLimLoY:    -5.5   MagLimLoZ:    -50.0
MagLimUpX:   5.5     MagLimUpY:     5.5   MagLimUpZ:     50.0

ShieldThickness:  0.5
ShieldMat:        "Al"
ShieldDensity:    2.6989

MagMat:       "NdFeB"
MagDensity:    7.7

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the magnet placement (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Magnet 1  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            1
InnerRad:          3.0     OuterRad:    16.500   Size3:          10.0
PositionsX:        0.0000  PositionsY:   0.000   PositionsZ:    -8.25
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:           0.0

// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- Magnet 2  -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
Inputs:            2
InnerRad:          5.8     OuterRad:    27.500   Size3:          10.0
PositionsX:        0.0000  PositionsY:   0.000   PositionsZ:     8.25
Tilt1:             0.0000  Tilt2:        0.000   Tilt3:           0.0




