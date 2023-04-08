// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for FOOT Calorimeter
//
// Created with macro: Reconstruction/BuildCaGeoFile.C
//
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of Geometry 
// type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD, FIVE_MOD, 
//                   SEVEN_MOD, SEVEN_MOD_HIT22, FULL_DET_V1) 
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TypeGeo:        "FULL_DET_V1"

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
Width:          64.157267     Height:      64.253381      Thick:     30.309574, 
PositionZ:      -2.404787

CrystalN:  333
ModulesN:  37
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
PositionX:  -23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  46
PositionX:  -21.075633173013   PositionY:   0.000000000000   PositionZ:   -2.861762132798
TiltX:      0.000000000000   TiltY:       -15.277460061955   TiltZ:       0.000000000000
CrystalId:  47
PositionX:  -26.032444833628   PositionY:   0.000000000000   PositionZ:   -4.394932823805
TiltX:      0.000000000000   TiltY:       -19.096825077444   TiltZ:       0.000000000000
CrystalId:  48
PositionX:  -23.554039003321   PositionY:   2.594252985282   PositionZ:   -3.628347478301
TiltX:      -1.909682507744   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  49
PositionX:  -21.062396241499   PositionY:   2.592759370739   PositionZ:   -2.904557743793
TiltX:      -1.891242014235   TiltY:       -15.278510266823   TiltZ:       -0.065957830679
CrystalId:  50
PositionX:  -26.019207902114   PositionY:   2.592759370739   PositionZ:   -4.437728434800
TiltX:      -1.930650078844   TiltY:       -19.095752991358   TiltZ:       0.067331688610
CrystalId:  51
PositionX:  -23.554039003321   PositionY:   -2.594252985282   PositionZ:   -3.628347478301
TiltX:      1.909682507744   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  52
PositionX:  -21.062396241499   PositionY:   -2.592759370739   PositionZ:   -2.904557743793
TiltX:      1.891242014235   TiltY:       -15.278510266823   TiltZ:       0.065957830679
CrystalId:  53
PositionX:  -26.019207902114   PositionY:   -2.592759370739   PositionZ:   -4.437728434800
TiltX:      1.930650078844   TiltY:       -19.095752991358   TiltZ:       -0.067331688610
CrystalId:  54
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  55
PositionX:  26.032444833628   PositionY:   0.000000000000   PositionZ:   -4.394932823805
TiltX:      0.000000000000   TiltY:       19.096825077444   TiltZ:       0.000000000000
CrystalId:  56
PositionX:  21.075633173013   PositionY:   0.000000000000   PositionZ:   -2.861762132798
TiltX:      0.000000000000   TiltY:       15.277460061955   TiltZ:       0.000000000000
CrystalId:  57
PositionX:  23.554039003321   PositionY:   2.594252985282   PositionZ:   -3.628347478301
TiltX:      -1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  58
PositionX:  26.019207902114   PositionY:   2.592759370739   PositionZ:   -4.437728434800
TiltX:      -1.930650078844   TiltY:       19.095752991358   TiltZ:       -0.067331688610
CrystalId:  59
PositionX:  21.062396241499   PositionY:   2.592759370739   PositionZ:   -2.904557743793
TiltX:      -1.891242014235   TiltY:       15.278510266823   TiltZ:       0.065957830679
CrystalId:  60
PositionX:  23.554039003321   PositionY:   -2.594252985282   PositionZ:   -3.628347478301
TiltX:      1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  61
PositionX:  26.019207902114   PositionY:   -2.592759370739   PositionZ:   -4.437728434800
TiltX:      1.930650078844   TiltY:       19.095752991358   TiltZ:       0.067331688610
CrystalId:  62
PositionX:  21.062396241499   PositionY:   -2.592759370739   PositionZ:   -2.904557743793
TiltX:      1.891242014235   TiltY:       15.278510266823   TiltZ:       -0.065957830679
CrystalId:  63
PositionX:  0.000000000000   PositionY:   7.961221871788   PositionZ:   -0.398356199580
TiltX:      -5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  64
PositionX:  2.594252985282   PositionY:   7.956747662759   PositionZ:   -0.442953206367
TiltX:      -5.732210015987   TiltY:       1.900140307152   TiltZ:       -0.190702457215
CrystalId:  65
PositionX:  -2.594252985282   PositionY:   7.956747662759   PositionZ:   -0.442953206367
TiltX:      -5.732210015987   TiltY:       -1.900140307152   TiltZ:       0.190702457215
CrystalId:  66
PositionX:  0.000000000000   PositionY:   10.538042586474   PositionZ:   -0.701922448649
TiltX:      -7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  67
PositionX:  2.594252985282   PositionY:   10.532084708353   PositionZ:   -0.746345587317
TiltX:      -7.642924785518   TiltY:       1.892729624041   TiltZ:       -0.253939578138
CrystalId:  68
PositionX:  -2.594252985282   PositionY:   10.532084708353   PositionZ:   -0.746345587317
TiltX:      -7.642924785518   TiltY:       -1.892729624041   TiltZ:       0.253939578138
CrystalId:  69
PositionX:  0.000000000000   PositionY:   5.375452739044   PositionZ:   -0.183983964085
TiltX:      -3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  70
PositionX:  2.594252985282   PositionY:   5.372467169068   PositionZ:   -0.228705300522
TiltX:      -3.821481187277   TiltY:       1.905439559434   TiltZ:       -0.127253039168
CrystalId:  71
PositionX:  -2.594252985282   PositionY:   5.372467169068   PositionZ:   -0.228705300522
TiltX:      -3.821481187277   TiltY:       -1.905439559434   TiltZ:       0.127253039168
CrystalId:  72
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  73
PositionX:  2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       1.900140307152   TiltZ:       0.190702457215
CrystalId:  74
PositionX:  -2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       -1.900140307152   TiltZ:       -0.190702457215
CrystalId:  75
PositionX:  0.000000000000   PositionY:   -5.375452739044   PositionZ:   -0.183983964085
TiltX:      3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  76
PositionX:  2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       1.905439559434   TiltZ:       0.127253039168
CrystalId:  77
PositionX:  -2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       -1.905439559434   TiltZ:       -0.127253039168
CrystalId:  78
PositionX:  0.000000000000   PositionY:   -10.538042586474   PositionZ:   -0.701922448649
TiltX:      7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  79
PositionX:  2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       1.892729624041   TiltZ:       0.253939578138
CrystalId:  80
PositionX:  -2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       -1.892729624041   TiltZ:       -0.253939578138
CrystalId:  81
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  82
PositionX:  -5.335634984228   PositionY:   -7.965758063537   PositionZ:   -0.581028685854
TiltX:      5.733071938606   TiltY:       -3.808942350299   TiltZ:       -0.008311202461
CrystalId:  83
PositionX:  -10.498373855837   PositionY:   -7.947737261980   PositionZ:   -1.097165102767
TiltX:      5.771355635844   TiltY:       -7.609089857272   TiltZ:       -0.392950494226
CrystalId:  84
PositionX:  -7.933845260534   PositionY:   -5.375468465176   PositionZ:   -0.580518778722
TiltX:      3.839294448546   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  85
PositionX:  -5.348016740602   PositionY:   -5.381493286925   PositionZ:   -0.366949047176
TiltX:      3.828668825381   TiltY:       -3.803638643438   TiltZ:       -0.071865450970
CrystalId:  86
PositionX:  -10.510755612211   PositionY:   -5.363472485367   PositionZ:   -0.883085464089
TiltX:      3.854259478278   TiltY:       -7.614428910880   TiltZ:       -0.328975678625
CrystalId:  87
PositionX:  -7.900163579531   PositionY:   -10.538026860342   PositionZ:   -1.097675009899
TiltX:      7.658659464034   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  88
PositionX:  -5.314354451481   PositionY:   -10.541079392054   PositionZ:   -0.883807530959
TiltX:      7.637482065971   TiltY:       -3.816350506889   TiltZ:       0.055031767893
CrystalId:  89
PositionX:  -10.477093323090   PositionY:   -10.523058590497   PositionZ:   -1.399943947872
TiltX:      7.688415998562   TiltY:       -7.601632495079   TiltZ:       -0.456708355702
CrystalId:  90
PositionX:  -7.921456280788   PositionY:   7.961221871788   PositionZ:   -0.794722645583
TiltX:      -5.748976956290   TiltY:       -5.709048052411   TiltZ:       0.199993008557
CrystalId:  91
PositionX:  -5.335634984228   PositionY:   7.965758063537   PositionZ:   -0.581028685854
TiltX:      -5.733071938606   TiltY:       -3.808942350299   TiltZ:       0.008311202461
CrystalId:  92
PositionX:  -10.498373855837   PositionY:   7.947737261980   PositionZ:   -1.097165102767
TiltX:      -5.771355635844   TiltY:       -7.609089857272   TiltZ:       0.392950494226
CrystalId:  93
PositionX:  -7.900163579531   PositionY:   10.538026860342   PositionZ:   -1.097675009899
TiltX:      -7.658659464034   TiltY:       -5.709048052411   TiltZ:       0.199993008557
CrystalId:  94
PositionX:  -5.314354451481   PositionY:   10.541079392054   PositionZ:   -0.883807530959
TiltX:      -7.637482065971   TiltY:       -3.816350506889   TiltZ:       -0.055031767893
CrystalId:  95
PositionX:  -10.477093323090   PositionY:   10.523058590497   PositionZ:   -1.399943947872
TiltX:      -7.688415998562   TiltY:       -7.601632495079   TiltZ:       0.456708355702
CrystalId:  96
PositionX:  -7.933845260534   PositionY:   5.375468465176   PositionZ:   -0.580518778722
TiltX:      -3.839294448546   TiltY:       -5.709048052411   TiltZ:       0.199993008557
CrystalId:  97
PositionX:  -5.348016740602   PositionY:   5.381493286925   PositionZ:   -0.366949047176
TiltX:      -3.828668825381   TiltY:       -3.803638643438   TiltZ:       0.071865450970
CrystalId:  98
PositionX:  -10.510755612211   PositionY:   5.363472485367   PositionZ:   -0.883085464089
TiltX:      -3.854259478278   TiltY:       -7.614428910880   TiltZ:       0.328975678625
CrystalId:  99
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       5.709048052411   TiltZ:       0.199993008557
CrystalId:  100
PositionX:  10.498373855837   PositionY:   -7.947737261980   PositionZ:   -1.097165102767
TiltX:      5.771355635844   TiltY:       7.609089857272   TiltZ:       0.392950494226
CrystalId:  101
PositionX:  5.335634984228   PositionY:   -7.965758063537   PositionZ:   -0.581028685854
TiltX:      5.733071938606   TiltY:       3.808942350299   TiltZ:       0.008311202461
CrystalId:  102
PositionX:  7.933845260534   PositionY:   -5.375468465176   PositionZ:   -0.580518778722
TiltX:      3.839294448546   TiltY:       5.709048052411   TiltZ:       0.199993008557
CrystalId:  103
PositionX:  10.510755612211   PositionY:   -5.363472485367   PositionZ:   -0.883085464089
TiltX:      3.854259478278   TiltY:       7.614428910880   TiltZ:       0.328975678625
CrystalId:  104
PositionX:  5.348016740602   PositionY:   -5.381493286925   PositionZ:   -0.366949047176
TiltX:      3.828668825381   TiltY:       3.803638643438   TiltZ:       0.071865450970
CrystalId:  105
PositionX:  7.900163579531   PositionY:   -10.538026860342   PositionZ:   -1.097675009899
TiltX:      7.658659464034   TiltY:       5.709048052411   TiltZ:       0.199993008557
CrystalId:  106
PositionX:  10.477093323090   PositionY:   -10.523058590497   PositionZ:   -1.399943947872
TiltX:      7.688415998562   TiltY:       7.601632495079   TiltZ:       0.456708355702
CrystalId:  107
PositionX:  5.314354451481   PositionY:   -10.541079392054   PositionZ:   -0.883807530959
TiltX:      7.637482065971   TiltY:       3.816350506889   TiltZ:       -0.055031767893
CrystalId:  108
PositionX:  7.921456280788   PositionY:   7.961221871788   PositionZ:   -0.794722645583
TiltX:      -5.748976956290   TiltY:       5.709048052411   TiltZ:       -0.199993008557
CrystalId:  109
PositionX:  10.498373855837   PositionY:   7.947737261980   PositionZ:   -1.097165102767
TiltX:      -5.771355635844   TiltY:       7.609089857272   TiltZ:       -0.392950494226
CrystalId:  110
PositionX:  5.335634984228   PositionY:   7.965758063537   PositionZ:   -0.581028685854
TiltX:      -5.733071938606   TiltY:       3.808942350299   TiltZ:       -0.008311202461
CrystalId:  111
PositionX:  7.900163579531   PositionY:   10.538026860342   PositionZ:   -1.097675009899
TiltX:      -7.658659464034   TiltY:       5.709048052411   TiltZ:       -0.199993008557
CrystalId:  112
PositionX:  10.477093323090   PositionY:   10.523058590497   PositionZ:   -1.399943947872
TiltX:      -7.688415998562   TiltY:       7.601632495079   TiltZ:       -0.456708355702
CrystalId:  113
PositionX:  5.314354451481   PositionY:   10.541079392054   PositionZ:   -0.883807530959
TiltX:      -7.637482065971   TiltY:       3.816350506889   TiltZ:       0.055031767893
CrystalId:  114
PositionX:  7.933845260534   PositionY:   5.375468465176   PositionZ:   -0.580518778722
TiltX:      -3.839294448546   TiltY:       5.709048052411   TiltZ:       -0.199993008557
CrystalId:  115
PositionX:  10.510755612211   PositionY:   5.363472485367   PositionZ:   -0.883085464089
TiltX:      -3.854259478278   TiltY:       7.614428910880   TiltZ:       -0.328975678625
CrystalId:  116
PositionX:  5.348016740602   PositionY:   5.381493286925   PositionZ:   -0.366949047176
TiltX:      -3.828668825381   TiltY:       3.803638643438   TiltZ:       -0.071865450970
CrystalId:  117
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.809568111256   TiltY:       -11.417885536730   TiltZ:       -0.406037856431
CrystalId:  118
PositionX:  -13.211541522392   PositionY:   -7.974768354527   PositionZ:   -1.525860969955
TiltX:      5.773959298369   TiltY:       -9.517949644905   TiltZ:       -0.210072868524
CrystalId:  119
PositionX:  -18.297236273086   PositionY:   -7.938726970990   PositionZ:   -2.552994908647
TiltX:      5.852113161125   TiltY:       -13.317689616049   TiltZ:       -0.604646185390
CrystalId:  120
PositionX:  -15.788081986095   PositionY:   -5.375515643380   PositionZ:   -1.782028270348
TiltX:      3.899885603512   TiltY:       -11.417885536730   TiltZ:       -0.406037856431
CrystalId:  121
PositionX:  -13.236360884516   PositionY:   -5.390550728956   PositionZ:   -1.312293420496
TiltX:      3.875965801064   TiltY:       -9.512597139083   TiltZ:       -0.274367420113
CrystalId:  122
PositionX:  -18.322055635210   PositionY:   -5.354509345420   PositionZ:   -2.339427359187
TiltX:      3.928479228395   TiltY:       -13.323114423973   TiltZ:       -0.539489451961
CrystalId:  123
PositionX:  -15.720695809384   PositionY:   -10.537979682137   PositionZ:   -2.296827608254
TiltX:      7.719250619001   TiltY:       -11.417885536730   TiltZ:       -0.406037856431
CrystalId:  124
PositionX:  -13.169013504706   PositionY:   -10.550042532001   PositionZ:   -1.826796367963
TiltX:      7.671981082454   TiltY:       -9.525399379148   TiltZ:       -0.145992140925
CrystalId:  125
PositionX:  -18.254708255400   PositionY:   -10.514001148465   PositionZ:   -2.853930306654
TiltX:      7.775687965351   TiltY:       -13.310139593957   TiltZ:       -0.669577332454
CrystalId:  126
PositionX:  -15.763248145959   PositionY:   7.961221871788   PositionZ:   -1.995719739437
TiltX:      -5.809568111256   TiltY:       -11.417885536730   TiltZ:       0.406037856431
CrystalId:  127
PositionX:  -13.211541522392   PositionY:   7.974768354527   PositionZ:   -1.525860969955
TiltX:      -5.773959298369   TiltY:       -9.517949644905   TiltZ:       0.210072868524
CrystalId:  128
PositionX:  -18.297236273086   PositionY:   7.938726970990   PositionZ:   -2.552994908647
TiltX:      -5.852113161125   TiltY:       -13.317689616049   TiltZ:       0.604646185390
CrystalId:  129
PositionX:  -15.720695809384   PositionY:   10.537979682137   PositionZ:   -2.296827608254
TiltX:      -7.719250619001   TiltY:       -11.417885536730   TiltZ:       0.406037856431
CrystalId:  130
PositionX:  -13.169013504706   PositionY:   10.550042532001   PositionZ:   -1.826796367963
TiltX:      -7.671981082454   TiltY:       -9.525399379148   TiltZ:       0.145992140925
CrystalId:  131
PositionX:  -18.254708255400   PositionY:   10.514001148465   PositionZ:   -2.853930306654
TiltX:      -7.775687965351   TiltY:       -13.310139593957   TiltZ:       0.669577332454
CrystalId:  132
PositionX:  -15.788081986095   PositionY:   5.375515643380   PositionZ:   -1.782028270348
TiltX:      -3.899885603512   TiltY:       -11.417885536730   TiltZ:       0.406037856431
CrystalId:  133
PositionX:  -13.236360884516   PositionY:   5.390550728956   PositionZ:   -1.312293420496
TiltX:      -3.875965801064   TiltY:       -9.512597139083   TiltZ:       0.274367420113
CrystalId:  134
PositionX:  -18.322055635210   PositionY:   5.354509345420   PositionZ:   -2.339427359187
TiltX:      -3.928479228395   TiltY:       -13.323114423973   TiltZ:       0.539489451961
CrystalId:  135
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.809568111256   TiltY:       11.417885536730   TiltZ:       0.406037856431
CrystalId:  136
PositionX:  18.297236273086   PositionY:   -7.938726970990   PositionZ:   -2.552994908647
TiltX:      5.852113161125   TiltY:       13.317689616049   TiltZ:       0.604646185390
CrystalId:  137
PositionX:  13.211541522392   PositionY:   -7.974768354527   PositionZ:   -1.525860969955
TiltX:      5.773959298369   TiltY:       9.517949644905   TiltZ:       0.210072868524
CrystalId:  138
PositionX:  15.788081986095   PositionY:   -5.375515643380   PositionZ:   -1.782028270348
TiltX:      3.899885603512   TiltY:       11.417885536730   TiltZ:       0.406037856431
CrystalId:  139
PositionX:  18.322055635210   PositionY:   -5.354509345420   PositionZ:   -2.339427359187
TiltX:      3.928479228395   TiltY:       13.323114423973   TiltZ:       0.539489451961
CrystalId:  140
PositionX:  13.236360884516   PositionY:   -5.390550728956   PositionZ:   -1.312293420496
TiltX:      3.875965801064   TiltY:       9.512597139083   TiltZ:       0.274367420113
CrystalId:  141
PositionX:  15.720695809384   PositionY:   -10.537979682137   PositionZ:   -2.296827608254
TiltX:      7.719250619001   TiltY:       11.417885536730   TiltZ:       0.406037856431
CrystalId:  142
PositionX:  18.254708255400   PositionY:   -10.514001148465   PositionZ:   -2.853930306654
TiltX:      7.775687965351   TiltY:       13.310139593957   TiltZ:       0.669577332454
CrystalId:  143
PositionX:  13.169013504706   PositionY:   -10.550042532001   PositionZ:   -1.826796367963
TiltX:      7.671981082454   TiltY:       9.525399379148   TiltZ:       0.145992140925
CrystalId:  144
PositionX:  15.763248145959   PositionY:   7.961221871788   PositionZ:   -1.995719739437
TiltX:      -5.809568111256   TiltY:       11.417885536730   TiltZ:       -0.406037856431
CrystalId:  145
PositionX:  18.297236273086   PositionY:   7.938726970990   PositionZ:   -2.552994908647
TiltX:      -5.852113161125   TiltY:       13.317689616049   TiltZ:       -0.604646185390
CrystalId:  146
PositionX:  13.211541522392   PositionY:   7.974768354527   PositionZ:   -1.525860969955
TiltX:      -5.773959298369   TiltY:       9.517949644905   TiltZ:       -0.210072868524
CrystalId:  147
PositionX:  15.720695809384   PositionY:   10.537979682137   PositionZ:   -2.296827608254
TiltX:      -7.719250619001   TiltY:       11.417885536730   TiltZ:       -0.406037856431
CrystalId:  148
PositionX:  18.254708255400   PositionY:   10.514001148465   PositionZ:   -2.853930306654
TiltX:      -7.775687965351   TiltY:       13.310139593957   TiltZ:       -0.669577332454
CrystalId:  149
PositionX:  13.169013504706   PositionY:   10.550042532001   PositionZ:   -1.826796367963
TiltX:      -7.671981082454   TiltY:       9.525399379148   TiltZ:       -0.145992140925
CrystalId:  150
PositionX:  15.788081986095   PositionY:   5.375515643380   PositionZ:   -1.782028270348
TiltX:      -3.899885603512   TiltY:       11.417885536730   TiltZ:       -0.406037856431
CrystalId:  151
PositionX:  18.322055635210   PositionY:   5.354509345420   PositionZ:   -2.339427359187
TiltX:      -3.928479228395   TiltY:       13.323114423973   TiltZ:       -0.539489451961
CrystalId:  152
PositionX:  13.236360884516   PositionY:   5.390550728956   PositionZ:   -1.312293420496
TiltX:      -3.875965801064   TiltY:       9.512597139083   TiltZ:       -0.274367420113
CrystalId:  153
PositionX:  -23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.913328510779   TiltY:       -17.126287421919   TiltZ:       -0.624704617965
CrystalId:  154
PositionX:  -20.957322142266   PositionY:   -7.983778425940   PositionZ:   -3.244748539242
TiltX:      5.856524854765   TiltY:       -15.226667192731   TiltZ:       -0.420840866967
CrystalId:  155
PositionX:  -25.915464698611   PositionY:   -7.929716899577   PositionZ:   -4.772653624288
TiltX:      5.977898118758   TiltY:       -19.025699302508   TiltZ:       -0.832778166639
CrystalId:  156
PositionX:  -23.486959666599   PositionY:   -5.375594273082   PositionZ:   -3.753282238773
TiltX:      4.003646003035   TiltY:       -17.126287421919   TiltZ:       -0.624704617965
CrystalId:  157
PositionX:  -20.994688461203   PositionY:   -5.399639384801   PositionZ:   -3.032058601776
TiltX:      3.965157152509   TiltY:       -15.221219650144   TiltZ:       -0.486548625343
CrystalId:  158
PositionX:  -25.952831017548   PositionY:   -5.345577858438   PositionZ:   -4.559963686822
TiltX:      4.047413155665   TiltY:       -19.031259501637   TiltZ:       -0.765721900949
CrystalId:  159
PositionX:  -23.385827174278   PositionY:   -10.537901052435   PositionZ:   -4.264119924757
TiltX:      7.823011018523   TiltY:       -17.126287421919   TiltZ:       -0.624704617965
CrystalId:  160
PositionX:  -20.893614194878   PositionY:   -10.558974018983   PositionZ:   -3.542602178203
TiltX:      7.747943197257   TiltY:       -15.234204348787   TiltZ:       -0.355353222733
CrystalId:  161
PositionX:  -25.851756751222   PositionY:   -10.504912492620   PositionZ:   -5.070507263249
TiltX:      7.908297902155   TiltY:       -19.018006820235   TiltZ:       -0.899595582093
CrystalId:  162
PositionX:  -23.449571553817   PositionY:   7.961221871788   PositionZ:   -3.966095567861
TiltX:      -5.913328510779   TiltY:       -17.126287421919   TiltZ:       0.624704617965
CrystalId:  163
PositionX:  -20.957322142266   PositionY:   7.983778425940   PositionZ:   -3.244748539242
TiltX:      -5.856524854765   TiltY:       -15.226667192731   TiltZ:       0.420840866967
CrystalId:  164
PositionX:  -25.915464698611   PositionY:   7.929716899577   PositionZ:   -4.772653624288
TiltX:      -5.977898118758   TiltY:       -19.025699302508   TiltZ:       0.832778166639
CrystalId:  165
PositionX:  -23.385827174278   PositionY:   10.537901052435   PositionZ:   -4.264119924757
TiltX:      -7.823011018523   TiltY:       -17.126287421919   TiltZ:       0.624704617965
CrystalId:  166
PositionX:  -20.893614194878   PositionY:   10.558974018983   PositionZ:   -3.542602178203
TiltX:      -7.747943197257   TiltY:       -15.234204348787   TiltZ:       0.355353222733
CrystalId:  167
PositionX:  -25.851756751222   PositionY:   10.504912492620   PositionZ:   -5.070507263249
TiltX:      -7.908297902155   TiltY:       -19.018006820235   TiltZ:       0.899595582093
CrystalId:  168
PositionX:  -23.486959666599   PositionY:   5.375594273082   PositionZ:   -3.753282238773
TiltX:      -4.003646003035   TiltY:       -17.126287421919   TiltZ:       0.624704617965
CrystalId:  169
PositionX:  -20.994688461203   PositionY:   5.399639384801   PositionZ:   -3.032058601776
TiltX:      -3.965157152509   TiltY:       -15.221219650144   TiltZ:       0.486548625343
CrystalId:  170
PositionX:  -25.952831017548   PositionY:   5.345577858438   PositionZ:   -4.559963686822
TiltX:      -4.047413155665   TiltY:       -19.031259501637   TiltZ:       0.765721900949
CrystalId:  171
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.913328510779   TiltY:       17.126287421919   TiltZ:       0.624704617965
CrystalId:  172
PositionX:  25.915464698611   PositionY:   -7.929716899577   PositionZ:   -4.772653624288
TiltX:      5.977898118758   TiltY:       19.025699302508   TiltZ:       0.832778166639
CrystalId:  173
PositionX:  20.957322142266   PositionY:   -7.983778425940   PositionZ:   -3.244748539242
TiltX:      5.856524854765   TiltY:       15.226667192731   TiltZ:       0.420840866967
CrystalId:  174
PositionX:  23.486959666599   PositionY:   -5.375594273082   PositionZ:   -3.753282238773
TiltX:      4.003646003035   TiltY:       17.126287421919   TiltZ:       0.624704617965
CrystalId:  175
PositionX:  25.952831017548   PositionY:   -5.345577858438   PositionZ:   -4.559963686822
TiltX:      4.047413155665   TiltY:       19.031259501637   TiltZ:       0.765721900949
CrystalId:  176
PositionX:  20.994688461203   PositionY:   -5.399639384801   PositionZ:   -3.032058601776
TiltX:      3.965157152509   TiltY:       15.221219650144   TiltZ:       0.486548625343
CrystalId:  177
PositionX:  23.385827174278   PositionY:   -10.537901052435   PositionZ:   -4.264119924757
TiltX:      7.823011018523   TiltY:       17.126287421919   TiltZ:       0.624704617965
CrystalId:  178
PositionX:  25.851756751222   PositionY:   -10.504912492620   PositionZ:   -5.070507263249
TiltX:      7.908297902155   TiltY:       19.018006820235   TiltZ:       0.899595582093
CrystalId:  179
PositionX:  20.893614194878   PositionY:   -10.558974018983   PositionZ:   -3.542602178203
TiltX:      7.747943197257   TiltY:       15.234204348787   TiltZ:       0.355353222733
CrystalId:  180
PositionX:  23.449571553817   PositionY:   7.961221871788   PositionZ:   -3.966095567861
TiltX:      -5.913328510779   TiltY:       17.126287421919   TiltZ:       -0.624704617965
CrystalId:  181
PositionX:  25.915464698611   PositionY:   7.929716899577   PositionZ:   -4.772653624288
TiltX:      -5.977898118758   TiltY:       19.025699302508   TiltZ:       -0.832778166639
CrystalId:  182
PositionX:  20.957322142266   PositionY:   7.983778425940   PositionZ:   -3.244748539242
TiltX:      -5.856524854765   TiltY:       15.226667192731   TiltZ:       -0.420840866967
CrystalId:  183
PositionX:  23.385827174278   PositionY:   10.537901052435   PositionZ:   -4.264119924757
TiltX:      -7.823011018523   TiltY:       17.126287421919   TiltZ:       -0.624704617965
CrystalId:  184
PositionX:  25.851756751222   PositionY:   10.504912492620   PositionZ:   -5.070507263249
TiltX:      -7.908297902155   TiltY:       19.018006820235   TiltZ:       -0.899595582093
CrystalId:  185
PositionX:  20.893614194878   PositionY:   10.558974018983   PositionZ:   -3.542602178203
TiltX:      -7.747943197257   TiltY:       15.234204348787   TiltZ:       -0.355353222733
CrystalId:  186
PositionX:  23.486959666599   PositionY:   5.375594273082   PositionZ:   -3.753282238773
TiltX:      -4.003646003035   TiltY:       17.126287421919   TiltZ:       -0.624704617965
CrystalId:  187
PositionX:  25.952831017548   PositionY:   5.345577858438   PositionZ:   -4.559963686822
TiltX:      -4.047413155665   TiltY:       19.031259501637   TiltZ:       -0.765721900949
CrystalId:  188
PositionX:  20.994688461203   PositionY:   5.399639384801   PositionZ:   -3.032058601776
TiltX:      -3.965157152509   TiltY:       15.221219650144   TiltZ:       -0.486548625343
CrystalId:  189
PositionX:  0.000000000000   PositionY:   15.842382076820   PositionZ:   -1.605302676887
TiltX:      -11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  190
PositionX:  2.594252985282   PositionY:   15.833478355309   PositionZ:   -1.649230291254
TiltX:      -11.464293875005   TiltY:       1.871609343693   TiltZ:       -0.379495682798
CrystalId:  191
PositionX:  -2.594252985282   PositionY:   15.833478355309   PositionZ:   -1.649230291254
TiltX:      -11.464293875005   TiltY:       -1.871609343693   TiltZ:       0.379495682798
CrystalId:  192
PositionX:  0.000000000000   PositionY:   18.376028543039   PositionZ:   -2.164581719425
TiltX:      -13.367777554211   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  193
PositionX:  2.594252985282   PositionY:   18.365665919477   PositionZ:   -2.208188228041
TiltX:      -13.374939286763   TiltY:       1.857923282113   TiltZ:       -0.441674923206
CrystalId:  194
PositionX:  -2.594252985282   PositionY:   18.365665919477   PositionZ:   -2.208188228041
TiltX:      -13.374939286763   TiltY:       -1.857923282113   TiltZ:       0.441674923206
CrystalId:  195
PositionX:  0.000000000000   PositionY:   13.290928167578   PositionZ:   -1.133878863084
TiltX:      -9.548412538722   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  196
PositionX:  2.594252985282   PositionY:   13.283493238393   PositionZ:   -1.178078788297
TiltX:      -9.553620907086   TiltY:       1.883215768831   TiltZ:       -0.316894010451
CrystalId:  197
PositionX:  -2.594252985282   PositionY:   13.283493238393   PositionZ:   -1.178078788297
TiltX:      -9.553620907086   TiltY:       -1.883215768831   TiltZ:       0.316894010451
CrystalId:  198
PositionX:  0.000000000000   PositionY:   -15.842382076820   PositionZ:   -1.605302676887
TiltX:      11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  199
PositionX:  2.594252985282   PositionY:   -15.833478355309   PositionZ:   -1.649230291254
TiltX:      11.464293875005   TiltY:       1.871609343693   TiltZ:       0.379495682798
CrystalId:  200
PositionX:  -2.594252985282   PositionY:   -15.833478355309   PositionZ:   -1.649230291254
TiltX:      11.464293875005   TiltY:       -1.871609343693   TiltZ:       -0.379495682798
CrystalId:  201
PositionX:  0.000000000000   PositionY:   -13.290928167578   PositionZ:   -1.133878863084
TiltX:      9.548412538722   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  202
PositionX:  2.594252985282   PositionY:   -13.283493238393   PositionZ:   -1.178078788297
TiltX:      9.553620907086   TiltY:       1.883215768831   TiltZ:       0.316894010451
CrystalId:  203
PositionX:  -2.594252985282   PositionY:   -13.283493238393   PositionZ:   -1.178078788297
TiltX:      9.553620907086   TiltY:       -1.883215768831   TiltZ:       -0.316894010451
CrystalId:  204
PositionX:  0.000000000000   PositionY:   -18.376028543039   PositionZ:   -2.164581719425
TiltX:      13.367777554211   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  205
PositionX:  2.594252985282   PositionY:   -18.365665919477   PositionZ:   -2.208188228041
TiltX:      13.374939286763   TiltY:       1.857923282113   TiltZ:       0.441674923206
CrystalId:  206
PositionX:  -2.594252985282   PositionY:   -18.365665919477   PositionZ:   -2.208188228041
TiltX:      13.374939286763   TiltY:       -1.857923282113   TiltZ:       -0.441674923206
CrystalId:  207
PositionX:  -7.900022178211   PositionY:   -15.846728891557   PositionZ:   -1.962815881555
TiltX:      11.526072551341   TiltY:       -5.589583053120   TiltZ:       -0.689326323217
CrystalId:  208
PositionX:  -5.313906360153   PositionY:   -15.868887507813   PositionZ:   -1.753838766727
TiltX:      11.495048044406   TiltY:       -3.718301224230   TiltZ:       -0.307008365005
CrystalId:  209
PositionX:  -10.477367917889   PositionY:   -15.806762832279   PositionZ:   -2.259209396110
TiltX:      11.569803271881   TiltY:       -7.460615996234   TiltZ:       -1.074096854538
CrystalId:  210
PositionX:  -7.915542164161   PositionY:   -13.295464733700   PositionZ:   -1.490621228820
TiltX:      9.616390043596   TiltY:       -5.589583053120   TiltZ:       -0.689326323217
CrystalId:  211
PositionX:  -5.329417321446   PositionY:   -13.319092033034   PositionZ:   -1.281916864178
TiltX:      9.590485316461   TiltY:       -3.706657880726   TiltZ:       -0.369701132024
CrystalId:  212
PositionX:  -10.492878879183   PositionY:   -13.256967357500   PositionZ:   -1.787287493561
TiltX:      9.652983938780   TiltY:       -7.472334265074   TiltZ:       -1.011014767776
CrystalId:  213
PositionX:  -7.875732113880   PositionY:   -18.380185606392   PositionZ:   -2.522426934017
TiltX:      13.435755059085   TiltY:       -5.589583053120   TiltZ:       -0.689326323217
CrystalId:  214
PositionX:  -5.289630191393   PositionY:   -18.400885429846   PositionZ:   -2.313128517824
TiltX:      13.399624313609   TiltY:       -3.732017350493   TiltZ:       -0.244737114688
CrystalId:  215
PositionX:  -10.453091749130   PositionY:   -18.338760754311   PositionZ:   -2.818499147207
TiltX:      13.486552577323   TiltY:       -7.446812207346   TiltZ:       -1.136746513317
CrystalId:  216
PositionX:  -7.900022178211   PositionY:   15.846728891557   PositionZ:   -1.962815881555
TiltX:      -11.526072551341   TiltY:       -5.589583053120   TiltZ:       0.689326323217
CrystalId:  217
PositionX:  -5.313906360153   PositionY:   15.868887507813   PositionZ:   -1.753838766727
TiltX:      -11.495048044406   TiltY:       -3.718301224230   TiltZ:       0.307008365005
CrystalId:  218
PositionX:  -10.477367917889   PositionY:   15.806762832279   PositionZ:   -2.259209396110
TiltX:      -11.569803271881   TiltY:       -7.460615996234   TiltZ:       1.074096854538
CrystalId:  219
PositionX:  -7.875732113880   PositionY:   18.380185606392   PositionZ:   -2.522426934017
TiltX:      -13.435755059085   TiltY:       -5.589583053120   TiltZ:       0.689326323217
CrystalId:  220
PositionX:  -5.289630191393   PositionY:   18.400885429846   PositionZ:   -2.313128517824
TiltX:      -13.399624313609   TiltY:       -3.732017350493   TiltZ:       0.244737114688
CrystalId:  221
PositionX:  -10.453091749130   PositionY:   18.338760754311   PositionZ:   -2.818499147207
TiltX:      -13.486552577323   TiltY:       -7.446812207346   TiltZ:       1.136746513317
CrystalId:  222
PositionX:  -7.915542164161   PositionY:   13.295464733700   PositionZ:   -1.490621228820
TiltX:      -9.616390043596   TiltY:       -5.589583053120   TiltZ:       0.689326323217
CrystalId:  223
PositionX:  -5.329417321446   PositionY:   13.319092033034   PositionZ:   -1.281916864178
TiltX:      -9.590485316461   TiltY:       -3.706657880726   TiltZ:       0.369701132024
CrystalId:  224
PositionX:  -10.492878879183   PositionY:   13.256967357500   PositionZ:   -1.787287493561
TiltX:      -9.652983938780   TiltY:       -7.472334265074   TiltZ:       1.011014767776
CrystalId:  225
PositionX:  7.900022178211   PositionY:   -15.846728891557   PositionZ:   -1.962815881555
TiltX:      11.526072551341   TiltY:       5.589583053120   TiltZ:       0.689326323217
CrystalId:  226
PositionX:  10.477367917889   PositionY:   -15.806762832279   PositionZ:   -2.259209396110
TiltX:      11.569803271881   TiltY:       7.460615996234   TiltZ:       1.074096854538
CrystalId:  227
PositionX:  5.313906360153   PositionY:   -15.868887507813   PositionZ:   -1.753838766727
TiltX:      11.495048044406   TiltY:       3.718301224230   TiltZ:       0.307008365005
CrystalId:  228
PositionX:  7.915542164161   PositionY:   -13.295464733700   PositionZ:   -1.490621228820
TiltX:      9.616390043596   TiltY:       5.589583053120   TiltZ:       0.689326323217
CrystalId:  229
PositionX:  10.492878879183   PositionY:   -13.256967357500   PositionZ:   -1.787287493561
TiltX:      9.652983938780   TiltY:       7.472334265074   TiltZ:       1.011014767776
CrystalId:  230
PositionX:  5.329417321446   PositionY:   -13.319092033034   PositionZ:   -1.281916864178
TiltX:      9.590485316461   TiltY:       3.706657880726   TiltZ:       0.369701132024
CrystalId:  231
PositionX:  7.875732113880   PositionY:   -18.380185606392   PositionZ:   -2.522426934017
TiltX:      13.435755059085   TiltY:       5.589583053120   TiltZ:       0.689326323217
CrystalId:  232
PositionX:  10.453091749130   PositionY:   -18.338760754311   PositionZ:   -2.818499147207
TiltX:      13.486552577323   TiltY:       7.446812207346   TiltZ:       1.136746513317
CrystalId:  233
PositionX:  5.289630191393   PositionY:   -18.400885429846   PositionZ:   -2.313128517824
TiltX:      13.399624313609   TiltY:       3.732017350493   TiltZ:       0.244737114688
CrystalId:  234
PositionX:  7.900022178211   PositionY:   15.846728891557   PositionZ:   -1.962815881555
TiltX:      -11.526072551341   TiltY:       5.589583053120   TiltZ:       -0.689326323217
CrystalId:  235
PositionX:  10.477367917889   PositionY:   15.806762832279   PositionZ:   -2.259209396110
TiltX:      -11.569803271881   TiltY:       7.460615996234   TiltZ:       -1.074096854538
CrystalId:  236
PositionX:  5.313906360153   PositionY:   15.868887507813   PositionZ:   -1.753838766727
TiltX:      -11.495048044406   TiltY:       3.718301224230   TiltZ:       -0.307008365005
CrystalId:  237
PositionX:  7.875732113880   PositionY:   18.380185606392   PositionZ:   -2.522426934017
TiltX:      -13.435755059085   TiltY:       5.589583053120   TiltZ:       -0.689326323217
CrystalId:  238
PositionX:  10.453091749130   PositionY:   18.338760754311   PositionZ:   -2.818499147207
TiltX:      -13.486552577323   TiltY:       7.446812207346   TiltZ:       -1.136746513317
CrystalId:  239
PositionX:  5.289630191393   PositionY:   18.400885429846   PositionZ:   -2.313128517824
TiltX:      -13.399624313609   TiltY:       3.732017350493   TiltZ:       -0.244737114688
CrystalId:  240
PositionX:  7.915542164161   PositionY:   13.295464733700   PositionZ:   -1.490621228820
TiltX:      -9.616390043596   TiltY:       5.589583053120   TiltZ:       -0.689326323217
CrystalId:  241
PositionX:  10.492878879183   PositionY:   13.256967357500   PositionZ:   -1.787287493561
TiltX:      -9.652983938780   TiltY:       7.472334265074   TiltZ:       -1.011014767776
CrystalId:  242
PositionX:  5.329417321446   PositionY:   13.319092033034   PositionZ:   -1.281916864178
TiltX:      -9.590485316461   TiltY:       3.706657880726   TiltZ:       -0.369701132024
CrystalId:  243
PositionX:  -15.719045654212   PositionY:   -15.846728891557   PositionZ:   -3.153859750286
TiltX:      11.636914633970   TiltY:       -11.373413093180   TiltZ:       -0.899731425766
CrystalId:  244
PositionX:  -13.167252132312   PositionY:   -15.877761812626   PositionZ:   -2.685304265200
TiltX:      11.566158757248   TiltY:       -9.502738313557   TiltZ:       -0.509241188389
CrystalId:  245
PositionX:  -18.253243161294   PositionY:   -15.797888527466   PositionZ:   -3.708490327464
TiltX:      11.721269861690   TiltY:       -13.243566556536   TiltZ:       -1.295385754777
CrystalId:  246
PositionX:  -15.773528568263   PositionY:   -13.295588650423   PositionZ:   -2.683888958274
TiltX:      9.727232126226   TiltY:       -11.373413093180   TiltZ:       -0.899731425766
CrystalId:  247
PositionX:  -13.221703499446   PositionY:   -13.328090183226   PositionZ:   -2.215604929368
TiltX:      9.667999808835   TiltY:       -9.491013837957   TiltZ:       -0.572656373667
CrystalId:  248
PositionX:  -18.307694528428   PositionY:   -13.248216898066   PositionZ:   -3.238790991632
TiltX:      9.797927636349   TiltY:       -13.255446574945   TiltZ:       -1.231159431714
CrystalId:  249
PositionX:  -15.646966725343   PositionY:   -18.380061689669   PositionZ:   -3.709905634390
TiltX:      13.546597141714   TiltY:       -11.373413093180   TiltZ:       -0.899731425766
CrystalId:  250
PositionX:  -13.095214523209   PositionY:   -18.409635889279   PositionZ:   -3.241030886908
TiltX:      13.464374194305   TiltY:       -9.516528103482   TiltZ:       -0.446252206436
CrystalId:  251
PositionX:  -18.181205552191   PositionY:   -18.329762604119   PositionZ:   -4.264216949172
TiltX:      13.644495133255   TiltY:       -13.229595086881   TiltZ:       -1.359162897817
CrystalId:  252
PositionX:  -15.719045654212   PositionY:   15.846728891557   PositionZ:   -3.153859750286
TiltX:      -11.636914633970   TiltY:       -11.373413093180   TiltZ:       0.899731425766
CrystalId:  253
PositionX:  -13.167252132312   PositionY:   15.877761812626   PositionZ:   -2.685304265200
TiltX:      -11.566158757248   TiltY:       -9.502738313557   TiltZ:       0.509241188389
CrystalId:  254
PositionX:  -18.253243161294   PositionY:   15.797888527466   PositionZ:   -3.708490327464
TiltX:      -11.721269861690   TiltY:       -13.243566556536   TiltZ:       1.295385754777
CrystalId:  255
PositionX:  -15.646966725343   PositionY:   18.380061689669   PositionZ:   -3.709905634390
TiltX:      -13.546597141714   TiltY:       -11.373413093180   TiltZ:       0.899731425766
CrystalId:  256
PositionX:  -13.095214523209   PositionY:   18.409635889279   PositionZ:   -3.241030886908
TiltX:      -13.464374194305   TiltY:       -9.516528103482   TiltZ:       0.446252206436
CrystalId:  257
PositionX:  -18.181205552191   PositionY:   18.329762604119   PositionZ:   -4.264216949172
TiltX:      -13.644495133255   TiltY:       -13.229595086881   TiltZ:       1.359162897817
CrystalId:  258
PositionX:  -15.773528568263   PositionY:   13.295588650423   PositionZ:   -2.683888958274
TiltX:      -9.727232126226   TiltY:       -11.373413093180   TiltZ:       0.899731425766
CrystalId:  259
PositionX:  -13.221703499446   PositionY:   13.328090183226   PositionZ:   -2.215604929368
TiltX:      -9.667999808835   TiltY:       -9.491013837957   TiltZ:       0.572656373667
CrystalId:  260
PositionX:  -18.307694528428   PositionY:   13.248216898066   PositionZ:   -3.238790991632
TiltX:      -9.797927636349   TiltY:       -13.255446574945   TiltZ:       1.231159431714
CrystalId:  261
PositionX:  15.719045654212   PositionY:   -15.846728891557   PositionZ:   -3.153859750286
TiltX:      11.636914633970   TiltY:       11.373413093180   TiltZ:       0.899731425766
CrystalId:  262
PositionX:  18.253243161294   PositionY:   -15.797888527466   PositionZ:   -3.708490327464
TiltX:      11.721269861690   TiltY:       13.243566556536   TiltZ:       1.295385754777
CrystalId:  263
PositionX:  13.167252132312   PositionY:   -15.877761812626   PositionZ:   -2.685304265200
TiltX:      11.566158757248   TiltY:       9.502738313557   TiltZ:       0.509241188389
CrystalId:  264
PositionX:  15.773528568263   PositionY:   -13.295588650423   PositionZ:   -2.683888958274
TiltX:      9.727232126226   TiltY:       11.373413093180   TiltZ:       0.899731425766
CrystalId:  265
PositionX:  18.307694528428   PositionY:   -13.248216898066   PositionZ:   -3.238790991632
TiltX:      9.797927636349   TiltY:       13.255446574945   TiltZ:       1.231159431714
CrystalId:  266
PositionX:  13.221703499446   PositionY:   -13.328090183226   PositionZ:   -2.215604929368
TiltX:      9.667999808835   TiltY:       9.491013837957   TiltZ:       0.572656373667
CrystalId:  267
PositionX:  15.646966725343   PositionY:   -18.380061689669   PositionZ:   -3.709905634390
TiltX:      13.546597141714   TiltY:       11.373413093180   TiltZ:       0.899731425766
CrystalId:  268
PositionX:  18.181205552191   PositionY:   -18.329762604119   PositionZ:   -4.264216949172
TiltX:      13.644495133255   TiltY:       13.229595086881   TiltZ:       1.359162897817
CrystalId:  269
PositionX:  13.095214523209   PositionY:   -18.409635889279   PositionZ:   -3.241030886908
TiltX:      13.464374194305   TiltY:       9.516528103482   TiltZ:       0.446252206436
CrystalId:  270
PositionX:  15.719045654212   PositionY:   15.846728891557   PositionZ:   -3.153859750286
TiltX:      -11.636914633970   TiltY:       11.373413093180   TiltZ:       -0.899731425766
CrystalId:  271
PositionX:  18.253243161294   PositionY:   15.797888527466   PositionZ:   -3.708490327464
TiltX:      -11.721269861690   TiltY:       13.243566556536   TiltZ:       -1.295385754777
CrystalId:  272
PositionX:  13.167252132312   PositionY:   15.877761812626   PositionZ:   -2.685304265200
TiltX:      -11.566158757248   TiltY:       9.502738313557   TiltZ:       -0.509241188389
CrystalId:  273
PositionX:  15.646966725343   PositionY:   18.380061689669   PositionZ:   -3.709905634390
TiltX:      -13.546597141714   TiltY:       11.373413093180   TiltZ:       -0.899731425766
CrystalId:  274
PositionX:  18.181205552191   PositionY:   18.329762604119   PositionZ:   -4.264216949172
TiltX:      -13.644495133255   TiltY:       13.229595086881   TiltZ:       -1.359162897817
CrystalId:  275
PositionX:  13.095214523209   PositionY:   18.409635889279   PositionZ:   -3.241030886908
TiltX:      -13.464374194305   TiltY:       9.516528103482   TiltZ:       -0.446252206436
CrystalId:  276
PositionX:  15.773528568263   PositionY:   13.295588650423   PositionZ:   -2.683888958274
TiltX:      -9.727232126226   TiltY:       11.373413093180   TiltZ:       -0.899731425766
CrystalId:  277
PositionX:  18.307694528428   PositionY:   13.248216898066   PositionZ:   -3.238790991632
TiltX:      -9.797927636349   TiltY:       13.255446574945   TiltZ:       -1.231159431714
CrystalId:  278
PositionX:  13.221703499446   PositionY:   13.328090183226   PositionZ:   -2.215604929368
TiltX:      -9.667999808835   TiltY:       9.491013837957   TiltZ:       -0.572656373667
CrystalId:  279
PositionX:  0.000000000000   PositionY:   23.567283290729   PositionZ:   -3.585528085360
TiltX:      -17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  280
PositionX:  2.594252985282   PositionY:   23.554039003321   PositionZ:   -3.628347478301
TiltX:      -17.196130464691   TiltY:       1.824375548871   TiltZ:       -0.564489888993
CrystalId:  281
PositionX:  -2.594252985282   PositionY:   23.554039003321   PositionZ:   -3.628347478301
TiltX:      -17.196130464691   TiltY:       -1.824375548871   TiltZ:       0.564489888993
CrystalId:  282
PositionX:  0.000000000000   PositionY:   26.032444833628   PositionZ:   -4.394932823805
TiltX:      -19.096825077444   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  283
PositionX:  2.594252985282   PositionY:   26.017780985382   PositionZ:   -4.437287081151
TiltX:      -19.106668115313   TiltY:       1.804551254619   TiltZ:       -0.624988972712
CrystalId:  284
PositionX:  -2.594252985282   PositionY:   26.017780985382   PositionZ:   -4.437287081151
TiltX:      -19.106668115313   TiltY:       -1.804551254619   TiltZ:       0.624988972712
CrystalId:  285
PositionX:  0.000000000000   PositionY:   21.075633173013   PositionZ:   -2.861762132798
TiltX:      -15.277460061955   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  286
PositionX:  2.594252985282   PositionY:   21.063823158231   PositionZ:   -2.904999097442
TiltX:      -15.285552862625   TiltY:       1.842172834120   TiltZ:       -0.503362537817
CrystalId:  287
PositionX:  -2.594252985282   PositionY:   21.063823158231   PositionZ:   -2.904999097442
TiltX:      -15.285552862625   TiltY:       -1.842172834120   TiltZ:       0.503362537817
CrystalId:  288
PositionX:  0.000000000000   PositionY:   -23.567283290729   PositionZ:   -3.585528085360
TiltX:      17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  289
PositionX:  2.594252985282   PositionY:   -23.554039003321   PositionZ:   -3.628347478301
TiltX:      17.196130464691   TiltY:       1.824375548871   TiltZ:       0.564489888993
CrystalId:  290
PositionX:  -2.594252985282   PositionY:   -23.554039003321   PositionZ:   -3.628347478301
TiltX:      17.196130464691   TiltY:       -1.824375548871   TiltZ:       -0.564489888993
CrystalId:  291
PositionX:  0.000000000000   PositionY:   -21.075633173013   PositionZ:   -2.861762132798
TiltX:      15.277460061955   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  292
PositionX:  2.594252985282   PositionY:   -21.063823158231   PositionZ:   -2.904999097442
TiltX:      15.285552862625   TiltY:       1.842172834120   TiltZ:       0.503362537817
CrystalId:  293
PositionX:  -2.594252985282   PositionY:   -21.063823158231   PositionZ:   -2.904999097442
TiltX:      15.285552862625   TiltY:       -1.842172834120   TiltZ:       -0.503362537817
CrystalId:  294
PositionX:  0.000000000000   PositionY:   -26.032444833628   PositionZ:   -4.394932823805
TiltX:      19.096825077444   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  295
PositionX:  2.594252985282   PositionY:   -26.017780985382   PositionZ:   -4.437287081151
TiltX:      19.106668115313   TiltY:       1.804551254619   TiltZ:       0.624988972712
CrystalId:  296
PositionX:  -2.594252985282   PositionY:   -26.017780985382   PositionZ:   -4.437287081151
TiltX:      19.106668115313   TiltY:       -1.804551254619   TiltZ:       -0.624988972712
CrystalId:  297
PositionX:  -7.981993952074   PositionY:   -23.567283290729   PositionZ:   -3.999189505472
TiltX:      17.289528387236   TiltY:       -5.403056403494   TiltZ:       -1.055563986347
CrystalId:  298
PositionX:  -5.395431257137   PositionY:   -23.601618056491   PositionZ:   -3.797516472510
TiltX:      17.244999403897   TiltY:       -3.579427225211   TiltZ:       -0.486992233046
CrystalId:  299
PositionX:  -10.560007823278   PositionY:   -23.506459950150   PositionZ:   -4.286073566242
TiltX:      17.352450466586   TiltY:       -7.226153474881   TiltZ:       -1.627569909517
CrystalId:  300
PositionX:  -8.004674950004   PositionY:   -21.076089913165   PositionZ:   -3.274207718723
TiltX:      15.379845879492   TiltY:       -5.403056403494   TiltZ:       -1.055563986347
CrystalId:  301
PositionX:  -5.418099109780   PositionY:   -21.111858688590   PositionZ:   -3.072952953118
TiltX:      15.340188595232   TiltY:       -3.561574762281   TiltZ:       -0.548190987337
CrystalId:  302
PositionX:  -10.582675675921   PositionY:   -21.016700582249   PositionZ:   -3.561510046850
TiltX:      15.436078187035   TiltY:       -7.244114294980   TiltZ:       -1.566031733769
CrystalId:  303
PositionX:  -7.950764130411   PositionY:   -26.031988093476   PositionZ:   -4.809382320029
TiltX:      19.199210894980   TiltY:       -5.403056403494   TiltZ:       -1.055563986347
CrystalId:  304
PositionX:  -5.364219328789   PositionY:   -26.064903561363   PositionZ:   -4.607243693396
TiltX:      19.149829365692   TiltY:       -3.599300076431   TiltZ:       -0.426420605015
CrystalId:  305
PositionX:  -10.528795894930   PositionY:   -25.969745455023   PositionZ:   -5.095800787128
TiltX:      19.268721832330   TiltY:       -7.206161254871   TiltZ:       -1.688465683220
CrystalId:  306
PositionX:  -7.981993952074   PositionY:   23.567283290729   PositionZ:   -3.999189505472
TiltX:      -17.289528387236   TiltY:       -5.403056403494   TiltZ:       1.055563986347
CrystalId:  307
PositionX:  -5.395431257137   PositionY:   23.601618056491   PositionZ:   -3.797516472510
TiltX:      -17.244999403897   TiltY:       -3.579427225211   TiltZ:       0.486992233046
CrystalId:  308
PositionX:  -10.560007823278   PositionY:   23.506459950150   PositionZ:   -4.286073566242
TiltX:      -17.352450466586   TiltY:       -7.226153474881   TiltZ:       1.627569909517
CrystalId:  309
PositionX:  -7.950764130411   PositionY:   26.031988093476   PositionZ:   -4.809382320029
TiltX:      -19.199210894980   TiltY:       -5.403056403494   TiltZ:       1.055563986347
CrystalId:  310
PositionX:  -5.364219328789   PositionY:   26.064903561363   PositionZ:   -4.607243693396
TiltX:      -19.149829365692   TiltY:       -3.599300076431   TiltZ:       0.426420605015
CrystalId:  311
PositionX:  -10.528795894930   PositionY:   25.969745455023   PositionZ:   -5.095800787128
TiltX:      -19.268721832330   TiltY:       -7.206161254871   TiltZ:       1.688465683220
CrystalId:  312
PositionX:  -8.004674950004   PositionY:   21.076089913165   PositionZ:   -3.274207718723
TiltX:      -15.379845879492   TiltY:       -5.403056403494   TiltZ:       1.055563986347
CrystalId:  313
PositionX:  -5.418099109780   PositionY:   21.111858688590   PositionZ:   -3.072952953118
TiltX:      -15.340188595232   TiltY:       -3.561574762281   TiltZ:       0.548190987337
CrystalId:  314
PositionX:  -10.582675675921   PositionY:   21.016700582249   PositionZ:   -3.561510046850
TiltX:      -15.436078187035   TiltY:       -7.244114294980   TiltZ:       1.566031733769
CrystalId:  315
PositionX:  7.981993952074   PositionY:   -23.567283290729   PositionZ:   -3.999189505472
TiltX:      17.289528387236   TiltY:       5.403056403494   TiltZ:       1.055563986347
CrystalId:  316
PositionX:  10.560007823278   PositionY:   -23.506459950150   PositionZ:   -4.286073566242
TiltX:      17.352450466586   TiltY:       7.226153474881   TiltZ:       1.627569909517
CrystalId:  317
PositionX:  5.395431257137   PositionY:   -23.601618056491   PositionZ:   -3.797516472510
TiltX:      17.244999403897   TiltY:       3.579427225211   TiltZ:       0.486992233046
CrystalId:  318
PositionX:  8.004674950004   PositionY:   -21.076089913165   PositionZ:   -3.274207718723
TiltX:      15.379845879492   TiltY:       5.403056403494   TiltZ:       1.055563986347
CrystalId:  319
PositionX:  10.582675675921   PositionY:   -21.016700582249   PositionZ:   -3.561510046850
TiltX:      15.436078187035   TiltY:       7.244114294980   TiltZ:       1.566031733769
CrystalId:  320
PositionX:  5.418099109780   PositionY:   -21.111858688590   PositionZ:   -3.072952953118
TiltX:      15.340188595232   TiltY:       3.561574762281   TiltZ:       0.548190987337
CrystalId:  321
PositionX:  7.950764130411   PositionY:   -26.031988093476   PositionZ:   -4.809382320029
TiltX:      19.199210894980   TiltY:       5.403056403494   TiltZ:       1.055563986347
CrystalId:  322
PositionX:  10.528795894930   PositionY:   -25.969745455023   PositionZ:   -5.095800787128
TiltX:      19.268721832330   TiltY:       7.206161254871   TiltZ:       1.688465683220
CrystalId:  323
PositionX:  5.364219328789   PositionY:   -26.064903561363   PositionZ:   -4.607243693396
TiltX:      19.149829365692   TiltY:       3.599300076431   TiltZ:       0.426420605015
CrystalId:  324
PositionX:  7.981993952074   PositionY:   23.567283290729   PositionZ:   -3.999189505472
TiltX:      -17.289528387236   TiltY:       5.403056403494   TiltZ:       -1.055563986347
CrystalId:  325
PositionX:  10.560007823278   PositionY:   23.506459950150   PositionZ:   -4.286073566242
TiltX:      -17.352450466586   TiltY:       7.226153474881   TiltZ:       -1.627569909517
CrystalId:  326
PositionX:  5.395431257137   PositionY:   23.601618056491   PositionZ:   -3.797516472510
TiltX:      -17.244999403897   TiltY:       3.579427225211   TiltZ:       -0.486992233046
CrystalId:  327
PositionX:  7.950764130411   PositionY:   26.031988093476   PositionZ:   -4.809382320029
TiltX:      -19.199210894980   TiltY:       5.403056403494   TiltZ:       -1.055563986347
CrystalId:  328
PositionX:  10.528795894930   PositionY:   25.969745455023   PositionZ:   -5.095800787128
TiltX:      -19.268721832330   TiltY:       7.206161254871   TiltZ:       -1.688465683220
CrystalId:  329
PositionX:  5.364219328789   PositionY:   26.064903561363   PositionZ:   -4.607243693396
TiltX:      -19.149829365692   TiltY:       3.599300076431   TiltZ:       -0.426420605015
CrystalId:  330
PositionX:  8.004674950004   PositionY:   21.076089913165   PositionZ:   -3.274207718723
TiltX:      -15.379845879492   TiltY:       5.403056403494   TiltZ:       -1.055563986347
CrystalId:  331
PositionX:  10.582675675921   PositionY:   21.016700582249   PositionZ:   -3.561510046850
TiltX:      -15.436078187035   TiltY:       7.244114294980   TiltZ:       -1.566031733769
CrystalId:  332
PositionX:  5.418099109780   PositionY:   21.111858688590   PositionZ:   -3.072952953118
TiltX:      -15.340188595232   TiltY:       3.561574762281   TiltZ:       -0.548190987337

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
PositionX:  -23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       -17.187142569699   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
ModuleId:   7
PositionX:  0.000000000000   PositionY:   7.961221871788   PositionZ:   -0.398356199580
TiltX:      -5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   8
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   9
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
ModuleId:   10
PositionX:  -7.921456280788   PositionY:   7.961221871788   PositionZ:   -0.794722645583
TiltX:      -5.748976956290   TiltY:       -5.709048052411   TiltZ:       0.199993008557
ModuleId:   11
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       5.709048052411   TiltZ:       0.199993008557
ModuleId:   12
PositionX:  7.921456280788   PositionY:   7.961221871788   PositionZ:   -0.794722645583
TiltX:      -5.748976956290   TiltY:       5.709048052411   TiltZ:       -0.199993008557
ModuleId:   13
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.809568111256   TiltY:       -11.417885536730   TiltZ:       -0.406037856431
ModuleId:   14
PositionX:  -15.763248145959   PositionY:   7.961221871788   PositionZ:   -1.995719739437
TiltX:      -5.809568111256   TiltY:       -11.417885536730   TiltZ:       0.406037856431
ModuleId:   15
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.809568111256   TiltY:       11.417885536730   TiltZ:       0.406037856431
ModuleId:   16
PositionX:  15.763248145959   PositionY:   7.961221871788   PositionZ:   -1.995719739437
TiltX:      -5.809568111256   TiltY:       11.417885536730   TiltZ:       -0.406037856431
ModuleId:   17
PositionX:  -23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.913328510779   TiltY:       -17.126287421919   TiltZ:       -0.624704617965
ModuleId:   18
PositionX:  -23.449571553817   PositionY:   7.961221871788   PositionZ:   -3.966095567861
TiltX:      -5.913328510779   TiltY:       -17.126287421919   TiltZ:       0.624704617965
ModuleId:   19
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      5.913328510779   TiltY:       17.126287421919   TiltZ:       0.624704617965
ModuleId:   20
PositionX:  23.449571553817   PositionY:   7.961221871788   PositionZ:   -3.966095567861
TiltX:      -5.913328510779   TiltY:       17.126287421919   TiltZ:       -0.624704617965
ModuleId:   21
PositionX:  0.000000000000   PositionY:   15.842382076820   PositionZ:   -1.605302676887
TiltX:      -11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   22
PositionX:  0.000000000000   PositionY:   -15.842382076820   PositionZ:   -1.605302676887
TiltX:      11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   23
PositionX:  -7.900022178211   PositionY:   -15.846728891557   PositionZ:   -1.962815881555
TiltX:      11.526072551341   TiltY:       -5.589583053120   TiltZ:       -0.689326323217
ModuleId:   24
PositionX:  -7.900022178211   PositionY:   15.846728891557   PositionZ:   -1.962815881555
TiltX:      -11.526072551341   TiltY:       -5.589583053120   TiltZ:       0.689326323217
ModuleId:   25
PositionX:  7.900022178211   PositionY:   -15.846728891557   PositionZ:   -1.962815881555
TiltX:      11.526072551341   TiltY:       5.589583053120   TiltZ:       0.689326323217
ModuleId:   26
PositionX:  7.900022178211   PositionY:   15.846728891557   PositionZ:   -1.962815881555
TiltX:      -11.526072551341   TiltY:       5.589583053120   TiltZ:       -0.689326323217
ModuleId:   27
PositionX:  -15.719045654212   PositionY:   -15.846728891557   PositionZ:   -3.153859750286
TiltX:      11.636914633970   TiltY:       -11.373413093180   TiltZ:       -0.899731425766
ModuleId:   28
PositionX:  -15.719045654212   PositionY:   15.846728891557   PositionZ:   -3.153859750286
TiltX:      -11.636914633970   TiltY:       -11.373413093180   TiltZ:       0.899731425766
ModuleId:   29
PositionX:  15.719045654212   PositionY:   -15.846728891557   PositionZ:   -3.153859750286
TiltX:      11.636914633970   TiltY:       11.373413093180   TiltZ:       0.899731425766
ModuleId:   30
PositionX:  15.719045654212   PositionY:   15.846728891557   PositionZ:   -3.153859750286
TiltX:      -11.636914633970   TiltY:       11.373413093180   TiltZ:       -0.899731425766
ModuleId:   31
PositionX:  0.000000000000   PositionY:   23.567283290729   PositionZ:   -3.585528085360
TiltX:      -17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   32
PositionX:  0.000000000000   PositionY:   -23.567283290729   PositionZ:   -3.585528085360
TiltX:      17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   33
PositionX:  -7.981993952074   PositionY:   -23.567283290729   PositionZ:   -3.999189505472
TiltX:      17.289528387236   TiltY:       -5.403056403494   TiltZ:       -1.055563986347
ModuleId:   34
PositionX:  -7.981993952074   PositionY:   23.567283290729   PositionZ:   -3.999189505472
TiltX:      -17.289528387236   TiltY:       -5.403056403494   TiltZ:       1.055563986347
ModuleId:   35
PositionX:  7.981993952074   PositionY:   -23.567283290729   PositionZ:   -3.999189505472
TiltX:      17.289528387236   TiltY:       5.403056403494   TiltZ:       1.055563986347
ModuleId:   36
PositionX:  7.981993952074   PositionY:   23.567283290729   PositionZ:   -3.999189505472
TiltX:      -17.289528387236   TiltY:       5.403056403494   TiltZ:       -1.055563986347
