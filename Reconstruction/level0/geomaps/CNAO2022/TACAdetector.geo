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
Widthback :   1.450000    Heightback :  1.450000      Length:           12.000000
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
Widthback :   4.509309    Heightback :  4.509309      Length:           5.762494
PositionZ:     5.755293

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Air around each module. It is need ONLY by FLUKA geometry (cm)
// it will be removed if a truncate pyramid body is implemented in FLUKA
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Widthfront:   3.258752   Heightfront:  3.258752    DummyLength:    0.
Widthback :   4.462378    Heightback :  4.462378      Length:         12.150000
PositionZ:    -0.082500

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Calorimeter bounding box (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Width:          64.157267     Height:      28.767023      Thick:     30.290749,
PositionZ:      -2.395375

CrystalN:  108
ModulesN:  12
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  1
PositionX:  2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       1.900140307152   TiltZ:       0.190702457215
CrystalId:  2
PositionX:  -2.594252985282   PositionY:   -7.956747662759   PositionZ:   -0.442953206367
TiltX:      5.732210015987   TiltY:       -1.900140307152   TiltZ:       -0.190702457215
CrystalId:  3
PositionX:  0.000000000000   PositionY:   -5.375452739044   PositionZ:   -0.183983964085
TiltX:      3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  4
PositionX:  2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       1.905439559434   TiltZ:       0.127253039168
CrystalId:  5
PositionX:  -2.594252985282   PositionY:   -5.372467169068   PositionZ:   -0.228705300522
TiltX:      3.821481187277   TiltY:       -1.905439559434   TiltZ:       -0.127253039168
CrystalId:  6
PositionX:  0.000000000000   PositionY:   -10.538042586474   PositionZ:   -0.701922448649
TiltX:      7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  7
PositionX:  2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       1.892729624041   TiltZ:       0.253939578138
CrystalId:  8
PositionX:  -2.594252985282   PositionY:   -10.532084708353   PositionZ:   -0.746345587317
TiltX:      7.642924785518   TiltY:       -1.892729624041   TiltZ:       -0.253939578138
CrystalId:  9
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  10
PositionX:  -5.335634984228   PositionY:   -7.965758063537   PositionZ:   -0.581028685854
TiltX:      5.733071938606   TiltY:       -3.808942350299   TiltZ:       -0.008311202461
CrystalId:  11
PositionX:  -10.498373855837   PositionY:   -7.947737261980   PositionZ:   -1.097165102767
TiltX:      5.771355635844   TiltY:       -7.609089857272   TiltZ:       -0.392950494226
CrystalId:  12
PositionX:  -7.933845260534   PositionY:   -5.375468465176   PositionZ:   -0.580518778722
TiltX:      3.839294448546   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  13
PositionX:  -5.348016740602   PositionY:   -5.381493286925   PositionZ:   -0.366949047176
TiltX:      3.828668825381   TiltY:       -3.803638643438   TiltZ:       -0.071865450970
CrystalId:  14
PositionX:  -10.510755612211   PositionY:   -5.363472485367   PositionZ:   -0.883085464089
TiltX:      3.854259478278   TiltY:       -7.614428910880   TiltZ:       -0.328975678625
CrystalId:  15
PositionX:  -7.900163579531   PositionY:   -10.538026860342   PositionZ:   -1.097675009899
TiltX:      7.658659464034   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
CrystalId:  16
PositionX:  -5.314354451481   PositionY:   -10.541079392054   PositionZ:   -0.883807530959
TiltX:      7.637482065971   TiltY:       -3.816350506889   TiltZ:       0.055031767893
CrystalId:  17
PositionX:  -10.477093323090   PositionY:   -10.523058590497   PositionZ:   -1.399943947872
TiltX:      7.688415998562   TiltY:       -7.601632495079   TiltZ:       -0.456708355702
CrystalId:  18
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.788622677155   TiltY:       5.668842078759   TiltZ:       0.599937083454
CrystalId:  19
PositionX:  10.498428519976   PositionY:   -7.929716899577   PositionZ:   -1.095353612695
TiltX:      5.811017396459   TiltY:       7.568750804118   TiltZ:       0.794202496782
CrystalId:  20
PositionX:  5.335580320089   PositionY:   -7.983778425940   PositionZ:   -0.582840175927
TiltX:      5.772743849352   TiltY:       3.768869034055   TiltZ:       0.406946833187
CrystalId:  21
PositionX:  7.915823638178   PositionY:   -5.375594273082   PositionZ:   -0.578723438750
TiltX:      3.878940169411   TiltY:       5.668842078759   TiltZ:       0.599937083454
CrystalId:  22
PositionX:  10.492799029758   PositionY:   -5.345577858438   PositionZ:   -0.879479667694
TiltX:      3.893967145300   TiltY:       7.574133776804   TiltZ:       0.730237426573
CrystalId:  23
PositionX:  5.329950829871   PositionY:   -5.399639384801   PositionZ:   -0.366966230926
TiltX:      3.868295922370   TiltY:       3.763521445021   TiltZ:       0.470494445069
CrystalId:  24
PositionX:  7.918185201887   PositionY:   -10.537901052435   PositionZ:   -1.099470349871
TiltX:      7.698305184900   TiltY:       5.668842078759   TiltZ:       0.599937083454
CrystalId:  25
PositionX:  10.495159233820   PositionY:   -10.504912492620   PositionZ:   -1.399926764122
TiltX:      7.728031764639   TiltY:       7.561249722327   TiltZ:       0.857949157927
CrystalId:  26
PositionX:  5.332311033934   PositionY:   -10.558974018983   PositionZ:   -0.887413327354
TiltX:      7.677198690887   TiltY:       3.776320975480   TiltZ:       0.343611956703
CrystalId:  27
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.889784546442   TiltY:       11.377117828446   TiltZ:       0.811959833235
CrystalId:  28
PositionX:  18.297409525910   PositionY:   -7.920707157525   PositionZ:   -2.551185419292
TiltX:      5.932767274499   TiltY:       13.276647728177   TiltZ:       1.013266803172
CrystalId:  29
PositionX:  13.211368269569   PositionY:   -7.992788167992   PositionZ:   -1.527670459310
TiltX:      5.853828598837   TiltY:       9.477452960691   TiltZ:       0.613321982672
CrystalId:  30
PositionX:  15.770328885592   PositionY:   -5.375704353324   PositionZ:   -1.778449198303
TiltX:      3.980102038698   TiltY:       11.377117828446   TiltZ:       0.811959833235
CrystalId:  31
PositionX:  18.304486008697   PositionY:   -5.336678133250   PositionZ:   -2.334040858401
TiltX:      4.009183062335   TiltY:       13.282161810484   TiltZ:       0.948129002905
CrystalId:  32
PositionX:  13.218444752356   PositionY:   -5.408759143717   PositionZ:   -1.310525898419
TiltX:      3.955787724231   TiltY:       9.472012093943   TiltZ:       0.677601301204
CrystalId:  33
PositionX:  15.738448909887   PositionY:   -10.537790972194   PositionZ:   -2.300406680299
TiltX:      7.799467054186   TiltY:       11.377117828446   TiltZ:       0.811959833235
CrystalId:  34
PositionX:  18.272624387560   PositionY:   -10.495792733704   PositionZ:   -2.855697828731
TiltX:      7.856291718216   TiltY:       13.269008795705   TiltZ:       1.078175969586
CrystalId:  35
PositionX:  13.186583131219   PositionY:   -10.567873744171   PositionZ:   -1.832182868748
TiltX:      7.751897997629   TiltY:       9.484990810674   TiltZ:       0.549259420934
CrystalId:  36
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      6.035758264314   TiltY:       17.084652596809   TiltZ:       1.040944371753
CrystalId:  37
PositionX:  25.915757301519   PositionY:   -7.911697854616   PositionZ:   -4.770851861876
TiltX:      6.101495709480   TiltY:       18.983635485170   TiltZ:       1.253256425454
CrystalId:  38
PositionX:  20.957029539358   PositionY:   -8.001797470902   PositionZ:   -3.246550301655
TiltX:      5.977938666172   TiltY:       15.185453235980   TiltZ:       0.832915943087
CrystalId:  39
PositionX:  23.469651392445   PositionY:   -5.375845882764   PositionZ:   -3.747955111596
TiltX:      4.126075756570   TiltY:       17.084652596809   TiltZ:       1.040944371753
CrystalId:  40
PositionX:  25.935825311364   PositionY:   -5.327810278296   PositionZ:   -4.552837864271
TiltX:      4.171066902196   TiltY:       18.989332450780   TiltZ:       1.186229931724
CrystalId:  41
PositionX:  20.977097549202   PositionY:   -5.417909894582   PositionZ:   -3.028536304050
TiltX:      4.086518739303   TiltY:       15.179871381560   TiltZ:       0.898598731208
CrystalId:  42
PositionX:  23.403135448432   PositionY:   -10.537649442754   PositionZ:   -4.269447051935
TiltX:      7.945440772059   TiltY:       17.084652596809   TiltZ:       1.040944371753
CrystalId:  43
PositionX:  25.869347663223   PositionY:   -10.486641982839   PositionZ:   -5.074029560975
TiltX:      8.031837778092   TiltY:       18.975806785899   TiltZ:       1.320039224870
CrystalId:  44
PositionX:  20.910619901062   PositionY:   -10.576741599125   PositionZ:   -3.549728000754
TiltX:      7.869410116809   TiltY:       15.193124303146   TiltZ:       0.767457784674
CrystalId:  45
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  46
PositionX:  2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       1.909682507744   TiltZ:       0.000000000000
CrystalId:  47
PositionX:  -2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       -1.909682507744   TiltZ:       0.000000000000
CrystalId:  48
PositionX:  0.000000000000   PositionY:   2.594252985282   PositionZ:   -0.044820883088
TiltX:      -1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  49
PositionX:  2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       1.908621475036   TiltZ:       -0.063661955951
CrystalId:  50
PositionX:  -2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       -1.908621475036   TiltZ:       0.063661955951
CrystalId:  51
PositionX:  0.000000000000   PositionY:   -2.594252985282   PositionZ:   -0.044820883088
TiltX:      1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  52
PositionX:  2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       1.908621475036   TiltZ:       0.063661955951
CrystalId:  53
PositionX:  -2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       -1.908621475036   TiltZ:       -0.063661955951
CrystalId:  54
PositionX:  -7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  55
PositionX:  -5.375452739044   PositionY:   0.000000000000   PositionZ:   -0.183983964085
TiltX:      0.000000000000   TiltY:       -3.819365015489   TiltZ:       0.000000000000
CrystalId:  56
PositionX:  -10.538042586474   PositionY:   0.000000000000   PositionZ:   -0.701922448649
TiltX:      0.000000000000   TiltY:       -7.638730030978   TiltZ:       0.000000000000
CrystalId:  57
PositionX:  -7.956747662759   PositionY:   2.594252985282   PositionZ:   -0.442953206367
TiltX:      -1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  58
PositionX:  -5.370981014996   PositionY:   2.592759370739   PositionZ:   -0.228556201638
TiltX:      -1.904373794316   TiltY:       -3.820422511741   TiltZ:       -0.063768344486
CrystalId:  59
PositionX:  -10.533570862425   PositionY:   2.592759370739   PositionZ:   -0.746494686201
TiltX:      -1.917154837490   TiltY:       -7.637665438087   TiltZ:       0.064196161297
CrystalId:  60
PositionX:  -7.956747662759   PositionY:   -2.594252985282   PositionZ:   -0.442953206367
TiltX:      1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  61
PositionX:  -5.370981014996   PositionY:   -2.592759370739   PositionZ:   -0.228556201638
TiltX:      1.904373794316   TiltY:       -3.820422511741   TiltZ:       0.063768344486
CrystalId:  62
PositionX:  -10.533570862425   PositionY:   -2.592759370739   PositionZ:   -0.746494686201
TiltX:      1.917154837490   TiltY:       -7.637665438087   TiltZ:       -0.064196161297
CrystalId:  63
PositionX:  7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  64
PositionX:  10.538042586474   PositionY:   0.000000000000   PositionZ:   -0.701922448649
TiltX:      0.000000000000   TiltY:       7.638730030978   TiltZ:       0.000000000000
CrystalId:  65
PositionX:  5.375452739044   PositionY:   0.000000000000   PositionZ:   -0.183983964085
TiltX:      0.000000000000   TiltY:       3.819365015489   TiltZ:       0.000000000000
CrystalId:  66
PositionX:  7.956747662759   PositionY:   2.594252985282   PositionZ:   -0.442953206367
TiltX:      -1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  67
PositionX:  10.533570862425   PositionY:   2.592759370739   PositionZ:   -0.746494686201
TiltX:      -1.917154837490   TiltY:       7.637665438087   TiltZ:       -0.064196161297
CrystalId:  68
PositionX:  5.370981014996   PositionY:   2.592759370739   PositionZ:   -0.228556201638
TiltX:      -1.904373794316   TiltY:       3.820422511741   TiltZ:       0.063768344486
CrystalId:  69
PositionX:  7.956747662759   PositionY:   -2.594252985282   PositionZ:   -0.442953206367
TiltX:      1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  70
PositionX:  10.533570862425   PositionY:   -2.592759370739   PositionZ:   -0.746494686201
TiltX:      1.917154837490   TiltY:       7.637665438087   TiltZ:       0.064196161297
CrystalId:  71
PositionX:  5.370981014996   PositionY:   -2.592759370739   PositionZ:   -0.228556201638
TiltX:      1.904373794316   TiltY:       3.820422511741   TiltZ:       -0.063768344486
CrystalId:  72
PositionX:  -15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  73
PositionX:  -13.290928167578   PositionY:   0.000000000000   PositionZ:   -1.133878863084
TiltX:      0.000000000000   TiltY:       -9.548412538722   TiltZ:       0.000000000000
CrystalId:  74
PositionX:  -18.376028543039   PositionY:   0.000000000000   PositionZ:   -2.164581719425
TiltX:      0.000000000000   TiltY:       -13.367777554211   TiltZ:       0.000000000000
CrystalId:  75
PositionX:  -15.833478355309   PositionY:   2.594252985282   PositionZ:   -1.649230291254
TiltX:      -1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  76
PositionX:  -13.282029391205   PositionY:   2.592759370739   PositionZ:   -1.177782079998
TiltX:      -1.897918721691   TiltY:       -9.549466450813   TiltZ:       -0.064520718033
CrystalId:  77
PositionX:  -18.367129766665   PositionY:   2.592759370739   PositionZ:   -2.208484936339
TiltX:      -1.923741599917   TiltY:       -13.366709304039   TiltZ:       0.065398252682
CrystalId:  78
PositionX:  -15.833478355309   PositionY:   -2.594252985282   PositionZ:   -1.649230291254
TiltX:      1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  79
PositionX:  -13.282029391205   PositionY:   -2.592759370739   PositionZ:   -1.177782079998
TiltX:      1.897918721691   TiltY:       -9.549466450813   TiltZ:       0.064520718033
CrystalId:  80
PositionX:  -18.367129766665   PositionY:   -2.592759370739   PositionZ:   -2.208484936339
TiltX:      1.923741599917   TiltY:       -13.366709304039   TiltZ:       -0.065398252682
CrystalId:  81
PositionX:  15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  82
PositionX:  18.376028543039   PositionY:   0.000000000000   PositionZ:   -2.164581719425
TiltX:      0.000000000000   TiltY:       13.367777554211   TiltZ:       0.000000000000
CrystalId:  83
PositionX:  13.290928167578   PositionY:   0.000000000000   PositionZ:   -1.133878863084
TiltX:      0.000000000000   TiltY:       9.548412538722   TiltZ:       0.000000000000
CrystalId:  84
PositionX:  15.833478355309   PositionY:   2.594252985282   PositionZ:   -1.649230291254
TiltX:      -1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  85
PositionX:  18.367129766665   PositionY:   2.592759370739   PositionZ:   -2.208484936339
TiltX:      -1.923741599917   TiltY:       13.366709304039   TiltZ:       -0.065398252682
CrystalId:  86
PositionX:  13.282029391205   PositionY:   2.592759370739   PositionZ:   -1.177782079998
TiltX:      -1.897918721691   TiltY:       9.549466450813   TiltZ:       0.064520718033
CrystalId:  87
PositionX:  15.833478355309   PositionY:   -2.594252985282   PositionZ:   -1.649230291254
TiltX:      1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  88
PositionX:  18.367129766665   PositionY:   -2.592759370739   PositionZ:   -2.208484936339
TiltX:      1.923741599917   TiltY:       13.366709304039   TiltZ:       0.065398252682
CrystalId:  89
PositionX:  13.282029391205   PositionY:   -2.592759370739   PositionZ:   -1.177782079998
TiltX:      1.897918721691   TiltY:       9.549466450813   TiltZ:       -0.064520718033
CrystalId:  90
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.648226777205   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  91
PositionX:  -13.212259817935   PositionY:   -7.938726970990   PositionZ:   -1.522317169807
TiltX:      5.613335214187   TiltY:       -9.597274429045   TiltZ:       0.596737593393
CrystalId:  92
PositionX:  -18.296517977544   PositionY:   -7.974768354527   PositionZ:   -2.556538708795
TiltX:      5.689871831888   TiltY:       -13.398088418139   TiltZ:       0.212977464005
CrystalId:  93
PositionX:  -15.823582395457   PositionY:   -5.375515643380   PositionZ:   -1.789223875278
TiltX:      3.738544269460   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  94
PositionX:  -13.272559150425   PositionY:   -5.354509345420   PositionZ:   -1.315941082482
TiltX:      3.715434588973   TiltY:       -9.592099657331   TiltZ:       0.532413078318
CrystalId:  95
PositionX:  -18.356817310033   PositionY:   -5.390550728956   PositionZ:   -2.350162621469
TiltX:      3.766140415424   TiltY:       -13.403333613465   TiltZ:       0.278171426200
CrystalId:  96
PositionX:  -15.685195400022   PositionY:   -10.537979682137   PositionZ:   -2.289632003324
TiltX:      7.557909284949   TiltY:       -11.497744179474   TiltZ:       0.406152581296
CrystalId:  97
PositionX:  -13.134251829883   PositionY:   -10.514001148465   PositionZ:   -1.816061105681
TiltX:      7.511263628404   TiltY:       -9.604546912534   TiltZ:       0.660854185510
CrystalId:  98
PositionX:  -18.218509989491   PositionY:   -10.550042532001   PositionZ:   -2.850282644668
TiltX:      7.613545430079   TiltY:       -13.390717291921   TiltZ:       0.148002950296
CrystalId:  99
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  100
PositionX:  26.032444833628   PositionY:   0.000000000000   PositionZ:   -4.394932823805
TiltX:      0.000000000000   TiltY:       19.096825077444   TiltZ:       0.000000000000
CrystalId:  101
PositionX:  21.075633173013   PositionY:   0.000000000000   PositionZ:   -2.861762132798
TiltX:      0.000000000000   TiltY:       15.277460061955   TiltZ:       0.000000000000
CrystalId:  102
PositionX:  23.554039003321   PositionY:   2.594252985282   PositionZ:   -3.628347478301
TiltX:      -1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  103
PositionX:  26.019207902114   PositionY:   2.592759370739   PositionZ:   -4.437728434800
TiltX:      -1.930650078844   TiltY:       19.095752991358   TiltZ:       -0.067331688610
CrystalId:  104
PositionX:  21.062396241499   PositionY:   2.592759370739   PositionZ:   -2.904557743793
TiltX:      -1.891242014235   TiltY:       15.278510266823   TiltZ:       0.065957830679
CrystalId:  105
PositionX:  23.554039003321   PositionY:   -2.594252985282   PositionZ:   -3.628347478301
TiltX:      1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  106
PositionX:  26.019207902114   PositionY:   -2.592759370739   PositionZ:   -4.437728434800
TiltX:      1.930650078844   TiltY:       19.095752991358   TiltZ:       0.067331688610
CrystalId:  107
PositionX:  21.062396241499   PositionY:   -2.592759370739   PositionZ:   -2.904557743793
TiltX:      1.891242014235   TiltY:       15.278510266823   TiltZ:       -0.065957830679

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  0.000000000000   PositionY:   -7.961221871788   PositionZ:   -0.398356199580
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   1
PositionX:  -7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.748976956290   TiltY:       -5.709048052411   TiltZ:       -0.199993008557
ModuleId:   2
PositionX:  7.921456280788   PositionY:   -7.961221871788   PositionZ:   -0.794722645583
TiltX:      5.788622677155   TiltY:       5.668842078759   TiltZ:       0.599937083454
ModuleId:   3
PositionX:  15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.889784546442   TiltY:       11.377117828446   TiltZ:       0.811959833235
ModuleId:   4
PositionX:  23.449571553817   PositionY:   -7.961221871788   PositionZ:   -3.966095567861
TiltX:      6.035758264314   TiltY:       17.084652596809   TiltZ:       1.040944371753
ModuleId:   5
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  -7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   7
PositionX:  7.961221871788   PositionY:   0.000000000000   PositionZ:   -0.398356199580
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   8
PositionX:  -15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   9
PositionX:  15.842382076820   PositionY:   0.000000000000   PositionZ:   -1.605302676887
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   10
PositionX:  -15.763248145959   PositionY:   -7.961221871788   PositionZ:   -1.995719739437
TiltX:      5.648226777205   TiltY:       -11.497744179474   TiltZ:       0.406152581296
ModuleId:   11
PositionX:  23.567283290729   PositionY:   0.000000000000   PositionZ:   -3.585528085360
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
