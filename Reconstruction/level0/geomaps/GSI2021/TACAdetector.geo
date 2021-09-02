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
TypeGeo:        "GSI2021"

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
Widthback :   1.500000	 Heightback :  1.500000	   Length:           12.000000
Delta:        0.100000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (temporally AIR)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Material:      "AIR"
Density:       1.2e-3
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.942290   Heightfront:  3.942290    DummyLength:       0.
Widthback :   4.659540	 Heightback :  4.659540	   Length:           5.765613
PositionZ:     5.754359

CrystalN:  18
ModulesN:  2
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  1
PositionX:  2.598828633406   PositionY:   0.000000000000   PositionZ:   -0.056225596529
TiltX:      0.000000000000   TiltY:       2.386978847964   TiltZ:       0.000000000000
CrystalId:  2
PositionX:  -2.598828633406   PositionY:   0.000000000000   PositionZ:   -0.056225596529
TiltX:      0.000000000000   TiltY:       -2.386978847964   TiltZ:       0.000000000000
CrystalId:  3
PositionX:  0.000000000000   PositionY:   2.598828633406   PositionZ:   -0.056225596529
TiltX:      -2.386978847964   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  4
PositionX:  2.598828633406   PositionY:   2.596486916587   PositionZ:   -0.112402407292
TiltX:      -2.389049377105   TiltY:       2.384906519147   TiltZ:       -0.099471758232
CrystalId:  5
PositionX:  -2.598828633406   PositionY:   2.596486916587   PositionZ:   -0.112402407292
TiltX:      -2.389049377105   TiltY:       -2.384906519147   TiltZ:       0.099471758232
CrystalId:  6
PositionX:  0.000000000000   PositionY:   -2.598828633406   PositionZ:   -0.056225596529
TiltX:      2.386978847964   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  7
PositionX:  2.598828633406   PositionY:   -2.596486916587   PositionZ:   -0.112402407292
TiltX:      2.389049377105   TiltY:       2.384906519147   TiltZ:       0.099471758232
CrystalId:  8
PositionX:  -2.598828633406   PositionY:   -2.596486916587   PositionZ:   -0.112402407292
TiltX:      2.389049377105   TiltY:       -2.384906519147   TiltZ:       -0.099471758232
CrystalId:  9
PositionX:  -7.877457278087   PositionY:   0.000000000000   PositionZ:   -0.505426148494
TiltX:      0.025127301635   TiltY:       -7.160892688432   TiltZ:       -0.201572266751
CrystalId:  10
PositionX:  -5.291906519730   PositionY:   -0.009071604847   PositionZ:   -0.237253507346
TiltX:      0.025018100553   TiltY:       -4.773914068948   TiltZ:       -0.200522106942
CrystalId:  11
PositionX:  -10.448990231163   PositionY:   0.009071604847   PositionZ:   -0.885172854528
TiltX:      0.025281525022   TiltY:       -9.547871305511   TiltZ:       -0.202633484065
CrystalId:  12
PositionX:  -7.861447529832   PositionY:   2.598812800452   PositionZ:   -0.560082343755
TiltX:      -2.361851546329   TiltY:       -7.160892688432   TiltZ:       -0.201572266751
CrystalId:  13
PositionX:  -5.275910963311   PositionY:   2.587399493053   PositionZ:   -0.291862316332
TiltX:      -2.351588287097   TiltY:       -4.775932209104   TiltZ:       -0.300254879541
CrystalId:  14
PositionX:  -10.432994674744   PositionY:   2.605542702747   PositionZ:   -0.939781663514
TiltX:      -2.376341897900   TiltY:       -9.545831924746   TiltZ:       -0.101851479063
CrystalId:  15
PositionX:  -7.879449221061   PositionY:   -2.598812800452   PositionZ:   -0.562344018120
TiltX:      2.412106149599   TiltY:       -7.160892688432   TiltZ:       -0.201572266751
CrystalId:  16
PositionX:  -5.293896433823   PositionY:   -2.605542702747   PositionZ:   -0.294121952779
TiltX:      2.401624562788   TiltY:       -4.776019002409   TiltZ:       -0.100791134637
CrystalId:  17
PositionX:  -10.450980145256   PositionY:   -2.587399493053   PositionZ:   -0.942041299961
TiltX:      2.426904560532   TiltY:       -9.545744218364   TiltZ:       -0.303413611845

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   1
PositionX:  -7.877457278087   PositionY:   0.000000000000   PositionZ:   -0.505426148494
TiltX:      0.025127301635   TiltY:       -7.160892688432   TiltZ:       -0.201572266751
