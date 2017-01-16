c----------------------------------------------------
      subroutine dump_common_part() 
c-----------------------------------------------------
c
      integer ii
      include 'mgdraw.inc'
      double precision ptot
c
      write(*,*)' '
      write(*,*)'------- dump_common_part ---------'
      write(*,*)' '
c
      write(*,*)'DUMP common PART'
      write(*,*)'numero particelle = ',nump
      do ii = 1, nump
         write(*,*)'particle = ', ii
         write(*,*)'idparent = ', idpa(ii),' igen = ',igen(ii), 
     &        ' charge = ',icha(ii),' reg =',numreg(ii)
         write(*,*)'idead = ',idead(ii),' jpa =',jpa(ii),
     &        ' mass = ',amass(ii),' time = ', tempo(ii)
         write(*,*)'vert in = ',vxi(ii),vyi(ii),vzi(ii)
         write(*,*)'vert fin = ',vxf(ii),vyf(ii),vzf(ii)
         write(*,*)'p ini = ',px(ii),py(ii),pz(ii)
         ptot = sqrt(px(ii)**2+py(ii)**2+pz(ii)**2)
         write(*,*)'p tot ini = ',ptot,' ekin ini= ',sqrt(ptot*ptot +
     &        amass(ii)*amass(ii))-amass(ii)
         write(*,*)'p fin = ',pxf(ii),pyf(ii),pzf(ii)
         write(*,*)'p tot fin = ',sqrt(pxf(ii)**2+pyf(ii)**2+pzf(ii)**2)
      end do
      return 
      end
c
c      
c----------------------------------------------------
      subroutine dump_common_SCN() 
c-----------------------------------------------------
c
      integer ii
      include 'mgdraw.inc'
c
      write(*,*)' '
      write(*,*)'------- dump_common_SCN ---------'
      write(*,*)' '
c
      write(*,*)'numero rilasci in SCN: ',nSCN
      do ii = 1,nSCN
         write(*,*)'rilascio numero= ',ii,' part id= ',idSCN(ii)
         write(*,*)'x,y,z  in= ',xinSCN(ii),yinSCN(ii),zinSCN(ii)
         write(*,*)'x,y,z out= ',xoutSCN(ii),youtSCN(ii),
     &        zoutSCN(ii)
         write(*,*)'px,py,pz  in= ',pxinSCN(ii),pyinSCN(ii),
     &        pzinSCN(ii)
         write(*,*)'px,py,pz out= ',pxoutSCN(ii),pyoutSCN(ii),
     &        pzoutSCN(ii)
         write(*,*)'de= ',deSCN(ii),' al= ',alSCN(ii),
     &        ' time= ',timSCN(ii)
         write(*,*)
      end do
c
      return
      end
c
c     
c----------------------------------------------------
      subroutine dump_common_CROSS() 
c-----------------------------------------------------
c
      integer ii
      include 'mgdraw.inc'
c
      write(*,*)' '
      write(*,*)'------- dump_common_CROSS ---------'
      write(*,*)' '
c
      write(*,*)'numero crossing: ',nCROSS
      do ii = 1,nCROSS
         write(*,*)'crossing = ',ii,' part id = ',idCROSS(ii)
         write(*,*)'x,y,z = ',xCROSS(ii),yCROSS(ii),zCROSS(ii)
         write(*,*)'px,py,pz  in = ',pxCROSS(ii),pyCROSS(ii),
     &        pzCROSS(ii)
         write(*,*)'reg = ',nregCROSS(ii),' regold = ',nregoldCROSS(ii),
     &        ' m = ',amaCROSS(ii),' time = ',tCROSS(ii),
     &        ' cha= ',chCROSS(ii)
         write(*,*)
      end do      
c     
      return
      end
c
c
c----------------------------------------------------
      subroutine dump_common() 
c-----------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      include 'mgdraw.inc'
c
      if(idbflg .gt. 0) then
c         
         call dump_common_part()
c
         call dump_common_SCN()
c
         call dump_common_CROSS()
c         
      endif
c
      return
      end
c
c      
c-------------------------------------------------------------------------
      subroutine score_STC(mreg,erawSTC,equenchedSTC,xcordin,
     &     ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawSTC, equenchedSTC
      include 'mgdraw.inc'
      integer ii, nSTC_now
c
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_STC-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedSTC= ',
     &        equenchedSTC
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
      endif             
c
c new hit in the start counter?
c
      nSTC_now = 0
      do ii=1,nSTC
         if(idSTC(ii).eq.idcurr) nSTC_now = ii
      end do
c
c if nSTC_now = 0 then ... new hit...
c
      if(nSTC_now.eq.0) then
         if(nSTC.eq.maxSTC) then
            write(*,*)'Error: Score_STC:'
            write(*,*)'Maximum number of STC scoring exceeded: ev= ',
     &           ncase
            return
         endif
         nSTC = nSTC + 1
         nSTC_now = nSTC
         idSTC(nSTC) = idcurr
         xinSTC(nSTC_now)  = sngl(xcordin)
         yinSTC(nSTC_now)  = sngl(ycordin)
         zinSTC(nSTC_now)  = sngl(zcordin)
         pxinSTC(nSTC_now) = sngl(ptrack*cxtrck)
         pyinSTC(nSTC_now) = sngl(ptrack*cytrck)
         pzinSTC(nSTC_now) = sngl(ptrack*cztrck)
         timSTC(nSTC_now)  = sngl(atrack)
      endif
c 
      xoutSTC(nSTC_now) = sngl(xcordout)
      youtSTC(nSTC_now) = sngl(ycordout)
      zoutSTC(nSTC_now) = sngl(zcordout)
      pxoutSTC(nSTC_now) = sngl(ptrack*cxtrck)
      pyoutSTC(nSTC_now) = sngl(ptrack*cytrck)
      pzoutSTC(nSTC_now) = sngl(ptrack*cztrck)
      deSTC(nSTC_now) = deSTC(nSTC_now) + sngl(erawSTC)
      alSTC(nSTC_now)=alSTC(nSTC_now)+ sngl(equenchedSTC)
c
      if(idbflg.gt.1) then
         write(*,*)'nSTC= ',nSTC_now,
     &         ' deSTC(nSTC)= ',deSTC(nSTC_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c
c-------------------------------------------------------------------------
      subroutine score_BMN(mreg,erawBMN,equenchedBMN,
     &     xcordin,ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawBMN, equenchedBMN
      include 'mgdraw.inc'
      integer ii, nBMN_now
      integer iview, ilay, icell
c
      iview = ireg2viewBMN(mreg)
      ilay  = ireg2layBMN(mreg)
      icell = ireg2cellBMN(mreg)
c
      if ((ilay.ge.nlayBMN.or.ilay.lt.0).or.
     &     (icell.ge.ncellBMN.or.icell.lt.0).or.
     &     (iview.ne.1.and.iview.ne.-1)) then
         write(*,*) ' WARNING!!!! ilay, iview, icell, mreg '
         write(*,*) ilay, iview, icell, mreg
      endif
c     
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_BMN-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedBMN= ',
     &        equenchedBMN,' view= ', iview,' lay= ', ilay,
     &       ' cell= ',icell,' mreg= ',mreg
         write(*,*)'xyz= ',xcordin,ycordin,zcordin
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
      endif         
c
c new hit in the beam monitor?
c
      nBMN_now = 0
      do ii=1,nBMN
         if( (idBMN(ii).eq.idcurr) .and. (iviewBMN(ii).eq.iview) .and.
     &        (ilayBMN(ii).eq.ilay) .and. (icellBMN(ii).eq.icell) )then
            nBMN_now = ii
         endif
      end do
c
c if nBMN_now = 0 then ... new hit...
c
      if(nBMN_now.eq.0) then
         if(nBMN.eq.maxBMN) then
            write(*,*)'Error: Score_BMN:'
            write(*,*)'Maximum number of BMN scoring exceeded: ev= ',
     &           ncase
            return
         endif
         nBMN = nBMN + 1
         nBMN_now = nBMN
         idBMN(nBMN) = idcurr
         xinBMN(nBMN_now)  = sngl(xcordin)
         yinBMN(nBMN_now)  = sngl(ycordin)
         zinBMN(nBMN_now)  = sngl(zcordin)
         pxinBMN(nBMN_now) = sngl(ptrack*cxtrck)
         pyinBMN(nBMN_now) = sngl(ptrack*cytrck)
         pzinBMN(nBMN_now) = sngl(ptrack*cztrck)
         timBMN(nBMN_now)  = sngl(atrack)
         iviewBMN(nBMN_now) = iview
         ilayBMN(nBMN_now)  = ilay
         icellBMN(nBMN_now) = icell
      endif
c 
      xoutBMN(nBMN_now) = sngl(xcordout)
      youtBMN(nBMN_now) = sngl(ycordout)
      zoutBMN(nBMN_now) = sngl(zcordout)
      pxoutBMN(nBMN_now) = sngl(ptrack*cxtrck)
      pyoutBMN(nBMN_now) = sngl(ptrack*cytrck)
      pzoutBMN(nBMN_now) = sngl(ptrack*cztrck)
      deBMN(nBMN_now) = deBMN(nBMN_now) + sngl(erawBMN)
      alBMN(nBMN_now)=alBMN(nBMN_now)+ sngl(equenchedBMN)
c
      if(idbflg.gt.1) then
         write(*,*)'nBMN= ',nBMN_now, ' deBMN(nBMN)= ',deBMN(nBMN_now)
         write(*,*)' '
      endif             
c    
      return
      end
c
c-------------------------------------------------------------------------
      subroutine score_VTX(mreg, erawVTX, equenchedVTX,xcordin,
     &     ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawVTX, equenchedVTX
      include 'mgdraw.inc'
      integer mreg, ii
      integer ilay
c
      ilay = mreg-nregFirstVTX
      if (ilay.ge.nlayVTX.or.ilay.lt.0) THEN
         write(*,*) ' WARNING!!!! ilayVTX= ',ilay,
     &    '  zcordin= ',zcordin
      ENDIF
c      
      nVTX_now = 0
c
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_VTX-----------------'
         write(*,*)'test ','idcurr= ',idcurr,' lay= ',
     &        ilay, ' mreg= ',mreg,' nVTX= ',nVTX
         write(*,*)'xyz= ',xcordin,ycordin,zcordin
      endif             
c
c new hit in the vertex?
c
      if (idbflg.gt.1) then
         write(*,*)'Check of previous hits nVTX_now= ',nVTX_now,
     &        ' nVTX= ',nVTX
      endif
      do ii=1,nVTX
         if (idbflg.gt.1) then
            write(*,*)'ii= ',ii
            write(*,*)'idVTX(ii)= ',idVTX(ii),' idcurr= ',idcurr
            write(*,*)'ilay= ',ilay
            write(*,*)'irow(ii)= ',irowVTX(ii),' icol= ',icolVTX(ii)
         endif
         if( ( idVTX(ii).eq.idcurr .and.
     &         ilayVTX(ii).eq.ilay ) .OR.
     &        ( sngl(xcordin).eq.xinVTX(ii) .and. 
     &          sngl(ycordin).eq.yinVTX(ii) .and.
     &          sngl(zcordin).eq.zinVTX(ii) )
     &        ) then 
            nVTX_now = ii
            if (idbflg.gt.1) then
               write(*,*)'Previous hit found: nVTX_now= ',nVTX_now,
     &              ' ii= ',ii
            endif
         endif
      end do
c
c if nVTX_now = 0 then ... new hit...
c
      if( nVTX_now.eq.0) then
         if(nVTX.eq.maxVTX) then
            write(*,*)'Error: Score_VTX:'
            write(*,*)'Maximum hit number exceeded : ev= ',ncase
            return
         endif
         nVTX = nVTX + 1
         nVTX_now = nVTX
         idVTX(nVTX) = idcurr
         ilayVTX(nVTX_now) = ilay
         xinVTX(nVTX_now)  = sngl(xcordin)
         yinVTX(nVTX_now)  = sngl(ycordin)
         zinVTX(nVTX_now)  = sngl(zcordin)
         pxinVTX(nVTX_now) = sngl(ptrack*cxtrck)
         pyinVTX(nVTX_now) = sngl(ptrack*cytrck)
         pzinVTX(nVTX_now) = sngl(ptrack*cztrck)
         timVTX(nVTX_now)  = sngl(atrack)
         if (ycordin.le.-yminVTX) then
            irowVTX(nVTX_now) = int((ycordin-yminVTX)/dyVTX)+1
         else
            irowVTX(nVTX_now) = xpixVTX
         endif
         if (xcordin.le.-xminVTX) then
            icolVTX(nVTX_now) = int((xcordin-xminVTX)/dxVTX)+1
         else
            icolVTX(nVTX_now) = ypixVTX
         endif
         if (irowVTX(nVTX_now).gt.ypixVTX
     &        .or. irowVTX(nVTX_now).lt.1)then
            write(*,*)'WARNING! max vertex row no. exceeded: irow= ',
     &           irowVTX(nVTX_now),'yin= ',yinVTX(nVTX_now),
     &           ' mreg= ',mreg
         endif
         if (icolVTX(nVTX_now).gt.ypixVTX
     &        .or. icolVTX(nVTX_now).lt.1)then
            write(*,*)'WARNING! max vertex col no. exceeded: icol= ',
     &           icolVTX(nVTX_now),'xin= ',xinVTX(nVTX_now),
     &           ' mreg= ',mreg
         endif
      endif
c 
      xoutVTX(nVTX_now) = sngl(xcordout)
      youtVTX(nVTX_now) = sngl(ycordout)
      zoutVTX(nVTX_now) = sngl(zcordout)
      pxoutVTX(nVTX_now) = sngl(ptrack*cxtrck)
      pyoutVTX(nVTX_now) = sngl(ptrack*cytrck)
      pzoutVTX(nVTX_now) = sngl(ptrack*cztrck)
      deVTX(nVTX_now) = deVTX(nVTX_now) + sngl(erawVTX)
      alVTX(nVTX_now) = alVTX(nVTX_now) + sngl(equenchedVTX)
c
      if(idbflg.gt.1) then
         do ii=1,nVTX
            write(*,*)'ii= ',ii
            write(*,*)'idVTX(ii)= ',idVTX(ii),' idcurr= ',idcurr
            write(*,*)'ilay= ',ilayVTX(ii)
            write(*,*)'irow(ii)= ',irowVTX(ii),' icol= ',icolVTX(ii)
         enddo
         write(*,*)'nVTX_now= ',nVTX_now,' nVTX= ',nVTX,
     &         ' deVTX(nVTX)= ',deVTX(nVTX_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c
c-------------------------------------------------------------------------
      subroutine score_ITR(mreg, erawITR, equenchedITR,xcordin,
     &     ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawITR, equenchedITR
      include 'mgdraw.inc'
      integer mreg, ii
      integer ilay
c
      ilay = mreg-nregFirstITR
      if (ilay.ge.nlayITR.or.ilay.lt.0) THEN
         write(*,*) ' WARNING!!!! ilayITR= ',ilay,
     &    '  zcordin= ',zcordin
      ENDIF
c      
      nITR_now = 0
c
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_ITR-----------------'
         write(*,*)'test ','idcurr= ',idcurr,' lay= ',
     &        ilay, ' mreg= ',mreg,' nITR= ',nITR
         write(*,*)'xyz= ',xcordin,ycordin,zcordin
      endif             
c
c new hit in the inner tracker?
c
      if (idbflg.gt.1) then
         write(*,*)'Check of previous hits nITR_now= ',nITR_now,
     &        ' nITR= ',nITR
      endif
      do ii=1,nITR
         if (idbflg.gt.1) then
            write(*,*)'ii= ',ii
            write(*,*)'idITR(ii)= ',idITR(ii),' idcurr= ',idcurr
            write(*,*)'ilay= ',ilay
            write(*,*)'irow(ii)= ',irowITR(ii),' icol= ',icolITR(ii)
         endif
         if( ( idITR(ii).eq.idcurr .and.
     &         ilayITR(ii).eq.ilay ) .OR.
     &        ( sngl(xcordin).eq.xinITR(ii) .and. 
     &          sngl(ycordin).eq.yinITR(ii) .and.
     &          sngl(zcordin).eq.zinITR(ii) )
     &        ) then 
            nITR_now = ii
            if (idbflg.gt.1) then
               write(*,*)'Previous hit found: nITR_now= ',nITR_now,
     &              ' ii= ',ii
            endif
         endif
      end do
c
c if nITR_now = 0 then ... new hit...
c
      if( nITR_now.eq.0) then
         if(nITR.eq.maxITR) then
            write(*,*)'Error: Score_ITR:'
            write(*,*)'Maximum hit number exceeded : ev= ',ncase
            return
         endif
         nITR = nITR + 1
         nITR_now = nITR
         idITR(nITR) = idcurr
         ilayITR(nITR_now) = ilay
         xinITR(nITR_now)  = sngl(xcordin)
         yinITR(nITR_now)  = sngl(ycordin)
         zinITR(nITR_now)  = sngl(zcordin)
         pxinITR(nITR_now) = sngl(ptrack*cxtrck)
         pyinITR(nITR_now) = sngl(ptrack*cytrck)
         pzinITR(nITR_now) = sngl(ptrack*cztrck)
         timITR(nITR_now)  = sngl(atrack)
         if (ycordin.le.-yminITR) then
            irowITR(nITR_now) = int((ycordin-yminITR)/dyITR)+1
         else
            irowITR(nITR_now) = xpixITR
         endif
         if (xcordin.le.-xminITR) then
            icolITR(nITR_now) = int((xcordin-xminITR)/dxITR)+1
         else
            icolITR(nITR_now) = ypixITR
         endif
         if (irowITR(nITR_now).gt.ypixITR
     &        .or. irowITR(nITR_now).lt.1)then
            write(*,*)'WARNING! max ITR row no. exceeded: irow= ',
     &           irowITR(nITR_now),'yin= ',yinITR(nITR_now),
     &           ' mreg= ',mreg
         endif
         if (icolITR(nITR_now).gt.(ypixITR)
     &        .or. icolITR(nITR_now).lt.1)then
            write(*,*)'WARNING! max ITR col no. exceeded: icol= ',
     &           icolITR(nITR_now),'xin= ',xinITR(nITR_now),
     &           ' mreg= ',mreg
         endif
      endif
c 
      xoutITR(nITR_now) = sngl(xcordout)
      youtITR(nITR_now) = sngl(ycordout)
      zoutITR(nITR_now) = sngl(zcordout)
      pxoutITR(nITR_now) = sngl(ptrack*cxtrck)
      pyoutITR(nITR_now) = sngl(ptrack*cytrck)
      pzoutITR(nITR_now) = sngl(ptrack*cztrck)
      deITR(nITR_now) = deITR(nITR_now) + sngl(erawITR)
      alITR(nITR_now) = alITR(nITR_now) + sngl(equenchedITR)
c
      if(idbflg.gt.1) then
         do ii=1,nITR
            write(*,*)'ii= ',ii
            write(*,*)'idITR(ii)= ',idITR(ii),' idcurr= ',idcurr
            write(*,*)'ilay= ',ilayITR(ii)
            write(*,*)'irow(ii)= ',irowITR(ii),' icol= ',icolITR(ii)
         enddo
         write(*,*)'nITR_now= ',nITR_now,' nITR= ',nITR,
     &         ' deITR(nITR)= ',deITR(nITR_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c
c-------------------------------------------------------------------------
      subroutine score_DCH(mreg,erawDCH,equenchedDCH,xcordin,
     &     ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawDCH, equenchedDCH
      include 'mgdraw.inc'
      integer ii, nDCH_now
      integer iview, ilay, icell
c
      iview = ireg2viewDCH(mreg)
      ilay = ireg2layDCH(mreg)
      icell = ireg2cellDCH(mreg)
c
      if ((ilay.ge.nlayDCH.or.ilay.lt.0).or.
     &     (icell.ge.ncellDCH.or.icell.lt.0).or.
     &     (iview.ne.-1.and.iview.ne.1)) then
         write(*,*) ' WARNING!!!! ilay, iview, icell, mreg '
         write(*,*) ilay, iview, icell, mreg
      endif
c     
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_DCH-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedDCH= ',
     &        equenchedDCH,' view= ', iview,' lay= ', ilay,
     &       ' cell= ',icell,' mreg= ',mreg
         write(*,*)'xyz= ',xcordin,ycordin,zcordin
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
      endif         
c
c new hit in the second drift chamber?   
c
      nDCH_now = 0
      do ii=1,nDCH
         if( (idDCH(ii).eq.idcurr) .and. (iviewDCH(ii).eq.iview) .and.
     &        (ilayDCH(ii).eq.ilay) .and. (icellDCH(ii).eq.icell) )then
            nDCH_now = ii
         endif
      end do
c
c if nDCH_now = 0 then ... new hit...
c
      if(nDCH_now.eq.0) then
         if(nDCH.eq.maxDCH) then
            write(*,*)'Error: Score_DCH:'
            write(*,*)'Maximum number of DCH scoring exceeded: ev= ',
     &           ncase
            return
         endif
         nDCH = nDCH + 1
         nDCH_now = nDCH
         idDCH(nDCH) = idcurr
         xinDCH(nDCH_now)   = sngl(xcordin)
         yinDCH(nDCH_now)   = sngl(ycordin)
         zinDCH(nDCH_now)   = sngl(zcordin)
         pxinDCH(nDCH_now)  = sngl(ptrack*cxtrck)
         pyinDCH(nDCH_now)  = sngl(ptrack*cytrck)
         pzinDCH(nDCH_now)  = sngl(ptrack*cztrck)
         timDCH(nDCH_now)   = sngl(atrack)
         iviewDCH(nDCH_now) = iview
         ilayDCH(nDCH_now)  = ilay 
         icellDCH(nDCH_now) = icell
      endif
c 
      xoutDCH(nDCH_now) = sngl(xcordout)
      youtDCH(nDCH_now) = sngl(ycordout)
      zoutDCH(nDCH_now) = sngl(zcordout)
      pxoutDCH(nDCH_now) = sngl(ptrack*cxtrck)
      pyoutDCH(nDCH_now) = sngl(ptrack*cytrck)
      pzoutDCH(nDCH_now) = sngl(ptrack*cztrck)
      deDCH(nDCH_now) = deDCH(nDCH_now) + sngl(erawDCH)
      alDCH(nDCH_now)=alDCH(nDCH_now)+ sngl(equenchedDCH)
c
      if(idbflg.gt.1) then
         write(*,*)'nDCH= ',nDCH_now,
     &         ' deDCH(nDCH)= ',deDCH(nDCH_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c     
c-------------------------------------------------------------------------
      subroutine score_SCN(mreg,erawSCN,equenchedSCN,xcordin,
     &     ycordin,zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawSCN, equenchedSCN
      include 'mgdraw.inc'
      integer ii, nSCN_now
      integer stripSCN, viewSCN
c
      stripSCN = ireg2stripSCN(mreg)
      viewSCN = ireg2viewSCN(mreg)
      if (stripSCNSCN.ge.nstripSCN .or. colSCN.lt.0) THEN
         write(*,*) ' WARNING!!!! stripSCN = ', stripSCN,
     &        ' xcordin= ',xcordin, ' ycordin= ',ycordin,
     &        ' zcordin= ',zcordin         
      ENDIF
      if (viewSCN.gt.1 .or. viewSCN.lt.-1) THEN
         write(*,*) ' WARNING!!!! viewSCN = ', viewSCN,
     &        ' xcordin= ',xcordin, ' ycordin= ',ycordin,
     &        ' zcordin= ',zcordin
      ENDIF
c
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_SCN-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedSCN= ',
     &        equenchedSCN
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
         write(*,*)'mreg= ',mreg,' strip= ',stripSCN,
     &    ' view= ',viewSCN
         write(*,*)'xyz= ',xcordin,ycordin,zcordin
      endif             
c
c new hit in the scintillator?
c
      nSCN_now = 0
      do ii=1,nSCN
         if((idSCN(ii).eq.idcurr.and.  
     &        iviewSCN(ii).eq.viewSCN.and. 
     &        istripSCN(ii).eq.stripSCN ) .OR.
     &        ( sngl(xcordin).eq.xinSCN(ii) .and. 
     &        sngl(ycordin).eq.yinSCN(ii) .and.
     &        sngl(zcordin).eq.zinSCN(ii) ))then
            nSCN_now = ii
         endif
      end do
c
c if nSCN_now = 0 then ... new hit...
c
      if(nSCN_now.eq.0) then
         if(nSCN.eq.maxSCN ) then
            write(*,*)'Error: Score_SCN:'
            write(*,*)'Maximum number of SCN scoring exceeded : ev= ',
     &           ncase
            return
         endif
         nSCN = nSCN + 1
         nSCN_now = nSCN
         idSCN(nSCN) = idcurr
         istripSCN(nSCN_now) = stripSCN
         iviewSCN(nSCN_now) = viewSCN
         xinSCN(nSCN_now)  = sngl(xcordin)
         yinSCN(nSCN_now)  = sngl(ycordin)
         zinSCN(nSCN_now)  = sngl(zcordin)
         pxinSCN(nSCN_now) = sngl(ptrack*cxtrck)
         pyinSCN(nSCN_now) = sngl(ptrack*cytrck)
         pzinSCN(nSCN_now) = sngl(ptrack*cztrck)
         timSCN(nSCN_now)  = sngl(atrack)
      endif
c 
      xoutSCN(nSCN_now) = sngl(xcordout)
      youtSCN(nSCN_now) = sngl(ycordout)
      zoutSCN(nSCN_now) = sngl(zcordout)
      pxoutSCN(nSCN_now) = sngl(ptrack*cxtrck)
      pyoutSCN(nSCN_now) = sngl(ptrack*cytrck)
      pzoutSCN(nSCN_now) = sngl(ptrack*cztrck)
      deSCN(nSCN_now) = deSCN(nSCN_now) + sngl(erawSCN)
      alSCN(nSCN_now) = alSCN(nSCN_now) + sngl(equenchedSCN)
c
      if(idbflg.gt.1) then
         write(*,*)'nSCN= ',nSCN_now,
     &         ' deSCN(nSCN)= ',deSCN(nSCN_now)
         write(*,*)'istripSCN= ',istripSCN(nSCN_now),
     &        'iviewSCN= ',iviewSCN(nSCN_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c
c-------------------------------------------------------------------------
      subroutine score_CAL(mreg, erawCAL, equenchedCAL,xcordin,ycordin,
     &     zcordin,xcordout,ycordout,zcordout)
c--------------------------------------------------------------------------
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      double precision erawCAL, equenchedCAL
      include 'mgdraw.inc'
      integer ii, mreg, nCAL_now
      integer cryCAL
c
      cryCAL = ireg2cryCAL(mreg)
      if (cryCAL.ge.ncryCAL .or. cryCAL.lt.0) THEN
         write(*,*) ' WARNING!!!! cryCAL = ', cryCAL,
     &        ' xcordin= ',xcordin, ' ycordin= ',ycordin
      ENDIF
c      
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'---------------Score_CAL-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedlyso= ',
     &        equenchedCAL
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
      endif
c      
      nCAL_now = 0
c     
c new hit in the calo?
c
      do ii=1,nCAL
         if( ((idCAL(ii).eq.idcurr) .and. (icryCAL(ii).eq.cryCAL)) .or.
     &        ( sngl(xcordin).eq.xinCAL(ii) .and. 
     &        sngl(ycordin).eq.yinCAL(ii) .and.
     &        sngl(zcordin).eq.zinCAL(ii) )) then
            nCAL_now = ii
         endif
      end do
c
c if nCAL_now = 0 then ... new hit...
c
      if(nCAL_now.eq.0) then
         if(nCAL.eq.maxCAL) then
            write(*,*)'Error: Score_CAL:'
            write(*,*)'Maximum number of lyso scoring exceeded : ev= ',
     &           ncase
            return
         endif
         nCAL = nCAL + 1
         nCAL_now = nCAL
         idCAL(nCAL_now) = idcurr
         icryCAL(nCAL_now) = cryCAL
         xinCAL(nCAL_now)  = sngl(xcordin)
         yinCAL(nCAL_now)  = sngl(ycordin)
         zinCAL(nCAL_now)  = sngl(zcordin)
         pxinCAL(nCAL_now) = sngl(ptrack*cxtrck)
         pyinCAL(nCAL_now) = sngl(ptrack*cytrck)
         pzinCAL(nCAL_now) = sngl(ptrack*cztrck)
         timCAL(nCAL_now)  = sngl(atrack)
      endif
c 
      xoutCAL(nCAL_now) = sngl(xcordout)
      youtCAL(nCAL_now) = sngl(ycordout)
      zoutCAL(nCAL_now) = sngl(zcordout)
      pxoutCAL(nCAL_now) = sngl(ptrack*cxtrck)
      pyoutCAL(nCAL_now) = sngl(ptrack*cytrck)
      pzoutCAL(nCAL_now) = sngl(ptrack*cztrck)
      deCAL(nCAL_now) = deCAL(nCAL_now) + sngl(erawCAL)
      alCAL(nCAL_now) = alCAL(nCAL_now) + sngl(equenchedCAL)
c      
      if(idbflg.gt.0) then
         write(*,*)' '
         write(*,*)'---------------Score_CAL-----------------'
         write(*,*)'idcurr= ',idcurr,' equenchedlyso= ',
     &        equenchedCAL
         write(*,*)'dtrack= ',(dtrack(ii),ii=1,mtrack)
      endif      
      if(idbflg.gt.1) then
         write(*,*)'nCAL= ',nCAL_now,
     &        ' deCAL(nCAL)= ',deCAL(nCAL_now)
         write(*,*)'icryCAL= ',icryCAL(nCAL_now)
         write(*,*)' '
      endif             
c
      return
      end
c
c          
c-------------------------------------------------------------------------
      SUBROUTINE score_CROSS(
     &        icharge,numbar,ampart,newreg,mreg,xsco,ysco,zsco)
c--------------------------------------------------------------------------
c
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(TRACKR)'
      include 'mgdraw.inc'
      integer  icharge, numbar, newreg, mreg 
      double precision ampart, xsco, ysco, zsco
c
      if(nCROSS.ge.maxCROSS) then
         write(*,*)"SCORE_CROSS: max number of CROSSing exceeded"
         return
      endif
      nCROSS = nCROSS + 1
      idCROSS(nCROSS) = idcurr
      nregCROSS(nCROSS) = newreg
      nregoldCROSS(nCROSS) = mreg
      xCROSS(nCROSS)  = sngl(xsco)
      yCROSS(nCROSS)  = sngl(ysco)
      zCROSS(nCROSS)  = sngl(zsco)
      pxCROSS(nCROSS) = sngl(ptrack*cxtrck)
      pyCROSS(nCROSS) = sngl(ptrack*cytrck)
      pzCROSS(nCROSS) = sngl(ptrack*cztrck)
      tCROSS(nCROSS)  = sngl(atrack)
      chCROSS(nCROSS) = icharge
      amaCROSS(nCROSS) = sngl(ampart)
c     
      if(idbflg.gt.1) then
         write(*,*)' '
         write(*,*)'--------------- Score_CROSS -----------------'
         write(*,*)'idcurr = ',idcurr,' nCROSS= ',nCROSS
         write(*,*)'reg= ',newreg,' mreg= ',mreg,
     &        ' pxy,z= ',pxCROSS(nCROSS),
     &        pyCROSS(nCROSS),pzCROSS(nCROSS),' mass= ',ampart
         write(*,*)'x,y,zCROSS = ',xsco,ysco,zsco,' t= ',atrack,
     &        ' cha= ',icharge
      endif             
c
      return
      end

