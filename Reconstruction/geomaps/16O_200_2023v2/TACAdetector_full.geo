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
Width:          64.148998     Height:      64.185098      Thick:     30.316929, 
PositionZ:      -2.408465

CrystalN:  333
ModulesN:  37
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  -10.474377479066   PositionY:   26.017001523419   PositionZ:   -5.099737876727
TiltX:      -19.128841790109   TiltY:       -7.661656962576   TiltZ:       1.079126472909
CrystalId:  1
PositionX:  -7.898184963775   PositionY:   26.051875543163   PositionZ:   -4.792861044578
TiltX:      -19.054802614511   TiltY:       -5.856997134390   TiltZ:       0.450155739584
CrystalId:  2
PositionX:  -5.313116434134   PositionY:   26.057552836435   PositionZ:   -4.570271897032
TiltX:      -19.000826607828   TiltY:       -4.051640921916   TiltZ:       -0.174761596714
CrystalId:  3
PositionX:  -10.537378185225   PositionY:   23.552095854543   PositionZ:   -4.296818958158
TiltX:      -17.212075459441   TiltY:       -7.681501370597   TiltZ:       1.018118269794
CrystalId:  4
PositionX:  -7.961221871788   PositionY:   23.585549644429   PositionZ:   -3.989480454407
TiltX:      -17.145120106767   TiltY:       -5.856997134390   TiltZ:       0.450155739584
CrystalId:  5
PositionX:  -5.376117140293   PositionY:   23.592647167559   PositionZ:   -3.767352978464
TiltX:      -17.096479751192   TiltY:       -4.031925095543   TiltZ:       -0.114106540155
CrystalId:  6
PositionX:  -10.591435443288   PositionY:   21.060848557215   PositionZ:   -3.579063741084
TiltX:      -15.295203836108   TiltY:       -7.699312302288   TiltZ:       0.956471710828
CrystalId:  7
PositionX:  -8.015310361743   PositionY:   21.092867478938   PositionZ:   -3.271310892044
TiltX:      -15.235437599022   TiltY:       -5.856997134390   TiltZ:       0.450155739584
CrystalId:  8
PositionX:  -5.430174398355   PositionY:   21.101399870231   PositionZ:   -3.049597761390
TiltX:      -15.192156772981   TiltY:       -4.014230761053   TiltZ:       -0.052829587254
CrystalId:  9
PositionX:  -2.594252985282   PositionY:   26.013646726631   PositionZ:   -4.460671967697
TiltX:      -19.106668115313   TiltY:       -1.804551254619   TiltZ:       0.624988972712
CrystalId:  10
PositionX:  0.000000000000   PositionY:   26.028310574878   PositionZ:   -4.418317710350
TiltX:      -19.096825077444   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  11
PositionX:  2.594252985282   PositionY:   26.013646726631   PositionZ:   -4.460671967697
TiltX:      -19.106668115313   TiltY:       1.804551254619   TiltZ:       -0.624988972712
CrystalId:  12
PositionX:  -2.594252985282   PositionY:   23.549904744570   PositionZ:   -3.651732364847
TiltX:      -17.196130464691   TiltY:       -1.824375548871   TiltZ:       0.564489888993
CrystalId:  13
PositionX:  0.000000000000   PositionY:   23.563149031979   PositionZ:   -3.608912971906
TiltX:      -17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  14
PositionX:  2.594252985282   PositionY:   23.549904744570   PositionZ:   -3.651732364847
TiltX:      -17.196130464691   TiltY:       1.824375548871   TiltZ:       -0.564489888993
CrystalId:  15
PositionX:  -2.594252985282   PositionY:   21.059688899480   PositionZ:   -2.928383983988
TiltX:      -15.285552862625   TiltY:       -1.842172834120   TiltZ:       0.503362537817
CrystalId:  16
PositionX:  0.000000000000   PositionY:   21.071498914262   PositionZ:   -2.885147019344
TiltX:      -15.277460061955   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  17
PositionX:  2.594252985282   PositionY:   21.059688899480   PositionZ:   -2.928383983988
TiltX:      -15.285552862625   TiltY:       1.842172834120   TiltZ:       -0.503362537817
CrystalId:  18
PositionX:  5.313116434134   PositionY:   26.057552836435   PositionZ:   -4.570271897032
TiltX:      -19.000826607828   TiltY:       4.051640921916   TiltZ:       0.174761596714
CrystalId:  19
PositionX:  7.898184963775   PositionY:   26.051875543163   PositionZ:   -4.792861044578
TiltX:      -19.054802614511   TiltY:       5.856997134390   TiltZ:       -0.450155739584
CrystalId:  20
PositionX:  10.474377479066   PositionY:   26.017001523419   PositionZ:   -5.099737876727
TiltX:      -19.128841790109   TiltY:       7.661656962576   TiltZ:       -1.079126472909
CrystalId:  21
PositionX:  5.376117140293   PositionY:   23.592647167559   PositionZ:   -3.767352978464
TiltX:      -17.096479751192   TiltY:       4.031925095543   TiltZ:       0.114106540155
CrystalId:  22
PositionX:  7.961221871788   PositionY:   23.585549644429   PositionZ:   -3.989480454407
TiltX:      -17.145120106767   TiltY:       5.856997134390   TiltZ:       -0.450155739584
CrystalId:  23
PositionX:  10.537378185225   PositionY:   23.552095854543   PositionZ:   -4.296818958158
TiltX:      -17.212075459441   TiltY:       7.681501370597   TiltZ:       -1.018118269794
CrystalId:  24
PositionX:  5.430174398355   PositionY:   21.101399870231   PositionZ:   -3.049597761390
TiltX:      -15.192156772981   TiltY:       4.014230761053   TiltZ:       0.052829587254
CrystalId:  25
PositionX:  8.015310361743   PositionY:   21.092867478938   PositionZ:   -3.271310892044
TiltX:      -15.235437599022   TiltY:       5.856997134390   TiltZ:       -0.450155739584
CrystalId:  26
PositionX:  10.591435443288   PositionY:   21.060848557215   PositionZ:   -3.579063741084
TiltX:      -15.295203836108   TiltY:       7.699312302288   TiltZ:       -0.956471710828
CrystalId:  27
PositionX:  -18.329680461155   PositionY:   18.338901553430   PositionZ:   -4.385431378214
TiltX:      -13.589830454564   TiltY:       -13.529088569433   TiltZ:       1.058572335785
CrystalId:  28
PositionX:  -15.798142733756   PositionY:   18.375889133399   PositionZ:   -3.817887444328
TiltX:      -13.489876545294   TiltY:       -11.672471155582   TiltZ:       0.600459596177
CrystalId:  29
PositionX:  -13.248751149328   PositionY:   18.392151626772   PositionZ:   -3.335709527206
TiltX:      -13.405601078907   TiltY:       -9.815137665631   TiltZ:       0.148442808118
CrystalId:  30
PositionX:  -18.417327952726   PositionY:   15.806853318638   PositionZ:   -3.832749291356
TiltX:      -11.666259108911   TiltY:       -13.543000613359   TiltZ:       0.994702089800
CrystalId:  31
PositionX:  -15.885840533853   PositionY:   15.842382076820   PositionZ:   -3.264887848190
TiltX:      -11.580194037549   TiltY:       -11.672471155582   TiltZ:       0.600459596177
CrystalId:  32
PositionX:  -13.336398640899   PositionY:   15.860103391979   PositionZ:   -2.783027440348
TiltX:      -11.507721259156   TiltY:       -9.801411426153   TiltZ:       0.211501974222
CrystalId:  33
PositionX:  -18.487030942154   PositionY:   13.257007531099   PositionZ:   -3.366020474736
TiltX:      -9.742568907027   TiltY:       -13.554820412970   TiltZ:       0.930384025642
CrystalId:  34
PositionX:  -15.955583859868   PositionY:   13.291067577218   PositionZ:   -2.797889287375
TiltX:      -9.670511529805   TiltY:       -11.672471155582   TiltZ:       0.600459596177
CrystalId:  35
PositionX:  -13.406101630327   PositionY:   13.310257604440   PositionZ:   -2.316298623727
TiltX:      -9.609899861251   TiltY:       -9.789750564761   TiltZ:       0.274985444219
CrystalId:  36
PositionX:  -10.515497641549   PositionY:   18.347579163310   PositionZ:   -2.864840009562
TiltX:      -13.436386871549   TiltY:       -7.551829566597   TiltZ:       1.193712263132
CrystalId:  37
PositionX:  -7.938726970990   PositionY:   18.391590597737   PositionZ:   -2.564052097249
TiltX:      -13.384955231809   TiltY:       -5.694209508692   TiltZ:       0.747845260727
CrystalId:  38
PositionX:  -5.353033602792   PositionY:   18.414965340020   PositionZ:   -2.350040671657
TiltX:      -13.348149474652   TiltY:       -3.836249102035   TiltZ:       0.304853000306
CrystalId:  39
PositionX:  -10.537979682137   PositionY:   15.815050134035   PositionZ:   -2.307884796856
TiltX:      -11.519520156702   TiltY:       -7.565578896718   TiltZ:       1.131035293934
CrystalId:  40
PositionX:  -7.961221871788   PositionY:   15.857602470609   PositionZ:   -2.006776928040
TiltX:      -11.475272724065   TiltY:       -5.694209508692   TiltZ:       0.747845260727
CrystalId:  41
PositionX:  -5.375515643380   PositionY:   15.882436310745   PositionZ:   -1.793085458951
TiltX:      -11.443687594562   TiltY:       -3.822588745890   TiltZ:       0.367144043644
CrystalId:  42
PositionX:  -10.551518274629   PositionY:   13.264812449197   PositionZ:   -1.838297432699
TiltX:      -9.602582909298   TiltY:       -7.577242204868   TiltZ:       1.067927593992
CrystalId:  43
PositionX:  -7.974768354527   PositionY:   13.305895847043   PositionZ:   -1.536918158558
TiltX:      -9.565590216321   TiltY:       -5.694209508692   TiltZ:       0.747845260727
CrystalId:  44
PositionX:  -5.389054235872   PositionY:   13.332198625907   PositionZ:   -1.323498094794
TiltX:      -9.539239955707   TiltY:       -3.811001424645   TiltZ:       0.429854745176
CrystalId:  45
PositionX:  -2.594252985282   PositionY:   18.363692290233   PositionZ:   -2.223896411136
TiltX:      -13.374939286763   TiltY:       -1.857923282113   TiltZ:       0.441674923206
CrystalId:  46
PositionX:  0.000000000000   PositionY:   18.374054913794   PositionZ:   -2.180289902519
TiltX:      -13.367777554211   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  47
PositionX:  2.594252985282   PositionY:   18.363692290233   PositionZ:   -2.223896411136
TiltX:      -13.374939286763   TiltY:       1.857923282113   TiltZ:       -0.441674923206
CrystalId:  48
PositionX:  -2.594252985282   PositionY:   15.831504726064   PositionZ:   -1.664938474349
TiltX:      -11.464293875005   TiltY:       -1.871609343693   TiltZ:       0.379495682798
CrystalId:  49
PositionX:  0.000000000000   PositionY:   15.840408447575   PositionZ:   -1.621010859981
TiltX:      -11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  50
PositionX:  2.594252985282   PositionY:   15.831504726064   PositionZ:   -1.664938474349
TiltX:      -11.464293875005   TiltY:       1.871609343693   TiltZ:       -0.379495682798
CrystalId:  51
PositionX:  -2.594252985282   PositionY:   13.281519609148   PositionZ:   -1.193786971391
TiltX:      -9.553620907086   TiltY:       -1.883215768831   TiltZ:       0.316894010451
CrystalId:  52
PositionX:  0.000000000000   PositionY:   13.288954538334   PositionZ:   -1.149587046178
TiltX:      -9.548412538722   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  53
PositionX:  2.594252985282   PositionY:   13.281519609148   PositionZ:   -1.193786971391
TiltX:      -9.553620907086   TiltY:       1.883215768831   TiltZ:       -0.316894010451
CrystalId:  54
PositionX:  5.353033602792   PositionY:   18.414965340020   PositionZ:   -2.350040671657
TiltX:      -13.348149474652   TiltY:       3.836249102035   TiltZ:       -0.304853000306
CrystalId:  55
PositionX:  7.938726970990   PositionY:   18.391590597737   PositionZ:   -2.564052097249
TiltX:      -13.384955231809   TiltY:       5.694209508692   TiltZ:       -0.747845260727
CrystalId:  56
PositionX:  10.515497641549   PositionY:   18.347579163310   PositionZ:   -2.864840009562
TiltX:      -13.436386871549   TiltY:       7.551829566597   TiltZ:       -1.193712263132
CrystalId:  57
PositionX:  5.375515643380   PositionY:   15.882436310745   PositionZ:   -1.793085458951
TiltX:      -11.443687594562   TiltY:       3.822588745890   TiltZ:       -0.367144043644
CrystalId:  58
PositionX:  7.961221871788   PositionY:   15.857602470609   PositionZ:   -2.006776928040
TiltX:      -11.475272724065   TiltY:       5.694209508692   TiltZ:       -0.747845260727
CrystalId:  59
PositionX:  10.537979682137   PositionY:   15.815050134035   PositionZ:   -2.307884796856
TiltX:      -11.519520156702   TiltY:       7.565578896718   TiltZ:       -1.131035293934
CrystalId:  60
PositionX:  5.389054235872   PositionY:   13.332198625907   PositionZ:   -1.323498094794
TiltX:      -9.539239955707   TiltY:       3.811001424645   TiltZ:       -0.429854745176
CrystalId:  61
PositionX:  7.974768354527   PositionY:   13.305895847043   PositionZ:   -1.536918158558
TiltX:      -9.565590216321   TiltY:       5.694209508692   TiltZ:       -0.747845260727
CrystalId:  62
PositionX:  10.551518274629   PositionY:   13.264812449197   PositionZ:   -1.838297432699
TiltX:      -9.602582909298   TiltY:       7.577242204868   TiltZ:       -1.067927593992
CrystalId:  63
PositionX:  13.248751149328   PositionY:   18.392151626772   PositionZ:   -3.335709527206
TiltX:      -13.405601078907   TiltY:       9.815137665631   TiltZ:       -0.148442808118
CrystalId:  64
PositionX:  15.798142733756   PositionY:   18.375889133399   PositionZ:   -3.817887444328
TiltX:      -13.489876545294   TiltY:       11.672471155582   TiltZ:       -0.600459596177
CrystalId:  65
PositionX:  18.329680461155   PositionY:   18.338901553430   PositionZ:   -4.385431378214
TiltX:      -13.589830454564   TiltY:       13.529088569433   TiltZ:       -1.058572335785
CrystalId:  66
PositionX:  13.336398640899   PositionY:   15.860103391979   PositionZ:   -2.783027440348
TiltX:      -11.507721259156   TiltY:       9.801411426153   TiltZ:       -0.211501974222
CrystalId:  67
PositionX:  15.885840533853   PositionY:   15.842382076820   PositionZ:   -3.264887848190
TiltX:      -11.580194037549   TiltY:       11.672471155582   TiltZ:       -0.600459596177
CrystalId:  68
PositionX:  18.417327952726   PositionY:   15.806853318638   PositionZ:   -3.832749291356
TiltX:      -11.666259108911   TiltY:       13.543000613359   TiltZ:       -0.994702089800
CrystalId:  69
PositionX:  13.406101630327   PositionY:   13.310257604440   PositionZ:   -2.316298623727
TiltX:      -9.609899861251   TiltY:       9.789750564761   TiltZ:       -0.274985444219
CrystalId:  70
PositionX:  15.955583859868   PositionY:   13.291067577218   PositionZ:   -2.797889287375
TiltX:      -9.670511529805   TiltY:       11.672471155582   TiltZ:       -0.600459596177
CrystalId:  71
PositionX:  18.487030942154   PositionY:   13.257007531099   PositionZ:   -3.366020474736
TiltX:      -9.742568907027   TiltY:       13.554820412970   TiltZ:       -0.930384025642
CrystalId:  72
PositionX:  -25.836688636932   PositionY:   10.518863478935   PositionZ:   -5.083662021993
TiltX:      -7.815409397264   TiltY:       -19.049093593042   TiltZ:       0.584130699229
CrystalId:  73
PositionX:  -23.371050332628   PositionY:   10.538336903543   PositionZ:   -4.275945486759
TiltX:      -7.730964880028   TiltY:       -17.156954975678   TiltZ:       0.312403520096
CrystalId:  74
PositionX:  -20.879143571651   PositionY:   10.545894612653   PositionZ:   -3.553103153444
TiltX:      -7.656628286558   TiltY:       -15.264460463492   TiltZ:       0.046163658504
CrystalId:  75
PositionX:  -25.913368745827   PositionY:   7.943561796721   PositionZ:   -4.789809607141
TiltX:      -5.884967043705   TiltY:       -19.056683631853   TiltZ:       0.517287684812
CrystalId:  76
PositionX:  -23.447774346565   PositionY:   7.961551572609   PositionZ:   -3.981924658963
TiltX:      -5.821282372283   TiltY:       -17.156954975678   TiltZ:       0.312403520096
CrystalId:  77
PositionX:  -20.955823680546   PositionY:   7.970592930439   PositionZ:   -3.259250738593
TiltX:      -5.765249104464   TiltY:       -15.257024000237   TiltZ:       0.111673109924
CrystalId:  78
PositionX:  -25.963707226271   PositionY:   5.359316666411   PositionZ:   -4.581120893784
TiltX:      -3.954440690886   TiltY:       -19.062140981804   TiltZ:       0.450209466099
CrystalId:  79
PositionX:  -23.498142093746   PositionY:   5.375817823617   PositionZ:   -3.773114858975
TiltX:      -3.911599864539   TiltY:       -17.156954975678   TiltZ:       0.312403520096
CrystalId:  80
PositionX:  -21.006162160990   PositionY:   5.386347800128   PositionZ:   -3.050562025235
TiltX:      -3.873919891546   TiltY:       -15.251677446359   TiltZ:       0.177399278927
CrystalId:  81
PositionX:  -18.236267327271   PositionY:   10.532414409174   PositionZ:   -2.860082049839
TiltX:      -7.694771179972   TiltY:       -13.350709839262   TiltZ:       0.260850595632
CrystalId:  82
PositionX:  -15.702541848652   PositionY:   10.538372287295   PositionZ:   -2.301192824776
TiltX:      -7.638730030978   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  83
PositionX:  -13.151166951811   PositionY:   10.532414409174   PositionZ:   -1.829379193498
TiltX:      -7.591768317379   TiltY:       -9.565252119262   TiltZ:       -0.257379402264
CrystalId:  84
PositionX:  -18.296536583086   PositionY:   7.957077363580   PositionZ:   -2.562736201336
TiltX:      -5.771146656188   TiltY:       -13.358170591105   TiltZ:       0.195897664723
CrystalId:  85
PositionX:  -15.762845643575   PositionY:   7.961551572609   PositionZ:   -2.003676573302
TiltX:      -5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  86
PositionX:  -13.211436207626   PositionY:   7.957077363580   PositionZ:   -1.532033344995
TiltX:      -5.693793529355   TiltY:       -9.557890841965   TiltZ:       -0.193280663294
CrystalId:  87
PositionX:  -18.339097183338   PositionY:   5.372796869889   PositionZ:   -2.352758201381
TiltX:      -3.847463653990   TiltY:       -13.363505768513   TiltZ:       0.130722209293
CrystalId:  88
PositionX:  -15.805430942059   PositionY:   5.375782439865   PositionZ:   -1.793576721555
TiltX:      -3.819365015489   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  89
PositionX:  -13.253996807878   PositionY:   5.372796869889   PositionZ:   -1.322055345040
TiltX:      -3.795846783025   TiltY:       -9.552627035954   TiltZ:       -0.128971044993
CrystalId:  90
PositionX:  -10.469132648231   PositionY:   10.532414409174   PositionZ:   -1.384164658281
TiltX:      -7.668499968183   TiltY:       -7.621720502450   TiltZ:       0.256063280705
CrystalId:  91
PositionX:  -7.892272229025   PositionY:   10.538372287295   PositionZ:   -1.080994166935
TiltX:      -7.638730030978   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  92
PositionX:  -5.306542800801   PositionY:   10.532414409174   PositionZ:   -0.866226173717
TiltX:      -7.617518624170   TiltY:       -3.836261628726   TiltZ:       -0.254370998153
CrystalId:  93
PositionX:  -10.499418551516   PositionY:   7.957077363580   PositionZ:   -1.082287695131
TiltX:      -5.751416718004   TiltY:       -7.629155884274   TiltZ:       0.192299819112
CrystalId:  94
PositionX:  -7.922575488556   PositionY:   7.961551572609   PositionZ:   -0.778944204122
TiltX:      -5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  95
PositionX:  -5.336828704086   PositionY:   7.957077363580   PositionZ:   -0.564349210568
TiltX:      -5.713130743513   TiltY:       -3.828875481922   TiltZ:       -0.191023974458
CrystalId:  96
PositionX:  -10.520805678427   PositionY:   5.372796869889   PositionZ:   -0.869109938408
TiltX:      -3.834297718900   TiltY:       -7.634472857871   TiltZ:       0.128320135143
CrystalId:  97
PositionX:  -7.943975026577   PositionY:   5.375782439865   PositionZ:   -0.565642738764
TiltX:      -3.819365015489   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  98
PositionX:  -5.358215830997   PositionY:   5.372796869889   PositionZ:   -0.351171453845
TiltX:      -3.808749925517   TiltY:       -3.823593833643   TiltZ:       -0.127466405094
CrystalId:  99
PositionX:  -2.594252985282   PositionY:   10.532414409174   PositionZ:   -0.738436614280
TiltX:      -7.642924785518   TiltY:       -1.892729624041   TiltZ:       0.253939578138
CrystalId:  100
PositionX:  0.000000000000   PositionY:   10.538372287295   PositionZ:   -0.694013475612
TiltX:      -7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  101
PositionX:  2.594252985282   PositionY:   10.532414409174   PositionZ:   -0.738436614280
TiltX:      -7.642924785518   TiltY:       1.892729624041   TiltZ:       -0.253939578138
CrystalId:  102
PositionX:  -2.594252985282   PositionY:   7.957077363580   PositionZ:   -0.435044233330
TiltX:      -5.732210015987   TiltY:       -1.900140307152   TiltZ:       0.190702457215
CrystalId:  103
PositionX:  0.000000000000   PositionY:   7.961551572609   PositionZ:   -0.390447226543
TiltX:      -5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  104
PositionX:  2.594252985282   PositionY:   7.957077363580   PositionZ:   -0.435044233330
TiltX:      -5.732210015987   TiltY:       1.900140307152   TiltZ:       -0.190702457215
CrystalId:  105
PositionX:  -2.594252985282   PositionY:   5.372796869889   PositionZ:   -0.220796327486
TiltX:      -3.821481187277   TiltY:       -1.905439559434   TiltZ:       0.127253039168
CrystalId:  106
PositionX:  0.000000000000   PositionY:   5.375782439865   PositionZ:   -0.176074991049
TiltX:      -3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  107
PositionX:  2.594252985282   PositionY:   5.372796869889   PositionZ:   -0.220796327486
TiltX:      -3.821481187277   TiltY:       1.905439559434   TiltZ:       -0.127253039168
CrystalId:  108
PositionX:  5.306542800801   PositionY:   10.532414409174   PositionZ:   -0.866226173717
TiltX:      -7.617518624170   TiltY:       3.836261628726   TiltZ:       0.254370998153
CrystalId:  109
PositionX:  7.892272229025   PositionY:   10.538372287295   PositionZ:   -1.080994166935
TiltX:      -7.638730030978   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  110
PositionX:  10.469132648231   PositionY:   10.532414409174   PositionZ:   -1.384164658281
TiltX:      -7.668499968183   TiltY:       7.621720502450   TiltZ:       -0.256063280705
CrystalId:  111
PositionX:  5.336828704086   PositionY:   7.957077363580   PositionZ:   -0.564349210568
TiltX:      -5.713130743513   TiltY:       3.828875481922   TiltZ:       0.191023974458
CrystalId:  112
PositionX:  7.922575488556   PositionY:   7.961551572609   PositionZ:   -0.778944204122
TiltX:      -5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  113
PositionX:  10.499418551516   PositionY:   7.957077363580   PositionZ:   -1.082287695131
TiltX:      -5.751416718004   TiltY:       7.629155884274   TiltZ:       -0.192299819112
CrystalId:  114
PositionX:  5.358215830997   PositionY:   5.372796869889   PositionZ:   -0.351171453845
TiltX:      -3.808749925517   TiltY:       3.823593833643   TiltZ:       0.127466405094
CrystalId:  115
PositionX:  7.943975026577   PositionY:   5.375782439865   PositionZ:   -0.565642738764
TiltX:      -3.819365015489   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  116
PositionX:  10.520805678427   PositionY:   5.372796869889   PositionZ:   -0.869109938408
TiltX:      -3.834297718900   TiltY:       7.634472857871   TiltZ:       -0.128320135143
CrystalId:  117
PositionX:  13.151166951811   PositionY:   10.532414409174   PositionZ:   -1.829379193498
TiltX:      -7.591768317379   TiltY:       9.565252119262   TiltZ:       0.257379402264
CrystalId:  118
PositionX:  15.702541848652   PositionY:   10.538372287295   PositionZ:   -2.301192824776
TiltX:      -7.638730030978   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  119
PositionX:  18.236267327271   PositionY:   10.532414409174   PositionZ:   -2.860082049839
TiltX:      -7.694771179972   TiltY:       13.350709839262   TiltZ:       -0.260850595632
CrystalId:  120
PositionX:  13.211436207626   PositionY:   7.957077363580   PositionZ:   -1.532033344995
TiltX:      -5.693793529355   TiltY:       9.557890841965   TiltZ:       0.193280663294
CrystalId:  121
PositionX:  15.762845643575   PositionY:   7.961551572609   PositionZ:   -2.003676573302
TiltX:      -5.729047523233   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  122
PositionX:  18.296536583086   PositionY:   7.957077363580   PositionZ:   -2.562736201336
TiltX:      -5.771146656188   TiltY:       13.358170591105   TiltZ:       -0.195897664723
CrystalId:  123
PositionX:  13.253996807878   PositionY:   5.372796869889   PositionZ:   -1.322055345040
TiltX:      -3.795846783025   TiltY:       9.552627035954   TiltZ:       0.128971044992
CrystalId:  124
PositionX:  15.805430942059   PositionY:   5.375782439865   PositionZ:   -1.793576721555
TiltX:      -3.819365015489   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  125
PositionX:  18.339097183338   PositionY:   5.372796869889   PositionZ:   -2.352758201381
TiltX:      -3.847463653990   TiltY:       13.363505768513   TiltZ:       -0.130722209293
CrystalId:  126
PositionX:  20.879143571651   PositionY:   10.545894612653   PositionZ:   -3.553103153444
TiltX:      -7.656628286558   TiltY:       15.264460463492   TiltZ:       -0.046163658504
CrystalId:  127
PositionX:  23.371050332628   PositionY:   10.538336903543   PositionZ:   -4.275945486759
TiltX:      -7.730964880028   TiltY:       17.156954975678   TiltZ:       -0.312403520096
CrystalId:  128
PositionX:  25.836688636932   PositionY:   10.518863478935   PositionZ:   -5.083662021993
TiltX:      -7.815409397264   TiltY:       19.049093593042   TiltZ:       -0.584130699229
CrystalId:  129
PositionX:  20.955823680546   PositionY:   7.970592930439   PositionZ:   -3.259250738593
TiltX:      -5.765249104464   TiltY:       15.257024000237   TiltZ:       -0.111673109924
CrystalId:  130
PositionX:  23.447774346565   PositionY:   7.961551572609   PositionZ:   -3.981924658963
TiltX:      -5.821282372283   TiltY:       17.156954975678   TiltZ:       -0.312403520096
CrystalId:  131
PositionX:  25.913368745827   PositionY:   7.943561796721   PositionZ:   -4.789809607141
TiltX:      -5.884967043705   TiltY:       19.056683631853   TiltZ:       -0.517287684812
CrystalId:  132
PositionX:  21.006162160990   PositionY:   5.386347800128   PositionZ:   -3.050562025235
TiltX:      -3.873919891546   TiltY:       15.251677446359   TiltZ:       -0.177399278927
CrystalId:  133
PositionX:  23.498142093746   PositionY:   5.375817823617   PositionZ:   -3.773114858975
TiltX:      -3.911599864539   TiltY:       17.156954975678   TiltZ:       -0.312403520096
CrystalId:  134
PositionX:  25.963707226271   PositionY:   5.359316666411   PositionZ:   -4.581120893784
TiltX:      -3.954440690886   TiltY:       19.062140981804   TiltZ:       -0.450209466099
CrystalId:  135
PositionX:  -26.015073643363   PositionY:   2.592759370739   PositionZ:   -4.461113321346
TiltX:      -1.930650078844   TiltY:       -19.095752991358   TiltZ:       0.067331688610
CrystalId:  136
PositionX:  -23.549904744570   PositionY:   2.594252985282   PositionZ:   -3.651732364847
TiltX:      -1.909682507744   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  137
PositionX:  -21.058261982748   PositionY:   2.592759370739   PositionZ:   -2.927942630339
TiltX:      -1.891242014235   TiltY:       -15.278510266823   TiltZ:       -0.065957830679
CrystalId:  138
PositionX:  -26.028310574878   PositionY:   0.000000000000   PositionZ:   -4.418317710350
TiltX:      0.000000000000   TiltY:       -19.096825077444   TiltZ:       0.000000000000
CrystalId:  139
PositionX:  -23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  140
PositionX:  -21.071498914262   PositionY:   0.000000000000   PositionZ:   -2.885147019344
TiltX:      0.000000000000   TiltY:       -15.277460061955   TiltZ:       0.000000000000
CrystalId:  141
PositionX:  -26.015073643363   PositionY:   -2.592759370739   PositionZ:   -4.461113321346
TiltX:      1.930650078844   TiltY:       -19.095752991358   TiltZ:       -0.067331688610
CrystalId:  142
PositionX:  -23.549904744570   PositionY:   -2.594252985282   PositionZ:   -3.651732364847
TiltX:      1.909682507744   TiltY:       -17.187142569699   TiltZ:       0.000000000000
CrystalId:  143
PositionX:  -21.058261982748   PositionY:   -2.592759370739   PositionZ:   -2.927942630339
TiltX:      1.891242014235   TiltY:       -15.278510266823   TiltZ:       0.065957830679
CrystalId:  144
PositionX:  -18.365156137421   PositionY:   2.592759370739   PositionZ:   -2.224193119434
TiltX:      -1.923741599917   TiltY:       -13.366709304039   TiltZ:       0.065398252682
CrystalId:  145
PositionX:  -15.831504726064   PositionY:   2.594252985282   PositionZ:   -1.664938474349
TiltX:      -1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  146
PositionX:  -13.280055761960   PositionY:   2.592759370739   PositionZ:   -1.193490263093
TiltX:      -1.897918721691   TiltY:       -9.549466450813   TiltZ:       -0.064520718033
CrystalId:  147
PositionX:  -18.374054913794   PositionY:   0.000000000000   PositionZ:   -2.180289902519
TiltX:      0.000000000000   TiltY:       -13.367777554211   TiltZ:       0.000000000000
CrystalId:  148
PositionX:  -15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  149
PositionX:  -13.288954538334   PositionY:   0.000000000000   PositionZ:   -1.149587046178
TiltX:      0.000000000000   TiltY:       -9.548412538722   TiltZ:       0.000000000000
CrystalId:  150
PositionX:  -18.365156137421   PositionY:   -2.592759370739   PositionZ:   -2.224193119434
TiltX:      1.923741599917   TiltY:       -13.366709304039   TiltZ:       -0.065398252682
CrystalId:  151
PositionX:  -15.831504726064   PositionY:   -2.594252985282   PositionZ:   -1.664938474349
TiltX:      1.909682507744   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  152
PositionX:  -13.280055761960   PositionY:   -2.592759370739   PositionZ:   -1.193490263093
TiltX:      1.897918721691   TiltY:       -9.549466450813   TiltZ:       0.064520718033
CrystalId:  153
PositionX:  -10.533900563246   PositionY:   2.592759370739   PositionZ:   -0.738585713164
TiltX:      -1.917154837490   TiltY:       -7.637665438087   TiltZ:       0.064196161297
CrystalId:  154
PositionX:  -7.957077363580   PositionY:   2.594252985282   PositionZ:   -0.435044233330
TiltX:      -1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  155
PositionX:  -5.371310715817   PositionY:   2.592759370739   PositionZ:   -0.220647228601
TiltX:      -1.904373794316   TiltY:       -3.820422511741   TiltZ:       -0.063768344486
CrystalId:  156
PositionX:  -10.538372287295   PositionY:   0.000000000000   PositionZ:   -0.694013475612
TiltX:      0.000000000000   TiltY:       -7.638730030978   TiltZ:       0.000000000000
CrystalId:  157
PositionX:  -7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  158
PositionX:  -5.375782439865   PositionY:   0.000000000000   PositionZ:   -0.176074991049
TiltX:      0.000000000000   TiltY:       -3.819365015489   TiltZ:       0.000000000000
CrystalId:  159
PositionX:  -10.533900563246   PositionY:   -2.592759370739   PositionZ:   -0.738585713164
TiltX:      1.917154837490   TiltY:       -7.637665438087   TiltZ:       -0.064196161297
CrystalId:  160
PositionX:  -7.957077363580   PositionY:   -2.594252985282   PositionZ:   -0.435044233330
TiltX:      1.909682507744   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  161
PositionX:  -5.371310715817   PositionY:   -2.592759370739   PositionZ:   -0.220647228601
TiltX:      1.904373794316   TiltY:       -3.820422511741   TiltZ:       0.063768344486
CrystalId:  162
PositionX:  -2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       -1.908621475036   TiltZ:       0.063661955951
CrystalId:  163
PositionX:  0.000000000000   PositionY:   2.594252985282   PositionZ:   -0.044820883088
TiltX:      -1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  164
PositionX:  2.594252985282   PositionY:   2.592759370739   PositionZ:   -0.089616872600
TiltX:      -1.910742950827   TiltY:       1.908621475036   TiltZ:       -0.063661955951
CrystalId:  165
PositionX:  -2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       -1.909682507744   TiltZ:       0.000000000000
CrystalId:  166
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  167
PositionX:  2.594252985282   PositionY:   0.000000000000   PositionZ:   -0.044820883088
TiltX:      0.000000000000   TiltY:       1.909682507744   TiltZ:       0.000000000000
CrystalId:  168
PositionX:  -2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       -1.908621475036   TiltZ:       -0.063661955951
CrystalId:  169
PositionX:  0.000000000000   PositionY:   -2.594252985282   PositionZ:   -0.044820883088
TiltX:      1.909682507744   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  170
PositionX:  2.594252985282   PositionY:   -2.592759370739   PositionZ:   -0.089616872600
TiltX:      1.910742950827   TiltY:       1.908621475036   TiltZ:       0.063661955951
CrystalId:  171
PositionX:  5.371310715817   PositionY:   2.592759370739   PositionZ:   -0.220647228601
TiltX:      -1.904373794316   TiltY:       3.820422511741   TiltZ:       0.063768344486
CrystalId:  172
PositionX:  7.957077363580   PositionY:   2.594252985282   PositionZ:   -0.435044233330
TiltX:      -1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  173
PositionX:  10.533900563246   PositionY:   2.592759370739   PositionZ:   -0.738585713164
TiltX:      -1.917154837490   TiltY:       7.637665438087   TiltZ:       -0.064196161297
CrystalId:  174
PositionX:  5.375782439865   PositionY:   0.000000000000   PositionZ:   -0.176074991049
TiltX:      0.000000000000   TiltY:       3.819365015489   TiltZ:       0.000000000000
CrystalId:  175
PositionX:  7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  176
PositionX:  10.538372287295   PositionY:   0.000000000000   PositionZ:   -0.694013475612
TiltX:      0.000000000000   TiltY:       7.638730030978   TiltZ:       0.000000000000
CrystalId:  177
PositionX:  5.371310715817   PositionY:   -2.592759370739   PositionZ:   -0.220647228601
TiltX:      1.904373794316   TiltY:       3.820422511741   TiltZ:       -0.063768344486
CrystalId:  178
PositionX:  7.957077363580   PositionY:   -2.594252985282   PositionZ:   -0.435044233330
TiltX:      1.909682507744   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  179
PositionX:  10.533900563246   PositionY:   -2.592759370739   PositionZ:   -0.738585713164
TiltX:      1.917154837490   TiltY:       7.637665438087   TiltZ:       0.064196161297
CrystalId:  180
PositionX:  13.280055761960   PositionY:   2.592759370739   PositionZ:   -1.193490263093
TiltX:      -1.897918721691   TiltY:       9.549466450813   TiltZ:       0.064520718033
CrystalId:  181
PositionX:  15.831504726064   PositionY:   2.594252985282   PositionZ:   -1.664938474349
TiltX:      -1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  182
PositionX:  18.365156137421   PositionY:   2.592759370739   PositionZ:   -2.224193119434
TiltX:      -1.923741599917   TiltY:       13.366709304039   TiltZ:       -0.065398252682
CrystalId:  183
PositionX:  13.288954538334   PositionY:   0.000000000000   PositionZ:   -1.149587046178
TiltX:      0.000000000000   TiltY:       9.548412538722   TiltZ:       0.000000000000
CrystalId:  184
PositionX:  15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  185
PositionX:  18.374054913794   PositionY:   0.000000000000   PositionZ:   -2.180289902519
TiltX:      0.000000000000   TiltY:       13.367777554211   TiltZ:       0.000000000000
CrystalId:  186
PositionX:  13.280055761960   PositionY:   -2.592759370739   PositionZ:   -1.193490263093
TiltX:      1.897918721691   TiltY:       9.549466450813   TiltZ:       -0.064520718033
CrystalId:  187
PositionX:  15.831504726064   PositionY:   -2.594252985282   PositionZ:   -1.664938474349
TiltX:      1.909682507744   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  188
PositionX:  18.365156137421   PositionY:   -2.592759370739   PositionZ:   -2.224193119434
TiltX:      1.923741599917   TiltY:       13.366709304039   TiltZ:       0.065398252682
CrystalId:  189
PositionX:  21.058261982748   PositionY:   2.592759370739   PositionZ:   -2.927942630339
TiltX:      -1.891242014235   TiltY:       15.278510266823   TiltZ:       0.065957830679
CrystalId:  190
PositionX:  23.549904744570   PositionY:   2.594252985282   PositionZ:   -3.651732364847
TiltX:      -1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  191
PositionX:  26.015073643363   PositionY:   2.592759370739   PositionZ:   -4.461113321346
TiltX:      -1.930650078844   TiltY:       19.095752991358   TiltZ:       -0.067331688610
CrystalId:  192
PositionX:  21.071498914262   PositionY:   0.000000000000   PositionZ:   -2.885147019344
TiltX:      0.000000000000   TiltY:       15.277460061955   TiltZ:       0.000000000000
CrystalId:  193
PositionX:  23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  194
PositionX:  26.028310574878   PositionY:   0.000000000000   PositionZ:   -4.418317710350
TiltX:      0.000000000000   TiltY:       19.096825077444   TiltZ:       0.000000000000
CrystalId:  195
PositionX:  21.058261982748   PositionY:   -2.592759370739   PositionZ:   -2.927942630339
TiltX:      1.891242014235   TiltY:       15.278510266823   TiltZ:       -0.065957830679
CrystalId:  196
PositionX:  23.549904744570   PositionY:   -2.594252985282   PositionZ:   -3.651732364847
TiltX:      1.909682507744   TiltY:       17.187142569699   TiltZ:       0.000000000000
CrystalId:  197
PositionX:  26.015073643363   PositionY:   -2.592759370739   PositionZ:   -4.461113321346
TiltX:      1.930650078844   TiltY:       19.095752991358   TiltZ:       0.067331688610
CrystalId:  198
PositionX:  -25.963707226271   PositionY:   -5.359316666411   PositionZ:   -4.581120893784
TiltX:      3.954440690886   TiltY:       -19.062140981804   TiltZ:       -0.450209466099
CrystalId:  199
PositionX:  -23.498142093746   PositionY:   -5.375817823617   PositionZ:   -3.773114858975
TiltX:      3.911599864539   TiltY:       -17.156954975678   TiltZ:       -0.312403520096
CrystalId:  200
PositionX:  -21.006162160990   PositionY:   -5.386347800128   PositionZ:   -3.050562025235
TiltX:      3.873919891546   TiltY:       -15.251677446359   TiltZ:       -0.177399278927
CrystalId:  201
PositionX:  -25.913368745827   PositionY:   -7.943561796721   PositionZ:   -4.789809607141
TiltX:      5.884967043705   TiltY:       -19.056683631853   TiltZ:       -0.517287684812
CrystalId:  202
PositionX:  -23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.821282372283   TiltY:       -17.156954975678   TiltZ:       -0.312403520096
CrystalId:  203
PositionX:  -20.955823680546   PositionY:   -7.970592930439   PositionZ:   -3.259250738593
TiltX:      5.765249104464   TiltY:       -15.257024000237   TiltZ:       -0.111673109924
CrystalId:  204
PositionX:  -25.836688636932   PositionY:   -10.518863478935   PositionZ:   -5.083662021993
TiltX:      7.815409397264   TiltY:       -19.049093593042   TiltZ:       -0.584130699229
CrystalId:  205
PositionX:  -23.371050332628   PositionY:   -10.538336903543   PositionZ:   -4.275945486759
TiltX:      7.730964880028   TiltY:       -17.156954975678   TiltZ:       -0.312403520096
CrystalId:  206
PositionX:  -20.879143571651   PositionY:   -10.545894612653   PositionZ:   -3.553103153444
TiltX:      7.656628286558   TiltY:       -15.264460463492   TiltZ:       -0.046163658504
CrystalId:  207
PositionX:  -18.339097183338   PositionY:   -5.372796869889   PositionZ:   -2.352758201381
TiltX:      3.847463653990   TiltY:       -13.363505768513   TiltZ:       -0.130722209293
CrystalId:  208
PositionX:  -15.805430942059   PositionY:   -5.375782439865   PositionZ:   -1.793576721555
TiltX:      3.819365015489   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  209
PositionX:  -13.253996807878   PositionY:   -5.372796869889   PositionZ:   -1.322055345040
TiltX:      3.795846783025   TiltY:       -9.552627035954   TiltZ:       0.128971044992
CrystalId:  210
PositionX:  -18.296536583086   PositionY:   -7.957077363580   PositionZ:   -2.562736201336
TiltX:      5.771146656188   TiltY:       -13.358170591105   TiltZ:       -0.195897664723
CrystalId:  211
PositionX:  -15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  212
PositionX:  -13.211436207626   PositionY:   -7.957077363580   PositionZ:   -1.532033344995
TiltX:      5.693793529355   TiltY:       -9.557890841965   TiltZ:       0.193280663294
CrystalId:  213
PositionX:  -18.236267327271   PositionY:   -10.532414409174   PositionZ:   -2.860082049839
TiltX:      7.694771179972   TiltY:       -13.350709839262   TiltZ:       -0.260850595632
CrystalId:  214
PositionX:  -15.702541848652   PositionY:   -10.538372287295   PositionZ:   -2.301192824776
TiltX:      7.638730030978   TiltY:       -11.458095046466   TiltZ:       0.000000000000
CrystalId:  215
PositionX:  -13.151166951811   PositionY:   -10.532414409174   PositionZ:   -1.829379193498
TiltX:      7.591768317379   TiltY:       -9.565252119262   TiltZ:       0.257379402264
CrystalId:  216
PositionX:  -10.520805678427   PositionY:   -5.372796869889   PositionZ:   -0.869109938408
TiltX:      3.834297718900   TiltY:       -7.634472857871   TiltZ:       -0.128320135143
CrystalId:  217
PositionX:  -7.943975026577   PositionY:   -5.375782439865   PositionZ:   -0.565642738764
TiltX:      3.819365015489   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  218
PositionX:  -5.358215830997   PositionY:   -5.372796869889   PositionZ:   -0.351171453845
TiltX:      3.808749925517   TiltY:       -3.823593833643   TiltZ:       0.127466405094
CrystalId:  219
PositionX:  -10.499418551516   PositionY:   -7.957077363580   PositionZ:   -1.082287695131
TiltX:      5.751416718004   TiltY:       -7.629155884274   TiltZ:       -0.192299819112
CrystalId:  220
PositionX:  -7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  221
PositionX:  -5.336828704086   PositionY:   -7.957077363580   PositionZ:   -0.564349210568
TiltX:      5.713130743513   TiltY:       -3.828875481922   TiltZ:       0.191023974458
CrystalId:  222
PositionX:  -10.469132648231   PositionY:   -10.532414409174   PositionZ:   -1.384164658281
TiltX:      7.668499968183   TiltY:       -7.621720502450   TiltZ:       -0.256063280705
CrystalId:  223
PositionX:  -7.892272229025   PositionY:   -10.538372287295   PositionZ:   -1.080994166935
TiltX:      7.638730030978   TiltY:       -5.729047523233   TiltZ:       0.000000000000
CrystalId:  224
PositionX:  -5.306542800801   PositionY:   -10.532414409174   PositionZ:   -0.866226173717
TiltX:      7.617518624170   TiltY:       -3.836261628726   TiltZ:       0.254370998153
CrystalId:  225
PositionX:  -2.594252985282   PositionY:   -5.372796869889   PositionZ:   -0.220796327486
TiltX:      3.821481187277   TiltY:       -1.905439559434   TiltZ:       -0.127253039168
CrystalId:  226
PositionX:  0.000000000000   PositionY:   -5.375782439865   PositionZ:   -0.176074991049
TiltX:      3.819365015489   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  227
PositionX:  2.594252985282   PositionY:   -5.372796869889   PositionZ:   -0.220796327486
TiltX:      3.821481187277   TiltY:       1.905439559434   TiltZ:       0.127253039168
CrystalId:  228
PositionX:  -2.594252985282   PositionY:   -7.957077363580   PositionZ:   -0.435044233330
TiltX:      5.732210015987   TiltY:       -1.900140307152   TiltZ:       -0.190702457215
CrystalId:  229
PositionX:  0.000000000000   PositionY:   -7.961551572609   PositionZ:   -0.390447226543
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  230
PositionX:  2.594252985282   PositionY:   -7.957077363580   PositionZ:   -0.435044233330
TiltX:      5.732210015987   TiltY:       1.900140307152   TiltZ:       0.190702457215
CrystalId:  231
PositionX:  -2.594252985282   PositionY:   -10.532414409174   PositionZ:   -0.738436614280
TiltX:      7.642924785518   TiltY:       -1.892729624041   TiltZ:       -0.253939578138
CrystalId:  232
PositionX:  0.000000000000   PositionY:   -10.538372287295   PositionZ:   -0.694013475612
TiltX:      7.638730030978   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  233
PositionX:  2.594252985282   PositionY:   -10.532414409174   PositionZ:   -0.738436614280
TiltX:      7.642924785518   TiltY:       1.892729624041   TiltZ:       0.253939578138
CrystalId:  234
PositionX:  5.358215830997   PositionY:   -5.372796869889   PositionZ:   -0.351171453845
TiltX:      3.808749925517   TiltY:       3.823593833643   TiltZ:       -0.127466405094
CrystalId:  235
PositionX:  7.943975026577   PositionY:   -5.375782439865   PositionZ:   -0.565642738764
TiltX:      3.819365015489   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  236
PositionX:  10.520805678427   PositionY:   -5.372796869889   PositionZ:   -0.869109938408
TiltX:      3.834297718900   TiltY:       7.634472857871   TiltZ:       0.128320135143
CrystalId:  237
PositionX:  5.336828704086   PositionY:   -7.957077363580   PositionZ:   -0.564349210568
TiltX:      5.713130743513   TiltY:       3.828875481922   TiltZ:       -0.191023974458
CrystalId:  238
PositionX:  7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  239
PositionX:  10.499418551516   PositionY:   -7.957077363580   PositionZ:   -1.082287695131
TiltX:      5.751416718004   TiltY:       7.629155884274   TiltZ:       0.192299819112
CrystalId:  240
PositionX:  5.306542800801   PositionY:   -10.532414409174   PositionZ:   -0.866226173717
TiltX:      7.617518624170   TiltY:       3.836261628726   TiltZ:       -0.254370998153
CrystalId:  241
PositionX:  7.892272229025   PositionY:   -10.538372287295   PositionZ:   -1.080994166935
TiltX:      7.638730030978   TiltY:       5.729047523233   TiltZ:       0.000000000000
CrystalId:  242
PositionX:  10.469132648231   PositionY:   -10.532414409174   PositionZ:   -1.384164658281
TiltX:      7.668499968183   TiltY:       7.621720502450   TiltZ:       0.256063280705
CrystalId:  243
PositionX:  13.253996807878   PositionY:   -5.372796869889   PositionZ:   -1.322055345040
TiltX:      3.795846783025   TiltY:       9.552627035954   TiltZ:       -0.128971044993
CrystalId:  244
PositionX:  15.805430942059   PositionY:   -5.375782439865   PositionZ:   -1.793576721555
TiltX:      3.819365015489   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  245
PositionX:  18.339097183338   PositionY:   -5.372796869889   PositionZ:   -2.352758201381
TiltX:      3.847463653990   TiltY:       13.363505768513   TiltZ:       0.130722209293
CrystalId:  246
PositionX:  13.211436207626   PositionY:   -7.957077363580   PositionZ:   -1.532033344995
TiltX:      5.693793529355   TiltY:       9.557890841965   TiltZ:       -0.193280663294
CrystalId:  247
PositionX:  15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  248
PositionX:  18.296536583086   PositionY:   -7.957077363580   PositionZ:   -2.562736201336
TiltX:      5.771146656188   TiltY:       13.358170591105   TiltZ:       0.195897664723
CrystalId:  249
PositionX:  13.151166951811   PositionY:   -10.532414409174   PositionZ:   -1.829379193498
TiltX:      7.591768317379   TiltY:       9.565252119262   TiltZ:       -0.257379402264
CrystalId:  250
PositionX:  15.702541848652   PositionY:   -10.538372287295   PositionZ:   -2.301192824776
TiltX:      7.638730030978   TiltY:       11.458095046466   TiltZ:       0.000000000000
CrystalId:  251
PositionX:  18.236267327271   PositionY:   -10.532414409174   PositionZ:   -2.860082049839
TiltX:      7.694771179972   TiltY:       13.350709839262   TiltZ:       0.260850595632
CrystalId:  252
PositionX:  21.006162160990   PositionY:   -5.386347800128   PositionZ:   -3.050562025235
TiltX:      3.873919891546   TiltY:       15.251677446359   TiltZ:       0.177399278927
CrystalId:  253
PositionX:  23.498142093746   PositionY:   -5.375817823617   PositionZ:   -3.773114858975
TiltX:      3.911599864539   TiltY:       17.156954975678   TiltZ:       0.312403520096
CrystalId:  254
PositionX:  25.963707226271   PositionY:   -5.359316666411   PositionZ:   -4.581120893784
TiltX:      3.954440690886   TiltY:       19.062140981804   TiltZ:       0.450209466099
CrystalId:  255
PositionX:  20.955823680546   PositionY:   -7.970592930439   PositionZ:   -3.259250738593
TiltX:      5.765249104464   TiltY:       15.257024000237   TiltZ:       0.111673109924
CrystalId:  256
PositionX:  23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.821282372283   TiltY:       17.156954975678   TiltZ:       0.312403520096
CrystalId:  257
PositionX:  25.913368745827   PositionY:   -7.943561796721   PositionZ:   -4.789809607141
TiltX:      5.884967043705   TiltY:       19.056683631853   TiltZ:       0.517287684812
CrystalId:  258
PositionX:  20.879143571651   PositionY:   -10.545894612653   PositionZ:   -3.553103153444
TiltX:      7.656628286558   TiltY:       15.264460463492   TiltZ:       0.046163658504
CrystalId:  259
PositionX:  23.371050332628   PositionY:   -10.538336903543   PositionZ:   -4.275945486759
TiltX:      7.730964880028   TiltY:       17.156954975678   TiltZ:       0.312403520096
CrystalId:  260
PositionX:  25.836688636932   PositionY:   -10.518863478935   PositionZ:   -5.083662021993
TiltX:      7.815409397264   TiltY:       19.049093593042   TiltZ:       0.584130699229
CrystalId:  261
PositionX:  -18.487030942154   PositionY:   -13.257007531099   PositionZ:   -3.366020474736
TiltX:      9.742568907027   TiltY:       -13.554820412970   TiltZ:       -0.930384025642
CrystalId:  262
PositionX:  -15.955583859868   PositionY:   -13.291067577218   PositionZ:   -2.797889287375
TiltX:      9.670511529805   TiltY:       -11.672471155582   TiltZ:       -0.600459596177
CrystalId:  263
PositionX:  -13.406101630327   PositionY:   -13.310257604440   PositionZ:   -2.316298623727
TiltX:      9.609899861251   TiltY:       -9.789750564761   TiltZ:       -0.274985444219
CrystalId:  264
PositionX:  -18.417327952726   PositionY:   -15.806853318638   PositionZ:   -3.832749291356
TiltX:      11.666259108911   TiltY:       -13.543000613359   TiltZ:       -0.994702089800
CrystalId:  265
PositionX:  -15.885840533853   PositionY:   -15.842382076820   PositionZ:   -3.264887848190
TiltX:      11.580194037549   TiltY:       -11.672471155582   TiltZ:       -0.600459596177
CrystalId:  266
PositionX:  -13.336398640899   PositionY:   -15.860103391979   PositionZ:   -2.783027440348
TiltX:      11.507721259156   TiltY:       -9.801411426153   TiltZ:       -0.211501974222
CrystalId:  267
PositionX:  -18.329680461155   PositionY:   -18.338901553430   PositionZ:   -4.385431378214
TiltX:      13.589830454564   TiltY:       -13.529088569433   TiltZ:       -1.058572335785
CrystalId:  268
PositionX:  -15.798142733756   PositionY:   -18.375889133399   PositionZ:   -3.817887444328
TiltX:      13.489876545294   TiltY:       -11.672471155582   TiltZ:       -0.600459596177
CrystalId:  269
PositionX:  -13.248751149328   PositionY:   -18.392151626772   PositionZ:   -3.335709527206
TiltX:      13.405601078907   TiltY:       -9.815137665631   TiltZ:       -0.148442808118
CrystalId:  270
PositionX:  -10.551518274629   PositionY:   -13.264812449197   PositionZ:   -1.838297432699
TiltX:      9.602582909298   TiltY:       -7.577242204868   TiltZ:       -1.067927593992
CrystalId:  271
PositionX:  -7.974768354527   PositionY:   -13.305895847043   PositionZ:   -1.536918158558
TiltX:      9.565590216321   TiltY:       -5.694209508692   TiltZ:       -0.747845260727
CrystalId:  272
PositionX:  -5.389054235872   PositionY:   -13.332198625907   PositionZ:   -1.323498094794
TiltX:      9.539239955707   TiltY:       -3.811001424645   TiltZ:       -0.429854745176
CrystalId:  273
PositionX:  -10.537979682137   PositionY:   -15.815050134035   PositionZ:   -2.307884796856
TiltX:      11.519520156702   TiltY:       -7.565578896718   TiltZ:       -1.131035293934
CrystalId:  274
PositionX:  -7.961221871788   PositionY:   -15.857602470609   PositionZ:   -2.006776928040
TiltX:      11.475272724065   TiltY:       -5.694209508692   TiltZ:       -0.747845260727
CrystalId:  275
PositionX:  -5.375515643380   PositionY:   -15.882436310745   PositionZ:   -1.793085458951
TiltX:      11.443687594562   TiltY:       -3.822588745890   TiltZ:       -0.367144043644
CrystalId:  276
PositionX:  -10.515497641549   PositionY:   -18.347579163310   PositionZ:   -2.864840009562
TiltX:      13.436386871549   TiltY:       -7.551829566597   TiltZ:       -1.193712263132
CrystalId:  277
PositionX:  -7.938726970990   PositionY:   -18.391590597737   PositionZ:   -2.564052097249
TiltX:      13.384955231809   TiltY:       -5.694209508692   TiltZ:       -0.747845260727
CrystalId:  278
PositionX:  -5.353033602792   PositionY:   -18.414965340020   PositionZ:   -2.350040671657
TiltX:      13.348149474652   TiltY:       -3.836249102035   TiltZ:       -0.304853000306
CrystalId:  279
PositionX:  -2.594252985282   PositionY:   -13.281519609148   PositionZ:   -1.193786971391
TiltX:      9.553620907086   TiltY:       -1.883215768831   TiltZ:       -0.316894010451
CrystalId:  280
PositionX:  0.000000000000   PositionY:   -13.288954538334   PositionZ:   -1.149587046178
TiltX:      9.548412538722   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  281
PositionX:  2.594252985282   PositionY:   -13.281519609148   PositionZ:   -1.193786971391
TiltX:      9.553620907086   TiltY:       1.883215768831   TiltZ:       0.316894010451
CrystalId:  282
PositionX:  -2.594252985282   PositionY:   -15.831504726064   PositionZ:   -1.664938474349
TiltX:      11.464293875005   TiltY:       -1.871609343693   TiltZ:       -0.379495682798
CrystalId:  283
PositionX:  0.000000000000   PositionY:   -15.840408447575   PositionZ:   -1.621010859981
TiltX:      11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  284
PositionX:  2.594252985282   PositionY:   -15.831504726064   PositionZ:   -1.664938474349
TiltX:      11.464293875005   TiltY:       1.871609343693   TiltZ:       0.379495682798
CrystalId:  285
PositionX:  -2.594252985282   PositionY:   -18.363692290233   PositionZ:   -2.223896411136
TiltX:      13.374939286763   TiltY:       -1.857923282113   TiltZ:       -0.441674923206
CrystalId:  286
PositionX:  0.000000000000   PositionY:   -18.374054913794   PositionZ:   -2.180289902519
TiltX:      13.367777554211   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  287
PositionX:  2.594252985282   PositionY:   -18.363692290233   PositionZ:   -2.223896411136
TiltX:      13.374939286763   TiltY:       1.857923282113   TiltZ:       0.441674923206
CrystalId:  288
PositionX:  5.389054235872   PositionY:   -13.332198625907   PositionZ:   -1.323498094794
TiltX:      9.539239955707   TiltY:       3.811001424645   TiltZ:       0.429854745176
CrystalId:  289
PositionX:  7.974768354527   PositionY:   -13.305895847043   PositionZ:   -1.536918158558
TiltX:      9.565590216321   TiltY:       5.694209508692   TiltZ:       0.747845260727
CrystalId:  290
PositionX:  10.551518274629   PositionY:   -13.264812449197   PositionZ:   -1.838297432699
TiltX:      9.602582909298   TiltY:       7.577242204868   TiltZ:       1.067927593992
CrystalId:  291
PositionX:  5.375515643380   PositionY:   -15.882436310745   PositionZ:   -1.793085458951
TiltX:      11.443687594562   TiltY:       3.822588745890   TiltZ:       0.367144043644
CrystalId:  292
PositionX:  7.961221871788   PositionY:   -15.857602470609   PositionZ:   -2.006776928040
TiltX:      11.475272724065   TiltY:       5.694209508692   TiltZ:       0.747845260727
CrystalId:  293
PositionX:  10.537979682137   PositionY:   -15.815050134035   PositionZ:   -2.307884796856
TiltX:      11.519520156702   TiltY:       7.565578896718   TiltZ:       1.131035293934
CrystalId:  294
PositionX:  5.353033602792   PositionY:   -18.414965340020   PositionZ:   -2.350040671657
TiltX:      13.348149474652   TiltY:       3.836249102035   TiltZ:       0.304853000306
CrystalId:  295
PositionX:  7.938726970990   PositionY:   -18.391590597737   PositionZ:   -2.564052097249
TiltX:      13.384955231809   TiltY:       5.694209508692   TiltZ:       0.747845260727
CrystalId:  296
PositionX:  10.515497641549   PositionY:   -18.347579163310   PositionZ:   -2.864840009562
TiltX:      13.436386871549   TiltY:       7.551829566597   TiltZ:       1.193712263132
CrystalId:  297
PositionX:  13.406101630327   PositionY:   -13.310257604440   PositionZ:   -2.316298623727
TiltX:      9.609899861251   TiltY:       9.789750564761   TiltZ:       0.274985444219
CrystalId:  298
PositionX:  15.955583859868   PositionY:   -13.291067577218   PositionZ:   -2.797889287375
TiltX:      9.670511529805   TiltY:       11.672471155582   TiltZ:       0.600459596177
CrystalId:  299
PositionX:  18.487030942154   PositionY:   -13.257007531099   PositionZ:   -3.366020474736
TiltX:      9.742568907027   TiltY:       13.554820412970   TiltZ:       0.930384025642
CrystalId:  300
PositionX:  13.336398640899   PositionY:   -15.860103391979   PositionZ:   -2.783027440348
TiltX:      11.507721259156   TiltY:       9.801411426153   TiltZ:       0.211501974222
CrystalId:  301
PositionX:  15.885840533853   PositionY:   -15.842382076820   PositionZ:   -3.264887848190
TiltX:      11.580194037549   TiltY:       11.672471155582   TiltZ:       0.600459596177
CrystalId:  302
PositionX:  18.417327952726   PositionY:   -15.806853318638   PositionZ:   -3.832749291356
TiltX:      11.666259108911   TiltY:       13.543000613359   TiltZ:       0.994702089800
CrystalId:  303
PositionX:  13.248751149328   PositionY:   -18.392151626772   PositionZ:   -3.335709527206
TiltX:      13.405601078907   TiltY:       9.815137665631   TiltZ:       0.148442808118
CrystalId:  304
PositionX:  15.798142733756   PositionY:   -18.375889133399   PositionZ:   -3.817887444328
TiltX:      13.489876545294   TiltY:       11.672471155582   TiltZ:       0.600459596177
CrystalId:  305
PositionX:  18.329680461155   PositionY:   -18.338901553430   PositionZ:   -4.385431378214
TiltX:      13.589830454564   TiltY:       13.529088569433   TiltZ:       1.058572335785
CrystalId:  306
PositionX:  -10.591435443288   PositionY:   -21.060848557215   PositionZ:   -3.579063741084
TiltX:      15.295203836108   TiltY:       -7.699312302288   TiltZ:       -0.956471710828
CrystalId:  307
PositionX:  -8.015310361743   PositionY:   -21.092867478938   PositionZ:   -3.271310892044
TiltX:      15.235437599022   TiltY:       -5.856997134390   TiltZ:       -0.450155739584
CrystalId:  308
PositionX:  -5.430174398355   PositionY:   -21.101399870231   PositionZ:   -3.049597761390
TiltX:      15.192156772981   TiltY:       -4.014230761053   TiltZ:       0.052829587254
CrystalId:  309
PositionX:  -10.537378185225   PositionY:   -23.552095854543   PositionZ:   -4.296818958158
TiltX:      17.212075459441   TiltY:       -7.681501370597   TiltZ:       -1.018118269794
CrystalId:  310
PositionX:  -7.961221871788   PositionY:   -23.585549644429   PositionZ:   -3.989480454407
TiltX:      17.145120106767   TiltY:       -5.856997134390   TiltZ:       -0.450155739584
CrystalId:  311
PositionX:  -5.376117140293   PositionY:   -23.592647167559   PositionZ:   -3.767352978464
TiltX:      17.096479751192   TiltY:       -4.031925095543   TiltZ:       0.114106540155
CrystalId:  312
PositionX:  -10.474377479066   PositionY:   -26.017001523419   PositionZ:   -5.099737876727
TiltX:      19.128841790109   TiltY:       -7.661656962576   TiltZ:       -1.079126472909
CrystalId:  313
PositionX:  -7.898184963775   PositionY:   -26.051875543163   PositionZ:   -4.792861044578
TiltX:      19.054802614511   TiltY:       -5.856997134390   TiltZ:       -0.450155739584
CrystalId:  314
PositionX:  -5.313116434134   PositionY:   -26.057552836435   PositionZ:   -4.570271897032
TiltX:      19.000826607828   TiltY:       -4.051640921916   TiltZ:       0.174761596714
CrystalId:  315
PositionX:  -2.594252985282   PositionY:   -21.059688899480   PositionZ:   -2.928383983988
TiltX:      15.285552862625   TiltY:       -1.842172834120   TiltZ:       -0.503362537817
CrystalId:  316
PositionX:  0.000000000000   PositionY:   -21.071498914262   PositionZ:   -2.885147019344
TiltX:      15.277460061955   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  317
PositionX:  2.594252985282   PositionY:   -21.059688899480   PositionZ:   -2.928383983988
TiltX:      15.285552862625   TiltY:       1.842172834120   TiltZ:       0.503362537817
CrystalId:  318
PositionX:  -2.594252985282   PositionY:   -23.549904744570   PositionZ:   -3.651732364847
TiltX:      17.196130464691   TiltY:       -1.824375548871   TiltZ:       -0.564489888993
CrystalId:  319
PositionX:  0.000000000000   PositionY:   -23.563149031979   PositionZ:   -3.608912971906
TiltX:      17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  320
PositionX:  2.594252985282   PositionY:   -23.549904744570   PositionZ:   -3.651732364847
TiltX:      17.196130464691   TiltY:       1.824375548871   TiltZ:       0.564489888993
CrystalId:  321
PositionX:  -2.594252985282   PositionY:   -26.013646726631   PositionZ:   -4.460671967697
TiltX:      19.106668115313   TiltY:       -1.804551254619   TiltZ:       -0.624988972712
CrystalId:  322
PositionX:  0.000000000000   PositionY:   -26.028310574878   PositionZ:   -4.418317710350
TiltX:      19.096825077444   TiltY:       -0.000000000000   TiltZ:       0.000000000000
CrystalId:  323
PositionX:  2.594252985282   PositionY:   -26.013646726631   PositionZ:   -4.460671967697
TiltX:      19.106668115313   TiltY:       1.804551254619   TiltZ:       0.624988972712
CrystalId:  324
PositionX:  5.430174398355   PositionY:   -21.101399870231   PositionZ:   -3.049597761390
TiltX:      15.192156772981   TiltY:       4.014230761053   TiltZ:       -0.052829587254
CrystalId:  325
PositionX:  8.015310361743   PositionY:   -21.092867478938   PositionZ:   -3.271310892044
TiltX:      15.235437599022   TiltY:       5.856997134390   TiltZ:       0.450155739584
CrystalId:  326
PositionX:  10.591435443288   PositionY:   -21.060848557215   PositionZ:   -3.579063741084
TiltX:      15.295203836108   TiltY:       7.699312302288   TiltZ:       0.956471710828
CrystalId:  327
PositionX:  5.376117140293   PositionY:   -23.592647167559   PositionZ:   -3.767352978464
TiltX:      17.096479751192   TiltY:       4.031925095543   TiltZ:       -0.114106540155
CrystalId:  328
PositionX:  7.961221871788   PositionY:   -23.585549644429   PositionZ:   -3.989480454407
TiltX:      17.145120106767   TiltY:       5.856997134390   TiltZ:       0.450155739584
CrystalId:  329
PositionX:  10.537378185225   PositionY:   -23.552095854543   PositionZ:   -4.296818958158
TiltX:      17.212075459441   TiltY:       7.681501370597   TiltZ:       1.018118269794
CrystalId:  330
PositionX:  5.313116434134   PositionY:   -26.057552836435   PositionZ:   -4.570271897032
TiltX:      19.000826607828   TiltY:       4.051640921916   TiltZ:       -0.174761596714
CrystalId:  331
PositionX:  7.898184963775   PositionY:   -26.051875543163   PositionZ:   -4.792861044578
TiltX:      19.054802614511   TiltY:       5.856997134390   TiltZ:       0.450155739584
CrystalId:  332
PositionX:  10.474377479066   PositionY:   -26.017001523419   PositionZ:   -5.099737876727
TiltX:      19.128841790109   TiltY:       7.661656962576   TiltZ:       1.079126472909

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  -7.961221871788   PositionY:   23.585549644429   PositionZ:   -3.989480454407
TiltX:      -17.145120106767   TiltY:       -5.856997134390   TiltZ:       0.450155739584
ModuleId:   1
PositionX:  0.000000000000   PositionY:   23.563149031979   PositionZ:   -3.608912971906
TiltX:      -17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   2
PositionX:  7.961221871788   PositionY:   23.585549644429   PositionZ:   -3.989480454407
TiltX:      -17.145120106767   TiltY:       5.856997134390   TiltZ:       -0.450155739584
ModuleId:   3
PositionX:  -15.885840533853   PositionY:   15.842382076820   PositionZ:   -3.264887848190
TiltX:      -11.580194037549   TiltY:       -11.672471155582   TiltZ:       0.600459596177
ModuleId:   4
PositionX:  -7.961221871788   PositionY:   15.857602470609   PositionZ:   -2.006776928040
TiltX:      -11.475272724065   TiltY:       -5.694209508692   TiltZ:       0.747845260727
ModuleId:   5
PositionX:  0.000000000000   PositionY:   15.840408447575   PositionZ:   -1.621010859981
TiltX:      -11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  7.961221871788   PositionY:   15.857602470609   PositionZ:   -2.006776928040
TiltX:      -11.475272724065   TiltY:       5.694209508692   TiltZ:       -0.747845260727
ModuleId:   7
PositionX:  15.885840533853   PositionY:   15.842382076820   PositionZ:   -3.264887848190
TiltX:      -11.580194037549   TiltY:       11.672471155582   TiltZ:       -0.600459596177
ModuleId:   8
PositionX:  -23.447774346565   PositionY:   7.961551572609   PositionZ:   -3.981924658963
TiltX:      -5.821282372283   TiltY:       -17.156954975678   TiltZ:       0.312403520096
ModuleId:   9
PositionX:  -15.762845643575   PositionY:   7.961551572609   PositionZ:   -2.003676573302
TiltX:      -5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   10
PositionX:  -7.922575488556   PositionY:   7.961551572609   PositionZ:   -0.778944204122
TiltX:      -5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   11
PositionX:  0.000000000000   PositionY:   7.961551572609   PositionZ:   -0.390447226543
TiltX:      -5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   12
PositionX:  7.922575488556   PositionY:   7.961551572609   PositionZ:   -0.778944204122
TiltX:      -5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   13
PositionX:  15.762845643575   PositionY:   7.961551572609   PositionZ:   -2.003676573302
TiltX:      -5.729047523233   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   14
PositionX:  23.447774346565   PositionY:   7.961551572609   PositionZ:   -3.981924658963
TiltX:      -5.821282372283   TiltY:       17.156954975678   TiltZ:       -0.312403520096
ModuleId:   15
PositionX:  -23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       -17.187142569699   TiltZ:       0.000000000000
ModuleId:   16
PositionX:  -15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   17
PositionX:  -7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   18
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   19
PositionX:  7.961551572609   PositionY:   0.000000000000   PositionZ:   -0.390447226543
TiltX:      0.000000000000   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   20
PositionX:  15.840408447575   PositionY:   0.000000000000   PositionZ:   -1.621010859981
TiltX:      0.000000000000   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   21
PositionX:  23.563149031979   PositionY:   0.000000000000   PositionZ:   -3.608912971906
TiltX:      0.000000000000   TiltY:       17.187142569699   TiltZ:       0.000000000000
ModuleId:   22
PositionX:  -23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.821282372283   TiltY:       -17.156954975678   TiltZ:       -0.312403520096
ModuleId:   23
PositionX:  -15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       -11.458095046466   TiltZ:       0.000000000000
ModuleId:   24
PositionX:  -7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       -5.729047523233   TiltZ:       0.000000000000
ModuleId:   25
PositionX:  0.000000000000   PositionY:   -7.961551572609   PositionZ:   -0.390447226543
TiltX:      5.729047523233   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   26
PositionX:  7.922575488556   PositionY:   -7.961551572609   PositionZ:   -0.778944204122
TiltX:      5.729047523233   TiltY:       5.729047523233   TiltZ:       0.000000000000
ModuleId:   27
PositionX:  15.762845643575   PositionY:   -7.961551572609   PositionZ:   -2.003676573302
TiltX:      5.729047523233   TiltY:       11.458095046466   TiltZ:       0.000000000000
ModuleId:   28
PositionX:  23.447774346565   PositionY:   -7.961551572609   PositionZ:   -3.981924658963
TiltX:      5.821282372283   TiltY:       17.156954975678   TiltZ:       0.312403520096
ModuleId:   29
PositionX:  -15.885840533853   PositionY:   -15.842382076820   PositionZ:   -3.264887848190
TiltX:      11.580194037549   TiltY:       -11.672471155582   TiltZ:       -0.600459596177
ModuleId:   30
PositionX:  -7.961221871788   PositionY:   -15.857602470609   PositionZ:   -2.006776928040
TiltX:      11.475272724065   TiltY:       -5.694209508692   TiltZ:       -0.747845260727
ModuleId:   31
PositionX:  0.000000000000   PositionY:   -15.840408447575   PositionZ:   -1.621010859981
TiltX:      11.458095046466   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   32
PositionX:  7.961221871788   PositionY:   -15.857602470609   PositionZ:   -2.006776928040
TiltX:      11.475272724065   TiltY:       5.694209508692   TiltZ:       0.747845260727
ModuleId:   33
PositionX:  15.885840533853   PositionY:   -15.842382076820   PositionZ:   -3.264887848190
TiltX:      11.580194037549   TiltY:       11.672471155582   TiltZ:       0.600459596177
ModuleId:   34
PositionX:  -7.961221871788   PositionY:   -23.585549644429   PositionZ:   -3.989480454407
TiltX:      17.145120106767   TiltY:       -5.856997134390   TiltZ:       -0.450155739584
ModuleId:   35
PositionX:  0.000000000000   PositionY:   -23.563149031979   PositionZ:   -3.608912971906
TiltX:      17.187142569699   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   36
PositionX:  7.961221871788   PositionY:   -23.585549644429   PositionZ:   -3.989480454407
TiltX:      17.145120106767   TiltY:       5.856997134390   TiltZ:       0.450155739584
