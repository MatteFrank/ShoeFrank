*$ CREATE USREOU.FOR
*COPY USREOU
*
*=== Usreou ===========================================================*
*
      SUBROUTINE USREOU

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
*     USeR Event OUtput: this routine is called at the end of each     *
*     event                                                            *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 09-apr-99     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      include '(CASLIM)'
      include '../mgdraw.inc'
      integer ii
      logical trigger
      double precision Eqcalo, Eqscint
c     
c
C      idbflg = 1
      trigger = .false.
      tarfrag = stcfrag + 10*bmnfrag + 100*tgtfrag + 1000*airfrag
     & + 10000*elsfrag
      if (stcfrag.eq.1) trig_stc = .true.
      if (bmnfrag.eq.1) trig_bmn = .true.
      if (tgtfrag.eq.1) trig_tgt = .true.
      if (airfrag.eq.1) trig_air = .true.
      if (elsfrag.eq.1) trig_els = .true.
      if(idbflg.gt.0) then
         write(*,*) '---------------------------'
         write(*,*)'Usreou. Ev= ',numev,
     &        ' trig_stc: ', trig_stc,
     &        ' trig_bmn: ', trig_bmn,
     &        ' trig_tgt: ', trig_tgt,
     &        ' trig_air: ', trig_air,
     &        ' trig_els: ', trig_els,
     &        ' trigger= ', Trigger,
     &        ' fragtrig= ',fragtrig,
     &     ' tarfrag= ',tarfrag
         write(*,*)
      endif
      Eqcalo = 0
      Eqscint = 0
c
      do ii =1,nCAL
         Eqcalo = Eqcalo + deCAL(ii)
      end do
c
      do ii =1,nSCN
         Eqscint = Eqscint + deSCN(ii)
      end do
c
C      if( (Eqcalo.ge.Ethrdep) .or. (Eqscint.gt.Ethrdep) ) then
C         trigger = .true.
C     endif
C
C     fragtrig = 0 ----- no trigger
C     fragtrig = 1 ----- only STC with inelastic interaction
C     fragtrig = 2 ----- only BMN with inelastic interaction
C     fragtrig = 3 ----- only TGT with inelastic interaction
C     fragtrig = 4 ----- only AIR with inelastic interaction
C     fragtrig = 5 ----- inelastic interaction in STC + X
C     fragtrig = 6 ----- inelastic interaction in TGT + X
C     fragtrig = 7 ----- inelastic interaction in AIR + X
C     fragtrig = 8 ----- inelastic interaction in ELS + X
C     fragtrig = 9 ----- inelastic interaction in BMN + X
C     fragtrig = 10 ---- inelastic interaction in STC - BMN +X
C     fragtrig = 11 ---- inelastic interaction in BMN - STC +X 
C     fragtrig = 34 ---- inelastic interaction in TGT + AIR - STC -BMN -ELS      
C     fragtrig = 35 ---- inelastic interaction in TGT + ELS - STC -BMN -AIR     
C     fragtrig = -1 ---- no inelastic interaction in STC
C     fragtrig = -2 ---- no inelastic interaction in BMN
C     fragtrig = -3 ---- no inelastic interaction in TGT
C     fragtrig = -4 ---- no inelastic interaction in AIR
C     fragtrig = -123 -- no inelastic interaction in STC, BMN and TGT
C     fragtrig = -1234 - no inelastic interaction in STC, BMN, TGT and AIR
C     
      if (fragtrig.eq.0 ) then
         trigger = .true.
      else if (fragtrig.eq.1) then
         if (trig_stc .and. .not.trig_bmn .and. .not.trig_tgt .and.
     &        .not.trig_air .and. .not.trig_els) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'STC Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.2) then
         if (trig_bmn .and. .not.trig_stc .and. .not.trig_tgt .and.
     &        .not.trig_air .and. .not.trig_els) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'BMN Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.3) then
         if (trig_tgt .and. .not.trig_stc .and. .not.trig_bmn .and.
     &        .not.trig_air .and. .not.trig_els) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'TGT Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.4) then
         if (trig_air .and. .not.trig_stc .and. .not.trig_bmn .and.
     &        .not.trig_tgt .and. .not.trig_els) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'AIR Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.5) then
         if (trig_stc) then
            trigger =  .true.
            if(idbflg.gt.0) then
               write(*,*)'STC+X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.6) then
         if (trig_tgt) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'TGT+X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.7) then
         if (trig_air) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'AIR+X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.8) then
         if (trig_els) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'ELS+X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.9) then
         if (trig_bmn) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'BMN+X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif         
      else if (fragtrig.eq.10) then
         if (trig_stc .and. .not.trig_bmn) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'SCT -BMN +X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif
      else if (fragtrig.eq.11) then
         if (trig_bmn .and. .not.trig_stc) then
            trigger = .true.
            if(idbflg.gt.0) then
               write(*,*)'BMN -STC +X Ev= ',numev,
     &              ' trig_stc: ', trig_stc,
     &              ' trig_bmn: ', trig_bmn,
     &              ' trig_tgt: ', trig_tgt,
     &              ' trig_air: ', trig_air,
     &              ' trig_els: ', trig_els,
     &              ' trigger= ', Trigger,
     &              ' fragtrig= ',fragtrig,
     &              ' tarfrag= ',tarfrag
            endif
         endif         
      else if (fragtrig.eq.-1) then
         if (.not.trig_stc) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-2) then
         if (.not.trig_bmn) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-3) then
         if (.not.trig_tgt) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-4) then
         if (.not.trig_air) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-5) then
         if (.not.trig_els) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-123) then
         if (.not.trig_stc .and. .not.trig_bmn
     &        .and. .not.trig_tgt) then
            trigger = .true.
         endif
      else if (fragtrig.eq.-1234) then
         if (.not.trig_stc .and. .not.trig_bmn
     &        .and. .not.trig_tgt .and. .not.trig_air) then
            trigger = .true.
         endif
      else if (fragtrig.eq.35) then
         if (trig_tgt .and. trig_els .and. .not.trig_stc
     &        .and. .not.trig_bmn .and. .not.trig_air) then
            trigger = .true.
         endif
      else if (fragtrig.eq.34) then
         if (trig_tgt .and. trig_air .and. .not.trig_stc
     &        .and. .not.trig_bmn .and. .not.trig_els) then
            trigger = .true.
         endif
      endif
c
C      idbflg = 0
      if(idbflg.gt.0) then
         write(*,*)'ev= ',numev,' Equench= ',Eqcalo,
     &        ' Eqscint= ',Eqscint,
     &        ' trigger= ', Trigger,
     &        ' Eqcalo= ',Eqcalo,' Eqscint= ',Eqscint,
     &        ' Ethrdep= ',Ethrdep,' tarfrag= ',tarfrag
      endif
c
c
      if (idbflg.eq.10) trigger = .false.
      if(trigger) then
c
         write(outunit,*) ncase,nump,nSTC,nBMN,nVTX,nITR,nMSD,nSCN,nCAL,
     &        nCROSS
c
c     scrivo la banca delle particelle
c
         do ii = 1,nump
            write(outunit,*)idpa(ii), igen(ii), icha(ii), 
     &           numreg(ii), iba(ii), idead(ii), jpa(ii), vxi(ii), 
     &           vyi(ii), vzi(ii), vxf(ii), vyf(ii), vzf(ii), px(ii), 
     &           py(ii),pz(ii),pxf(ii),pyf(ii),pzf(ii),amass(ii), 
     &           tempo(ii),tof(ii),trlen(ii)
         end do
c
c     scrivo lo start counter
c         
         do ii = 1,nSTC   
            write(outunit,*) idSTC(ii), 
     &           xinSTC(ii), yinSTC(ii), zinSTC(ii),
     &           xoutSTC(ii), youtSTC(ii), zoutSTC(ii),
     &           pxinSTC(ii), pyinSTC(ii), pzinSTC(ii), 
     &           pxoutSTC(ii), pyoutSTC(ii), pzoutSTC(ii),
     &           deSTC(ii), alSTC(ii), timSTC(ii)
         end do
c
c     scrivo il beam monitor
c         
         do ii = 1,nBMN   
            write(outunit,*) idBMN(ii), iviewBMN(ii), ilayBMN(ii), 
     &           icellBMN(ii), xinBMN(ii), yinBMN(ii), zinBMN(ii),
     &           xoutBMN(ii), youtBMN(ii), zoutBMN(ii),
     &           pxinBMN(ii), pyinBMN(ii), pzinBMN(ii), 
     &           pxoutBMN(ii), pyoutBMN(ii), pzoutBMN(ii),
     &           deBMN(ii), alBMN(ii), timBMN(ii)
         end do
c     
c     scrivo il vertice
c         
         do ii = 1,nVTX
            write(outunit,*) idVTX(ii), ilayVTX(ii),
     &           xinVTX(ii), yinVTX(ii), zinVTX(ii),
     &           xoutVTX(ii), youtVTX(ii), zoutVTX(ii),
     &           pxinVTX(ii), pyinVTX(ii), pzinVTX(ii), 
     &           pxoutVTX(ii), pyoutVTX(ii), pzoutVTX(ii),
     &           deVTX(ii), alVTX(ii), timVTX(ii)
         end do
c     
c     scrivo l'inner tracker
c         
         do ii = 1,nITR
            write(outunit,*) idITR(ii), isensITR(ii),
c     &       iplumeITR(ii), imimoITR(ii), ilayITR(ii),
     &           xinITR(ii), yinITR(ii), zinITR(ii),
     &           xoutITR(ii), youtITR(ii), zoutITR(ii),
     &           pxinITR(ii), pyinITR(ii), pzinITR(ii), 
     &           pxoutITR(ii), pyoutITR(ii), pzoutITR(ii),
     &           deITR(ii), alITR(ii), timITR(ii)
         end do
c     
c     scrivo le microstrip
c         
         do ii = 1,nMSD   
            write(outunit,*) idMSD(ii), 
     &           ilayMSD(ii),
     &           xinMSD(ii), yinMSD(ii), zinMSD(ii),
     &           xoutMSD(ii), youtMSD(ii), zoutMSD(ii),
     &           pxinMSD(ii), pyinMSD(ii), pzinMSD(ii), 
     &           pxoutMSD(ii), pyoutMSD(ii), pzoutMSD(ii),
     &           deMSD(ii), alMSD(ii), timMSD(ii)
         end do
c
c     scrivo lo scintillatore
c
         do ii = 1,nSCN
            write(outunit,*) idSCN(ii),
     &           istripSCN(ii), iviewSCN(ii),
     &           xinSCN(ii), yinSCN(ii), zinSCN(ii),
     &           xoutSCN(ii), youtSCN(ii), zoutSCN(ii),
     &           pxinSCN(ii), pyinSCN(ii), pzinSCN(ii), 
     &           pxoutSCN(ii), pyoutSCN(ii), pzoutSCN(ii),
     &           deSCN(ii), alSCN(ii), timSCN(ii)
         end do
c     
c     scrivo il calorimetro
c         
         do ii = 1,nCAL
            write(outunit,*) idCAL(ii), icryCAL(ii),
     &           xinCAL(ii), yinCAL(ii), zinCAL(ii),
     &           xoutCAL(ii), youtCAL(ii), zoutCAL(ii),
     &           pxinCAL(ii), pyinCAL(ii), pzinCAL(ii), 
     &           pxoutCAL(ii), pyoutCAL(ii), pzoutCAL(ii),
     &           deCAL(ii), alCAL(ii), timCAL(ii)
         end do
c
c     scrivo i crossings
c
         do ii = 1,nCROSS 
            write(outunit,*) idCROSS(ii), nregCROSS(ii),
     &           nregoldCROSS(ii), xCROSS(ii), yCROSS(ii), zCROSS(ii),
     &           pxCROSS(ii), pyCROSS(ii), pzCROSS(ii),
     &           amaCROSS(ii), chCROSS(ii), tCROSS(ii)
         end do
c
c     
c
      endif
c     
      if(idbflg .gt. 0) then
         call dump_common()
         write(*,*)'***************************************************'
         write(*,*)'***************** EVENT END ***********************'
         write(*,*)'***************************************************'
         write(*,*)''
      endif
c
c
c
      RETURN
*===  End of subroutine Usreou =========================================*
      END





