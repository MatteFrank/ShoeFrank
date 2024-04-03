*$ CREATE MGDRAW.FOR
*COPY MGDRAW
*                                                                      *
*=== mgdraw ===========================================================*
*                                                                      *
      SUBROUTINE MGDRAW ( ICODE, MREG )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1990-2006      by        Alfredo Ferrari           *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     MaGnetic field trajectory DRAWing: actually this entry manages   *
*                                        all trajectory dumping for    *
*                                        drawing                       *
*                                                                      *
*     Created on   01 march 1990   by        Alfredo Ferrari           *
*                                              INFN - Milan            *
*     Last change  05-may-06       by        Alfredo Ferrari           *
*                                              INFN - Milan            *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(CASLIM)'
      INCLUDE '(COMPUT)'
      INCLUDE '(SOURCM)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(GENSTK)'
      INCLUDE '(MGDDCM)'
      INCLUDE '(PAPROP)'
      INCLUDE '(QUEMGD)'
      INCLUDE '(SUMCOU)'
      INCLUDE '(TRACKR)'
      INCLUDE '(LTCLCM)'
c
      include 'mgdraw.inc'
*
      DIMENSION DTQUEN ( MXTRCK, MAXQMG )
      CHARACTER*20 FILNAM
      LOGICAL LFCOPE
      SAVE LFCOPE
      DATA LFCOPE / .FALSE. /
c
      integer  ICPART, IBPART 
      character*8 NEWREGNAM,MREGNAM
*      CHARACTER*8 MLATNAM
      logical ldead
      double precision erawSTC, equenchedSTC
      double precision erawBMN, equenchedBMN
      double precision erawMSD, equenchedMSD
      double precision erawVTX, equenchedVTX
      double precision erawITR, equenchedITR
      double precision erawSCN, equenchedSCN
      double precision erawCAL, equenchedCAL
*
*----------------------------------------------------------------------*
*                                                                      *
*     Icode = 1: call from Kaskad                                      *
*     Icode = 2: call from Emfsco                                      *
*     Icode = 3: call from Kasneu                                      *
*     Icode = 4: call from Kashea                                      *
*     Icode = 5: call from Kasoph                                      *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
c      write(*,*)"sono in mgdraw"
*  +-------------------------------------------------------------------*
      if(idbflg.lt.0) then
         IF ( .NOT. LFCOPE ) THEN
            LFCOPE = .TRUE.
            IF ( KOMPUT .EQ. 2 ) THEN
               FILNAM = '/'//CFDRAW(1:8)//' DUMP A'
            ELSE
               FILNAM = CFDRAW
            END IF
            OPEN ( UNIT = IODRAW, FILE = FILNAM, STATUS = 'NEW',
     &           FORM = 'UNFORMATTED' )
         END IF
         WRITE (IODRAW) NTRACK, MTRACK, JTRACK, SNGL (ETRACK),
     &        SNGL (WTRACK)
         WRITE (IODRAW) ( SNGL (XTRACK (I)), SNGL (YTRACK (I)),
     &        SNGL (ZTRACK (I)), I = 0, NTRACK ),
     &        ( SNGL (DTRACK (I)), I = 1, MTRACK ),
     &        SNGL (CTRACK)
      endif
      if(idbflg.gt.1) then
         call GEOR2N ( mreg, MREGNAM, IERR ) 
         write(*,*)' '
         write(*,*)'------------- mgdraw: Ev =',ncase,' -------------'
         write(*,*)'jtrack = ',jtrack,' regione = ',MREGNAM
         write(*,*)' '
      endif
c
c************* PARTICLES AND HEAVY IONS WITH JTRACK=-2
      IF((JTRACK.GE.1).OR.((JTRACK.LE.-2).AND.(JTRACK.GE.-6)))THEN
         ICPART = ICHRGE(JTRACK)
         IBPART = IBARCH(JTRACK)
         AMPART = AM(JTRACK)
c*************HEAVY IONS WITH JTRACK.LT.-6
      ELSEIF(JTRACK.LT.-6)THEN
         ICPART = ICHEAV(-JTRACK)
         IBPART = IBHEAV(-JTRACK)
         AMPART = AMNHEA(-JTRACK)
      ELSE
         ICPART=int(zfrttk)
C         IBPART=0
C         ICPART = ICHRGE(JTRACK)
         IBPART = IBARCH(JTRACK)
         AMPART = AM(JTRACK)
      ENDIF
c
c**************************************************************
      CALL GEOR2N(MREG,MREGNAM,IERR)
      CALL GEON2R(MREGNAM,MREG,IERR)
*      CALL GEOL2N(MLATTC,MLATNAM,IRTLAT,IERR)
c      write(*,*) jtrack, mreg,' ',MREGNAM, MLATTC
c
      if(idbflg.gt.1) then
         write(*,*)'ptrack = ',ptrack,' ekin = ',etrack-ampart
         write(*,*)'zfrttk = ',zfrttk,' icpart = ',icpart,'  m =',ampart
         WRITE (*,*)'x,y,z = ', ( SNGL (XTRACK (I)), SNGL (YTRACK (I)),
     &        SNGL (ZTRACK (I)), I = 0, NTRACK )
         WRITE (*,*)'cx,cy,cz = ',SNGL(CXTRCK),SNGL(CYTRCK),SNGL(CZTRCK)
         WRITE (*,*)'px,py,pz = ',SNGL(ptrack*CXTRCK),
     &        SNGL(ptrack*CYTRCK),SNGL(ptrack*CZTRCK)
         write(*,*)'time = ',atrack,' step = ',ctrack,' path = ',cmtrck
      endif
c
c Find which particle of the part common we are following:
c let's assume it's the last one...
c
*      if ((etrack-ampart).le.ZERZER) then
*         idead(idcurr) = 17
*         if (idbflg.gt.1) then
*            write(*,*) ' particle at end of range!  =>',
*     &           idcurr,nump,jpa(idcurr)
*         endif
*      endif

      call UpdateCurrPart(mreg,icpart,ibpart,ampart,icode,
     &     xtrack(0),ytrack(0),ztrack(0),JRUN)
c      call UpdateCurrPart(mreg,icpart,ibpart,ampart,icode)
c
      if (idbflg.lt.0) then
         IF ( LQEMGD )THEN
            RULLL  = ZERZER
            CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
               WRITE (IODRAW) ( ( SNGL (DTQUEN (I,JBK))
     &              , I = 1, MTRACK ),
     &              JBK = 1, NQEMGD )
         ENDIF
      endif
c  *****************************************************************   
c  inside the start counter
c
      if( mreg.eq.nregSTC )then
         erawSTC = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawSTC = erawSTC + dtrack(ii)
            end do
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nello start counter
c     
               do ii = 1,mtrack
                  equenchedSTC = equenchedSTC + dtquen(ii,3)
               end do
               equenchedSTC = equenchedSTC*abs_STC
            endif
         endif
         if(erawSTC.gt.0) then
            call score_STC(mreg,erawSTC,equenchedSTC,
     &      xtrack(0),ytrack(0),ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &      ztrack(ntrack))
         endif
      endif

c  *****************************************************************   
c  inside the beam monitor
c
      if( mreg.ge.nregFirstBMN.and.mreg.le.nregLastBMN )then
         erawBMN = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawBMN = erawBMN + dtrack(ii)
            end do
            
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nel beam monitor
c     
               do ii = 1,mtrack
                  equenchedBMN = equenchedBMN + dtquen(ii,3)
               end do
               equenchedBMN = equenchedBMN*abs_BMN
            endif
         endif
         if(erawBMN.gt.0) then
            call score_BMN(mreg,erawBMN,equenchedBMN,
     &      xtrack(0),ytrack(0),ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &      ztrack(ntrack))
         endif
      endif

c     *************************************************************
c     inside the vertex
c
      if( mreg.le.nregLastVTX .and. mreg.ge.nregFirstVTX )then
         erawVTX = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawVTX = erawVTX + dtrack(ii)
            end do
            
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nel vertice
c     
               do ii = 1,mtrack
                  equenchedVTX = equenchedVTX + dtquen(ii,3)
               end do
               equenchedVTX = equenchedVTX*abs_VTX
            endif
         endif
         if(erawVTX.gt.0) then
            call score_VTX(mreg,erawVTX,equenchedVTX,
     &      xtrack(0),ytrack(0),ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &      ztrack(ntrack))
         endif
      endif

c     *************************************************************
c     inside the inner tracker
c
      if( mreg.le.nregLastITR .and. mreg.ge.nregFirstITR )then
         erawITR = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawITR = erawITR + dtrack(ii)
            end do
            
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nell'inner tracker
c     
               do ii = 1,mtrack
                  equenchedITR = equenchedITR + dtquen(ii,3)
               end do
               equenchedITR = equenchedITR*abs_ITR
            endif
         endif
         if(erawITR.gt.0) then
            call score_ITR(mreg,erawITR,equenchedITR,
     &      xtrack(0),ytrack(0),ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &      ztrack(ntrack))
         endif
      endif

c  *****************************************************************   
c  inside the second drift chamber
c
      if( mreg.ge.nregFirstMSD.and.mreg.le.nregLastMSD )then
         erawMSD = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawMSD = erawMSD + dtrack(ii)
            end do
            
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nella camera a drift
c     
               do ii = 1,mtrack
                  equenchedMSD = equenchedMSD + dtquen(ii,3)
               end do
               equenchedMSD = equenchedMSD*abs_MSD
            endif
         endif
         if(erawMSD.gt.0) then
            call score_MSD(mreg,erawMSD,equenchedMSD,
     &      xtrack(0),ytrack(0),ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &      ztrack(ntrack))
         endif
      endif
c      
c     **********************************************************
c     Inside the scintillator
c
      if( mreg.le.nregLastSCN .and. mreg.ge.nregFirstSCN ) then
c         equenchedSCN= 0.
         erawSCN = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawSCN = erawSCN + dtrack(ii)
            end do
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nello scintillatore
c     
               do ii = 1,mtrack
                  equenchedSCN = equenchedSCN + dtquen(ii,3)
               end do
               equenchedSCN = equenchedSCN*abs_plastic
            endif

         END IF
         if(erawSCN.gt.0) then
            call score_SCN(mreg,erawSCN,equenchedSCN,xtrack(0),
     &           ytrack(0),
     &           ztrack(0),xtrack(ntrack),ytrack(ntrack),ztrack(ntrack))
         endif
      endif
      
c     *************************************************************
c     inside the calorimeter
c      
      if( mreg.le.nregLastCAL .and. mreg.ge.nregFirstCAL )then
         erawCAL = 0.
         IF ( MTRACK .GT. 0 )THEN
            do ii = 1,MTRACK
               erawCAL = erawCAL + dtrack(ii)
            end do
            IF ( LQEMGD )THEN
               RULLL  = ZERZER
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
c     
c     DTQUEN(MTRACK,1) e' il rilascio di energia quenchato nel calorimetro
c     
               do ii = 1,mtrack
                  equenchedCAL = equenchedCAL + dtquen(ii,3)
               end do
               equenchedCAL = equenchedCAL*abs_CAL
            endif
         endif
         if(erawCAL.gt.0) then
            call score_CAL(mreg,erawCAL,equenchedCAL,xtrack(0),
     &           ytrack(0), ztrack(0),xtrack(ntrack),ytrack(ntrack),
     &           ztrack(ntrack))
         endif
      endif

*     +----------------------------------------------------------
c     
c      write(*,*)'Esco da Mgdraw'
      RETURN
*     
*================================================================*
*                                                                      *
*     Boundary-(X)crossing DRAWing:                                    *
*                                                                      *
*     Icode = 1x: call from Kaskad                                     *
*             19: boundary crossing                                    *
*     Icode = 2x: call from Emfsco                                     *
*             29: boundary crossing                                    *
*     Icode = 3x: call from Kasneu                                     *
*             39: boundary crossing                                    *
*     Icode = 4x: call from Kashea                                     *
*             49: boundary crossing                                    *
*     Icode = 5x: call from Kasoph                                     *
*             59: boundary crossing                                    *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY BXDRAW ( ICODE, MREG, NEWREG, XSCO, YSCO, ZSCO )
c     scoring at the boundaries... select the region
c     
c         write(*,*)'Sono in Bxdraw'
c************* PARTICLES AND HEAVY IONS WITH JTRACK=-2
       IF((JTRACK.GE.1).OR.((JTRACK.LE.-2).AND.(JTRACK.GE.-6)))THEN
         ICPART = ICHRGE(JTRACK)
         IBPART = IBARCH(JTRACK)
         AMPART = AM(JTRACK)
c************* HEAVY IONS WITH JTRACK.LT.-6
       ELSEIF(JTRACK.LT.-6)THEN
         ICPART = ICHEAV(-JTRACK)
         IBPART = IBHEAV(-JTRACK)
         AMPART = AMNHEA(-JTRACK)
       ELSE
         ICPART= ICHRGE(JTRACK)
         ICPART = ICHRGE(JTRACK)
         IBPART = IBARCH(JTRACK)
         AMPART = AM(JTRACK)
       ENDIF
c
c debug printing
c
      if(idbflg.gt.0) then
         call GEOR2N ( newreg, NEWREGNAM, IERR ) 
         call GEOR2N ( mreg, MREGNAM, IERR ) 
*         CALL GEOL2N ( MLATTC,MLATNAM,IRTLAT,IERR )
         write(*,*)' '
         write(*,*)'------------- bxdraw: Ev =',ncase,' -------------'
         write(*,*)'jtrack = ',jtrack,' icode = ',icode
*         write(*,*)'da -> ',mregnam,'a -> = ',newregnam, ' lat_idx = ',
*     &        MLATTC
         write(*,*)'da -> ',mregnam,'a -> = ',newregnam

         write(*,*)'zfrttk = ',zfrttk,' icpart = ',icpart,
     &        ' m =',AMPART
         write(*,*)'ptrack = ',ptrack,' ekin = ',etrack-AMPART
         write(*,*)'x,y,z = ',XSCO,YSCO,ZSCO
         WRITE (*,*)'cx,cy,cz = ',SNGL(CXTRCK), SNGL(CYTRCK)
     &           ,SNGL(CZTRCK)
         write(*,*)'number of track segments: ntrack = ',NTRACK
         do ii =1,ntrack
            write(*,*)'track seg # ',ii,' track seg lenght = '
     &           ,ttrack(ii)
         end do
         write(*,*)'number of en deposition: mtrack = ',MTRACK
         do ii =1,mtrack
            write(*,*)'track dep # ',ii,' track dep val = ',
     &           dtrack(ii)
         end do
         write(*,*)' '
      endif
cf
cf      
cf      CALL GEOL2N ( MLATTC,MLATNAM,IRTLAT,IERR )      
cf
c
c scoring the crossing info.
c
      if(JTRACK.ne.-1) then
         call score_cross(
     &        ICPART,IBPART,AMPART,newreg,mreg,xsco,ysco,zsco)
      endif
c      write(*,*)'Esco da Bxdraw'
c     
      RETURN
*     
* ======================================================================*
*                                                                      *
*     Event End DRAWing:                                               *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY EEDRAW ( ICODE )
      if(idbflg.gt.2) then
         call dump_common()
      endif
      RETURN
*
*======================================================================*
*                                                                      *
*     ENergy deposition DRAWing:                                       *
*                                                                      *
*     Icode = 1x: call from Kaskad                                     *
*             10: elastic interaction recoil                           *
*             11: inelastic interaction recoil                         *
*             12: stopping particle                                    *
*             13: pseudo-neutron deposition                            *
*             14: escape                                               *
*             15: time kill                                            *
*     Icode = 2x: call from Emfsco                                     *
*             20: local energy deposition (i.e. photoelectric)         *
*             21: below threshold, iarg=1                              *
*             22: below threshold, iarg=2                              *
*             23: escape                                               *
*             24: time kill                                            *
*     Icode = 3x: call from Kasneu                                     *
*             30: target recoil                                        *
*             31: below threshold                                      *
*             32: escape                                               *
*             33: time kill                                            *
*     Icode = 4x: call from Kashea                                     *
*             40: escape                                               *
*             41: time kill                                            *
*             42: delta ray stack overflow                             *
*     Icode = 5x: call from Kasoph                                     *
*             50: optical photon absorption                            *
*             51: escape                                               *
*             52: time kill                                            *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY ENDRAW ( ICODE, MREG, RULL, XSCO, YSCO, ZSCO )
*  +-------------------------------------------------------------------*
c
c debug
c
      if (idbflg.lt.0) then
         IF ( .NOT. LFCOPE ) THEN
            LFCOPE = .TRUE.
            IF ( KOMPUT .EQ. 2 ) THEN
               FILNAM = '/'//CFDRAW(1:8)//' DUMP A'
            ELSE
               FILNAM = CFDRAW
            END IF
            OPEN ( UNIT = IODRAW, FILE = FILNAM, STATUS = 'NEW',
     &           FORM = 'UNFORMATTED' )
            WRITE (IODRAW)  0, ICODE, JTRACK, SNGL (ETRACK),
     &           SNGL (WTRACK)
            WRITE (IODRAW)  SNGL (XSCO), SNGL (YSCO), SNGL (ZSCO),
     &           SNGL (RULL)
         endif
      endif
      if(idbflg.gt.1) then
         write(*,*)'------------- endraw: Ev =',ncase,' -------------'
         write(*,*)'jtrack = ',jtrack,' mreg = ',mreg
         write(*,*)'rull = ',rull,' icode = ',icode,' idcurr = ',idcurr
         write(*,*)'idead(idcurr) = ',idead(idcurr)
         write(*,*)'x,y,x = ',sngl(XSCO), sngl(YSCO), sngl(ZSCO) 
         WRITE (*,*)'cx,cy,cz = ',SNGL(CXTRCK),SNGL(CYTRCK),SNGL(CZTRCK)
      endif
c
Cgb
Cgb   to preserve idead flag correctly
Cgb
      idead (idcurr) = icode
Cgb
      if(idbflg.gt.1) then
         write(*,*)'ENDRAW: Now idead(idcurr) = ',idead(idcurr)
      endif
Cgb
      if(jtrack.lt.62) then
c************* PARTICLES AND HEAVY IONS WITH JTRACK=-2
         IF((JTRACK.GE.1).OR.((JTRACK.LE.-2).AND.(JTRACK.GE.-6)))THEN
            ICPART = ICHRGE(JTRACK)
            IBPART = IBARCH(JTRACK)
            AMPART = AM(JTRACK)
c*************HEAVY IONS WITH JTRACK.LT.-6
         ELSEIF(JTRACK.LT.-6)THEN
            ICPART = ICHEAV(-JTRACK)
            IBPART = IBHEAV(-JTRACK)
            AMPART = AMNHEA(-JTRACK)
         ELSE
            ICPART= ICHRGE(JTRACK)
            IBPART = IBARCH(JTRACK)
            AMPART = AM(JTRACK)
         ENDIF
c
c     if not delta ray below threshold than update the current particle
c
         if( (JTRACK.ne.3).or.(ICODE.ne.22)) then
c            write(*,*)'***********************************************'
c            write(*,*)' NB UpdateCurrentParticle Called from ENDRAW!!!'
c            write(*,*)'***********************************************'
            call UpdateCurrPart(mreg,icpart,ibpart,ampart,icode,
     &           xsco,ysco,zsco,JEND)
            if (jtrack.eq.7) then
               px(idcurr) = pxf(idcurr)
               py(idcurr) = pyf(idcurr)
               pz(idcurr) = pzf(idcurr)
            endif
         endif
      endif
      if (idbflg.lt.0) then
         IF ( LQEMGD) THEN
            RULLL = RULL
            CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
            WRITE (IODRAW) ( ( SNGL (DTQUEN (I,JBK))
     &           , I = 1, MTRACK ),
     &           JBK = 1, NQEMGD )
         ENDIF
      endif
      if (rull.ne.0) then         
c     
c     inside the start counter
c
         if( mreg.eq.nregSTC ) then
            erawSTC = rull
            equenchedSTC=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedSTC = dtquen(1,1)*abs_STC
            END IF
            call score_STC(mreg,erawSTC,equenchedSTC,xsco,
     &           ysco,zsco,xsco,ysco,zsco)
         endif
c     
c     inside the beam monitor
c
         if( mreg.ge.nregFirstBMN.and.mreg.le.nregLastBMN ) then
            erawBMN = rull
            equenchedBMN=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedBMN = dtquen(1,1)*abs_BMN
            END IF
            call score_BMN(mreg,erawBMN,equenchedBMN,xsco,
     &           ysco,zsco,xsco,ysco,zsco)
         endif
c     
c     inside the vertex
c
         if( mreg.le.nregLastVTX .and. mreg.ge.nregFirstVTX )
     $        then
            erawVTX = rull
            equenchedVTX=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedVTX = dtquen(1,1)*abs_VTX
            END IF
            call score_VTX(mreg,erawVTX,equenchedVTX,xsco,
     &           ysco,zsco,xsco,ysco,zsco)
         endif         
c     
c     inside the inner tracker
c
         if( mreg.le.nregLastITR .and. mreg.ge.nregFirstITR )
     $        then
            erawITR = rull
            equenchedITR=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedITR = dtquen(1,1)*abs_ITR
            END IF
            call score_ITR(mreg,erawITR,equenchedITR,xsco,
     &           ysco,zsco,xsco,ysco,zsco)
         endif
c     
c     inside the second drift chamber
c
         if( mreg.ge.nregFirstMSD .and. mreg.le.nregLastMSD ) then
            erawMSD = rull
            equenchedMSD=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedMSD = dtquen(1,1)*abs_MSD
            END IF
            call score_MSD(mreg,erawMSD,equenchedMSD,xsco,
     &           ysco,zsco,xsco,ysco,zsco)
         endif
c     
c     inside the scintillator
c
         if ( mreg.le.nregLastSCN .and. mreg.ge.nregFirstSCN ) then
            erawSCN = rull
            equenchedSCN=0
            IF ( LQEMGD) THEN
               RULLL = RULL
               CALL QUENMG ( ICODE, MREG, RULL, DTQUEN )
               equenchedSCN = dtquen(1,1)*abs_SCN
            END IF
            call score_SCN(mreg,erawSCN,equenchedSCN,xsco,ysco,
     &           zsco,xsco,ysco,zsco)
         endif
c
c     inside the calorimeter
c
         if( mreg.le.nregLastCAL .and. mreg.ge.nregFirstCAL )then
            erawCAL = rull
            equenchedCAL = 0
            IF ( LQEMGD )THEN
               RULLL  = RULL
               CALL QUENMG ( ICODE, MREG, RULLL, DTQUEN )
               equenchedCAL = dtquen(1,1)*abs_CAL
            END IF
            call score_CAL(mreg,erawCAL,equenchedCAL,xsco,ysco,
     &           zsco,xsco,ysco,zsco)
         endif
      endif
c
c     end debug
c     
      RETURN
*
*======================================================================*
*                                                                      *
*     SOurce particle DRAWing:                                         *
*                                                                      *
*======================================================================*
*
      ENTRY SODRAW
c
      if (idbflg.lt.0) then
         IF ( .NOT. LFCOPE ) THEN
            LFCOPE = .TRUE.
            IF ( KOMPUT .EQ. 2 ) THEN
               FILNAM = '/'//CFDRAW(1:8)//' DUMP A'
            ELSE
               FILNAM = CFDRAW
            END IF
            OPEN ( UNIT = IODRAW, FILE = FILNAM, STATUS = 'NEW',
     &           FORM ='UNFORMATTED' )
         END IF
         WRITE (IODRAW) -NCASE, NPFLKA, NSTMAX, SNGL (TKESUM),
     &        SNGL (WEIPRI)
      endif
      if(idbflg.gt.1) then
         write(*,*)'------------- sodraw: Ev =',ncase,' -------------'
      endif
*  +-------------------------------------------------------------------*
*  |  (Radioactive) isotope: it works only for 1 source particle on
*  |  the stack for the time being
      IF ( ILOFLK (NPFLKA) .GE. 100000 .AND. LRADDC (NPFLKA) ) THEN
         IARES  = MOD ( ILOFLK (NPFLKA), 100000  )  / 100
         IZRES  = MOD ( ILOFLK (NPFLKA), 10000000 ) / 100000
         IISRES = ILOFLK (NPFLKA) / 10000000
         IONID  = ILOFLK (NPFLKA)
         if (idbflg.lt.0) then
            WRITE (IODRAW) ( IONID,SNGL(-TKEFLK(I)),
     &           SNGL (WTFLK(I)), SNGL (XFLK (I)),
     &           SNGL (YFLK (I)), SNGL (ZFLK (I)),
     &           SNGL (TXFLK(I)), SNGL (TYFLK(I)),
     &           SNGL (TZFLK(I)), I = 1, NPFLKA )
         endif
c     
*  |
*  +-------------------------------------------------------------------*
*  |  Patch for heavy ions: it works only for 1 source particle on
*  |  the stack for the time being
      ELSE IF ( ABS (ILOFLK (NPFLKA)) .GE. 10000 ) THEN
         IONID = ILOFLK (NPFLKA)
         CALL DCDION ( IONID )
         if (idbflg.lt.0) then
            WRITE (IODRAW) ( IONID,SNGL(TKEFLK(I)+AMNHEA(-IONID)),
     &           SNGL (WTFLK(I)), SNGL (XFLK (I)),
     &           SNGL (YFLK (I)), SNGL (ZFLK (I)),
     &           SNGL (TXFLK(I)), SNGL (TYFLK(I)),
     &           SNGL (TZFLK(I)), I = 1, NPFLKA )
         endif
*  |
*  +-------------------------------------------------------------------*
*  |  Patch for heavy ions: ???
      ELSE IF ( ILOFLK (NPFLKA) .LT. -6 ) THEN
         if (idbflg.lt.0) then
            WRITE (IODRAW)
     &           ( IONID,SNGL(TKEFLK(I)+AMNHEA(-ILOFLK(NPFLKA))),
     &           SNGL (WTFLK(I)), SNGL (XFLK (I)),
     &           SNGL (YFLK (I)), SNGL (ZFLK (I)),
     &           SNGL (TXFLK(I)), SNGL (TYFLK(I)),
     &           SNGL (TZFLK(I)), I = 1, NPFLKA )
         endif
*  |
*  +-------------------------------------------------------------------*
*  |
      ELSE
         IONID = ILOFLK (NPFLKA)
         if (idbflg.lt.0) then
            WRITE (IODRAW) ( ILOFLK(I), SNGL (TKEFLK(I)+AM(ILOFLK(I))),
     &           SNGL (WTFLK(I)), SNGL (XFLK (I)),
     &           SNGL (YFLK (I)), SNGL (ZFLK (I)),
     &           SNGL (TXFLK(I)), SNGL (TYFLK(I)),
     &           SNGL (TZFLK(I)), I = 1, NPFLKA )
         endif
      END IF
c
c   first call to initialing the kinematic tracking 
c 
      call UpdateCurrPart(mreg,icpart,ibpart,ampart,icode,
     &     0.d+00,0.d+00,0.d+00,JSTART)
c
      if(idbflg.gt.0) then
         write(*,*)'reg = ',numreg(nump),' tprod = ',tempo(nump),
     &        ' jpa = ',jpa(nump)
         write(*,*)'vert = ',vxi(nump),vyi(nump),vzi(nump),
     &        ' p = ',px(nump),py(nump),pz(nump)
      endif
*  |
*  +-------------------------------------------------------------------*
      RETURN
*
*======================================================================*
*                                                                      *
*     USer dependent DRAWing:                                          *
*                                                                      *
*     Icode =  99: call from Doiosp, ion splitting secondaries         *
*     Icode = 1xy: call from Kaskad                                    *
*             100: elastic   interaction secondaries                   *
*             101: inelastic interaction secondaries                   *
*             102: particle decay  secondaries                         *
*             103: delta ray  generation secondaries                   *
*             104: pair production secondaries                         *
*             105: bremsstrahlung  secondaries                         *
*             106: de-excitation in flight secondaries                 *
*             110: radioactive decay products                          *
*     Icode = 2xy: call from Emfsco                                    *
*             208: bremsstrahlung secondaries                          *
*             210: Moller secondaries                                  *
*             212: Bhabha secondaries                                  *
*             214: in-flight annihilation secondaries                  *
*             215: annihilation at rest   secondaries                  *
*             217: pair production        secondaries                  *
*             219: Compton scattering     secondaries                  *
*             221: photoelectric          secondaries                  *
*             225: Rayleigh scattering    secondaries                  *
*             230: inverse Compton        secondaries                  *
*             237: mu pair production     secondaries                  *
*     Icode = 30x: call from Kasneu                                    *
*             300: interaction secondaries                             *
*     Icode = 40x: call from Kashea                                    *
*             400: delta ray  generation secondaries                   *
*                                                                      *
*  For all interactions secondaries are put on GENSTK common (kp=1,np) *
*  but for KASHEA delta ray generation where only the secondary elec-  *
*  tron is present and stacked on FLKSTK common for kp=npflka          *
*                                                                      *
*======================================================================*
*
      ENTRY USDRAW ( ICODE, MREG, XSCO, YSCO, ZSCO )
Cgb
Cgb     To mark correctly interaction code of current particle
Cgb
      intcode(idcurr) = icode
c     
c debug printing
c
      if (idblfg.lt.0) then
         IF ( .NOT. LFCOPE ) THEN
            LFCOPE = .TRUE.
            IF ( KOMPUT .EQ. 2 ) THEN
               FILNAM = '/'//CFDRAW(1:8)//' DUMP A'
            ELSE
               FILNAM = CFDRAW
            END IF
            OPEN ( UNIT = IODRAW, FILE = FILNAM, STATUS = 'NEW',
     &           FORM = 'UNFORMATTED' )
         END IF
      endif
      if(idbflg.gt.0) then
         call GEOR2N ( mreg, MREGNAM, IERR ) 
         write(*,*)' '
         write(*,*)'------------- usdraw: Ev =',ncase,' -------------'
         write(*,*)' Fluka index= ',ISPUSR(MKBMX2),' idcurr= ',idcurr,
     &        'jtrack = ',jtrack,' icode = ',icode
         write(*,*)'regione = ',mregnam, 'x,y,z = ',XSCO,YSCO,ZSCO
         if(idbflg.gt.2) then
            write(*,*)'zfrttk = ',zfrttk,' icpart = ',icpart,',m = ',
     &           AMPART
            write(*,*)'ptrack = ',ptrack,' ekin = ',etrack-AMPART,
     &           ' t = ',atrack
            WRITE (*,*)'cx,cy,cz = ',SNGL(CXTRCK), SNGL(CYTRCK)
     &           ,SNGL(CZTRCK)
         endif
         write(*,*)'  USDRAW secondary writing : NP=',NP
         do  ip = 1, NP
            WRITE(*, *) ip,' jpa sec= ',KPART(ip),' Ek sec= ',
     &           TKI(ip),' p sec= ',plr(ip)
         end do
         write(*,*)'  USDRAW heavy secondary writing : NPHEAV=',NPHEAV
         do  ip = 1, NPHEAV
            WRITE(*, *) ip,' jpa sec= ',KHEAVY(ip),' Ek sec= ',
     &           tkheav(ip),' p sec= ',pheavy(ip)
         end do
         write(*,*)' '
      endif
      IF (icode.eq.101) THEN   !! inelastic interaction
         IF (MREG.eq.nregSTC) THEN
            stcfrag = LTRACK
            if (idbflg.gt.2) then
               write(*,*) ' mgdraw: STC ',stcfrag
               do  ip = 1, NP
                  if(kpart(ip).lt.-1) then
                     CALL USRDCI(kpart(ip),IONA,IONZ,IONM)
                     write(*,*)'STC: A= ',iona,' Z= ',ionz
                  endif
               end do
               do  ip = 1, NPHEAV
                  if(kpart(ip).lt.-1) then
                     write(*,*)'STC: A= ',ibheav(ip),' Z= ',icheav(ip)
                  endif
               end do
            endif
         ENDIF
         IF (MREG.ge.4 .AND. MREG.le.45) THEN
            bmnfrag = LTRACK
            if (idbflg.gt.2) then
               write(*,*) ' mgdraw: BMN ',bmnfrag
               do  ip = 1, NP
                  if(kpart(ip).lt.-1) then
                     CALL USRDCI(kpart(ip),IONA,IONZ,IONM)
                     write(*,*)'BMN: A= ',iona,' Z= ',ionz
                  endif
               end do
               do  ip = 1, NPHEAV
                  if(kpart(ip).lt.-1) then
                     write(*,*)'BMN: A= ',ibheav(ip),' Z= ',icheav(ip)
                  endif
               end do
            endif
         ENDIF
         IF (MREG.eq.nregtarg) THEN
            tgtfrag = LTRACK
            if (idbflg.gt.2) then
               write(*,*) ' mgdraw: TGT ',tgtfrag
               do  ip = 1, NP
                  if(kpart(ip).lt.-1) then
                     CALL USRDCI(kpart(ip),IONA,IONZ,IONM)
                     write(*,*)'TGT: A= ',iona,' Z= ',ionz
                  endif
               end do
               do  ip = 1, NPHEAV
                  if(kpart(ip).lt.-1) then
                     write(*,*)'TGT: A= ',ibheav(ip),' Z= ',icheav(ip)
                  endif
               end do
            endif
         ENDIF
         IF ( (MREG.eq.nregaria .or. MREG.eq.nregMagAir) ) THEN
            airfrag = LTRACK
            if (idbflg.gt.2) then
               write(*,*) ' mgdraw: AIR ',airfrag
               do  ip = 1, NP
                  if(kpart(ip).lt.-1) then
                     CALL USRDCI(kpart(ip),IONA,IONZ,IONM)
                     write(*,*)'AIR: A= ',iona,' Z= ',ionz
                  endif
               end do
               do  ip = 1, NPHEAV
                  if(kpart(ip).lt.-1) then
                     write(*,*)'AIR: A= ',ibheav(ip),' Z= ',icheav(ip)
                  endif
               end do
            endif
         ENDIF
         IF ( MREG.ne.nregaria .AND. MREG.ne.nregMagAir
     &        .AND. MREG.ne.nregSTC
     &        .AND. MREG.ne.nregtarg
     &        .AND. MREG.lt.nregFirstSCN ) THEN
            elsfrag = LTRACK
            if (idbflg.gt.2) then
               write(*,*) ' mgdraw: Else ', elsfrag
               do  ip = 1, NP
                  if(kpart(ip).lt.-1) then
                     CALL USRDCI(kpart(ip),IONA,IONZ,IONM)
                     write(*,*)'AIR: A= ',iona,' Z= ',ionz
                  endif
               end do
               do  ip = 1, NPHEAV
                  if(kpart(ip).lt.-1) then
                     write(*,*)'AIR: A= ',ibheav(ip),' Z= ',icheav(ip)
                  endif
               end do
            endif
         ENDIF
      ENDIF
*
*   Do nothing for elastic scattering
c      if ( icode .eq.  100) return
*  +-------------------------------------------------------------------*
*  |  Delta rays, Compton and Moller-Bhabba scattering:
      IF ( ICODE.EQ.103 .OR. ICODE.EQ.210 .OR. ICODE.EQ. 212 ) THEN
         ldead = .false.
*  |
*  +-------------------------------------------------------------------*
*  |  Bremsstrahlung:
      ELSE IF ( ICODE .EQ. 208 .OR. ICODE .EQ. 105 ) THEN
         NPA    = NP
         ldead = .false.
*  +-------------------------------------------------------------------*
*  |  Compton scattering:
      ELSE IF ( ICODE .EQ. 219 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  +-------------------------------------------------------------------*
*  |  PhotoElectric:
      ELSE IF ( ICODE .EQ. 221 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  |
*  +-------------------------------------------------------------------*
*  |  In flight or at rest annihilation:
      ELSE IF ( ICODE .EQ. 214 .OR. ICODE .EQ. 215 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  |
*  +-------------------------------------------------------------------*
*  |  Pair production:
      ELSE IF ( ICODE.EQ.217 .OR. ICODE.EQ.104 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  |
*  +-------------------------------------------------------------------*
*  |  Low energy neutron secondaries:
      ELSE IF ( ICODE.EQ.300 ) THEN
         idead (idcurr) = icode
         ldead =.true.
*  |
*  +-------------------------------------------------------------------*
*  |  Decays
      ELSE IF ( ICODE.EQ.102 .OR. icode.eq.110) THEN
         idead (idcurr) = icode
         ldead = .true.
*  |
*  +-------------------------------------------------------------------*
*  |  Inelastic interactions:
      ELSE IF ( ICODE.EQ.101 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  +-------------------------------------------------------------------*
*  |  In-flight De-excitation:
      ELSE IF ( ICODE .EQ.106 ) THEN
         idead (idcurr) = icode
         ldead = .true.
*  |
*  +-------------------------------------------------------------------*
*  |  Everything else:
      ELSE
         ldead = .false.
      END IF
*  |
*  +-------------------------------------------------------------------*
      IF(ICODE.LT.106) THEN
         numint = numint +1
         if (numint.GT.maxint)
     &        write(*,*) 'MGDRAW: numint>maxint',ncase,numint,maxint
         xint(numint) = xsco
         yint(numint) = ysco
         zint(numint) = zsco
         intpa(numint) = idcurr
C     idead(numint) = idead(idcurr)
         if(idbflg.gt.1) then
            write(*,*)'USDRAW: Now numint, idead(numint) = ',
     &           numint,intpa(numint)
         endif
         intcode(numint) = intcode(idcurr)
Cgb
Cgb      idead (numint) = intcode(idcurr)
Cgb
      ENDIF   
      IF (icode.eq.219 .or. icode.eq.101 .or. icode.eq.102 .or. 
     &     icode.eq.110 .or. icode.eq.214 .or. icode.eq.215 .or.
     &     icode.eq.217 .or. icode.eq.221 ) then
Cgb
         call UpdateCurrPart(mreg,0,0,zerzer,icode,
     &        xsco,ysco,zsco,JEND)
Cgb
      endif
c
      if ( ldead ) then
         idead (idcurr) = icode
c         idead (idcurr) = intcode(idcurr)
         if(idbflg.gt.1) then
            write(*,*)'USDRAW: ldead Now idcurr, idead(idcurr) = ',
     &           idcurr,idead(idcurr),icode
         endif
         vxf (idcurr) = sngl(xsco)
         vyf (idcurr) = sngl(ysco)
         vzf (idcurr) = sngl(zsco)
      end if
c
      RETURN
*=== End of subrutine Mgdraw ==========================================*
      END

