COPY USREIN
*
*=== Usrein ===========================================================*
*
      SUBROUTINE USREIN

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(CASLIM)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1991-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     USeR Event INitialization: this routine is called before the     *
*     showering of an event is started, but after the source particles *
*     of that event have been already loaded on the stack              *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 09-apr-99     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
c
      include 'mgdraw.inc'
      integer ii  
c
      if(idbflg .gt. 0)then
         write(*,*)'***************************************************'
         write(*,*)'                   EVENT',ncase
         write(*,*)'***************************************************'
         write(*,*)''
      endif
c
      numev = ncase
      tarfrag = 0
c
      do ii = 1,min(nump,maxnump)
         idpa(ii)   = 0
         igen(ii)   = 0
         icha(ii)   = 0
         numreg(ii) = 0
         iba(ii)    = 0
         idead(ii)  = 0
         jpa(ii)    = 0
         vxi(ii)    = 0.
         vyi(ii)    = 0.
         vzi(ii)    = 0.
         vxf(ii)    = 0.
         vyf(ii)    = 0. 
         vzf(ii)    = 0.
         px(ii)     = 0.
         py(ii)     = 0.
         pz(ii)     = 0.
         pxf(ii)    = 0.
         pyf(ii)    = 0.
         pzf(ii)    = 0.
         amass(ii)  = 0.
         tempo(ii)  = 0.
         tof(ii)    = 0.
         trlen(ii)  = 0.
c
         idfluka(ii) = 0   ! aux variables for particle latching
c
      end do
      nump = 0
c
c
c
      do ii = 1,min(numint,maxint)
         xint(ii)    = 0
         yint(ii)    = 0
         zint(ii)    = 0
         intpa(ii)   = 0
         intcode(ii) = 0
      end do
      numint = 0
c
c     start counter
c
      nSTC = 0
      do ii = 1,maxSTC
         idSTC(ii)    = 0
         xinSTC(ii)   = 0.  
         xoutSTC(ii)  = 0. 
         yinSTC(ii)   = 0. 
         youtSTC(ii)  = 0. 
         zinSTC(ii)   = 0.
         zoutSTC(ii)  = 0.
         pxinSTC(ii)  = 0.  
         pxoutSTC(ii) = 0. 
         pyinSTC(ii)  = 0. 
         pyoutSTC(ii) = 0. 
         pzinSTC(ii)  = 0.
         pzoutSTC(ii) = 0.
         deSTC(ii)    = 0.
         alSTC(ii)    = 0.
         timSTC(ii)   = 0.
      end do
c
c     beam monitor
c
      nBMN = 0
      do ii = 1,maxBMN
         idBMN(ii)     = 0
         iviewBMN(ii)  = 0
         ilayBMN(ii)   = 0
         icellBMN(ii)  = 0
         xinBMN(ii)    = 0.  
         xoutBMN(ii)   = 0. 
         yinBMN(ii)    = 0. 
         youtBMN(ii)   = 0. 
         zinBMN(ii)    = 0.
         zoutBMN(ii)   = 0.
         pxinBMN(ii)   = 0.  
         pxoutBMN(ii)  = 0. 
         pyinBMN(ii)   = 0. 
         pyoutBMN(ii)  = 0. 
         pzinBMN(ii)   = 0.
         pzoutBMN(ii)  = 0.
         deBMN(ii)     = 0.
         alBMN(ii)     = 0.
         timBMN(ii)    = 0.
      end do
c     
c     vertex 
c
      nVTX = 0
      do ii = 1,maxVTX
         idVTX(ii)    = 0
         ilayVTX(ii)  = 0
         xinVTX(ii)   = 0.  
         xoutVTX(ii)  = 0. 
         yinVTX(ii)   = 0. 
         youtVTX(ii)  = 0. 
         zinVTX(ii)   = 0.
         zoutVTX(ii)  = 0.
         pxinVTX(ii)  = 0.  
         pxoutVTX(ii) = 0. 
         pyinVTX(ii)  = 0. 
         pyoutVTX(ii) = 0. 
         pzinVTX(ii)  = 0.
         pzoutVTX(ii) = 0.
         deVTX(ii)    = 0.
         alVTX(ii)    = 0.
         timVTX(ii)   = 0.
      end do
c     
c     inner tracker 
c
      nITR = 0
      do ii = 1,maxITR
         idITR(ii)    = 0
         isensITR(ii)= 0
c         iplumeITR(ii)= 0
c         imimoITR(ii) = 0
c         ilayITR(ii)  = 0
         xinITR(ii)   = 0.  
         xoutITR(ii)  = 0. 
         yinITR(ii)   = 0. 
         youtITR(ii)  = 0. 
         zinITR(ii)   = 0.
         zoutITR(ii)  = 0.
         pxinITR(ii)  = 0.  
         pxoutITR(ii) = 0. 
         pyinITR(ii)  = 0. 
         pyoutITR(ii) = 0. 
         pzinITR(ii)  = 0.
         pzoutITR(ii) = 0.
         deITR(ii)    = 0.
         alITR(ii)    = 0.
         timITR(ii)   = 0.
      end do
c     
c     second drift chamber
c
      nMSD = 0
      do ii = 1,maxMSD
         idMSD(ii)     = 0
         ilayMSD(ii)   = 0
         xinMSD(ii)    = 0.  
         xoutMSD(ii)   = 0. 
         yinMSD(ii)    = 0. 
         youtMSD(ii)   = 0. 
         zinMSD(ii)    = 0.
         zoutMSD(ii)   = 0.
         pxinMSD(ii)   = 0.  
         pxoutMSD(ii)  = 0. 
         pyinMSD(ii)   = 0. 
         pyoutMSD(ii)  = 0. 
         pzinMSD(ii)   = 0.
         pzoutMSD(ii)  = 0.
         deMSD(ii)     = 0.
         alMSD(ii)     = 0.
         timMSD(ii)    = 0.
      end do
c
c     scintillator
c      
      nSCN = 0
      do ii= 1, maxSCN
         idSCN(ii)     = 0
         istripSCN(ii) = 0
         iviewSCN(ii)   = 0
         xinSCN(ii)    = 0.
         yinSCN(ii)    = 0.
         zinSCN(ii)    = 0.
         pxinSCN(ii)   = 0.
         pyinSCN(ii)   = 0.
         pzinSCN(ii)   = 0.
         timSCN(ii)    = 0.
         xoutSCN(ii)   = 0. 
         youtSCN(ii)   = 0.
         zoutSCN(ii)   = 0.
         pxoutSCN(ii)  = 0.
         pyoutSCN(ii)  = 0.
         pzoutSCN(ii)  = 0.
         deSCN(ii)     = 0.
         alSCN(ii)     = 0.
      enddo
c
c     calorimeter
c      
      nCAL = 0
      do ii= 1, maxCAL
         idCAL(ii)    = 0
         icryCAL(ii)    = 0
         xinCAL(ii)   = 0.
         yinCAL(ii)   = 0.
         zinCAL(ii)   = 0.
         pxinCAL(ii)  = 0.
         pyinCAL(ii)  = 0.
         pzinCAL(ii)  = 0.
         timCAL(ii)   = 0.
         xoutCAL(ii)  = 0. 
         youtCAL(ii)  = 0.
         zoutCAL(ii)  = 0.
         pxoutCAL(ii) = 0.
         pyoutCAL(ii) = 0.
         pzoutCAL(ii) = 0.
         deCAL(ii)    = 0.
         alCAL(ii)    = 0.
      enddo
c
c     crossings
c      
      nCROSS = 0
      do ii= 1, nmaxCROSS
         idCROSS(ii)       = 0
         nregCROSS(ii)     = 0
         nregoldCROSS(ii)  = 0
         pxCROSS(ii)       = 0.
         pyCROSS(ii)       = 0.
         pzCROSS(ii)       = 0.
         xCROSS(ii)        = 0.
         yCROSS(ii)        = 0.
         zCROSS(ii)        = 0.
         tCROSS(ii)        = 0.
         chCROSS(ii)       = 0.
         amaCROSS(ii)      = 0.
      end do
c
c     triggers
c
      stcfrag = 0
      bmnfrag = 0
      tgtfrag = 0
      airfrag = 0
      elsfrag = 0
      trig_tgt = .false.
      trig_bmn = .false.
      trig_stc = .false.
      trig_air = .false.
      trig_els = .false.
c
c
      RETURN
*=== End of subroutine Usrein =========================================*
      END


