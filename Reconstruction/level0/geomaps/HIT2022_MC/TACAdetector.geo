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
TypeGeo:        "FIVE_MOD"

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
Widthfront:   3.114221   Heightfront:  3.114221    DummyLength:    0.
Widthback :   4.468810	 Heightback :  4.468810	   Length:         12.150000
PositionZ:    -0.082500

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the Calorimeter bounding box (cm)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
Width:          44.784256     Height:      9.018349      Thick:     33.650664, 
PositionZ:      -4.775332

CrystalN:  45
ModulesN:  5
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
CrystalId:  9
PositionX:  -7.731122208839   PositionY:   0.000000000000   PositionZ:   -0.457804866612
TiltX:      0.022592688033   TiltY:       -6.444980558516   TiltZ:       -0.201272023589
CrystalId:  10
PositionX:  -5.192603552272   PositionY:   -0.008897910219   PositionZ:   -0.221030416795
TiltX:      0.022513177232   TiltY:       -4.296640728162   TiltZ:       -0.200422708581
CrystalId:  11
PositionX:  -10.258489966944   PositionY:   0.008897910219   PositionZ:   -0.793291565487
TiltX:      0.022704792964   TiltY:       -8.593320387454   TiltZ:       -0.202128567345
CrystalId:  12
PositionX:  -7.716705083950   PositionY:   2.549053156991   PositionZ:   -0.506162202962
TiltX:      -2.125747308712   TiltY:       -6.444980558516   TiltZ:       -0.201272023589
CrystalId:  13
PositionX:  -5.178196804660   PositionY:   2.538293286295   PositionZ:   -0.269353791384
TiltX:      -2.118266810107   TiltY:       -4.298113446457   TiltZ:       -0.281165971480
CrystalId:  14
PositionX:  -10.244083219332   PositionY:   2.556089106733   PositionZ:   -0.841614940075
TiltX:      -2.136291774355   TiltY:       -8.591835138799   TiltZ:       -0.120698547549
CrystalId:  15
PositionX:  -7.734388435266   PositionY:   -2.549053156991   PositionZ:   -0.508159779321
TiltX:      2.170932684779   TiltY:       -6.444980558516   TiltZ:       -0.201272023589
CrystalId:  16
PositionX:  -5.195867239140   PositionY:   -2.556089106733   PositionZ:   -0.271349908609
TiltX:      2.163293208697   TiltY:       -4.298176720394   TiltZ:       -0.119680627875
CrystalId:  17
PositionX:  -10.261753653812   PositionY:   -2.538293286295   PositionZ:   -0.843611057301
TiltX:      2.181701132734   TiltY:       -8.591771326702   TiltZ:       -0.283557364226
CrystalId:  18
PositionX:  7.731122208839   PositionY:   0.000000000000   PositionZ:   -0.457804866612
TiltX:      0.022592688033   TiltY:       6.444980558516   TiltZ:       0.201272023589
CrystalId:  19
PositionX:  10.258489966944   PositionY:   0.008897910219   PositionZ:   -0.793291565487
TiltX:      0.022704792964   TiltY:       8.593320387454   TiltZ:       0.202128567345
CrystalId:  20
PositionX:  5.192603552272   PositionY:   -0.008897910219   PositionZ:   -0.221030416795
TiltX:      0.022513177232   TiltY:       4.296640728162   TiltZ:       0.200422708581
CrystalId:  21
PositionX:  7.716705083950   PositionY:   2.549053156991   PositionZ:   -0.506162202962
TiltX:      -2.125747308712   TiltY:       6.444980558516   TiltZ:       0.201272023589
CrystalId:  22
PositionX:  10.244083219332   PositionY:   2.556089106733   PositionZ:   -0.841614940075
TiltX:      -2.136291774355   TiltY:       8.591835138799   TiltZ:       0.120698547549
CrystalId:  23
PositionX:  5.178196804660   PositionY:   2.538293286295   PositionZ:   -0.269353791384
TiltX:      -2.118266810107   TiltY:       4.298113446457   TiltZ:       0.281165971480
CrystalId:  24
PositionX:  7.734388435266   PositionY:   -2.549053156991   PositionZ:   -0.508159779321
TiltX:      2.170932684779   TiltY:       6.444980558516   TiltZ:       0.201272023589
CrystalId:  25
PositionX:  10.261753653812   PositionY:   -2.538293286295   PositionZ:   -0.843611057301
TiltX:      2.181701132734   TiltY:       8.591771326702   TiltZ:       0.283557364226
CrystalId:  26
PositionX:  5.195867239140   PositionY:   -2.556089106733   PositionZ:   -0.271349908609
TiltX:      2.163293208697   TiltY:       4.298176720394   TiltZ:       0.119680627875
CrystalId:  27
PositionX:  -15.354443430381   PositionY:   0.000000000000   PositionZ:   -1.824865304356
TiltX:      0.000000000000   TiltY:       -12.890039980471   TiltZ:       0.000000000000
CrystalId:  28
PositionX:  -12.858530879561   PositionY:   0.000000000000   PositionZ:   -1.304635719178
TiltX:      0.000000000000   TiltY:       -10.741699983726   TiltZ:       0.000000000000
CrystalId:  29
PositionX:  -17.828195131051   PositionY:   0.000000000000   PositionZ:   -2.441931594205
TiltX:      0.000000000000   TiltY:       -15.038379977217   TiltZ:       0.000000000000
CrystalId:  30
PositionX:  -15.343363005306   PositionY:   2.549068686790   PositionZ:   -1.873283656691
TiltX:      -2.148339996745   TiltY:       -12.890039980471   TiltZ:       0.000000000000
CrystalId:  31
PositionX:  -12.847458242672   PositionY:   2.547206714969   PositionZ:   -1.353020039324
TiltX:      -2.131554919216   TiltY:       -10.743197857270   TiltZ:       -0.081952005857
CrystalId:  32
PositionX:  -17.817122494162   PositionY:   2.547206714969   PositionZ:   -2.490315914351
TiltX:      -2.168461013317   TiltY:       -15.036856172299   TiltZ:       0.083370265644
CrystalId:  33
PositionX:  -15.343363005306   PositionY:   -2.549068686790   PositionZ:   -1.873283656691
TiltX:      2.148339996745   TiltY:       -12.890039980471   TiltZ:       0.000000000000
CrystalId:  34
PositionX:  -12.847458242672   PositionY:   -2.547206714969   PositionZ:   -1.353020039324
TiltX:      2.131554919216   TiltY:       -10.743197857270   TiltZ:       0.081952005857
CrystalId:  35
PositionX:  -17.817122494162   PositionY:   -2.547206714969   PositionZ:   -2.490315914351
TiltX:      2.168461013317   TiltY:       -15.036856172299   TiltZ:       -0.083370265644
CrystalId:  36
PositionX:  15.354443430381   PositionY:   0.000000000000   PositionZ:   -1.824865304356
TiltX:      0.000000000000   TiltY:       12.890039980471   TiltZ:       0.000000000000
CrystalId:  37
PositionX:  17.828195131051   PositionY:   0.000000000000   PositionZ:   -2.441931594205
TiltX:      0.000000000000   TiltY:       15.038379977217   TiltZ:       0.000000000000
CrystalId:  38
PositionX:  12.858530879561   PositionY:   0.000000000000   PositionZ:   -1.304635719178
TiltX:      0.000000000000   TiltY:       10.741699983726   TiltZ:       0.000000000000
CrystalId:  39
PositionX:  15.343363005306   PositionY:   2.549068686790   PositionZ:   -1.873283656691
TiltX:      -2.148339996745   TiltY:       12.890039980471   TiltZ:       0.000000000000
CrystalId:  40
PositionX:  17.817122494162   PositionY:   2.547206714969   PositionZ:   -2.490315914351
TiltX:      -2.168461013317   TiltY:       15.036856172299   TiltZ:       -0.083370265644
CrystalId:  41
PositionX:  12.847458242672   PositionY:   2.547206714969   PositionZ:   -1.353020039324
TiltX:      -2.131554919216   TiltY:       10.743197857270   TiltZ:       0.081952005857
CrystalId:  42
PositionX:  15.343363005306   PositionY:   -2.549068686790   PositionZ:   -1.873283656691
TiltX:      2.148339996745   TiltY:       12.890039980471   TiltZ:       0.000000000000
CrystalId:  43
PositionX:  17.817122494162   PositionY:   -2.547206714969   PositionZ:   -2.490315914351
TiltX:      2.168461013317   TiltY:       15.036856172299   TiltZ:       0.083370265644
CrystalId:  44
PositionX:  12.847458242672   PositionY:   -2.547206714969   PositionZ:   -1.353020039324
TiltX:      2.131554919216   TiltY:       10.743197857270   TiltZ:       -0.081952005857

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  0.000000000000   PositionY:   0.000000000000   PositionZ:   0.000000000000
TiltX:      0.000000000000   TiltY:       -0.000000000000   TiltZ:       0.000000000000
ModuleId:   1
PositionX:  -7.731122208839   PositionY:   0.000000000000   PositionZ:   -0.457804866612
TiltX:      0.022592688033   TiltY:       -6.444980558516   TiltZ:       -0.201272023589
ModuleId:   2
PositionX:  7.731122208839   PositionY:   0.000000000000   PositionZ:   -0.457804866612
TiltX:      0.022592688033   TiltY:       6.444980558516   TiltZ:       0.201272023589
ModuleId:   3
PositionX:  -15.354443430381   PositionY:   0.000000000000   PositionZ:   -1.824865304356
TiltX:      0.000000000000   TiltY:       -12.890039980471   TiltZ:       0.000000000000
ModuleId:   4
PositionX:  15.354443430381   PositionY:   0.000000000000   PositionZ:   -1.824865304356
TiltX:      0.000000000000   TiltY:       12.890039980471   TiltZ:       0.000000000000
