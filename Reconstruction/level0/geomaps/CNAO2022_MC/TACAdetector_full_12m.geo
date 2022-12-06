// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for FOOT Calorimeter
//
// Created with macro: Reconstruction/level0/BuildCaGeoFile.C
//
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of Geometry 
// type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD, FIVE_MOD, 
//                   SEVEN_MOD, SEVEN_MOD_HIT22, FULL_DET_V1) 
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TypeGeo:        "TWELVE_MOD_CNAO22"

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameters of the Crystals
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Material:      "BGO"
Density:       7.13
Excitation:    4.341e-4
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Crystals (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   1.050000   Heightfront:  1.050000    DummyLength:       0.
Widthback :   1.450000	 Heightback :  1.450000	   Length:           12.000000
Delta:        0.095000

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (temporally AIR)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Material:      "AIR"
Density:       1.2e-3
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Support (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.935077   Heightfront:  3.935077    DummyLength:       0.
Widthback :   4.509309	 Heightback :  4.509309	   Length:           5.762494
PositionZ:     5.755293

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Air around each module. It is need ONLY by FLUKA geometry (cm)
// it will be removed if a truncate pyramid body is implemented in FLUKA 
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.258752   Heightfront:  3.258752    DummyLength:    0.
Widthback :   4.462378	 Heightback :  4.462378	   Length:         12.150000
PositionZ:    -0.082500

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Calorimeter bounding box (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Width:          64.157267     Height:      28.650602      Thick:     30.281881, 
PositionZ:      -2.390941

CrystalN:  108
ModulesN:  12
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  1
PositionX:  2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       1.909682507744   TiltZ:       0.000000000000
CrystalId:  2
PositionX:  -2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       -1.909682507744   TiltZ:       0.000000000000
CrystalId:  3
PositionX:  0.000000000000   PositionY:   2.594252985282   PositionZ:   -0.044820883088
TiltX:      -1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  4
PositionX:  2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       1.908621475036   TiltZ:       -0.063661955951
CrystalId:  5
PositionX:  -2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       -1.908621475036   TiltZ:       0.063661955951
CrystalId:  6
PositionX:  0.000000000000   PositionY:   -2.594252985282   PositionZ:   -0.044820883088
TiltX:      1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  7
PositionX:  2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       1.908621475036   TiltZ:       0.063661955951
CrystalId:  8
PositionX:  -2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       -1.908621475036   TiltZ:       -0.063661955951
CrystalId:  9
PositionX:  -7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  10
PositionX:  -5.375452739044   PositionY:   0.000000000000   PositionZ:   -0.183983964085
TiltX:      0.000000000000   TiltY:       -3.819365015489   TiltZ:       0.000000000000
CrystalId:  11
PositionX:  -10.538042586474   PositionY:   0.000000000000   PositionZ:   -0.701922448649
TiltX:      0.000000000000   TiltY:       -7.638730030978   TiltZ:       0.000000000000
CrystalId:  12
PositionX:  -7.956747662759   PositionY:   2.594252985282   PositionZ:   -0.442953206367
TiltX:      -1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  13
PositionX:  -5.370981014996   PositionY:   2.592759370739   PositionZ:   -0.228556201638
TiltX:      -1.904373794316   TiltY:       -3.820422511741   TiltZ:       -0.063768344486
CrystalId:  14
PositionX:  -10.533570862425   PositionY:   2.592759370739   PositionZ:   -0.746494686201
TiltX:      -1.917154837490   TiltY:       -7.637665438087   TiltZ:       0.064196161297
CrystalId:  15
PositionX:  -7.956747662759   PositionY:   -2.594252985282   PositionZ:   -0.442953206367
TiltX:      1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  16
PositionX:  -5.370981014996   PositionY:   -2.592759370739   PositionZ:   -0.228556201638
TiltX:      1.904373794316   TiltY:       -3.820422511741   TiltZ:       0.063768344486
CrystalId:  17
PositionX:  -10.533570862425   PositionY:   -2.592759370739   PositionZ:   -0.746494686201
TiltX:      1.917154837490   TiltY:       -7.637665438087   TiltZ:       -0.064196161297
CrystalId:  18
PositionX:  7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  19
PositionX:  10.538042586474   PositionY:   0.000000000000   PositionZ:   -0.701922448649
TiltX:      0.000000000000   TiltY:       7.638730030978   TiltZ:       0.000000000000
CrystalId:  20
PositionX:  5.375452739044   PositionY:   0.000000000000   PositionZ:   -0.183983964085
TiltX:      0.000000000000   TiltY:       3.819365015489   TiltZ:       0.000000000000
CrystalId:  21
PositionX:  7.956747662759   PositionY:   2.594252985282   PositionZ:   -0.442953206367
TiltX:      -1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  22
PositionX:  10.533570862425   PositionY:   2.592759370739   PositionZ:   -0.746494686201
TiltX:      -1.917154837490   TiltY:       7.637665438087   TiltZ:       -0.064196161297
CrystalId:  23
PositionX:  5.370981014996   PositionY:   2.592759370739   PositionZ:   -0.228556201638
TiltX:      -1.904373794316   TiltY:       3.820422511741   TiltZ:       0.063768344486
CrystalId:  24
PositionX:  7.956747662759   PositionY:   -2.594252985282   PositionZ:   -0.442953206367
TiltX:      1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  25
PositionX:  10.533570862425   PositionY:   -2.592759370739   PositionZ:   -0.746494686201
TiltX:      1.917154837490   TiltY:       7.637665438087   TiltZ:       0.064196161297
CrystalId:  26
PositionX:  5.370981014996   PositionY:   -2.592759370739   PositionZ:   -0.228556201638
TiltX:      1.904373794316   TiltY:       3.820422511741   TiltZ:       -0.063768344486
CrystalId:  27
PositionX:  -15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  28
PositionX:  -13.290928167578   PositionY:   0.000000000000   PositionZ:   -1.133878863084
TiltX:      0.000000000000   TiltY:       -9.548412538722   TiltZ:       0.000000000000
CrystalId:  29
PositionX:  -18.376028543039   PositionY:   0.000000000000   PositionZ:   -2.164581719425
TiltX:      0.000000000000   TiltY:       -13.367777554211   TiltZ:       0.000000000000
CrystalId:  30
PositionX:  -15.833478355309   PositionY:   2.594252985282   PositionZ:   -1.649230291254
TiltX:      -1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  31
PositionX:  -13.282029391205   PositionY:   2.592759370739   PositionZ:   -1.177782079998
TiltX:      -1.897918721691   TiltY:       -9.549466450813   TiltZ:       -0.064520718033
CrystalId:  32
PositionX:  -18.367129766665   PositionY:   2.592759370739   PositionZ:   -2.208484936339
TiltX:      -1.923741599917   TiltY:       -13.366709304039   TiltZ:       0.065398252682
CrystalId:  33
PositionX:  -15.833478355309   PositionY:   -2.594252985282   PositionZ:   -1.649230291254
TiltX:      1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  34
PositionX:  -13.282029391205   PositionY:   -2.592759370739   PositionZ:   -1.177782079998
TiltX:      1.897918721691   TiltY:       -9.549466450813   TiltZ:       0.064520718033
CrystalId:  35
PositionX:  -18.367129766665   PositionY:   -2.592759370739   PositionZ:   -2.208484936339
TiltX:      1.923741599917   TiltY:       -13.366709304039   TiltZ:       -0.065398252682
CrystalId:  36
PositionX:  15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  37
PositionX:  18.376028543039   PositionY:   0.000000000000   PositionZ:   -2.164581719425
TiltX:      0.000000000000   TiltY:       13.367777554211   TiltZ:       0.000000000000
CrystalId:  38
PositionX:  13.290928167578   PositionY:   0.000000000000   PositionZ:   -1.133878863084
TiltX:      0.000000000000   TiltY:       9.548412538722   TiltZ:       0.000000000000
CrystalId:  39
PositionX:  15.833478355309   PositionY:   2.594252985282   PositionZ:   -1.649230291254
TiltX:      -1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  40
PositionX:  18.367129766665   PositionY:   2.592759370739   PositionZ:   -2.208484936339
TiltX:      -1.923741599917   TiltY:       13.366709304039   TiltZ:       -0.065398252682
CrystalId:  41
PositionX:  13.282029391205   PositionY:   2.592759370739   PositionZ:   -1.177782079998
TiltX:      -1.897918721691   TiltY:       9.549466450813   TiltZ:       0.064520718033
CrystalId:  42
PositionX:  15.833478355309   PositionY:   -2.594252985282   PositionZ:   -1.649230291254
TiltX:      1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  43
PositionX:  18.367129766665   PositionY:   -2.592759370739   PositionZ:   -2.208484936339
TiltX:      1.923741599917   TiltY:       13.366709304039   TiltZ:       0.065398252682
CrystalId:  44
PositionX:  13.282029391205   PositionY:   -2.592759370739   PositionZ:   -1.177782079998
TiltX:      1.897918721691   TiltY:       9.549466450813   TiltZ:       -0.064520718033
CrystalId:  45
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  46
PositionX:  26.032444833628   PositionY:   0.000000000000   PositionZ:   -4.394932823805
TiltX:      0.000000000000   TiltY:       19.096825077444   TiltZ:       0.000000000000
CrystalId:  47
PositionX:  21.075633173013   PositionY:   0.000000000000   PositionZ:   -2.861762132798
TiltX:      0.000000000000   TiltY:       15.277460061955   TiltZ:       0.000000000000
CrystalId:  48
PositionX:  23.554039003321   PositionY:   2.594252985282   PositionZ:   -3.628347478301
TiltX:      -1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  49
PositionX:  26.019207902114   PositionY:   2.592759370739   PositionZ:   -4.437728434800
TiltX:      -1.930650078844   TiltY:       19.095752991358   TiltZ:       -0.067331688610
CrystalId:  50
PositionX:  21.062396241499   PositionY:   2.592759370739   PositionZ:   -2.904557743793
TiltX:      -1.891242014235   TiltY:       15.278510266823   TiltZ:       0.065957830679
CrystalId:  51
PositionX:  23.554039003321   PositionY:   -2.594252985282   PositionZ:   -3.628347478301
TiltX:      1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  52
PositionX:  26.019207902114   PositionY:   -2.592759370739   PositionZ:   -4.437728434800
TiltX:      1.930650078844   TiltY:       19.095752991358   TiltZ:       0.067331688610
CrystalId:  53
PositionX:  21.062396241499   PositionY:   -2.592759370739   PositionZ:   -2.904557743793
TiltX:      1.891242014235   TiltY:       15.278510266823   TiltZ:       -0.065957830679
CrystalId:  54
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  55
PositionX:  2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       1.900140307152   TiltZ:       0.190702457215
CrystalId:  56
PositionX:  -2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       -1.900140307152   TiltZ:       -0.190702457215
CrystalId:  57
PositionX:  0.000000000000   PositionY:   -5.375452739044   PositionZ:   -0.183983964085
TiltX:      3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  58
PositionX:  2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       1.905439559434   TiltZ:       0.127253039168
CrystalId:  59
PositionX:  -2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       -1.905439559434   TiltZ:       -0.127253039168
CrystalId:  60
PositionX:  0.000000000000   PositionY:   -10.538042586474   PositionZ:   -0.701922448649
TiltX:      7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  61
PositionX:  2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       1.892729624041   TiltZ:       0.253939578138
CrystalId:  62
PositionX:  -2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       -1.892729624041   TiltZ:       -0.253939578138
CrystalId:  63
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  64
PositionX:  -5.335709496318   PositionY:   -7.956747662759   PositionZ:   -0.580127652029
TiltX:      5.713130743513   TiltY:       -3.828875481922   TiltZ:       0.191023974458
CrystalId:  65
PositionX:  -10.498299343747   PositionY:   -7.956747662759   PositionZ:   -1.098066136592
TiltX:      5.751416718004   TiltY:       -7.629155884274   TiltZ:       -0.192299819112
CrystalId:  66
PositionX:  -7.942855818808   PositionY:   -5.375452739044   PositionZ:   -0.581421180225
TiltX:      3.819365015489   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  67
PositionX:  -5.357096623229   PositionY:   -5.372467169068   PositionZ:   -0.366949895306
TiltX:      3.808749925517   TiltY:       -3.823593833643   TiltZ:       0.127466405094
CrystalId:  68
PositionX:  -10.519686470659   PositionY:   -5.372467169068   PositionZ:   -0.884888379870
TiltX:      3.834297718900   TiltY:       -7.634472857871   TiltZ:       -0.128320135143
CrystalId:  69
PositionX:  -7.891153021257   PositionY:   -10.538042586474   PositionZ:   -1.096772608396
TiltX:      7.638730030978   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  70
PositionX:  -5.305423593033   PositionY:   -10.532084708353   PositionZ:   -0.882004615178
TiltX:      7.617518624170   TiltY:       -3.836261628726   TiltZ:       0.254370998153
CrystalId:  71
PositionX:  -10.468013440462   PositionY:   -10.532084708353   PositionZ:   -1.399943099742
TiltX:      7.668499968183   TiltY:       -7.621720502450   TiltZ:       -0.256063280705
CrystalId:  72
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  73
PositionX:  10.498299343747   PositionY:   -7.956747662759   PositionZ:   -1.098066136592
TiltX:      5.751416718004   TiltY:       7.629155884274   TiltZ:       0.192299819112
CrystalId:  74
PositionX:  5.335709496318   PositionY:   -7.956747662759   PositionZ:   -0.580127652029
TiltX:      5.713130743513   TiltY:       3.828875481922   TiltZ:       -0.191023974458
CrystalId:  75
PositionX:  7.942855818808   PositionY:   -5.375452739044   PositionZ:   -0.581421180225
TiltX:      3.819365015489   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  76
PositionX:  10.519686470659   PositionY:   -5.372467169068   PositionZ:   -0.884888379870
TiltX:      3.834297718900   TiltY:       7.634472857871   TiltZ:       0.128320135143
CrystalId:  77
PositionX:  5.357096623229   PositionY:   -5.372467169068   PositionZ:   -0.366949895306
TiltX:      3.808749925517   TiltY:       3.823593833643   TiltZ:       -0.127466405094
CrystalId:  78
PositionX:  7.891153021257   PositionY:   -10.538042586474   PositionZ:   -1.096772608396
TiltX:      7.638730030978   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  79
PositionX:  10.468013440462   PositionY:   -10.532084708353   PositionZ:   -1.399943099742
TiltX:      7.668499968183   TiltY:       7.621720502450   TiltZ:       0.256063280705
CrystalId:  80
PositionX:  5.305423593033   PositionY:   -10.532084708353   PositionZ:   -0.882004615178
TiltX:      7.617518624170   TiltY:       3.836261628726   TiltZ:       -0.254370998153
CrystalId:  81
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  82
PositionX:  -13.211838710009   PositionY:   -7.956747662759   PositionZ:   -1.524076511131
TiltX:      5.693793529355   TiltY:       -9.557890841965   TiltZ:       0.193280663294
CrystalId:  83
PositionX:  -18.296939085470   PositionY:   -7.956747662759   PositionZ:   -2.554779367472
TiltX:      5.771146656188   TiltY:       -13.358170591105   TiltZ:       -0.195897664723
CrystalId:  84
PositionX:  -15.805833444443   PositionY:   -5.375452739044   PositionZ:   -1.785619887690
TiltX:      3.819365015489   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  85
PositionX:  -13.254399310262   PositionY:   -5.372467169068   PositionZ:   -1.314098511176
TiltX:      3.795846783025   TiltY:       -9.552627035954   TiltZ:       0.128971044992
CrystalId:  86
PositionX:  -18.339499685722   PositionY:   -5.372467169068   PositionZ:   -2.344801367517
TiltX:      3.847463653990   TiltY:       -13.363505768513   TiltZ:       -0.130722209293
CrystalId:  87
PositionX:  -15.702944351036   PositionY:   -10.538042586474   PositionZ:   -2.293235990912
TiltX:      7.638730030978   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  88
PositionX:  -13.151569454195   PositionY:   -10.532084708353   PositionZ:   -1.821422359633
TiltX:      7.591768317379   TiltY:       -9.565252119262   TiltZ:       0.257379402264
CrystalId:  89
PositionX:  -18.236669829655   PositionY:   -10.532084708353   PositionZ:   -2.852125215974
TiltX:      7.694771179972   TiltY:       -13.350709839262   TiltZ:       -0.260850595632
CrystalId:  90
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.769345587240   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  91
PositionX:  18.297103170364   PositionY:   -7.947737261980   PositionZ:   -2.553890272340
TiltX:      5.811668703700   TiltY:       13.338000358047   TiltZ:       0.400287832603
CrystalId:  92
PositionX:  13.211674625115   PositionY:   -7.965758063537   PositionZ:   -1.524965606262
TiltX:      5.733913227807   TiltY:       9.537989812649   TiltZ:       0.008409091651
CrystalId:  93
PositionX:  15.796957974614   PositionY:   -5.375468465176   PositionZ:   -1.783822521784
TiltX:      3.859663079495   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  94
PositionX:  18.330793410748   PositionY:   -5.363472485367   PositionZ:   -2.342115941293
TiltX:      3.888010154592   TiltY:       13.343380394944   TiltZ:       0.335121711523
CrystalId:  95
PositionX:  13.245364865499   PositionY:   -5.381493286925   PositionZ:   -1.313191275216
TiltX:      3.835943184502   TiltY:       9.532681614646   TiltZ:       0.072711197615
CrystalId:  96
PositionX:  15.711819820865   PositionY:   -10.538026860342   PositionZ:   -2.295033356818
TiltX:      7.679028094984   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  97
PositionX:  18.245704274417   PositionY:   -10.523058590497   PositionZ:   -2.853032451934
TiltX:      7.735268448088   TiltY:       13.330494926376   TiltZ:       0.465229896456
CrystalId:  98
PositionX:  13.160275729169   PositionY:   -10.541079392054   PositionZ:   -1.824107785857
TiltX:      7.631911436304   TiltY:       9.545395360800   TiltZ:       -0.055680661178
CrystalId:  99
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.790558250539   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  100
PositionX:  25.915051269266   PositionY:   -7.947737261980   PositionZ:   -4.774418152732
TiltX:      5.853946236937   TiltY:       19.066904181601   TiltZ:       0.412099384373
CrystalId:  101
PositionX:  20.957735571611   PositionY:   -7.965758063537   PositionZ:   -3.242984010799
TiltX:      5.734783366708   TiltY:       15.267037272912   TiltZ:       0.008596212304
CrystalId:  102
PositionX:  23.504265476225   PositionY:   -5.375468465176   PositionZ:   -3.758621814705
TiltX:      3.880875742794   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  103
PositionX:  25.969713434187   PositionY:   -5.363472485367   PositionZ:   -4.567064716991
TiltX:      3.923406226639   TiltY:       19.072327196915   TiltZ:       0.345013920223
CrystalId:  104
PositionX:  21.012397736532   PositionY:   -5.381493286925   PositionZ:   -3.035630575058
TiltX:      3.843466853718   TiltY:       15.261724429845   TiltZ:       0.074328457251
CrystalId:  105
PositionX:  23.368521364652   PositionY:   -10.538026860342   PositionZ:   -4.258780348826
TiltX:      7.700240758283   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  106
PositionX:  25.834047475893   PositionY:   -10.523058590497   PositionZ:   -5.066935289967
TiltX:      7.784402643565   TiltY:       19.059348343417   TiltZ:       0.478950861965
CrystalId:  107
PositionX:  20.876731778239   PositionY:   -10.541079392054   PositionZ:   -3.535501148034
TiltX:      7.626149623605   TiltY:       15.274440123100   TiltZ:       -0.056920449108

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   1
PositionX:  -7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   2
PositionX:  7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   3
PositionX:  -15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   4
PositionX:  15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   5
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   7
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   8
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   9
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   10
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.769345587240   TiltY:       11.438060203208   TiltZ:       0.203033341571
ModuleId:   11
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.790558250539   TiltY:       17.167070888243   TiltZ:       0.208280324993
