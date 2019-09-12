*$ CREATE MAGFLD.FOR
*COPY MAGFLD
*
*===magfld=============================================================*
*
      SUBROUTINE MAGFLD ( X, Y, Z, BTX, BTY, BTZ, B, NREG, IDISC )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1988-2010      by Alberto Fasso` & Alfredo Ferrari *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     Created  in     1988         by    Alberto Fasso`                *
*                                                                      *
*                                                                      *
*     Last change on 06-Nov-10     by    Alfredo Ferrari               *
*                                                                      *
*     Input variables:                                                 *
*            x,y,z = current position                                  *
*            nreg  = current region                                    *
*     Output variables:                                                *
*            btx,bty,btz = cosines of the magn. field vector           *
*            B = magnetic field intensity (Tesla)                      *
*            idisc = set to 1 if the particle has to be discarded      *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(CMEMFL)'
      INCLUDE '(CSMCRY)'
*     
      INTEGER NX, NY, NZ, NROW
      INTEGER XDIM, YDIM, ZDIM
      DOUBLE PRECISION G2T

      PARAMETER (G2T=1.D-04)
      PARAMETER (XDIM=50) 
      PARAMETER (YDIM=50) 
      PARAMETER (ZDIM=200)

      DOUBLE PRECISION XLAT, YLAT, ZLAT
      DOUBLE PRECISION XDIFF, YDIFF, ZDIFF
      DOUBLE PRECISION BXLAT, BYLAT, BZLAT
      
      DIMENSION XLAT(XDIM), YLAT(YDIM), ZLAT(ZDIM)
      DIMENSION BXLAT(XDIM,YDIM,ZDIM), BYLAT(XDIM,YDIM,ZDIM),
     &     BZLAT(XDIM,YDIM,ZDIM)
      
      INTEGER I0, J0, K0
      INTEGER I1, J1, K1
      INTEGER ICOUNT
      
      DOUBLE PRECISION  B00X,B01X,B10X,B11X,B0X,B1X,BXFIN
      DOUBLE PRECISION  B00Y,B01Y,B10Y,B11Y,B0Y,B1Y,BYFIN
      DOUBLE PRECISION  B00Z,B01Z,B10Z,B11Z,B0Z,B1Z,BZFIN
      
      LOGICAL LFIRST 
      DATA LFIRST /.TRUE./ 
      SAVE LFIRST
      
      IDISC = 0
         
      IF (LFIRST) THEN 
         
         CALL OAUXFI('DoubleDipole.table',22,'OLD',IERR)
*         CALL OAUXFI(mapname,22,'OLD',IERR)
         
         READ (22,*) NROW, NX, NY, NZ

         IF (XDIM.LT.NX .OR. YDIM.LT.NY .OR. ZDIM.LT.NZ) THEN
            WRITE(*,*)'magfld.f error: problem in arrays dimension'
            CALL FLABRT('MAGFLD','Error in arrays dimension')
         ENDIF
         
         DO I=1,NX
            DO J=1,NY
               DO K=1,NZ
                  ICOUNT = ICOUNT+1
*     riempio i vettori XB(I), YB(J), ZB(K) che individuano un
*     punto nel reticolo spaziale e i vettori BXLAT(I,J,K),
*     BYLAT(I,J,K), BZLAT(I,J,K) che sono le componenti di B
*     corrispondenti a quel punto
                  READ (22,*) XLAT(I), YLAT(J), ZLAT(K),
     &                 BXLAT(I,J,K), BYLAT(I,J,K), BZLAT(I,J,K)
                  XLAT(I) = XLAT(I)+MagCenterX
                  YLAT(J) = YLAT(J)+MagCenterY
                  ZLAT(K) = ZLAT(K)+MagCenterZ
                  BXLAT(I,J,K) = BXLAT(I,J,K)*G2T
                  BYLAT(I,J,K) = BYLAT(I,J,K)*G2T
                  BZLAT(I,J,K) = BZLAT(I,J,K)*G2T
               ENDDO
            END DO
         END DO  
         
*     controllo che abbia letto tutte le righe
         IF (ICOUNT.NE.NROW) THEN
            WRITE(*,*)'Mag field lattice: reading problem'
            CALL FLABRT('MAGFLD','Error in map readout')
         ENDIF
         
         LFIRST = .FALSE.
      ENDIF
      
      IF ( X.LE.XLAT(1) .OR. X.GT.XLAT(NX) .OR.
     &     Y.LE.YLAT(1) .OR. Y.GT.YLAT(NY) .OR.
     &     Z.LE.ZLAT(1) .OR. Z.GT.ZLAT(NZ) ) THEN
         B = ZERZER
         BTX = ZERZER
         BTY = ZERZER
         BTZ = ZERZER
         
      ELSE
         
*     trovo le coordinate XB dei punti del lattice che stanno 
*     appena prima e appena dopo al mio X e mi salvo i loro 
*     indici (I0 e I1 rispettivamente), poi faccio la stessa cosa
*     per le coordinate YB e ZB
         DO I=1,NX
            IF (XLAT(I).GE.X) THEN
               I0=(I-1)
               I1=(I)
               GO TO 30
            ENDIF
         END DO
         WRITE(*,*) ' Warning: I = NX',I,I1,I0,NREG,X
 30      DO J=1,NY
            IF (YLAT(J).GE.Y) THEN
               J0=(J-1)
               J1=(J)
               GO TO 31
            ENDIF
         END DO
         WRITE(*,*) ' Warning: J = NY',J,J1,J0,NREG,Y
 31      DO K=1,NZ
            IF (ZLAT(K).GE.Z) THEN
               K0=(K-1)
               K1=(K)
               GO TO 32
            ENDIF
         END DO
         WRITE(*,*) ' Warning: K = NZ',K,K1,K0,NREG,Z
*     il procedimento seguente prende spunto da
*     https://en.wikipedia.org/wiki/Trilinear_interpolation
            
*     differences between each of X, Y, Z and the smaller
*     coordinate related
 32      XDIFF=(X-XLAT(I0))/(XLAT(I1)-XLAT(I0))
         YDIFF=(Y-YLAT(J0))/(YLAT(J1)-YLAT(J0))
         ZDIFF=(Z-ZLAT(K0))/(ZLAT(K1)-ZLAT(K0))
      
*     lavoro prima sulla componente x del campo
         B00X=BXLAT(I0,J0,K0)*(1-XDIFF)+BXLAT(I1,J0,K0)*XDIFF
         B01X=BXLAT(I0,J0,K1)*(1-XDIFF)+BXLAT(I1,J0,K1)*XDIFF
         B10X=BXLAT(I0,J1,K0)*(1-XDIFF)+BXLAT(I1,J1,K0)*XDIFF
         B11X=BXLAT(I0,J1,K1)*(1-XDIFF)+BXLAT(I1,J1,K1)*XDIFF
            
         B0X=B00X*(1-YDIFF)+B10X*YDIFF
         B1X=B01X*(1-YDIFF)+B11X*YDIFF
*     componente x del mio punto definito da X, Y, Z
         BXFIN=B0X*(1-ZDIFF)+B1X*(ZDIFF)

*     ora lavoro con la componente y di B
         B00Y=BYLAT(I0,J0,K0)*(1-XDIFF)+BYLAT(I1,J0,K0)*XDIFF
         B01Y=BYLAT(I0,J0,K1)*(1-XDIFF)+BYLAT(I1,J0,K1)*XDIFF
         B10Y=BYLAT(I0,J1,K0)*(1-XDIFF)+BYLAT(I1,J1,K0)*XDIFF
         B11Y=BYLAT(I0,J1,K1)*(1-XDIFF)+BYLAT(I1,J1,K1)*XDIFF     
         
         B0Y=B00Y*(1-YDIFF)+B10Y*YDIFF
         B1Y=B01Y*(1-YDIFF)+B11Y*YDIFF
*     componente y del mio punto definito da X, Y, Z
         BYFIN=B0Y*(1-ZDIFF)+B1Y*(ZDIFF)

*     ora lavoro con la componente z di B
         B00Z=BZLAT(I0,J0,K0)*(1-XDIFF)+BZLAT(I1,J0,K0)*XDIFF
         B01Z=BZLAT(I0,J0,K1)*(1-XDIFF)+BZLAT(I1,J0,K1)*XDIFF
         B10Z=BZLAT(I0,J1,K0)*(1-XDIFF)+BZLAT(I1,J1,K0)*XDIFF
         B11Z=BZLAT(I0,J1,K1)*(1-XDIFF)+BZLAT(I1,J1,K1)*XDIFF
         
         B0Z=B00Z*(1-YDIFF)+B10Z*YDIFF
         B1Z=B01Z*(1-YDIFF)+B11Z*YDIFF
*     componente z del mio punto definito da X, Y, Z
         BZFIN=B0Z*(1-ZDIFF)+B1Z*(ZDIFF)
         
         B=SQRT(BXFIN**2+BYFIN**2+BZFIN**2)
            
         BTX=BXFIN/B
         BTY=BYFIN/B
         BTZ=BZFIN/B
      ENDIF

      RETURN 
      
*===  End of subroutine Magfld =========================================*
      END

