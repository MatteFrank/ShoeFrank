# SHOE (Software for Hadrontherapy Optimization Experiment)
<img src="docs/FOOT_logo.png" align="right" width="200" hspace="100">
This is the repository for the official simulation and analysis software of the FOOT  experiment.

Useful links:
- FOOT [webpage](https://web.infn.it/foot/en/home/)
- FOOT [TWiki](http://arpg-serv.ing2.uniroma1.it/twiki/bin/view/Main/FOOTHome) containing also campaigns
- SHOE [TWiki](http://arpg-serv.ing2.uniroma1.it/twiki/bin/view/Main/FOOTSoftware) containing also the installation guide
- SHOE [Doxygen manual](https://asarti.baltig-pages.infn.it/shoe/) with classes and methods documentation
- FOOT [Indico page](https://agenda.infn.it/category/903/):
  - FOOT [Analysis and Reconstruction Meeting](https://agenda.infn.it/category/1858/), status and update meeting about SHOE 
  - FOOT [Physics Meeting](https://agenda.infn.it/category/1375/), report on ongoing physics analysis

## Local reconstruction tools:

Flags in FootGlobal.par for local reconstruction
```
EnableTree:          y
EnableHisto:         y
EnableTracking:      y

EnableSaveHits:      n
EnableRootObject:    y
```

1) Running from raw data:
   - command:
   ```
   DecodeGlb -in dataRaw/data_test.00004313.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data -out run4313_Out.root -nev 10000 -exp GSI2021 -run 4313
   ```

2) Running from MC data:
   - command:
   ```
   DecodeGlb -in 16O_C_200_trig_shoereg.root -out 16O_C_200_1_Out.root -nev 10000 -exp GSI2021_MC -run 200 -mc
   ```

## Global reconstruction tools with TOE:

1) Running from raw data:
   - activate TOE in FootGlobal.par
   ```
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in dataRaw/data_built.2239.physics_foot.daq.VTX.1.dat -out 12C_C_200_1_GlbOut.root -nev 10000 -exp GSI -run 2239
   ```

2) Running from MC data:
   - activate TOE in FootGlobal.par
   ```
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in 12C_C_200_1.root -out 12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1 -mc
   ```

3) Running from local reconstruction tree:
   - activate TOE in FootGlobal.par and local reco
   ```
   IncludeKalman:   n
   IncludeTOE:      y
   FromLocalReco: y

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in 12C_C_200_L0Out.root -out  12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1
   ```

## Global reconstruction tools with GenFit:

1) Running from raw data:
   - activate Kalman in FootGlobal.par
   ```
   IncludeKalman:   y
   IncludeTOE:      n
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in dataRaw/data_built.2239.physics_foot.daq.VTX.1.dat -out 12C_C_200_1_GlbOut.root -nev 10000 -exp GSI -run 2239
   ```

2) Running from MC data:
   - activate Kalman in FootGlobal.par
   ```
   IncludeKalman:   y
   IncludeTOE:      n
   FromLocalReco:   n

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in 12C_C_200_1.root -out 12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1
   ```

3) Running from local reconstruction tree:
   - activate Kalman in FootGlobal.par and local reco
   ```
   IncludeKalman:   y
   IncludeTOE:      n
   FromLocalReco:   y

   Kalman Mode:       ON
   Tracking Systems Considered:      VT IT MSD
   Reverse Tracking:   false
   ```
   - command:
   ```
   DecodeGlb -in 12C_C_200_L0Out.root -out  12C_C_200_1_GlbOut.root -nev 10000 -exp 12C_200 -run 1 -mc
   ```

## Convert recontructed tree into a flat Ntpule (std::vector)
Command:
```
ConvertNtuple -in run4313_Out.root -out run4313_FlatOut.root -nev 10000 -exp GSI2021 -run 4313 (-mc)
```
