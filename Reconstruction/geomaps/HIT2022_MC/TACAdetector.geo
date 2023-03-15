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
TypeGeo:        "SEVEN_MOD_HIT22"

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
Width:          64.157267     Height:      28.675565      Thick:     29.647902, 
PositionZ:      -2.073951

CrystalN:  63
ModulesN:  7
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
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.648226777205   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  46
PositionX:  -13.212259817935   PositionY:   -7.938726970990   PositionZ:   -1.522317169807
TiltX:      5.613335214187   TiltY:       -9.597274429045   TiltZ:       0.596737593393
CrystalId:  47
PositionX:  -18.296517977544   PositionY:   -7.974768354527   PositionZ:   -2.556538708795
TiltX:      5.689871831888   TiltY:       -13.398088418139   TiltZ:       0.212977464005
CrystalId:  48
PositionX:  -15.823582395457   PositionY:   -5.375515643380   PositionZ:   -1.789223875278
TiltX:      3.738544269460   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  49
PositionX:  -13.272559150425   PositionY:   -5.354509345420   PositionZ:   -1.315941082482
TiltX:      3.715434588973   TiltY:       -9.592099657331   TiltZ:       0.532413078318
CrystalId:  50
PositionX:  -18.356817310033   PositionY:   -5.390550728956   PositionZ:   -2.350162621469
TiltX:      3.766140415424   TiltY:       -13.403333613465   TiltZ:       0.278171426200
CrystalId:  51
PositionX:  -15.685195400022   PositionY:   -10.537979682137   PositionZ:   -2.289632003324
TiltX:      7.557909284949   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  52
PositionX:  -13.134251829883   PositionY:   -10.514001148465   PositionZ:   -1.816061105681
TiltX:      7.511263628404   TiltY:       -9.604546912534   TiltZ:       0.660854185510
CrystalId:  53
PositionX:  -18.218509989491   PositionY:   -10.550042532001   PositionZ:   -2.850282644668
TiltX:      7.613545430079   TiltY:       -13.390717291921   TiltZ:       0.148002950296
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
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.648226777205   TiltY:       -11.497744179474   TiltZ:       0.406152581296
ModuleId:   6
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
