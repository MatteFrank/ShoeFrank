// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
//
// Geometry file for FOOT Calorimeter
//
// Created with macro: Reconstruction/BuildCaGeoFile.C
//
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of Geometry 
// type of geometry (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD)
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
TypeGeo:        "FULL_DET"

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
Width:          55.241361     Height:      55.175392      Thick:     26.411979, 
PositionZ:      0.460181

CrystalN:  288
ModulesN:  32
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the crystals used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
CrystalId:  0
PositionX:  -3.865876853086   PositionY:   -3.872316177238   PositionZ:   -0.228920210837
TiltX:      3.233733009436   TiltY:       -3.211247674512   TiltZ:       -0.199997790452
CrystalId:  1
PositionX:  -1.318038528351   PositionY:   -3.878407878024   PositionZ:   -0.135640819601
TiltX:      3.229209790049   TiltY:       -1.066322841445   TiltZ:       -0.078818690679
CrystalId:  2
PositionX:  -6.408139728589   PositionY:   -3.860640197114   PositionZ:   -0.421225764479
TiltX:      3.242824209331   TiltY:       -5.356158112993   TiltZ:       -0.321687240244
CrystalId:  3
PositionX:  -3.862260262843   PositionY:   -1.324501554730   PositionZ:   -0.134867819863
TiltX:      1.085393012690   TiltY:       -3.211247674512   TiltZ:       -0.199997790452
CrystalId:  4
PositionX:  -1.314424502984   PositionY:   -1.332454234313   PositionZ:   -0.041658494635
TiltX:      1.083875169673   TiltY:       -1.063292516992   TiltZ:       -0.159305279928
CrystalId:  5
PositionX:  -6.404525703222   PositionY:   -1.314686553404   PositionZ:   -0.327243439514
TiltX:      1.088446981333   TiltY:       -5.359201208820   TiltZ:       -0.240861922394
CrystalId:  6
PositionX:  -3.863917994098   PositionY:   -6.414546520407   PositionZ:   -0.421998764217
TiltX:      5.382073006181   TiltY:       -3.211247674512   TiltZ:       -0.199997790452
CrystalId:  7
PositionX:  -1.316081023347   PositionY:   -6.418781167462   PositionZ:   -0.328579703673
TiltX:      5.374541208299   TiltY:       -1.072363487369   TiltZ:       0.001498145077
CrystalId:  8
PositionX:  -6.406182223584   PositionY:   -6.401013486552   PositionZ:   -0.614164648551
TiltX:      5.397178935241   TiltY:       -5.350092062775   TiltZ:       -0.402339929855
CrystalId:  9
PositionX:  -3.865876853086   PositionY:   3.872316177238   PositionZ:   -0.228920210837
TiltX:      -3.233733009436   TiltY:       -3.211247674512   TiltZ:       0.199997790452
CrystalId:  10
PositionX:  -1.318038528351   PositionY:   3.878407878024   PositionZ:   -0.135640819601
TiltX:      -3.229209790049   TiltY:       -1.066322841445   TiltZ:       0.078818690679
CrystalId:  11
PositionX:  -6.408139728589   PositionY:   3.860640197114   PositionZ:   -0.421225764479
TiltX:      -3.242824209331   TiltY:       -5.356158112993   TiltZ:       0.321687240244
CrystalId:  12
PositionX:  -3.863917994098   PositionY:   6.414546520407   PositionZ:   -0.421998764217
TiltX:      -5.382073006181   TiltY:       -3.211247674512   TiltZ:       0.199997790452
CrystalId:  13
PositionX:  -1.316081023347   PositionY:   6.418781167462   PositionZ:   -0.328579703673
TiltX:      -5.374541208299   TiltY:       -1.072363487369   TiltZ:       -0.001498145077
CrystalId:  14
PositionX:  -6.406182223584   PositionY:   6.401013486552   PositionZ:   -0.614164648551
TiltX:      -5.397178935241   TiltY:       -5.350092062775   TiltZ:       0.402339929855
CrystalId:  15
PositionX:  -3.862260262843   PositionY:   1.324501554730   PositionZ:   -0.134867819863
TiltX:      -1.085393012690   TiltY:       -3.211247674512   TiltZ:       0.199997790452
CrystalId:  16
PositionX:  -1.314424502984   PositionY:   1.332454234313   PositionZ:   -0.041658494635
TiltX:      -1.083875169673   TiltY:       -1.063292516992   TiltZ:       0.159305279928
CrystalId:  17
PositionX:  -6.404525703222   PositionY:   1.314686553404   PositionZ:   -0.327243439514
TiltX:      -1.088446981333   TiltY:       -5.359201208820   TiltZ:       0.240861922394
CrystalId:  18
PositionX:  3.865876853086   PositionY:   -3.872316177238   PositionZ:   -0.228920210837
TiltX:      3.233733009436   TiltY:       3.211247674512   TiltZ:       0.199997790452
CrystalId:  19
PositionX:  6.408139728589   PositionY:   -3.860640197114   PositionZ:   -0.421225764479
TiltX:      3.242824209331   TiltY:       5.356158112993   TiltZ:       0.321687240244
CrystalId:  20
PositionX:  1.318038528351   PositionY:   -3.878407878024   PositionZ:   -0.135640819601
TiltX:      3.229209790049   TiltY:       1.066322841445   TiltZ:       0.078818690679
CrystalId:  21
PositionX:  3.862260262843   PositionY:   -1.324501554730   PositionZ:   -0.134867819863
TiltX:      1.085393012690   TiltY:       3.211247674512   TiltZ:       0.199997790452
CrystalId:  22
PositionX:  6.404525703222   PositionY:   -1.314686553404   PositionZ:   -0.327243439514
TiltX:      1.088446981333   TiltY:       5.359201208820   TiltZ:       0.240861922394
CrystalId:  23
PositionX:  1.314424502984   PositionY:   -1.332454234313   PositionZ:   -0.041658494635
TiltX:      1.083875169673   TiltY:       1.063292516992   TiltZ:       0.159305279928
CrystalId:  24
PositionX:  3.863917994098   PositionY:   -6.414546520407   PositionZ:   -0.421998764217
TiltX:      5.382073006181   TiltY:       3.211247674512   TiltZ:       0.199997790452
CrystalId:  25
PositionX:  6.406182223584   PositionY:   -6.401013486552   PositionZ:   -0.614164648551
TiltX:      5.397178935241   TiltY:       5.350092062775   TiltZ:       0.402339929855
CrystalId:  26
PositionX:  1.316081023347   PositionY:   -6.418781167462   PositionZ:   -0.328579703673
TiltX:      5.374541208299   TiltY:       1.072363487369   TiltZ:       -0.001498145077
CrystalId:  27
PositionX:  3.865876853086   PositionY:   3.872316177238   PositionZ:   -0.228920210837
TiltX:      -3.233733009436   TiltY:       3.211247674512   TiltZ:       -0.199997790452
CrystalId:  28
PositionX:  6.408139728589   PositionY:   3.860640197114   PositionZ:   -0.421225764479
TiltX:      -3.242824209331   TiltY:       5.356158112993   TiltZ:       -0.321687240244
CrystalId:  29
PositionX:  1.318038528351   PositionY:   3.878407878024   PositionZ:   -0.135640819601
TiltX:      -3.229209790049   TiltY:       1.066322841445   TiltZ:       -0.078818690679
CrystalId:  30
PositionX:  3.863917994098   PositionY:   6.414546520407   PositionZ:   -0.421998764217
TiltX:      -5.382073006181   TiltY:       3.211247674512   TiltZ:       -0.199997790452
CrystalId:  31
PositionX:  6.406182223584   PositionY:   6.401013486552   PositionZ:   -0.614164648551
TiltX:      -5.397178935241   TiltY:       5.350092062775   TiltZ:       -0.402339929855
CrystalId:  32
PositionX:  1.316081023347   PositionY:   6.418781167462   PositionZ:   -0.328579703673
TiltX:      -5.374541208299   TiltY:       1.072363487369   TiltZ:       0.001498145077
CrystalId:  33
PositionX:  3.862260262843   PositionY:   1.324501554730   PositionZ:   -0.134867819863
TiltX:      -1.085393012690   TiltY:       3.211247674512   TiltZ:       -0.199997790452
CrystalId:  34
PositionX:  6.404525703222   PositionY:   1.314686553404   PositionZ:   -0.327243439514
TiltX:      -1.088446981333   TiltY:       5.359201208820   TiltZ:       -0.240861922394
CrystalId:  35
PositionX:  1.314424502984   PositionY:   1.332454234313   PositionZ:   -0.041658494635
TiltX:      -1.083875169673   TiltY:       1.063292516992   TiltZ:       -0.159305279928
CrystalId:  36
PositionX:  -11.745234987481   PositionY:   -3.872316177238   PositionZ:   0.041464145089
TiltX:      3.222509995118   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  37
PositionX:  -9.224038210031   PositionY:   -3.869524037569   PositionZ:   0.420643912435
TiltX:      3.204305859188   TiltY:       -7.522567062977   TiltZ:       0.121786235966
CrystalId:  38
PositionX:  -14.249775890625   PositionY:   -3.869524037569   PositionZ:   -0.435490099122
TiltX:      3.245500647788   TiltY:       -11.812449503303   TiltZ:       -0.123350266769
CrystalId:  39
PositionX:  -11.760970306949   PositionY:   -1.324486049488   PositionZ:   0.133834716632
TiltX:      1.074169998373   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  40
PositionX:  -9.239761805972   PositionY:   -1.323554899942   PositionZ:   0.512945663715
TiltX:      1.068100205532   TiltY:       -7.519565305733   TiltZ:       0.040614135210
CrystalId:  41
PositionX:  -14.265499486567   PositionY:   -1.323554899942   PositionZ:   -0.343188347841
TiltX:      1.081838295575   TiltY:       -11.815489837722   TiltZ:       -0.041136459661
CrystalId:  42
PositionX:  -11.712843793707   PositionY:   -6.414562025649   PositionZ:   -0.148680903319
TiltX:      5.370849991863   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  43
PositionX:  -9.191670446829   PositionY:   -6.409912820924   PositionZ:   0.230636407808
TiltX:      5.340527251265   TiltY:       -7.528566419966   TiltZ:       0.202789902701
CrystalId:  44
PositionX:  -14.217408127423   PositionY:   -6.409912820924   PositionZ:   -0.625497603749
TiltX:      5.409120323365   TiltY:       -11.806373209523   TiltZ:       -0.205386828889
CrystalId:  45
PositionX:  -11.745234987481   PositionY:   3.872316177238   PositionZ:   0.041464145089
TiltX:      -3.222509995118   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  46
PositionX:  -9.224038210031   PositionY:   3.869524037569   PositionZ:   0.420643912435
TiltX:      -3.204305859188   TiltY:       -7.522567062977   TiltZ:       -0.121786235966
CrystalId:  47
PositionX:  -14.249775890625   PositionY:   3.869524037569   PositionZ:   -0.435490099122
TiltX:      -3.245500647788   TiltY:       -11.812449503303   TiltZ:       0.123350266769
CrystalId:  48
PositionX:  -11.712843793707   PositionY:   6.414562025649   PositionZ:   -0.148680903319
TiltX:      -5.370849991863   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  49
PositionX:  -9.191670446829   PositionY:   6.409912820924   PositionZ:   0.230636407808
TiltX:      -5.340527251265   TiltY:       -7.528566419966   TiltZ:       -0.202789902701
CrystalId:  50
PositionX:  -14.217408127423   PositionY:   6.409912820924   PositionZ:   -0.625497603749
TiltX:      -5.409120323365   TiltY:       -11.806373209523   TiltZ:       0.205386828889
CrystalId:  51
PositionX:  -11.760970306949   PositionY:   1.324486049488   PositionZ:   0.133834716632
TiltX:      -1.074169998373   TiltY:       -9.667529985354   TiltZ:       0.000000000000
CrystalId:  52
PositionX:  -9.239761805972   PositionY:   1.323554899942   PositionZ:   0.512945663715
TiltX:      -1.068100205532   TiltY:       -7.519565305733   TiltZ:       -0.040614135210
CrystalId:  53
PositionX:  -14.265499486567   PositionY:   1.323554899942   PositionZ:   -0.343188347841
TiltX:      -1.081838295575   TiltY:       -11.815489837722   TiltZ:       0.041136459661
CrystalId:  54
PositionX:  11.745234987481   PositionY:   -3.872316177238   PositionZ:   0.041464145089
TiltX:      3.222509995118   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  55
PositionX:  14.249775890625   PositionY:   -3.869524037569   PositionZ:   -0.435490099122
TiltX:      3.245500647788   TiltY:       11.812449503303   TiltZ:       0.123350266769
CrystalId:  56
PositionX:  9.224038210031   PositionY:   -3.869524037569   PositionZ:   0.420643912435
TiltX:      3.204305859188   TiltY:       7.522567062977   TiltZ:       -0.121786235966
CrystalId:  57
PositionX:  11.760970306949   PositionY:   -1.324486049488   PositionZ:   0.133834716632
TiltX:      1.074169998373   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  58
PositionX:  14.265499486567   PositionY:   -1.323554899942   PositionZ:   -0.343188347841
TiltX:      1.081838295575   TiltY:       11.815489837722   TiltZ:       0.041136459661
CrystalId:  59
PositionX:  9.239761805972   PositionY:   -1.323554899942   PositionZ:   0.512945663715
TiltX:      1.068100205532   TiltY:       7.519565305733   TiltZ:       -0.040614135210
CrystalId:  60
PositionX:  11.712843793707   PositionY:   -6.414562025649   PositionZ:   -0.148680903319
TiltX:      5.370849991863   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  61
PositionX:  14.217408127423   PositionY:   -6.409912820924   PositionZ:   -0.625497603749
TiltX:      5.409120323365   TiltY:       11.806373209523   TiltZ:       0.205386828889
CrystalId:  62
PositionX:  9.191670446829   PositionY:   -6.409912820924   PositionZ:   0.230636407808
TiltX:      5.340527251265   TiltY:       7.528566419966   TiltZ:       -0.202789902701
CrystalId:  63
PositionX:  11.745234987481   PositionY:   3.872316177238   PositionZ:   0.041464145089
TiltX:      -3.222509995118   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  64
PositionX:  14.249775890625   PositionY:   3.869524037569   PositionZ:   -0.435490099122
TiltX:      -3.245500647788   TiltY:       11.812449503303   TiltZ:       -0.123350266769
CrystalId:  65
PositionX:  9.224038210031   PositionY:   3.869524037569   PositionZ:   0.420643912435
TiltX:      -3.204305859188   TiltY:       7.522567062977   TiltZ:       0.121786235966
CrystalId:  66
PositionX:  11.712843793707   PositionY:   6.414562025649   PositionZ:   -0.148680903319
TiltX:      -5.370849991863   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  67
PositionX:  14.217408127423   PositionY:   6.409912820924   PositionZ:   -0.625497603749
TiltX:      -5.409120323365   TiltY:       11.806373209523   TiltZ:       -0.205386828889
CrystalId:  68
PositionX:  9.191670446829   PositionY:   6.409912820924   PositionZ:   0.230636407808
TiltX:      -5.340527251265   TiltY:       7.528566419966   TiltZ:       0.202789902701
CrystalId:  69
PositionX:  11.760970306949   PositionY:   1.324486049488   PositionZ:   0.133834716632
TiltX:      -1.074169998373   TiltY:       9.667529985354   TiltZ:       0.000000000000
CrystalId:  70
PositionX:  14.265499486567   PositionY:   1.323554899942   PositionZ:   -0.343188347841
TiltX:      -1.081838295575   TiltY:       11.815489837722   TiltZ:       -0.041136459661
CrystalId:  71
PositionX:  9.239761805972   PositionY:   1.323554899942   PositionZ:   0.512945663715
TiltX:      -1.068100205532   TiltY:       7.519565305733   TiltZ:       0.040614135210
CrystalId:  72
PositionX:  -3.880994910796   PositionY:   -11.764471567213   PositionZ:   0.039593262399
TiltX:      9.686777919761   TiltY:       -3.163676101701   TiltZ:       -0.345556168089
CrystalId:  73
PositionX:  -1.333104926343   PositionY:   -11.771480587466   PositionZ:   0.131385328479
TiltX:      9.673500069598   TiltY:       -1.045917538339   TiltZ:       0.015905537649
CrystalId:  74
PositionX:  -6.423379919405   PositionY:   -11.740780293947   PositionZ:   -0.149973280546
TiltX:      9.713505903614   TiltY:       -5.281308493326   TiltZ:       -0.708498439585
CrystalId:  75
PositionX:  -3.886758574850   PositionY:   -9.243308484911   PositionZ:   0.418963497453
TiltX:      7.538437923016   TiltY:       -3.163676101701   TiltZ:       -0.345556168089
CrystalId:  76
PositionX:  -1.338864304422   PositionY:   -9.252158863288   PositionZ:   0.510477103848
TiltX:      7.528115045477   TiltY:       -1.033874373900   TiltZ:       -0.063731689131
CrystalId:  77
PositionX:  -6.429139297484   PositionY:   -9.221458569768   PositionZ:   0.229118494823
TiltX:      7.559307485321   TiltY:       -5.293401127693   TiltZ:       -0.628541615549
CrystalId:  78
PositionX:  -3.869726270898   PositionY:   -14.268952396503   PositionZ:   -0.437551449520
TiltX:      11.835117916507   TiltY:       -3.163676101701   TiltZ:       -0.345556168089
CrystalId:  79
PositionX:  -1.321844441747   PositionY:   -14.274131784218   PositionZ:   -0.345412200237
TiltX:      11.818875601899   TiltY:       -1.060933006204   TiltZ:       0.095036372116
CrystalId:  80
PositionX:  -6.412119434809   PositionY:   -14.243431490699   PositionZ:   -0.626770809261
TiltX:      11.867638133397   TiltY:       -5.266231741583   TiltZ:       -0.787940532338
CrystalId:  81
PositionX:  -3.880994910796   PositionY:   11.764471567213   PositionZ:   0.039593262399
TiltX:      -9.686777919761   TiltY:       -3.163676101701   TiltZ:       0.345556168089
CrystalId:  82
PositionX:  -1.333104926343   PositionY:   11.771480587466   PositionZ:   0.131385328479
TiltX:      -9.673500069598   TiltY:       -1.045917538339   TiltZ:       -0.015905537649
CrystalId:  83
PositionX:  -6.423379919405   PositionY:   11.740780293947   PositionZ:   -0.149973280546
TiltX:      -9.713505903614   TiltY:       -5.281308493326   TiltZ:       0.708498439585
CrystalId:  84
PositionX:  -3.869726270898   PositionY:   14.268952396503   PositionZ:   -0.437551449520
TiltX:      -11.835117916507   TiltY:       -3.163676101701   TiltZ:       0.345556168089
CrystalId:  85
PositionX:  -1.321844441747   PositionY:   14.274131784218   PositionZ:   -0.345412200237
TiltX:      -11.818875601899   TiltY:       -1.060933006204   TiltZ:       -0.095036372117
CrystalId:  86
PositionX:  -6.412119434809   PositionY:   14.243431490699   PositionZ:   -0.626770809261
TiltX:      -11.867638133397   TiltY:       -5.266231741583   TiltZ:       0.787940532338
CrystalId:  87
PositionX:  -3.886758574850   PositionY:   9.243308484911   PositionZ:   0.418963497453
TiltX:      -7.538437923016   TiltY:       -3.163676101701   TiltZ:       0.345556168089
CrystalId:  88
PositionX:  -1.338864304422   PositionY:   9.252158863288   PositionZ:   0.510477103848
TiltX:      -7.528115045477   TiltY:       -1.033874373900   TiltZ:       0.063731689131
CrystalId:  89
PositionX:  -6.429139297484   PositionY:   9.221458569768   PositionZ:   0.229118494823
TiltX:      -7.559307485321   TiltY:       -5.293401127693   TiltZ:       0.628541615549
CrystalId:  90
PositionX:  3.880994910796   PositionY:   -11.764471567213   PositionZ:   0.039593262399
TiltX:      9.686777919761   TiltY:       3.163676101701   TiltZ:       0.345556168089
CrystalId:  91
PositionX:  6.423379919405   PositionY:   -11.740780293947   PositionZ:   -0.149973280546
TiltX:      9.713505903614   TiltY:       5.281308493326   TiltZ:       0.708498439585
CrystalId:  92
PositionX:  1.333104926343   PositionY:   -11.771480587466   PositionZ:   0.131385328479
TiltX:      9.673500069598   TiltY:       1.045917538339   TiltZ:       -0.015905537649
CrystalId:  93
PositionX:  3.886758574850   PositionY:   -9.243308484911   PositionZ:   0.418963497453
TiltX:      7.538437923016   TiltY:       3.163676101701   TiltZ:       0.345556168089
CrystalId:  94
PositionX:  6.429139297484   PositionY:   -9.221458569768   PositionZ:   0.229118494823
TiltX:      7.559307485321   TiltY:       5.293401127693   TiltZ:       0.628541615549
CrystalId:  95
PositionX:  1.338864304422   PositionY:   -9.252158863288   PositionZ:   0.510477103848
TiltX:      7.528115045477   TiltY:       1.033874373900   TiltZ:       0.063731689131
CrystalId:  96
PositionX:  3.869726270898   PositionY:   -14.268952396503   PositionZ:   -0.437551449520
TiltX:      11.835117916507   TiltY:       3.163676101701   TiltZ:       0.345556168089
CrystalId:  97
PositionX:  6.412119434809   PositionY:   -14.243431490699   PositionZ:   -0.626770809261
TiltX:      11.867638133397   TiltY:       5.266231741583   TiltZ:       0.787940532338
CrystalId:  98
PositionX:  1.321844441747   PositionY:   -14.274131784218   PositionZ:   -0.345412200237
TiltX:      11.818875601899   TiltY:       1.060933006204   TiltZ:       -0.095036372117
CrystalId:  99
PositionX:  3.880994910796   PositionY:   11.764471567213   PositionZ:   0.039593262399
TiltX:      -9.686777919761   TiltY:       3.163676101701   TiltZ:       -0.345556168089
CrystalId:  100
PositionX:  6.423379919405   PositionY:   11.740780293947   PositionZ:   -0.149973280546
TiltX:      -9.713505903614   TiltY:       5.281308493326   TiltZ:       -0.708498439585
CrystalId:  101
PositionX:  1.333104926343   PositionY:   11.771480587466   PositionZ:   0.131385328479
TiltX:      -9.673500069598   TiltY:       1.045917538339   TiltZ:       0.015905537649
CrystalId:  102
PositionX:  3.869726270898   PositionY:   14.268952396503   PositionZ:   -0.437551449520
TiltX:      -11.835117916507   TiltY:       3.163676101701   TiltZ:       -0.345556168089
CrystalId:  103
PositionX:  6.412119434809   PositionY:   14.243431490699   PositionZ:   -0.626770809261
TiltX:      -11.867638133397   TiltY:       5.266231741583   TiltZ:       -0.787940532338
CrystalId:  104
PositionX:  1.321844441747   PositionY:   14.274131784218   PositionZ:   -0.345412200237
TiltX:      -11.818875601899   TiltY:       1.060933006204   TiltZ:       0.095036372116
CrystalId:  105
PositionX:  3.886758574850   PositionY:   9.243308484911   PositionZ:   0.418963497453
TiltX:      -7.538437923016   TiltY:       3.163676101701   TiltZ:       -0.345556168089
CrystalId:  106
PositionX:  6.429139297484   PositionY:   9.221458569768   PositionZ:   0.229118494823
TiltX:      -7.559307485321   TiltY:       5.293401127693   TiltZ:       -0.628541615549
CrystalId:  107
PositionX:  1.338864304422   PositionY:   9.252158863288   PositionZ:   0.510477103848
TiltX:      -7.528115045477   TiltY:       1.033874373900   TiltZ:       -0.063731689131
CrystalId:  108
PositionX:  -11.790398069147   PositionY:   -11.764471567213   PositionZ:   0.306583621991
TiltX:      9.784329971522   TiltY:       -9.549273675971   TiltZ:       -0.699755447422
CrystalId:  109
PositionX:  -9.268615866068   PositionY:   -11.786830161427   PositionZ:   0.681193947703
TiltX:      9.729971616739   TiltY:       -7.432002236126   TiltZ:       -0.331657233008
CrystalId:  110
PositionX:  -14.295734927305   PositionY:   -11.725430719986   PositionZ:   -0.164565316285
TiltX:      9.852950837661   TiltY:       -11.666153391620   TiltZ:       -1.072460537896
CrystalId:  111
PositionX:  -11.823355922669   PositionY:   -9.243449136664   PositionZ:   0.685500456130
TiltX:      7.635989974776   TiltY:       -9.549273675971   TiltZ:       -0.699755447422
CrystalId:  112
PositionX:  -9.301549418735   PositionY:   -9.267648986263   PositionZ:   1.059832670384
TiltX:      7.593517636824   TiltY:       -7.419874394031   TiltZ:       -0.411941857010
CrystalId:  113
PositionX:  -14.328668479972   PositionY:   -9.206249544822   PositionZ:   0.214073406395
TiltX:      7.689678772711   TiltY:       -11.678433456959   TiltZ:       -0.991192048248
CrystalId:  114
PositionX:  -11.740994870704   PositionY:   -14.268811744749   PositionZ:   -0.168871824712
TiltX:      11.932669968267   TiltY:       -9.549273675971   TiltZ:       -0.699755447422
CrystalId:  115
PositionX:  -9.219248527548   PositionY:   -14.289340809165   PositionZ:   0.206084467315
TiltX:      11.866472631927   TiltY:       -7.447089549937   TiltZ:       -0.251881895444
CrystalId:  116
PositionX:  -14.246367588786   PositionY:   -14.227941367723   PositionZ:   -0.639674796673
TiltX:      12.016095604247   TiltY:       -11.650877940316   TiltZ:       -1.153193791321
CrystalId:  117
PositionX:  -11.790398069147   PositionY:   11.764471567213   PositionZ:   0.306583621991
TiltX:      -9.784329971522   TiltY:       -9.549273675971   TiltZ:       0.699755447422
CrystalId:  118
PositionX:  -9.268615866068   PositionY:   11.786830161427   PositionZ:   0.681193947703
TiltX:      -9.729971616739   TiltY:       -7.432002236126   TiltZ:       0.331657233008
CrystalId:  119
PositionX:  -14.295734927305   PositionY:   11.725430719986   PositionZ:   -0.164565316285
TiltX:      -9.852950837661   TiltY:       -11.666153391620   TiltZ:       1.072460537896
CrystalId:  120
PositionX:  -11.740994870704   PositionY:   14.268811744749   PositionZ:   -0.168871824712
TiltX:      -11.932669968267   TiltY:       -9.549273675971   TiltZ:       0.699755447422
CrystalId:  121
PositionX:  -9.219248527548   PositionY:   14.289340809165   PositionZ:   0.206084467315
TiltX:      -11.866472631927   TiltY:       -7.447089549937   TiltZ:       0.251881895444
CrystalId:  122
PositionX:  -14.246367588786   PositionY:   14.227941367723   PositionZ:   -0.639674796673
TiltX:      -12.016095604247   TiltY:       -11.650877940316   TiltZ:       1.153193791321
CrystalId:  123
PositionX:  -11.823355922669   PositionY:   9.243449136664   PositionZ:   0.685500456130
TiltX:      -7.635989974776   TiltY:       -9.549273675971   TiltZ:       0.699755447422
CrystalId:  124
PositionX:  -9.301549418735   PositionY:   9.267648986263   PositionZ:   1.059832670384
TiltX:      -7.593517636824   TiltY:       -7.419874394031   TiltZ:       0.411941857010
CrystalId:  125
PositionX:  -14.328668479972   PositionY:   9.206249544822   PositionZ:   0.214073406395
TiltX:      -7.689678772711   TiltY:       -11.678433456959   TiltZ:       0.991192048248
CrystalId:  126
PositionX:  11.790398069147   PositionY:   -11.764471567213   PositionZ:   0.306583621991
TiltX:      9.784329971522   TiltY:       9.549273675971   TiltZ:       0.699755447422
CrystalId:  127
PositionX:  14.295734927305   PositionY:   -11.725430719986   PositionZ:   -0.164565316285
TiltX:      9.852950837661   TiltY:       11.666153391620   TiltZ:       1.072460537896
CrystalId:  128
PositionX:  9.268615866068   PositionY:   -11.786830161427   PositionZ:   0.681193947703
TiltX:      9.729971616739   TiltY:       7.432002236126   TiltZ:       0.331657233008
CrystalId:  129
PositionX:  11.823355922669   PositionY:   -9.243449136664   PositionZ:   0.685500456130
TiltX:      7.635989974776   TiltY:       9.549273675971   TiltZ:       0.699755447422
CrystalId:  130
PositionX:  14.328668479972   PositionY:   -9.206249544822   PositionZ:   0.214073406395
TiltX:      7.689678772711   TiltY:       11.678433456959   TiltZ:       0.991192048248
CrystalId:  131
PositionX:  9.301549418735   PositionY:   -9.267648986263   PositionZ:   1.059832670384
TiltX:      7.593517636824   TiltY:       7.419874394031   TiltZ:       0.411941857010
CrystalId:  132
PositionX:  11.740994870704   PositionY:   -14.268811744749   PositionZ:   -0.168871824712
TiltX:      11.932669968267   TiltY:       9.549273675971   TiltZ:       0.699755447422
CrystalId:  133
PositionX:  14.246367588786   PositionY:   -14.227941367723   PositionZ:   -0.639674796673
TiltX:      12.016095604247   TiltY:       11.650877940316   TiltZ:       1.153193791321
CrystalId:  134
PositionX:  9.219248527548   PositionY:   -14.289340809165   PositionZ:   0.206084467315
TiltX:      11.866472631927   TiltY:       7.447089549937   TiltZ:       0.251881895444
CrystalId:  135
PositionX:  11.790398069147   PositionY:   11.764471567213   PositionZ:   0.306583621991
TiltX:      -9.784329971522   TiltY:       9.549273675971   TiltZ:       -0.699755447422
CrystalId:  136
PositionX:  14.295734927305   PositionY:   11.725430719986   PositionZ:   -0.164565316285
TiltX:      -9.852950837661   TiltY:       11.666153391620   TiltZ:       -1.072460537896
CrystalId:  137
PositionX:  9.268615866068   PositionY:   11.786830161427   PositionZ:   0.681193947703
TiltX:      -9.729971616739   TiltY:       7.432002236126   TiltZ:       -0.331657233008
CrystalId:  138
PositionX:  11.740994870704   PositionY:   14.268811744749   PositionZ:   -0.168871824712
TiltX:      -11.932669968267   TiltY:       9.549273675971   TiltZ:       -0.699755447422
CrystalId:  139
PositionX:  14.246367588786   PositionY:   14.227941367723   PositionZ:   -0.639674796673
TiltX:      -12.016095604247   TiltY:       11.650877940316   TiltZ:       -1.153193791321
CrystalId:  140
PositionX:  9.219248527548   PositionY:   14.289340809165   PositionZ:   0.206084467315
TiltX:      -11.866472631927   TiltY:       7.447089549937   TiltZ:       -0.251881895444
CrystalId:  141
PositionX:  11.823355922669   PositionY:   9.243449136664   PositionZ:   0.685500456130
TiltX:      -7.635989974776   TiltY:       9.549273675971   TiltZ:       -0.699755447422
CrystalId:  142
PositionX:  14.328668479972   PositionY:   9.206249544822   PositionZ:   0.214073406395
TiltX:      -7.689678772711   TiltY:       11.678433456959   TiltZ:       -0.991192048248
CrystalId:  143
PositionX:  9.301549418735   PositionY:   9.267648986263   PositionZ:   1.059832670384
TiltX:      -7.593517636824   TiltY:       7.419874394031   TiltZ:       -0.411941857010
CrystalId:  144
PositionX:  -19.880328160436   PositionY:   -3.872316177238   PositionZ:   0.244148546259
TiltX:      3.222509995118   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  145
PositionX:  -17.417628114353   PositionY:   -3.869524037569   PositionZ:   0.903935610101
TiltX:      3.190218878127   TiltY:       -13.967572210623   TiltZ:       0.124416765094
CrystalId:  146
PositionX:  -22.315502436522   PositionY:   -3.869524037569   PositionZ:   -0.510925444605
TiltX:      3.260080654829   TiltY:       -18.257454131476   TiltZ:       -0.127138430665
CrystalId:  147
PositionX:  -19.906332606466   PositionY:   -1.324486049488   PositionZ:   0.334169052548
TiltX:      1.074169998373   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  148
PositionX:  -17.443613185892   PositionY:   -1.323554899942   PositionZ:   0.993889047032
TiltX:      1.063402437307   TiltY:       -13.964583645280   TiltZ:       0.041491130212
CrystalId:  149
PositionX:  -22.341487508061   PositionY:   -1.323554899942   PositionZ:   -0.420972007673
TiltX:      1.086700715744   TiltY:       -18.260508119414   TiltZ:       -0.042400059298
CrystalId:  150
PositionX:  -19.826797944409   PositionY:   -6.414562025649   PositionZ:   0.058841112947
TiltX:      5.370849991863   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  151
PositionX:  -17.364136620072   PositionY:   -6.409912820924   PositionZ:   0.718762221222
TiltX:      5.317070020452   TiltY:       -13.973545256309   TiltZ:       0.207172594820
CrystalId:  152
PositionX:  -22.262010942241   PositionY:   -6.409912820924   PositionZ:   -0.696098833484
TiltX:      5.433396169961   TiltY:       -18.251350609663   TiltZ:       -0.211691660881
CrystalId:  153
PositionX:  -19.880328160436   PositionY:   3.872316177238   PositionZ:   0.244148546259
TiltX:      -3.222509995118   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  154
PositionX:  -17.417628114353   PositionY:   3.869524037569   PositionZ:   0.903935610101
TiltX:      -3.190218878127   TiltY:       -13.967572210623   TiltZ:       -0.124416765094
CrystalId:  155
PositionX:  -22.315502436522   PositionY:   3.869524037569   PositionZ:   -0.510925444605
TiltX:      -3.260080654829   TiltY:       -18.257454131476   TiltZ:       0.127138430665
CrystalId:  156
PositionX:  -19.826797944409   PositionY:   6.414562025649   PositionZ:   0.058841112947
TiltX:      -5.370849991863   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  157
PositionX:  -17.364136620072   PositionY:   6.409912820924   PositionZ:   0.718762221222
TiltX:      -5.317070020452   TiltY:       -13.973545256309   TiltZ:       -0.207172594820
CrystalId:  158
PositionX:  -22.262010942241   PositionY:   6.409912820924   PositionZ:   -0.696098833484
TiltX:      -5.433396169961   TiltY:       -18.251350609663   TiltZ:       0.211691660881
CrystalId:  159
PositionX:  -19.906332606466   PositionY:   1.324486049488   PositionZ:   0.334169052548
TiltX:      -1.074169998373   TiltY:       -16.112549975589   TiltZ:       0.000000000000
CrystalId:  160
PositionX:  -17.443613185892   PositionY:   1.323554899942   PositionZ:   0.993889047032
TiltX:      -1.063402437307   TiltY:       -13.964583645280   TiltZ:       -0.041491130212
CrystalId:  161
PositionX:  -22.341487508061   PositionY:   1.323554899942   PositionZ:   -0.420972007673
TiltX:      -1.086700715744   TiltY:       -18.260508119414   TiltZ:       0.042400059298
CrystalId:  162
PositionX:  19.880328160436   PositionY:   -3.872316177238   PositionZ:   0.244148546259
TiltX:      3.222509995118   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  163
PositionX:  22.315502436522   PositionY:   -3.869524037569   PositionZ:   -0.510925444605
TiltX:      3.260080654829   TiltY:       18.257454131476   TiltZ:       0.127138430665
CrystalId:  164
PositionX:  17.417628114353   PositionY:   -3.869524037569   PositionZ:   0.903935610101
TiltX:      3.190218878127   TiltY:       13.967572210623   TiltZ:       -0.124416765094
CrystalId:  165
PositionX:  19.906332606466   PositionY:   -1.324486049488   PositionZ:   0.334169052548
TiltX:      1.074169998373   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  166
PositionX:  22.341487508061   PositionY:   -1.323554899942   PositionZ:   -0.420972007673
TiltX:      1.086700715744   TiltY:       18.260508119414   TiltZ:       0.042400059298
CrystalId:  167
PositionX:  17.443613185892   PositionY:   -1.323554899942   PositionZ:   0.993889047032
TiltX:      1.063402437307   TiltY:       13.964583645280   TiltZ:       -0.041491130212
CrystalId:  168
PositionX:  19.826797944409   PositionY:   -6.414562025649   PositionZ:   0.058841112947
TiltX:      5.370849991863   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  169
PositionX:  22.262010942241   PositionY:   -6.409912820924   PositionZ:   -0.696098833484
TiltX:      5.433396169961   TiltY:       18.251350609663   TiltZ:       0.211691660881
CrystalId:  170
PositionX:  17.364136620072   PositionY:   -6.409912820924   PositionZ:   0.718762221222
TiltX:      5.317070020452   TiltY:       13.973545256309   TiltZ:       -0.207172594820
CrystalId:  171
PositionX:  19.880328160436   PositionY:   3.872316177238   PositionZ:   0.244148546259
TiltX:      -3.222509995118   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  172
PositionX:  22.315502436522   PositionY:   3.869524037569   PositionZ:   -0.510925444605
TiltX:      -3.260080654829   TiltY:       18.257454131476   TiltZ:       -0.127138430665
CrystalId:  173
PositionX:  17.417628114353   PositionY:   3.869524037569   PositionZ:   0.903935610101
TiltX:      -3.190218878127   TiltY:       13.967572210623   TiltZ:       0.124416765094
CrystalId:  174
PositionX:  19.826797944409   PositionY:   6.414562025649   PositionZ:   0.058841112947
TiltX:      -5.370849991863   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  175
PositionX:  22.262010942241   PositionY:   6.409912820924   PositionZ:   -0.696098833484
TiltX:      -5.433396169961   TiltY:       18.251350609663   TiltZ:       -0.211691660881
CrystalId:  176
PositionX:  17.364136620072   PositionY:   6.409912820924   PositionZ:   0.718762221222
TiltX:      -5.317070020452   TiltY:       13.973545256309   TiltZ:       0.207172594820
CrystalId:  177
PositionX:  19.906332606466   PositionY:   1.324486049488   PositionZ:   0.334169052548
TiltX:      -1.074169998373   TiltY:       16.112549975589   TiltZ:       0.000000000000
CrystalId:  178
PositionX:  22.341487508061   PositionY:   1.323554899942   PositionZ:   -0.420972007673
TiltX:      -1.086700715744   TiltY:       18.260508119414   TiltZ:       -0.042400059298
CrystalId:  179
PositionX:  17.443613185892   PositionY:   1.323554899942   PositionZ:   0.993889047032
TiltX:      -1.063402437307   TiltY:       13.964583645280   TiltZ:       0.041491130212
CrystalId:  180
PositionX:  -3.919979571957   PositionY:   -19.912118846381   PositionZ:   0.239088735595
TiltX:      16.159334327042   TiltY:       -2.972379654807   TiltZ:       -0.865808716723
CrystalId:  181
PositionX:  -1.371948460886   PositionY:   -19.936800397508   PositionZ:   0.323626068682
TiltX:      16.139085801372   TiltY:       -0.908793086609   TiltZ:       -0.267959396464
CrystalId:  182
PositionX:  -6.462645763180   PositionY:   -19.859867931376   PositionZ:   0.059264475486
TiltX:      16.201243594830   TiltY:       -5.035641969984   TiltZ:       -1.465899090851
CrystalId:  183
PositionX:  -3.917083817816   PositionY:   -17.449699122657   PositionZ:   0.899920787979
TiltX:      14.010994330297   TiltY:       -2.972379654807   TiltZ:       -0.865808716723
CrystalId:  184
PositionX:  -1.369054747988   PositionY:   -17.476178972616   PositionZ:   0.983974101385
TiltX:      13.993474777154   TiltY:       -0.887860487309   TiltZ:       -0.345730533959
CrystalId:  185
PositionX:  -6.459752050282   PositionY:   -17.399246506484   PositionZ:   0.719612508188
TiltX:      14.047548770610   TiltY:       -5.056653433184   TiltZ:       -1.387856304310
CrystalId:  186
PositionX:  -3.917510406251   PositionY:   -22.346969206227   PositionZ:   -0.517030243811
TiltX:      18.307674323787   TiltY:       -2.972379654807   TiltZ:       -0.865808716723
CrystalId:  187
PositionX:  -1.369481024821   PositionY:   -22.369871836417   PositionZ:   -0.431941915967
TiltX:      18.284679913393   TiltY:       -0.932622332466   TiltZ:       -0.191026099843
CrystalId:  188
PositionX:  -6.460178327115   PositionY:   -22.292939370286   PositionZ:   -0.696303509164
TiltX:      18.354833465086   TiltY:       -5.011723921625   TiltZ:       -1.543091416905
CrystalId:  189
PositionX:  -3.919979571957   PositionY:   19.912118846381   PositionZ:   0.239088735595
TiltX:      -16.159334327042   TiltY:       -2.972379654807   TiltZ:       0.865808716723
CrystalId:  190
PositionX:  -1.371948460886   PositionY:   19.936800397508   PositionZ:   0.323626068682
TiltX:      -16.139085801372   TiltY:       -0.908793086609   TiltZ:       0.267959396464
CrystalId:  191
PositionX:  -6.462645763180   PositionY:   19.859867931376   PositionZ:   0.059264475486
TiltX:      -16.201243594830   TiltY:       -5.035641969984   TiltZ:       1.465899090851
CrystalId:  192
PositionX:  -3.917510406251   PositionY:   22.346969206227   PositionZ:   -0.517030243811
TiltX:      -18.307674323787   TiltY:       -2.972379654807   TiltZ:       0.865808716723
CrystalId:  193
PositionX:  -1.369481024821   PositionY:   22.369871836417   PositionZ:   -0.431941915967
TiltX:      -18.284679913393   TiltY:       -0.932622332466   TiltZ:       0.191026099843
CrystalId:  194
PositionX:  -6.460178327115   PositionY:   22.292939370286   PositionZ:   -0.696303509164
TiltX:      -18.354833465086   TiltY:       -5.011723921625   TiltZ:       1.543091416904
CrystalId:  195
PositionX:  -3.917083817816   PositionY:   17.449699122657   PositionZ:   0.899920787979
TiltX:      -14.010994330297   TiltY:       -2.972379654807   TiltZ:       0.865808716723
CrystalId:  196
PositionX:  -1.369054747988   PositionY:   17.476178972616   PositionZ:   0.983974101385
TiltX:      -13.993474777154   TiltY:       -0.887860487309   TiltZ:       0.345730533959
CrystalId:  197
PositionX:  -6.459752050282   PositionY:   17.399246506484   PositionZ:   0.719612508188
TiltX:      -14.047548770610   TiltY:       -5.056653433184   TiltZ:       1.387856304310
CrystalId:  198
PositionX:  3.919979571957   PositionY:   -19.912118846381   PositionZ:   0.239088735595
TiltX:      16.159334327042   TiltY:       2.972379654807   TiltZ:       0.865808716723
CrystalId:  199
PositionX:  6.462645763180   PositionY:   -19.859867931376   PositionZ:   0.059264475486
TiltX:      16.201243594830   TiltY:       5.035641969984   TiltZ:       1.465899090851
CrystalId:  200
PositionX:  1.371948460886   PositionY:   -19.936800397508   PositionZ:   0.323626068682
TiltX:      16.139085801372   TiltY:       0.908793086609   TiltZ:       0.267959396464
CrystalId:  201
PositionX:  3.917083817816   PositionY:   -17.449699122657   PositionZ:   0.899920787979
TiltX:      14.010994330297   TiltY:       2.972379654807   TiltZ:       0.865808716723
CrystalId:  202
PositionX:  6.459752050282   PositionY:   -17.399246506484   PositionZ:   0.719612508188
TiltX:      14.047548770610   TiltY:       5.056653433184   TiltZ:       1.387856304310
CrystalId:  203
PositionX:  1.369054747988   PositionY:   -17.476178972616   PositionZ:   0.983974101385
TiltX:      13.993474777154   TiltY:       0.887860487309   TiltZ:       0.345730533959
CrystalId:  204
PositionX:  3.917510406251   PositionY:   -22.346969206227   PositionZ:   -0.517030243811
TiltX:      18.307674323787   TiltY:       2.972379654807   TiltZ:       0.865808716723
CrystalId:  205
PositionX:  6.460178327115   PositionY:   -22.292939370286   PositionZ:   -0.696303509164
TiltX:      18.354833465086   TiltY:       5.011723921625   TiltZ:       1.543091416904
CrystalId:  206
PositionX:  1.369481024821   PositionY:   -22.369871836417   PositionZ:   -0.431941915967
TiltX:      18.284679913393   TiltY:       0.932622332466   TiltZ:       0.191026099843
CrystalId:  207
PositionX:  3.919979571957   PositionY:   19.912118846381   PositionZ:   0.239088735595
TiltX:      -16.159334327042   TiltY:       2.972379654807   TiltZ:       -0.865808716723
CrystalId:  208
PositionX:  6.462645763180   PositionY:   19.859867931376   PositionZ:   0.059264475486
TiltX:      -16.201243594830   TiltY:       5.035641969984   TiltZ:       -1.465899090851
CrystalId:  209
PositionX:  1.371948460886   PositionY:   19.936800397508   PositionZ:   0.323626068682
TiltX:      -16.139085801372   TiltY:       0.908793086609   TiltZ:       -0.267959396464
CrystalId:  210
PositionX:  3.917510406251   PositionY:   22.346969206227   PositionZ:   -0.517030243811
TiltX:      -18.307674323787   TiltY:       2.972379654807   TiltZ:       -0.865808716723
CrystalId:  211
PositionX:  6.460178327115   PositionY:   22.292939370286   PositionZ:   -0.696303509164
TiltX:      -18.354833465086   TiltY:       5.011723921625   TiltZ:       -1.543091416905
CrystalId:  212
PositionX:  1.369481024821   PositionY:   22.369871836417   PositionZ:   -0.431941915967
TiltX:      -18.284679913393   TiltY:       0.932622332466   TiltZ:       -0.191026099843
CrystalId:  213
PositionX:  3.917083817816   PositionY:   17.449699122657   PositionZ:   0.899920787979
TiltX:      -14.010994330297   TiltY:       2.972379654807   TiltZ:       -0.865808716723
CrystalId:  214
PositionX:  6.459752050282   PositionY:   17.399246506484   PositionZ:   0.719612508188
TiltX:      -14.047548770610   TiltY:       5.056653433184   TiltZ:       -1.387856304310
CrystalId:  215
PositionX:  1.369054747988   PositionY:   17.476178972616   PositionZ:   0.983974101385
TiltX:      -13.993474777154   TiltY:       0.887860487309   TiltZ:       -0.345730533959
CrystalId:  216
PositionX:  -19.954965407055   PositionY:   -11.764471567213   PositionZ:   0.502522930429
TiltX:      10.006802379881   TiltY:       -15.907523397074   TiltZ:       -1.230067992729
CrystalId:  217
PositionX:  -17.490488469933   PositionY:   -11.808755995072   PositionZ:   1.154144445171
TiltX:      9.908295910139   TiltY:       -13.791537499709   TiltZ:       -0.845766551554
CrystalId:  218
PositionX:  -22.392264498482   PositionY:   -11.703504886341   PositionZ:   -0.243181089535
TiltX:      10.121344046972   TiltY:       -18.022815399622   TiltZ:       -1.622547595657
CrystalId:  219
PositionX:  -20.008863095345   PositionY:   -9.243812714072   PositionZ:   0.881458634619
TiltX:      7.858462383136   TiltY:       -15.907523397074   TiltZ:       -1.230067992729
CrystalId:  220
PositionX:  -17.544346413859   PositionY:   -9.289938131739   PositionZ:   1.532802057982
TiltX:      7.780988798452   TiltY:       -13.779155587196   TiltZ:       -0.927698724767
CrystalId:  221
PositionX:  -22.446122442408   PositionY:   -9.184687023009   PositionZ:   0.135476523275
TiltX:      7.948627942537   TiltY:       -18.035461591952   TiltZ:       -1.538910379952
CrystalId:  222
PositionX:  -19.873889873070   PositionY:   -14.268448167342   PositionZ:   0.029504721018
TiltX:      12.155142376627   TiltY:       -15.907523397074   TiltZ:       -1.230067992729
CrystalId:  223
PositionX:  -17.409471783018   PositionY:   -14.310903330978   PositionZ:   0.681470455653
TiltX:      12.035708979660   TiltY:       -13.806864668815   TiltZ:       -0.764359275359
CrystalId:  224
PositionX:  -22.311247811567   PositionY:   -14.205652222247   PositionZ:   -0.715855079053
TiltX:      12.293863928277   TiltY:       -18.007163273874   TiltZ:       -1.705613838711
CrystalId:  225
PositionX:  -19.954965407055   PositionY:   11.764471567213   PositionZ:   0.502522930429
TiltX:      -10.006802379881   TiltY:       -15.907523397074   TiltZ:       1.230067992729
CrystalId:  226
PositionX:  -17.490488469933   PositionY:   11.808755995072   PositionZ:   1.154144445171
TiltX:      -9.908295910139   TiltY:       -13.791537499709   TiltZ:       0.845766551554
CrystalId:  227
PositionX:  -22.392264498482   PositionY:   11.703504886341   PositionZ:   -0.243181089535
TiltX:      -10.121344046972   TiltY:       -18.022815399622   TiltZ:       1.622547595657
CrystalId:  228
PositionX:  -19.873889873070   PositionY:   14.268448167342   PositionZ:   0.029504721018
TiltX:      -12.155142376627   TiltY:       -15.907523397074   TiltZ:       1.230067992729
CrystalId:  229
PositionX:  -17.409471783018   PositionY:   14.310903330978   PositionZ:   0.681470455653
TiltX:      -12.035708979660   TiltY:       -13.806864668815   TiltZ:       0.764359275359
CrystalId:  230
PositionX:  -22.311247811567   PositionY:   14.205652222247   PositionZ:   -0.715855079053
TiltX:      -12.293863928277   TiltY:       -18.007163273874   TiltZ:       1.705613838711
CrystalId:  231
PositionX:  -20.008863095345   PositionY:   9.243812714072   PositionZ:   0.881458634619
TiltX:      -7.858462383136   TiltY:       -15.907523397074   TiltZ:       1.230067992729
CrystalId:  232
PositionX:  -17.544346413859   PositionY:   9.289938131739   PositionZ:   1.532802057982
TiltX:      -7.780988798452   TiltY:       -13.779155587196   TiltZ:       0.927698724767
CrystalId:  233
PositionX:  -22.446122442408   PositionY:   9.184687023009   PositionZ:   0.135476523275
TiltX:      -7.948627942537   TiltY:       -18.035461591952   TiltZ:       1.538910379952
CrystalId:  234
PositionX:  19.954965407055   PositionY:   -11.764471567213   PositionZ:   0.502522930429
TiltX:      10.006802379881   TiltY:       15.907523397074   TiltZ:       1.230067992729
CrystalId:  235
PositionX:  22.392264498482   PositionY:   -11.703504886341   PositionZ:   -0.243181089535
TiltX:      10.121344046972   TiltY:       18.022815399622   TiltZ:       1.622547595657
CrystalId:  236
PositionX:  17.490488469933   PositionY:   -11.808755995072   PositionZ:   1.154144445171
TiltX:      9.908295910139   TiltY:       13.791537499709   TiltZ:       0.845766551554
CrystalId:  237
PositionX:  20.008863095345   PositionY:   -9.243812714072   PositionZ:   0.881458634619
TiltX:      7.858462383136   TiltY:       15.907523397074   TiltZ:       1.230067992729
CrystalId:  238
PositionX:  22.446122442408   PositionY:   -9.184687023009   PositionZ:   0.135476523275
TiltX:      7.948627942537   TiltY:       18.035461591952   TiltZ:       1.538910379952
CrystalId:  239
PositionX:  17.544346413859   PositionY:   -9.289938131739   PositionZ:   1.532802057982
TiltX:      7.780988798452   TiltY:       13.779155587196   TiltZ:       0.927698724767
CrystalId:  240
PositionX:  19.873889873070   PositionY:   -14.268448167342   PositionZ:   0.029504721018
TiltX:      12.155142376627   TiltY:       15.907523397074   TiltZ:       1.230067992729
CrystalId:  241
PositionX:  22.311247811567   PositionY:   -14.205652222247   PositionZ:   -0.715855079053
TiltX:      12.293863928277   TiltY:       18.007163273874   TiltZ:       1.705613838711
CrystalId:  242
PositionX:  17.409471783018   PositionY:   -14.310903330978   PositionZ:   0.681470455653
TiltX:      12.035708979660   TiltY:       13.806864668815   TiltZ:       0.764359275359
CrystalId:  243
PositionX:  19.954965407055   PositionY:   11.764471567213   PositionZ:   0.502522930429
TiltX:      -10.006802379881   TiltY:       15.907523397074   TiltZ:       -1.230067992729
CrystalId:  244
PositionX:  22.392264498482   PositionY:   11.703504886341   PositionZ:   -0.243181089535
TiltX:      -10.121344046972   TiltY:       18.022815399622   TiltZ:       -1.622547595657
CrystalId:  245
PositionX:  17.490488469933   PositionY:   11.808755995072   PositionZ:   1.154144445171
TiltX:      -9.908295910139   TiltY:       13.791537499709   TiltZ:       -0.845766551554
CrystalId:  246
PositionX:  19.873889873070   PositionY:   14.268448167342   PositionZ:   0.029504721018
TiltX:      -12.155142376627   TiltY:       15.907523397074   TiltZ:       -1.230067992729
CrystalId:  247
PositionX:  22.311247811567   PositionY:   14.205652222247   PositionZ:   -0.715855079053
TiltX:      -12.293863928277   TiltY:       18.007163273874   TiltZ:       -1.705613838711
CrystalId:  248
PositionX:  17.409471783018   PositionY:   14.310903330978   PositionZ:   0.681470455653
TiltX:      -12.035708979660   TiltY:       13.806864668815   TiltZ:       -0.764359275359
CrystalId:  249
PositionX:  20.008863095345   PositionY:   9.243812714072   PositionZ:   0.881458634619
TiltX:      -7.858462383136   TiltY:       15.907523397074   TiltZ:       -1.230067992729
CrystalId:  250
PositionX:  22.446122442408   PositionY:   9.184687023009   PositionZ:   0.135476523275
TiltX:      -7.948627942537   TiltY:       18.035461591952   TiltZ:       -1.538910379952
CrystalId:  251
PositionX:  17.544346413859   PositionY:   9.289938131739   PositionZ:   1.532802057982
TiltX:      -7.780988798452   TiltY:       13.779155587196   TiltZ:       -0.927698724767
CrystalId:  252
PositionX:  -11.990467607068   PositionY:   -19.912118846381   PositionZ:   0.503557482782
TiltX:      16.305386816423   TiltY:       -9.332548521574   TiltZ:       -1.168321871295
CrystalId:  253
PositionX:  -9.467648461238   PositionY:   -19.949620834737   PositionZ:   0.869884441851
TiltX:      16.217600864159   TiltY:       -7.270136108765   TiltZ:       -0.560403162525
CrystalId:  254
PositionX:  -14.497259805665   PositionY:   -19.847047494147   PositionZ:   0.043148018493
TiltX:      16.416064719393   TiltY:       -11.393916520340   TiltZ:       -1.783476890321
CrystalId:  255
PositionX:  -12.048601860519   PositionY:   -17.449934095746   PositionZ:   1.162712202820
TiltX:      14.157046819677   TiltY:       -9.332548521574   TiltZ:       -1.168321871295
CrystalId:  256
PositionX:  -9.525740029457   PositionY:   -17.489234211297   PositionZ:   1.528556384022
TiltX:      14.080672451157   TiltY:       -7.249091079002   TiltZ:       -0.638762536525
CrystalId:  257
PositionX:  -14.555351373883   PositionY:   -17.386660870706   PositionZ:   0.701819960665
TiltX:      14.253568417394   TiltY:       -11.415213344897   TiltZ:       -1.704249970912
CrystalId:  258
PositionX:  -11.916306406384   PositionY:   -22.346734233138   PositionZ:   -0.249679742476
TiltX:      18.453726813168   TiltY:       -9.332548521574   TiltZ:       -1.168321871295
CrystalId:  259
PositionX:  -9.393541210774   PositionY:   -22.382457472195   PositionZ:   0.117196122971
TiltX:      18.354605459101   TiltY:       -7.294064980355   TiltZ:       -0.482885480649
CrystalId:  260
PositionX:  -14.423152555201   PositionY:   -22.279884131604   PositionZ:   -0.709540300387
TiltX:      18.578356030376   TiltY:       -11.369704500259   TiltZ:       -1.861821365895
CrystalId:  261
PositionX:  -11.990467607068   PositionY:   19.912118846381   PositionZ:   0.503557482782
TiltX:      -16.305386816423   TiltY:       -9.332548521574   TiltZ:       1.168321871295
CrystalId:  262
PositionX:  -9.467648461238   PositionY:   19.949620834737   PositionZ:   0.869884441851
TiltX:      -16.217600864159   TiltY:       -7.270136108765   TiltZ:       0.560403162525
CrystalId:  263
PositionX:  -14.497259805665   PositionY:   19.847047494147   PositionZ:   0.043148018493
TiltX:      -16.416064719393   TiltY:       -11.393916520340   TiltZ:       1.783476890321
CrystalId:  264
PositionX:  -11.916306406384   PositionY:   22.346734233138   PositionZ:   -0.249679742476
TiltX:      -18.453726813168   TiltY:       -9.332548521574   TiltZ:       1.168321871295
CrystalId:  265
PositionX:  -9.393541210774   PositionY:   22.382457472195   PositionZ:   0.117196122971
TiltX:      -18.354605459101   TiltY:       -7.294064980355   TiltZ:       0.482885480649
CrystalId:  266
PositionX:  -14.423152555201   PositionY:   22.279884131604   PositionZ:   -0.709540300387
TiltX:      -18.578356030376   TiltY:       -11.369704500259   TiltZ:       1.861821365895
CrystalId:  267
PositionX:  -12.048601860519   PositionY:   17.449934095746   PositionZ:   1.162712202820
TiltX:      -14.157046819677   TiltY:       -9.332548521574   TiltZ:       1.168321871295
CrystalId:  268
PositionX:  -9.525740029457   PositionY:   17.489234211297   PositionZ:   1.528556384022
TiltX:      -14.080672451157   TiltY:       -7.249091079002   TiltZ:       0.638762536525
CrystalId:  269
PositionX:  -14.555351373883   PositionY:   17.386660870706   PositionZ:   0.701819960665
TiltX:      -14.253568417394   TiltY:       -11.415213344897   TiltZ:       1.704249970912
CrystalId:  270
PositionX:  11.990467607068   PositionY:   -19.912118846381   PositionZ:   0.503557482782
TiltX:      16.305386816423   TiltY:       9.332548521574   TiltZ:       1.168321871295
CrystalId:  271
PositionX:  14.497259805665   PositionY:   -19.847047494147   PositionZ:   0.043148018493
TiltX:      16.416064719393   TiltY:       11.393916520340   TiltZ:       1.783476890321
CrystalId:  272
PositionX:  9.467648461238   PositionY:   -19.949620834737   PositionZ:   0.869884441851
TiltX:      16.217600864159   TiltY:       7.270136108765   TiltZ:       0.560403162525
CrystalId:  273
PositionX:  12.048601860519   PositionY:   -17.449934095746   PositionZ:   1.162712202820
TiltX:      14.157046819677   TiltY:       9.332548521574   TiltZ:       1.168321871295
CrystalId:  274
PositionX:  14.555351373883   PositionY:   -17.386660870706   PositionZ:   0.701819960665
TiltX:      14.253568417394   TiltY:       11.415213344897   TiltZ:       1.704249970912
CrystalId:  275
PositionX:  9.525740029457   PositionY:   -17.489234211297   PositionZ:   1.528556384022
TiltX:      14.080672451157   TiltY:       7.249091079002   TiltZ:       0.638762536525
CrystalId:  276
PositionX:  11.916306406384   PositionY:   -22.346734233138   PositionZ:   -0.249679742476
TiltX:      18.453726813168   TiltY:       9.332548521574   TiltZ:       1.168321871295
CrystalId:  277
PositionX:  14.423152555201   PositionY:   -22.279884131604   PositionZ:   -0.709540300387
TiltX:      18.578356030376   TiltY:       11.369704500259   TiltZ:       1.861821365895
CrystalId:  278
PositionX:  9.393541210774   PositionY:   -22.382457472195   PositionZ:   0.117196122971
TiltX:      18.354605459101   TiltY:       7.294064980355   TiltZ:       0.482885480649
CrystalId:  279
PositionX:  11.990467607068   PositionY:   19.912118846381   PositionZ:   0.503557482782
TiltX:      -16.305386816423   TiltY:       9.332548521574   TiltZ:       -1.168321871295
CrystalId:  280
PositionX:  14.497259805665   PositionY:   19.847047494147   PositionZ:   0.043148018493
TiltX:      -16.416064719393   TiltY:       11.393916520340   TiltZ:       -1.783476890321
CrystalId:  281
PositionX:  9.467648461238   PositionY:   19.949620834737   PositionZ:   0.869884441851
TiltX:      -16.217600864159   TiltY:       7.270136108765   TiltZ:       -0.560403162525
CrystalId:  282
PositionX:  11.916306406384   PositionY:   22.346734233138   PositionZ:   -0.249679742476
TiltX:      -18.453726813168   TiltY:       9.332548521574   TiltZ:       -1.168321871295
CrystalId:  283
PositionX:  14.423152555201   PositionY:   22.279884131604   PositionZ:   -0.709540300387
TiltX:      -18.578356030376   TiltY:       11.369704500259   TiltZ:       -1.861821365895
CrystalId:  284
PositionX:  9.393541210774   PositionY:   22.382457472195   PositionZ:   0.117196122971
TiltX:      -18.354605459101   TiltY:       7.294064980355   TiltZ:       -0.482885480649
CrystalId:  285
PositionX:  12.048601860519   PositionY:   17.449934095746   PositionZ:   1.162712202820
TiltX:      -14.157046819677   TiltY:       9.332548521574   TiltZ:       -1.168321871295
CrystalId:  286
PositionX:  14.555351373883   PositionY:   17.386660870706   PositionZ:   0.701819960665
TiltX:      -14.253568417394   TiltY:       11.415213344897   TiltZ:       -1.704249970912
CrystalId:  287
PositionX:  9.525740029457   PositionY:   17.489234211297   PositionZ:   1.528556384022
TiltX:      -14.080672451157   TiltY:       7.249091079002   TiltZ:       -0.638762536525

// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
// Parameter of the modules used in the run
// -+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+--+-+-+-+-+-
ModuleId:   0
PositionX:  -3.865876853086   PositionY:   -3.872316177238   PositionZ:   -0.228920210837
TiltX:      3.233733009436   TiltY:       -3.211247674512   TiltZ:       -0.199997790452
ModuleId:   1
PositionX:  -3.865876853086   PositionY:   3.872316177238   PositionZ:   -0.228920210837
TiltX:      -3.233733009436   TiltY:       -3.211247674512   TiltZ:       0.199997790452
ModuleId:   2
PositionX:  3.865876853086   PositionY:   -3.872316177238   PositionZ:   -0.228920210837
TiltX:      3.233733009436   TiltY:       3.211247674512   TiltZ:       0.199997790452
ModuleId:   3
PositionX:  3.865876853086   PositionY:   3.872316177238   PositionZ:   -0.228920210837
TiltX:      -3.233733009436   TiltY:       3.211247674512   TiltZ:       -0.199997790452
ModuleId:   4
PositionX:  -11.745234987481   PositionY:   -3.872316177238   PositionZ:   0.041464145089
TiltX:      3.222509995118   TiltY:       -9.667529985354   TiltZ:       0.000000000000
ModuleId:   5
PositionX:  -11.745234987481   PositionY:   3.872316177238   PositionZ:   0.041464145089
TiltX:      -3.222509995118   TiltY:       -9.667529985354   TiltZ:       0.000000000000
ModuleId:   6
PositionX:  11.745234987481   PositionY:   -3.872316177238   PositionZ:   0.041464145089
TiltX:      3.222509995118   TiltY:       9.667529985354   TiltZ:       0.000000000000
ModuleId:   7
PositionX:  11.745234987481   PositionY:   3.872316177238   PositionZ:   0.041464145089
TiltX:      -3.222509995118   TiltY:       9.667529985354   TiltZ:       0.000000000000
ModuleId:   8
PositionX:  -3.880994910796   PositionY:   -11.764471567213   PositionZ:   0.039593262399
TiltX:      9.686777919761   TiltY:       -3.163676101701   TiltZ:       -0.345556168089
ModuleId:   9
PositionX:  -3.880994910796   PositionY:   11.764471567213   PositionZ:   0.039593262399
TiltX:      -9.686777919761   TiltY:       -3.163676101701   TiltZ:       0.345556168089
ModuleId:   10
PositionX:  3.880994910796   PositionY:   -11.764471567213   PositionZ:   0.039593262399
TiltX:      9.686777919761   TiltY:       3.163676101701   TiltZ:       0.345556168089
ModuleId:   11
PositionX:  3.880994910796   PositionY:   11.764471567213   PositionZ:   0.039593262399
TiltX:      -9.686777919761   TiltY:       3.163676101701   TiltZ:       -0.345556168089
ModuleId:   12
PositionX:  -11.790398069147   PositionY:   -11.764471567213   PositionZ:   0.306583621991
TiltX:      9.784329971522   TiltY:       -9.549273675971   TiltZ:       -0.699755447422
ModuleId:   13
PositionX:  -11.790398069147   PositionY:   11.764471567213   PositionZ:   0.306583621991
TiltX:      -9.784329971522   TiltY:       -9.549273675971   TiltZ:       0.699755447422
ModuleId:   14
PositionX:  11.790398069147   PositionY:   -11.764471567213   PositionZ:   0.306583621991
TiltX:      9.784329971522   TiltY:       9.549273675971   TiltZ:       0.699755447422
ModuleId:   15
PositionX:  11.790398069147   PositionY:   11.764471567213   PositionZ:   0.306583621991
TiltX:      -9.784329971522   TiltY:       9.549273675971   TiltZ:       -0.699755447422
ModuleId:   16
PositionX:  -19.880328160436   PositionY:   -3.872316177238   PositionZ:   0.244148546259
TiltX:      3.222509995118   TiltY:       -16.112549975589   TiltZ:       0.000000000000
ModuleId:   17
PositionX:  -19.880328160436   PositionY:   3.872316177238   PositionZ:   0.244148546259
TiltX:      -3.222509995118   TiltY:       -16.112549975589   TiltZ:       0.000000000000
ModuleId:   18
PositionX:  19.880328160436   PositionY:   -3.872316177238   PositionZ:   0.244148546259
TiltX:      3.222509995118   TiltY:       16.112549975589   TiltZ:       0.000000000000
ModuleId:   19
PositionX:  19.880328160436   PositionY:   3.872316177238   PositionZ:   0.244148546259
TiltX:      -3.222509995118   TiltY:       16.112549975589   TiltZ:       0.000000000000
ModuleId:   20
PositionX:  -3.919979571957   PositionY:   -19.912118846381   PositionZ:   0.239088735595
TiltX:      16.159334327042   TiltY:       -2.972379654807   TiltZ:       -0.865808716723
ModuleId:   21
PositionX:  -3.919979571957   PositionY:   19.912118846381   PositionZ:   0.239088735595
TiltX:      -16.159334327042   TiltY:       -2.972379654807   TiltZ:       0.865808716723
ModuleId:   22
PositionX:  3.919979571957   PositionY:   -19.912118846381   PositionZ:   0.239088735595
TiltX:      16.159334327042   TiltY:       2.972379654807   TiltZ:       0.865808716723
ModuleId:   23
PositionX:  3.919979571957   PositionY:   19.912118846381   PositionZ:   0.239088735595
TiltX:      -16.159334327042   TiltY:       2.972379654807   TiltZ:       -0.865808716723
ModuleId:   24
PositionX:  -19.954965407055   PositionY:   -11.764471567213   PositionZ:   0.502522930429
TiltX:      10.006802379881   TiltY:       -15.907523397074   TiltZ:       -1.230067992729
ModuleId:   25
PositionX:  -19.954965407055   PositionY:   11.764471567213   PositionZ:   0.502522930429
TiltX:      -10.006802379881   TiltY:       -15.907523397074   TiltZ:       1.230067992729
ModuleId:   26
PositionX:  19.954965407055   PositionY:   -11.764471567213   PositionZ:   0.502522930429
TiltX:      10.006802379881   TiltY:       15.907523397074   TiltZ:       1.230067992729
ModuleId:   27
PositionX:  19.954965407055   PositionY:   11.764471567213   PositionZ:   0.502522930429
TiltX:      -10.006802379881   TiltY:       15.907523397074   TiltZ:       -1.230067992729
ModuleId:   28
PositionX:  -11.990467607068   PositionY:   -19.912118846381   PositionZ:   0.503557482782
TiltX:      16.305386816423   TiltY:       -9.332548521574   TiltZ:       -1.168321871295
ModuleId:   29
PositionX:  -11.990467607068   PositionY:   19.912118846381   PositionZ:   0.503557482782
TiltX:      -16.305386816423   TiltY:       -9.332548521574   TiltZ:       1.168321871295
ModuleId:   30
PositionX:  11.990467607068   PositionY:   -19.912118846381   PositionZ:   0.503557482782
TiltX:      16.305386816423   TiltY:       9.332548521574   TiltZ:       1.168321871295
ModuleId:   31
PositionX:  11.990467607068   PositionY:   19.912118846381   PositionZ:   0.503557482782
TiltX:      -16.305386816423   TiltY:       9.332548521574   TiltZ:       -1.168321871295
