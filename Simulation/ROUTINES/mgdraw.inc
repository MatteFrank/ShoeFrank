c     
c     idcurr = position of the current particle in the particle common block
c     if idcurr<=0  -> new particle to be followed     
c     
      integer idbflg, outunit
      integer tarfrag, fragtrig, stcfrag, bmnfrag, tgtfrag, airfrag
      integer elsfrag
      logical trig_stc, trig_bmn, trig_tgt, trig_air, trig_els
      integer numev
      integer idcurr
      integer JSTART, JRUN, JEND
      parameter (JSTART=1, JRUN=0, JEND=-1)
      double precision Ethrdep
      common /comunico/Ethrdep, idbflg, numev,
     &     idcurr, outunit, stcfrag, bmnfrag, tgtfrag, airfrag,
     &     tarfrag, elsfrag, fragtrig, trig_stc, trig_bmn, trig_tgt,
     &     trig_air, trig_els
c     
c     particle common block
c     
c     nump          = number of tracks
c     
c     for each of the produced track we register:
c     
c     idpa          = index in the part common of the track parent
c     icha          = charge
c     iba           = barionic number
c     idead         = with which interaction the particle dies
c     jpa           = kind of particle
c     igen          = kind of interaction
c     vxi, vyi, vzi = production position of the track
c     vxf, vyf, vzf = final position of the track
c     px,py,pz,     = production momentum of the track
c     amass         = track mass
c     tempo         = production time of the track
c     tof           = tof of the particle
c     trlen         = track lenght of the particle
c     
      integer nump, maxnump
      parameter(maxnump=8000)

      integer idpa(maxnump), igen(maxnump)
      integer icha(maxnump), numreg(maxnump), iba(maxnump)
      integer idead(maxnump), jpa(maxnump)
      real vxi(maxnump),vyi(maxnump),vzi(maxnump)
      real vxf(maxnump),vyf(maxnump),vzf(maxnump)
      real px(maxnump),py(maxnump),pz(maxnump)
      real pxf(maxnump),pyf(maxnump),pzf(maxnump)
      real amass(maxnump), tempo(maxnump), tof(maxnump)
      real trlen(maxnump)
      common /particle_common/ vxi, vyi, vzi, 
     &     vxf, vyf, vzf,px, py, pz, pxf, pyf, pzf,
     &     amass, tempo, tof, trlen, nump, idpa, igen, 
     &     icha, numreg, iba, idead, jpa

c     *************************************************************
c     common needed to score the hit in the start counter
c     
      integer nSTC, maxSTC 
      parameter (maxSTC=200)
      integer idSTC (maxSTC)

      real xinSTC(maxSTC), xoutSTC(maxSTC), 
     &     yinSTC(maxSTC), youtSTC(maxSTC), 
     &     zinSTC(maxSTC), zoutSTC(maxSTC)
      real pxinSTC(maxSTC), pxoutSTC(maxSTC), 
     &     pyinSTC(maxSTC), pyoutSTC(maxSTC), 
     &     pzinSTC(maxSTC), pzoutSTC(maxSTC)
      real deSTC(maxSTC), timSTC(maxSTC), 
     &     alSTC(maxSTC)
      common /STC_common/ xinSTC, xoutSTC, yinSTC, 
     &     youtSTC, zinSTC, zoutSTC, pxinSTC, pxoutSTC,
     &     pyinSTC, pyoutSTC, pzinSTC, pzoutSTC,
     &     deSTC, alSTC, timSTC, nSTC, idSTC

c     *************************************************************
c     common needed to score the hit in the beam monitor
c     
      integer nBMN, maxBMN 
      parameter (maxBMN=1000)
      integer idBMN (maxBMN), iviewBMN(maxBMN),ilayBMN(maxBMN),
     &     icellBMN(maxBMN) 

      real xinBMN(maxBMN), xoutBMN(maxBMN), yinBMN(maxBMN), 
     &     youtBMN(maxBMN), zinBMN(maxBMN), zoutBMN(maxBMN)
      real pxinBMN(maxBMN), pxoutBMN(maxBMN), pyinBMN(maxBMN), 
     &     pyoutBMN(maxBMN), pzinBMN(maxBMN), pzoutBMN(maxBMN)
      real deBMN(maxBMN), timBMN(maxBMN), alBMN(maxBMN)
      common /BMN_common/ xinBMN, xoutBMN, yinBMN, youtBMN, zinBMN, 
     &     zoutBMN, pxinBMN, pxoutBMN, pyinBMN, pyoutBMN, pzinBMN, 
     &     pzoutBMN,deBMN, alBMN, timBMN, nBMN, idBMN, iviewBMN,
     &     ilayBMN, icellBMN
c     
c  common for the indexing of the beam monitor
c
      integer MAXBMNREG
      parameter (MAXBMNREG=200)
      integer ireg2layBMN(MAXBMNREG), ireg2viewBMN(MAXBMNREG), 
     & ireg2cellBMN(MAXBMNREG)
      common /BMN_index_common/ireg2layBMN, ireg2viewBMN, ireg2cellBMN

c     *************************************************************
c     common needed to score the hit in the vertex
c 
      integer nVTX, maxVTX 
      parameter (maxVTX=300)
      integer idVTX (maxVTX), ilayVTX(maxVTX)

      real xinVTX(maxVTX), xoutVTX(maxVTX), yinVTX(maxVTX),
     &     youtVTX(maxVTX), zinVTX(maxVTX), zoutVTX(maxVTX)
      real pxinVTX(maxVTX), pxoutVTX(maxVTX), pyinVTX(maxVTX), 
     &     pyoutVTX(maxVTX), pzinVTX(maxVTX), pzoutVTX(maxVTX)
      real deVTX(maxVTX), timVTX(maxVTX), alVTX(maxVTX)
      common /VTX_common/ xinVTX, xoutVTX, yinVTX, youtVTX, 
     &     zinVTX, zoutVTX, pxinVTX, pxoutVTX, pyinVTX, pyoutVTX,
     &     pzinVTX, pzoutVTX, deVTX, alVTX, timVTX, nVTX, idVTX,
     &     ilayVTX

c     *************************************************************
c     common needed to score the hit in the inner tracker
c     
      integer nITR, maxITR 
      parameter (maxITR=300)
      integer idITR (maxITR), isensITR(maxITR)
c      integer idITR (maxITR), iplumeITR(maxITR), ilayITR(maxITR), 
c     &     imimoITR(maxITR)

      real xinITR(maxITR), xoutITR(maxITR), yinITR(maxITR),
     &     youtITR(maxITR), zinITR(maxITR), zoutITR(maxITR)
      real pxinITR(maxITR), pxoutITR(maxITR), pyinITR(maxITR), 
     &     pyoutITR(maxITR), pzinITR(maxITR), pzoutITR(maxITR)
      real deITR(maxITR), timITR(maxITR), alITR(maxITR)
      common /ITR_common/ xinITR, xoutITR, yinITR, youtITR, 
     &     zinITR, zoutITR, pxinITR, pxoutITR, pyinITR, pyoutITR,
     &     pzinITR, pzoutITR, deITR, alITR, timITR, nITR, idITR,
     &     isensITR
c     &     iplumeITR, ilayITR, imimoITR
c     
c  common for the indexing of the ITR
c
       integer MAXITRREG
       parameter (MAXITRREG=300)
       integer ireg2sensITR(MAXITRREG)
       common /ITR_index_common/ireg2sensITR
c       integer ireg2plumeITR(MAXITRREG), ireg2layITR(MAXITRREG),
c     &      ireg2mimoITR(MAXITRREG)
c       common /ITR_index_common/ireg2plumeITR, ireg2layITR,
c     &      ireg2mimoITR
      
c     *************************************************************
c     common needed to score the hit in the microstrip detector
c
      integer nMSD, maxMSD 
      parameter (maxMSD=1000)
      integer idMSD (maxMSD), ilayMSD(maxMSD)

      real xinMSD(maxMSD), xoutMSD(maxMSD), yinMSD(maxMSD), 
     &     youtMSD(maxMSD), zinMSD(maxMSD), zoutMSD(maxMSD)
      real pxinMSD(maxMSD), pxoutMSD(maxMSD),pyinMSD(maxMSD), 
     &     pyoutMSD(maxMSD), pzinMSD(maxMSD), pzoutMSD(maxMSD)
      real deMSD(maxMSD), timMSD(maxMSD), alMSD(maxMSD)
      common /MSD_common/ xinMSD, xoutMSD, yinMSD, youtMSD, zinMSD, 
     &     zoutMSD, pxinMSD, pxoutMSD, pyinMSD, pyoutMSD, pzinMSD, 
     &     pzoutMSD,deMSD, alMSD, timMSD, nMSD, idMSD, 
     &     ilayMSD
c     
c  common for the indexing of the microstrip detector
c
      integer MAXMSDREG
      parameter (MAXMSDREG=300)
      integer ireg2layMSD(MAXMSDREG)
      common /MSD_index_common/ireg2layMSD

c      
c     *************************************************************
c     common needed to score the hit in the scintillator
c      
      integer nSCN, maxSCN
      parameter (maxSCN=1000)
      integer idSCN(maxSCN)
      integer istripSCN(maxSCN), iviewSCN(maxSCN)
      real xinSCN(maxSCN), xoutSCN(maxSCN), yinSCN(maxSCN),
     &     youtSCN(maxSCN), zinSCN(maxSCN), zoutSCN(maxSCN)
      real pxinSCN(maxSCN), pxoutSCN(maxSCN),
     &     pyinSCN(maxSCN), pyoutSCN(maxSCN),
     &     pzinSCN(maxSCN), pzoutSCN(maxSCN)
      real deSCN(maxSCN),alSCN(maxSCN), timSCN(maxSCN)
      common /SCN_common/ xinSCN, 
     &     xoutSCN, yinSCN, youtSCN, zinSCN, zoutSCN, 
     &     pxinSCN, pxoutSCN, pyinSCN, pyoutSCN, pzinSCN, 
     &     pzoutSCN, deSCN, alSCN, timSCN, nSCN, idSCN,
     &     istripSCN, iviewSCN
c     
c  common for the indexing of the SCN
c
       integer MAXSCNREG
       parameter (MAXSCNREG=300)
       integer ireg2stripSCN(MAXSCNREG),ireg2viewSCN(MAXSCNREG)
       common /SCN_index_common/ireg2stripSCN,ireg2viewSCN
      
c     *************************************************************
c     common neede to score the hit in the Calo regions
c     
      integer nCAL, maxCAL
      parameter (maxCAL=8000)
      integer idCAL(maxCAL),icryCAL(maxCAL)

      real xinCAL(maxCAL), xoutCAL(maxCAL), yinCAL(maxCAL),
     &     youtCAL(maxCAL), zinCAL(maxCAL), zoutCAL(maxCAL)
      real pxinCAL(maxCAL), pxoutCAL(maxCAL), pyinCAL(maxCAL), 
     &     pyoutCAL(maxCAL), pzinCAL(maxCAL), pzoutCAL(maxCAL)
      real deCAL(maxCAL), timCAL(maxCAL), alCAL(maxCAL)
      common /CAL_common/ xinCAL, xoutCAL, yinCAL, youtCAL, zinCAL, 
     &     zoutCAL, pxinCAL, pxoutCAL, pyinCAL, pyoutCAL, pzinCAL, 
     &     pzoutCAL, deCAL, alCAL, timCAL, nCAL, idCAL, icryCAL
c     
c  common for the indexing of the SCN
c
       integer MAXCALREG
       parameter (MAXCALREG=700)
       integer ireg2cryCAL(MAXCALREG)
       common /CAL_index_common/ireg2cryCAL
      
c     *************************************************************
c     common needed to score the boundary crossing info 
c     and put the right pointer to the particle common of the produced electrons
c     
      integer nCROSS, maxCROSS
      parameter (maxCROSS=15000)
      integer idCROSS(maxCROSS), nregCROSS(maxCROSS) 
      integer nregoldCROSS(maxCROSS)
      real pxCROSS(maxCROSS), pyCROSS(maxCROSS), pzCROSS(maxCROSS)
      real xCROSS(maxCROSS), yCROSS(maxCROSS), zCROSS(maxCROSS)
      real tCROSS(maxCROSS), amaCROSS(maxCROSS), chCROSS(maxCROSS)
      common /crossing_common/ 
     &     pxCROSS, pyCROSS, pzCROSS, 
     &     xCROSS, yCROSS, zCROSS, tCROSS, chCROSS, amaCROSS, nCROSS,  
     &     idCROSS, nregCROSS, nregoldCROSS
      
c     ************************************************************
c     Numbers of fluka region to be scored
c     
      integer  nregaria, nregMagAir, nregSTC, nregtarg,
     &     nregFirstBMN, nregLastBMN, nregFirstVTX, nregLastVTX,
     &     nregFirstITR, nregLastITR, nregFirstMSD, nregLastMSD,
     &     nregFirstSCN, nregLastSCN, nregFirstCAL, nregLastCAL
      
      common/region_common/
     &     nregaria, nregMagAir, nregSTC, nregtarg, nregFirstBMN,
     &     nregLastBMN, nregFirstVTX, nregLastVTX, nregFirstITR,
     &     nregLastITR, nregFirstMSD, nregLastMSD, nregFirstSCN,
     &     nregLastSCN, nregFirstCAL, nregLastCAL
      
c     ***********************************************************
c     absolute light efficiency of material in quenched formula
c     
      double precision abs_STC,abs_BMN,abs_VTX,abs_ITR,
     &     abs_MSD, abs_SCN, abs_CAL
      parameter (abs_STC=1.0)
      parameter (abs_BMN=1.0)
      parameter (abs_VTX=1.0)
      parameter (abs_ITR=1.0)
      parameter (abs_MSD=1.0)
      parameter (abs_SCN=1.0)
      parameter (abs_CAL=1.0)   

c     ***********************************************************
c     interaction stack: common block for the parent particle
c     recording with the info on the interaction point
c     
      integer idfluka(maxnump)
      integer numint, maxint
      parameter (MAXINT=8000)
      double precision xint(maxint),yint(maxint),zint(maxint)
      integer intpa(maxint),intcode(maxint)
      common /particle_stack/xint,yint,zint,intpa,intcode,
     &     idfluka,numint
