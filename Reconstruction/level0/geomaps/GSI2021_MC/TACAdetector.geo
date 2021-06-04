// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for FOOT Calorimeter
//
// Created with macro: Reconstruction/level0/BuildCaGeoFile.C
//
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of Geometry 
// type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TypeGeo:        "ONE_MOD"

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Crystals
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Material:      "BGO"
Density:       7.13
Excitation:    4.341e-4
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Crystals (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   1.000000   Heightfront:  1.000000    DummyLength:       0.
Widthback :   1.450000	 Heightback :  1.450000	   Length:           12.000000
Delta:        0.100000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (temporally AIR)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Material:      "AIR"
Density:       1.2e-3
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.868873   Heightfront:  3.868873    DummyLength:       0.
Widthback :   4.514654	 Heightback :  4.514654	   Length:           5.764053
PositionZ:     5.754939

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Air around each module. It is need ONLY by FLUKA geometry (cm)
// it will be removed if a truncate piramid body is implemented in FLUKA 
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.113376   Heightfront:  3.113376    DummyLength:    0.
Widthback :   4.467966	 Heightback :  4.467966	   Length:         12.120000
PositionZ:    -0.066000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Calorimeter bounding box (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Width:          8.995783     Height:      8.995783      Thick:     24.257389, 
PositionZ:      -0.078695

CrystalN:  9
ModulesN:  1
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  1
PositionX:  2.549068686790   PositionY:   0.000000000000   PositionZ:   -0.049670037877
TiltX:      0.000000000000   TiltY:       2.148339996745   TiltZ:       0.000000000000
CrystalId:  2
PositionX:  -2.549068686790   PositionY:   0.000000000000   PositionZ:   -0.049670037877
TiltX:      0.000000000000   TiltY:       -2.148339996745   TiltZ:       0.000000000000
CrystalId:  3
PositionX:  0.000000000000   PositionY:   2.549068686790   PositionZ:   -0.049670037877
TiltX:      -2.148339996745   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  4
PositionX:  2.549068686790   PositionY:   2.547206714969   PositionZ:   -0.099305163783
TiltX:      -2.149849662143   TiltY:       2.146829268744   TiltZ:       -0.080572146787
CrystalId:  5
PositionX:  -2.549068686790   PositionY:   2.547206714969   PositionZ:   -0.099305163783
TiltX:      -2.149849662143   TiltY:       -2.146829268744   TiltZ:       0.080572146787
CrystalId:  6
PositionX:  0.000000000000   PositionY:   -2.549068686790   PositionZ:   -0.049670037877
TiltX:      2.148339996745   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  7
PositionX:  2.549068686790   PositionY:   -2.547206714969   PositionZ:   -0.099305163783
TiltX:      2.149849662143   TiltY:       2.146829268744   TiltZ:       0.080572146787
CrystalId:  8
PositionX:  -2.549068686790   PositionY:   -2.547206714969   PositionZ:   -0.099305163783
TiltX:      2.149849662143   TiltY:       -2.146829268744   TiltZ:       -0.080572146787

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
