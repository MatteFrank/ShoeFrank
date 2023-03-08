# SHOE (Software for Hadrontherapy Optimization Experiment)

This is the repository for the official simulation and analysis software of the FOOT  experiment.

Useful links:
- [FOOT webpage](https://web.infn.it/foot/en/home/)
- [SHOE TWiki](http://arpg-serv.ing2.uniroma1.it/twiki/bin/view/Main/FOOTSoftware) containing also the installation guide
- SHOE [Doxygen manual](http://arpg-serv.ing2.uniroma1.it/FOOTshoe/shoe/html/index.html) with classes and methods documentation

A) Local reconstruction tools

1)Running from raw data:
- command:
DecodeGlb -in dataRaw/data_test.00004313.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data -out run4313_Out.root -nev 10000 -exp GSI2021 -run 4313

2)Running from MC data:
- command:
DecodeGlb -in 16O_C_200_trig_shoereg.root -out 16O_C_200_1_Out.root -nev 10000 -exp GSI2021_MC -run 200 -mc

B) Flags in FootGlobal.par for reconstruction

...
EnableTree:          y
EnableHisto:         y
EnableTracking:      y

EnableSaveHits:      n
EnableRootObject:    n
...

A) Global reconstruction tools with TOE

1)Running from raw data:
- activate TOE in FootGlobal.par
   . . .
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   . . .
- command:
DecodeGlb -in dataRaw/data_built.2239.physics_foot.daq.VTX.1.dat -out 12C_C_200_1_GlbOut.root -nev 10000
             -exp GSI -run 2239

2)Running from MC data:
- activate TOE in FootGlobal.par
   . . .
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   . . .
- command:
DecodeGlb -in 12C_C_200_1.root -out 12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1 -mc

3)Running from local reconstruction tree:
- activate TOE in FootGlobal.par and local reco
   . . .
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco: y

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   . . .
- command:
DecodeGlb -in 12C_C_200_L0Out.root -out  12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1

B) Global reconstruction tools with GenFit
1)Running from raw data:
 - activate Kalman in FootGlobal.par
   . . .
   IncludeKalman:   y
   IncludeTOE:      n
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   . . .
- command:
DecodeGlb -in dataRaw/data_built.2239.physics_foot.daq.VTX.1.dat -out 12C_C_200_1_GlbOut.root -nev 10000
          -exp GSI -run 2239

2)Running from MC data:
- activate Kalman in FootGlobal.par
   . . .
   IncludeKalman:   y
   IncludeTOE:      n
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   . . .
- command:
DecodeGlb -in 12C_C_200_1.root -out 12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1

3)Running from local reconstruction tree:
- activate Kalman in FootGlobal.par and local reco
. . .
IncludeKalman:   y
IncludeTOE:      n
FromLocalReco:   y

Kalman Mode:       ON
Tracking Systems Considered:      VT IT MSD
Reverse Tracking:   false
. . .
- command:
DecodeGlb -in 12C_C_200_L0Out.root -out  12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1 -mc

4) Convert recontructed tree into a flat Ntpule (std::vector)
ConvertNtuple -in run4313_Out.root -out run4313_FlatOut.root -nev 10000 -exp GSI2021 -run 4313 (-mc)
