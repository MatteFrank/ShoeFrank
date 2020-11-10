*$ CREATE USRINI.FOR
*COPY USRINI
*
*=== usrini ===========================================================*
*
      SUBROUTINE USRINI ( WHAT, SDUM )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1991-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     USeR INItialization: this routine is called every time the       *
*                          USRICALL card is found in the input stream  *
*                                                                      *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*      Infn -     Milan                                                *
*                                                                      *
*     Last change on 20-mar-05     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      DIMENSION WHAT (6)
      CHARACTER SDUM*8, SDUM10*10
c
      INCLUDE '(FLKMAT)'
      include "../mgdraw.inc"
      include "../parameters.inc"
      character*8 REGNAM
      integer cellBMN, strip, cry, layMSD
      integer sensITR
c      integer plmITR, layITR, mimoITR
      integer ia,ib,ic,id,ie,ig,ih,il,im
*
*
*  Don't change the following line:
      LUSRIN = .TRUE.
* *** Write from here on *** *
c
      idbflg = int(what(1))
      fragtrig = int(what(2))
      Ethrdep = what(3)/1000.
c 
c  init of geometry parameters
c
      write(*,*)" "
      write(*,*)"fragtrig= ",fragtrig," Ethrdep= ",Ethrdep
c
      outunit=60
      open(unit=outunit,file='TXT.dat',form='formatted',status='new')
c
c writing the header
c
      if(idbflg.ne.10) write(outunit,*) int(abs(fragtrig)), Ethrdep
c
c find the region number of the region of interest
c
      nregtarg = 0
      nregaria = 0
      nregMagAir = 0
      nregSTC = 0
      nregFirstVTX = 1000000
      nregLastVTX = 0
      nregFirstBMN = 1000000
      nregLastBMN = 0
      nregFirstITR = 1000000
      nregLastITR = 0
      nregFirstMSD = 1000000
      nregLastMSD = 0
      nregFirstSCN = 1000000
      nregLastSCN = 0
      nregFirstCAL = 1000000 
      nregLastCAL = 0 
c
       ia = 1
       ib = 1
       ic = 1
       id = 1
       ie = 1
       ig = 1
       ih = 1
       ik = 1
       il = 1
       im = 1
c
      do nn = 1,MAXBMNREG
         ireg2viewBMN(nn)  = -10
         ireg2cellBMN(nn)  = -10
         ireg2layBMN(nn)   = -10
      end do
      do nn = 1,MAXITRREG
         ireg2sensITR(nn) = -10
c         ireg2plumeITR(nn) = -10
c         ireg2layITR(nn)   = -10
c         ireg2mimoITR(nn)  = -10
      end do
      do nn = 1,MAXMSDREG
         ireg2layMSD(nn)   = -10
      end do
      do nn = 1,MAXSCNREG
         ireg2stripSCN(nn) = -10
         ireg2viewSCN(nn)  = -10
      end do
      do nn = 1,MAXCALREG
         ireg2cryCAL(nn)   = -10
      end do
c
c
      do ii = 1,NREGS
         call GEOR2N ( ii, REGNAM, IERR )
         if(ierr.eq.0) then
            if(REGNAM.eq.'AIR') then
               nregaria=ii
            else if(REGNAM.eq.'MAG_AIR') then
               nregMagAir=ii
            elseif(REGNAM.eq.'STC')then     
               nregSTC=ii
            elseif(REGNAM(1:5).eq.'BMN_C') then
               read(REGNAM(7:8),*) cellBMN
               if (REGNAM(6:6).eq.'0')then
                  ireg2viewBMN(ii) = 1
                  ireg2layBMN(ii) = cellBMN/ncellBMN
                  ireg2cellBMN(ii) = cellBMN - cellBMN/ncellBMN*ncellBMN
               elseif(REGNAM(6:6).eq.'1') then
                  ireg2viewBMN(ii) = -1
                  ireg2layBMN(ii) = cellBMN/ncellBMN
                  ireg2cellBMN(ii) = cellBMN - cellBMN/ncellBMN*ncellBMN
               endif
               if(ig.eq.1) then
                  nregFirstBMN=ii
               elseif(ig.eq.(nlayBMN*ncellBMN*2)) then
                  nregLastBMN=ii
               endif
               ig = ig + 1
            elseif(REGNAM.eq.'TARGET')then
               nregtarg = ii
            elseif(REGNAM(1:4).eq.'VTXE') then
               if(ia.eq.1) then
                  nregFirstVTX=ii
               elseif(ia.eq.nlayVTX) then
                  nregLastVTX=ii
               endif
               ia = ia + 1
            elseif(REGNAM(1:4).eq.'ITRE') then
                read(REGNAM(5:6),*) sensITR
                ireg2sensITR(ii) = sensITR
c                read(REGNAM(5:5),*) plmITR
c               ireg2plumeITR(ii) = plmITR
c               read(REGNAM(6:6),*) layITR
c               ireg2layITR(ii) = layITR
c               read(REGNAM(7:7),*) mimoITR
c               ireg2mimoITR(ii) = mimoITR
               if(ib.eq.1) then
                  nregFirstITR=ii
               elseif(ib.eq.nsensITR) then
                  nregLastITR=ii
               endif
               ib = ib + 1
            elseif(REGNAM(1:4).eq.'MSDS') then
               read(REGNAM(5:5),*) layMSD
               ireg2layMSD(ii) = layMSD
               if(il.eq.1) then
                  nregFirstMSD=ii
               elseif(il.eq.(nlayMSD)) then
                  nregLastMSD=ii
               endif
               il = il + 1
            elseif(REGNAM(1:3).eq.'SCN') then
               if (REGNAM(4:4).eq.'0')then
                  ireg2viewSCN(ii) = 0
               elseif(REGNAM(4:4).eq.'1') then
                  ireg2viewSCN(ii) = 1
               endif
               read(REGNAM(5:6),*) strip
               ireg2stripSCN(ii) = strip
               if(ic.eq.1) then
                  nregFirstSCN=ii
               elseif(ic.eq.nstripSCN*2) then
                  nregLastSCN=ii
               endif
               ic = ic + 1
            elseif(REGNAM(1:3).eq.'CAL') then
               read(REGNAM(4:6),*) cry
               ireg2cryCAL(ii) = cry
               if(id.eq.1) then
                  nregFirstCAL=ii
               elseif(id.eq.ncryCAL) then
                  nregLastCAL=ii
               endif
               id = id + 1
            endif
         endif
      end do
c
c       
c
      write(*,*)'======================================'
      write(*,*)'USRINI: idbflg =  ',idbflg
      write(*,*)'USRINI: =  ',ib, "  ", nsensITR
c     
      if(((nregtarg*nregLastVTX*nregaria*nregLastSCN*nregSTC*nregLastITR
     &     *nregLastCAL*nregLastBMN*nregMagAir
     &     *nregLastMSD).eq.0).or.(nregFirstVTX.eq.1000000).or.
     &     (nregFirstCAL.eq.1000000).or.(nregFirstSCN.eq.1000000).or.
     &     (nregFirstBMN.eq.1000000).or.
     &     (nregFirstMSD.eq.1000000).or.
     &     (nregFirstITR.eq.1000000)) then
         write(*,*)'Non ho trovato tutte le regioni!!!!'
      endif
      write(*,*)'**************** Inizio Geometria *****************'
      write(*,*)'        nregaria           = ',nregaria
      write(*,*)'        nregMagAir         = ',nregMagAir
      write(*,*)'        nregSTC            = ',nregSTC
      write(*,*)'        nregFirstBMN       = ',nregFirstBMN
      write(*,*)'        nregLastBMN        = ',nregLastBMN
      write(*,*)'        nregtarg           = ',nregtarg
      write(*,*)'        nregFirstVTX       = ',nregFirstVTX
      write(*,*)'        nregLastVTX        = ',nregLastVTX
      write(*,*)'        nregFirstITR       = ',nregFirstITR
      write(*,*)'        nregLastITR        = ',nregLastITR
      write(*,*)'        nregFirstMSD       = ',nregFirstMSD
      write(*,*)'        nregLastMSD        = ',nregLastMSD
      write(*,*)'        nregFirstSCN       = ',nregFirstSCN
      write(*,*)'        nregLastSCN        = ',nregLastSCN
      write(*,*)'        nregFirstCAL       = ',nregFirstCAL
      write(*,*)'        nregLastCAL        = ',nregLastCAL
      write(*,*)'**************** Fine Geometria *******************'
      write(*,*)''
      write(*,*)' '
      write(*,*)'======================================'
c     
c     init of the root interface
c     
c     CALL myusrini();
c     
c     write(*,*)"esco da usrini"

C 100  FORMAT(I5, 1X, F6.4, 3(1X, I2))
      RETURN
*===  End of subroutine Usrini ================================*

      END

