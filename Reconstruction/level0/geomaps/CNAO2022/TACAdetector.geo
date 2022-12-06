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
Width:          64.148998     Height:      28.651262      Thick:     30.297710, 
PositionZ:      -2.398855

CrystalN:  108
ModulesN:  12
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  -18.365156137421   PositionY:   2.592759370739   PositionZ:   -2.224193119434
TiltX:      -1.923741599917   TiltY:       -13.366709304039   TiltZ:       0.065398252682
CrystalId:  1
PositionX:  -15.831504726064   PositionY:   2.594252985282   PositionZ:   -1.664938474349
TiltX:      -1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  2
PositionX:  -13.280055761960   PositionY:   2.592759370739   PositionZ:   -1.193490263093
TiltX:      -1.897918721691   TiltY:       -9.549466450813   TiltZ:       -0.064520718033
CrystalId:  3
PositionX:  -18.374054913794   PositionY:   0.000000000000   PositionZ:   -2.180289902519
TiltX:      0.000000000000   TiltY:       -13.367777554211   TiltZ:       0.000000000000
CrystalId:  4
PositionX:  -15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  5
PositionX:  -13.288954538334   PositionY:   0.000000000000   PositionZ:   -1.149587046178
TiltX:      0.000000000000   TiltY:       -9.548412538722   TiltZ:       0.000000000000
CrystalId:  6
PositionX:  -18.365156137421   PositionY:   -2.592759370739   PositionZ:   -2.224193119434
TiltX:      1.923741599917   TiltY:       -13.366709304039   TiltZ:       -0.065398252682
CrystalId:  7
PositionX:  -15.831504726064   PositionY:   -2.594252985282   PositionZ:   -1.664938474349
TiltX:      1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  8
PositionX:  -13.280055761960   PositionY:   -2.592759370739   PositionZ:   -1.193490263093
TiltX:      1.897918721691   TiltY:       -9.549466450813   TiltZ:       0.064520718033
CrystalId:  9
PositionX:  -10.533900563246   PositionY:   2.592759370739   PositionZ:   -0.738585713164
TiltX:      -1.917154837490   TiltY:       -7.637665438087   TiltZ:       0.064196161297
CrystalId:  10
PositionX:  -7.957077363580   PositionY:   2.594252985282   PositionZ:   -0.435044233330
TiltX:      -1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  11
PositionX:  -5.371310715817   PositionY:   2.592759370739   PositionZ:   -0.220647228601
TiltX:      -1.904373794316   TiltY:       -3.820422511741   TiltZ:       -0.063768344486
CrystalId:  12
PositionX:  -10.538372287295   PositionY:   0.000000000000   PositionZ:   -0.694013475612
TiltX:      0.000000000000   TiltY:       -7.638730030978   TiltZ:       0.000000000000
CrystalId:  13
PositionX:  -7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  14
PositionX:  -5.375782439865   PositionY:   0.000000000000   PositionZ:   -0.176074991049
TiltX:      0.000000000000   TiltY:       -3.819365015489   TiltZ:       0.000000000000
CrystalId:  15
PositionX:  -10.533900563246   PositionY:   -2.592759370739   PositionZ:   -0.738585713164
TiltX:      1.917154837490   TiltY:       -7.637665438087   TiltZ:       -0.064196161297
CrystalId:  16
PositionX:  -7.957077363580   PositionY:   -2.594252985282   PositionZ:   -0.435044233330
TiltX:      1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  17
PositionX:  -5.371310715817   PositionY:   -2.592759370739   PositionZ:   -0.220647228601
TiltX:      1.904373794316   TiltY:       -3.820422511741   TiltZ:       0.063768344486
CrystalId:  18
PositionX:  -2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       -1.908621475036   TiltZ:       0.063661955951
CrystalId:  19
PositionX:  0.000000000000   PositionY:   2.594252985282   PositionZ:   -0.044820883088
TiltX:      -1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  20
PositionX:  2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       1.908621475036   TiltZ:       -0.063661955951
CrystalId:  21
PositionX:  -2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       -1.909682507744   TiltZ:       0.000000000000
CrystalId:  22
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  23
PositionX:  2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       1.909682507744   TiltZ:       0.000000000000
CrystalId:  24
PositionX:  -2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       -1.908621475036   TiltZ:       -0.063661955951
CrystalId:  25
PositionX:  0.000000000000   PositionY:   -2.594252985282   PositionZ:   -0.044820883088
TiltX:      1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  26
PositionX:  2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       1.908621475036   TiltZ:       0.063661955951
CrystalId:  27
PositionX:  5.371310715817   PositionY:   2.592759370739   PositionZ:   -0.220647228601
TiltX:      -1.904373794316   TiltY:       3.820422511741   TiltZ:       0.063768344486
CrystalId:  28
PositionX:  7.957077363580   PositionY:   2.594252985282   PositionZ:   -0.435044233330
TiltX:      -1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  29
PositionX:  10.533900563246   PositionY:   2.592759370739   PositionZ:   -0.738585713164
TiltX:      -1.917154837490   TiltY:       7.637665438087   TiltZ:       -0.064196161297
CrystalId:  30
PositionX:  5.375782439865   PositionY:   0.000000000000   PositionZ:   -0.176074991049
TiltX:      0.000000000000   TiltY:       3.819365015489   TiltZ:       0.000000000000
CrystalId:  31
PositionX:  7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  32
PositionX:  10.538372287295   PositionY:   0.000000000000   PositionZ:   -0.694013475612
TiltX:      0.000000000000   TiltY:       7.638730030978   TiltZ:       0.000000000000
CrystalId:  33
PositionX:  5.371310715817   PositionY:   -2.592759370739   PositionZ:   -0.220647228601
TiltX:      1.904373794316   TiltY:       3.820422511741   TiltZ:       -0.063768344486
CrystalId:  34
PositionX:  7.957077363580   PositionY:   -2.594252985282   PositionZ:   -0.435044233330
TiltX:      1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  35
PositionX:  10.533900563246   PositionY:   -2.592759370739   PositionZ:   -0.738585713164
TiltX:      1.917154837490   TiltY:       7.637665438087   TiltZ:       0.064196161297
CrystalId:  36
PositionX:  13.280055761960   PositionY:   2.592759370739   PositionZ:   -1.193490263093
TiltX:      -1.897918721691   TiltY:       9.549466450813   TiltZ:       0.064520718033
CrystalId:  37
PositionX:  15.831504726064   PositionY:   2.594252985282   PositionZ:   -1.664938474349
TiltX:      -1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  38
PositionX:  18.365156137421   PositionY:   2.592759370739   PositionZ:   -2.224193119434
TiltX:      -1.923741599917   TiltY:       13.366709304039   TiltZ:       -0.065398252682
CrystalId:  39
PositionX:  13.288954538334   PositionY:   0.000000000000   PositionZ:   -1.149587046178
TiltX:      0.000000000000   TiltY:       9.548412538722   TiltZ:       0.000000000000
CrystalId:  40
PositionX:  15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  41
PositionX:  18.374054913794   PositionY:   0.000000000000   PositionZ:   -2.180289902519
TiltX:      0.000000000000   TiltY:       13.367777554211   TiltZ:       0.000000000000
CrystalId:  42
PositionX:  13.280055761960   PositionY:   -2.592759370739   PositionZ:   -1.193490263093
TiltX:      1.897918721691   TiltY:       9.549466450813   TiltZ:       -0.064520718033
CrystalId:  43
PositionX:  15.831504726064   PositionY:   -2.594252985282   PositionZ:   -1.664938474349
TiltX:      1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  44
PositionX:  18.365156137421   PositionY:   -2.592759370739   PositionZ:   -2.224193119434
TiltX:      1.923741599917   TiltY:       13.366709304039   TiltZ:       0.065398252682
CrystalId:  45
PositionX:  21.058261982748   PositionY:   2.592759370739   PositionZ:   -2.927942630339
TiltX:      -1.891242014235   TiltY:       15.278510266823   TiltZ:       0.065957830679
CrystalId:  46
PositionX:  23.549904744570   PositionY:   2.594252985282   PositionZ:   -3.651732364847
TiltX:      -1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  47
PositionX:  26.015073643363   PositionY:   2.592759370739   PositionZ:   -4.461113321346
TiltX:      -1.930650078844   TiltY:       19.095752991358   TiltZ:       -0.067331688610
CrystalId:  48
PositionX:  21.071498914262   PositionY:   0.000000000000   PositionZ:   -2.885147019344
TiltX:      0.000000000000   TiltY:       15.277460061955   TiltZ:       0.000000000000
CrystalId:  49
PositionX:  23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  50
PositionX:  26.028310574878   PositionY:   0.000000000000   PositionZ:   -4.418317710350
TiltX:      0.000000000000   TiltY:       19.096825077444   TiltZ:       0.000000000000
CrystalId:  51
PositionX:  21.058261982748   PositionY:   -2.592759370739   PositionZ:   -2.927942630339
TiltX:      1.891242014235   TiltY:       15.278510266823   TiltZ:       -0.065957830679
CrystalId:  52
PositionX:  23.549904744570   PositionY:   -2.594252985282   PositionZ:   -3.651732364847
TiltX:      1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  53
PositionX:  26.015073643363   PositionY:   -2.592759370739   PositionZ:   -4.461113321346
TiltX:      1.930650078844   TiltY:       19.095752991358   TiltZ:       0.067331688610
CrystalId:  54
PositionX:  -18.339097183338   PositionY:   -5.372796869889   PositionZ:   -2.352758201381
TiltX:      3.847463653990   TiltY:       -13.363505768513   TiltZ:       -0.130722209293
CrystalId:  55
PositionX:  -15.805430942059   PositionY:   -5.375782439865   PositionZ:   -1.793576721555
TiltX:      3.819365015489   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  56
PositionX:  -13.253996807878   PositionY:   -5.372796869889   PositionZ:   -1.322055345040
TiltX:      3.795846783025   TiltY:       -9.552627035954   TiltZ:       0.128971044992
CrystalId:  57
PositionX:  -18.296536583086   PositionY:   -7.957077363580   PositionZ:   -2.562736201336
TiltX:      5.771146656188   TiltY:       -13.358170591105   TiltZ:       -0.195897664723
CrystalId:  58
PositionX:  -15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  59
PositionX:  -13.211436207626   PositionY:   -7.957077363580   PositionZ:   -1.532033344995
TiltX:      5.693793529355   TiltY:       -9.557890841965   TiltZ:       0.193280663294
CrystalId:  60
PositionX:  -18.236267327271   PositionY:   -10.532414409174   PositionZ:   -2.860082049839
TiltX:      7.694771179972   TiltY:       -13.350709839262   TiltZ:       -0.260850595632
CrystalId:  61
PositionX:  -15.702541848652   PositionY:   -10.538372287295   PositionZ:   -2.301192824776
TiltX:      7.638730030978   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  62
PositionX:  -13.151166951811   PositionY:   -10.532414409174   PositionZ:   -1.829379193498
TiltX:      7.591768317379   TiltY:       -9.565252119262   TiltZ:       0.257379402264
CrystalId:  63
PositionX:  -10.520805678427   PositionY:   -5.372796869889   PositionZ:   -0.869109938408
TiltX:      3.834297718900   TiltY:       -7.634472857871   TiltZ:       -0.128320135143
CrystalId:  64
PositionX:  -7.943975026577   PositionY:   -5.375782439865   PositionZ:   -0.565642738764
TiltX:      3.819365015489   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  65
PositionX:  -5.358215830997   PositionY:   -5.372796869889   PositionZ:   -0.351171453845
TiltX:      3.808749925517   TiltY:       -3.823593833643   TiltZ:       0.127466405094
CrystalId:  66
PositionX:  -10.499418551516   PositionY:   -7.957077363580   PositionZ:   -1.082287695131
TiltX:      5.751416718004   TiltY:       -7.629155884274   TiltZ:       -0.192299819112
CrystalId:  67
PositionX:  -7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  68
PositionX:  -5.336828704086   PositionY:   -7.957077363580   PositionZ:   -0.564349210568
TiltX:      5.713130743513   TiltY:       -3.828875481922   TiltZ:       0.191023974458
CrystalId:  69
PositionX:  -10.469132648231   PositionY:   -10.532414409174   PositionZ:   -1.384164658281
TiltX:      7.668499968183   TiltY:       -7.621720502450   TiltZ:       -0.256063280705
CrystalId:  70
PositionX:  -7.892272229025   PositionY:   -10.538372287295   PositionZ:   -1.080994166935
TiltX:      7.638730030978   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  71
PositionX:  -5.306542800801   PositionY:   -10.532414409174   PositionZ:   -0.866226173717
TiltX:      7.617518624170   TiltY:       -3.836261628726   TiltZ:       0.254370998153
CrystalId:  72
PositionX:  -2.594252985282   PositionY:   -5.372796869889   PositionZ:   -0.220796327486
TiltX:      3.821481187277   TiltY:       -1.905439559434   TiltZ:       -0.127253039168
CrystalId:  73
PositionX:  0.000000000000   PositionY:   -5.375782439865   PositionZ:   -0.176074991049
TiltX:      3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  74
PositionX:  2.594252985282   PositionY:   -5.372796869889   PositionZ:   -0.220796327486
TiltX:      3.821481187277   TiltY:       1.905439559434   TiltZ:       0.127253039168
CrystalId:  75
PositionX:  -2.594252985282   PositionY:   -7.957077363580   PositionZ:   -0.435044233330
TiltX:      5.732210015987   TiltY:       -1.900140307152   TiltZ:       -0.190702457215
CrystalId:  76
PositionX:  0.000000000000   PositionY:   -7.961551572609   PositionZ:   -0.390447226543
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  77
PositionX:  2.594252985282   PositionY:   -7.957077363580   PositionZ:   -0.435044233330
TiltX:      5.732210015987   TiltY:       1.900140307152   TiltZ:       0.190702457215
CrystalId:  78
PositionX:  -2.594252985282   PositionY:   -10.532414409174   PositionZ:   -0.738436614280
TiltX:      7.642924785518   TiltY:       -1.892729624041   TiltZ:       -0.253939578138
CrystalId:  79
PositionX:  0.000000000000   PositionY:   -10.538372287295   PositionZ:   -0.694013475612
TiltX:      7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  80
PositionX:  2.594252985282   PositionY:   -10.532414409174   PositionZ:   -0.738436614280
TiltX:      7.642924785518   TiltY:       1.892729624041   TiltZ:       0.253939578138
CrystalId:  81
PositionX:  5.358215830997   PositionY:   -5.372796869889   PositionZ:   -0.351171453845
TiltX:      3.808749925517   TiltY:       3.823593833643   TiltZ:       -0.127466405094
CrystalId:  82
PositionX:  7.943975026577   PositionY:   -5.375782439865   PositionZ:   -0.565642738764
TiltX:      3.819365015489   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  83
PositionX:  10.520805678427   PositionY:   -5.372796869889   PositionZ:   -0.869109938408
TiltX:      3.834297718900   TiltY:       7.634472857871   TiltZ:       0.128320135143
CrystalId:  84
PositionX:  5.336828704086   PositionY:   -7.957077363580   PositionZ:   -0.564349210568
TiltX:      5.713130743513   TiltY:       3.828875481922   TiltZ:       -0.191023974458
CrystalId:  85
PositionX:  7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  86
PositionX:  10.499418551516   PositionY:   -7.957077363580   PositionZ:   -1.082287695131
TiltX:      5.751416718004   TiltY:       7.629155884274   TiltZ:       0.192299819112
CrystalId:  87
PositionX:  5.306542800801   PositionY:   -10.532414409174   PositionZ:   -0.866226173717
TiltX:      7.617518624170   TiltY:       3.836261628726   TiltZ:       -0.254370998153
CrystalId:  88
PositionX:  7.892272229025   PositionY:   -10.538372287295   PositionZ:   -1.080994166935
TiltX:      7.638730030978   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  89
PositionX:  10.469132648231   PositionY:   -10.532414409174   PositionZ:   -1.384164658281
TiltX:      7.668499968183   TiltY:       7.621720502450   TiltZ:       0.256063280705
CrystalId:  90
PositionX:  13.244962363116   PositionY:   -5.381822987746   PositionZ:   -1.321148109080
TiltX:      3.835943184502   TiltY:       9.532681614646   TiltZ:       0.072711197615
CrystalId:  91
PositionX:  15.796555472231   PositionY:   -5.375798165997   PositionZ:   -1.791779355648
TiltX:      3.859663079495   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  92
PositionX:  18.330390908364   PositionY:   -5.363802186188   PositionZ:   -2.350072775158
TiltX:      3.888010154592   TiltY:       13.343380394944   TiltZ:       0.335121711523
CrystalId:  93
PositionX:  13.211272122732   PositionY:   -7.966087764358   PositionZ:   -1.532922440127
TiltX:      5.733913227807   TiltY:       9.537989812649   TiltZ:       0.008409091651
CrystalId:  94
PositionX:  15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.769345587240   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  95
PositionX:  18.296700667980   PositionY:   -7.948066962801   PositionZ:   -2.561847106204
TiltX:      5.811668703700   TiltY:       13.338000358047   TiltZ:       0.400287832603
CrystalId:  96
PositionX:  13.159873226785   PositionY:   -10.541409092875   PositionZ:   -1.832064619721
TiltX:      7.631911436304   TiltY:       9.545395360800   TiltZ:       -0.055680661178
CrystalId:  97
PositionX:  15.711417318481   PositionY:   -10.538356561163   PositionZ:   -2.302990190683
TiltX:      7.679028094984   TiltY:       11.438060203208   TiltZ:       0.203033341571
CrystalId:  98
PositionX:  18.245301772033   PositionY:   -10.523388291318   PositionZ:   -2.860989285798
TiltX:      7.735268448088   TiltY:       13.330494926376   TiltZ:       0.465229896456
CrystalId:  99
PositionX:  21.010600529281   PositionY:   -5.381822987746   PositionZ:   -3.051459666160
TiltX:      3.843466853718   TiltY:       15.261724429845   TiltZ:       0.074328457251
CrystalId:  100
PositionX:  23.502468268973   PositionY:   -5.375798165997   PositionZ:   -3.774450905807
TiltX:      3.880875742794   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  101
PositionX:  25.967916226935   PositionY:   -5.363802186188   PositionZ:   -4.582893808093
TiltX:      3.923406226639   TiltY:       19.072327196915   TiltZ:       0.345013920223
CrystalId:  102
PositionX:  20.955938364360   PositionY:   -7.966087764358   PositionZ:   -3.258813101901
TiltX:      5.734783366708   TiltY:       15.267037272912   TiltZ:       0.008596212304
CrystalId:  103
PositionX:  23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.790558250539   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  104
PositionX:  25.913254062014   PositionY:   -7.948066962801   PositionZ:   -4.790247243833
TiltX:      5.853946236937   TiltY:       19.066904181601   TiltZ:       0.412099384373
CrystalId:  105
PositionX:  20.874934570987   PositionY:   -10.541409092875   PositionZ:   -3.551330239136
TiltX:      7.626149623605   TiltY:       15.274440123100   TiltZ:       -0.056920449108
CrystalId:  106
PositionX:  23.366724157400   PositionY:   -10.538356561163   PositionZ:   -4.274609439928
TiltX:      7.700240758283   TiltY:       17.167070888243   TiltZ:       0.208280324993
CrystalId:  107
PositionX:  25.832250268642   PositionY:   -10.523388291318   PositionZ:   -5.082764381068
TiltX:      7.784402643565   TiltY:       19.059348343417   TiltZ:       0.478950861965

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  -15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   1
PositionX:  -7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   2
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   3
PositionX:  7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   4
PositionX:  15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   5
PositionX:  23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  -15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   7
PositionX:  -7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   8
PositionX:  0.000000000000   PositionY:   -7.961551572609   PositionZ:   -0.390447226543
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   9
PositionX:  7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   10
PositionX:  15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.769345587240   TiltY:       11.438060203208   TiltZ:       0.203033341571
ModuleId:   11
PositionX:  23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.790558250539   TiltY:       17.167070888243   TiltZ:       0.208280324993
