#!/bin/sh
cd ROUTINES
$FLUPRO/flutil/fff  PRO/usrini.f
$FLUPRO/flutil/fff  usrein.f
$FLUPRO/flutil/fff  PRO/usreou.f
$FLUPRO/flutil/fff  usrout.f
$FLUPRO/flutil/fff  mgdraw.f
$FLUPRO/flutil/fff  magfld.f
$FLUPRO/flutil/fff  mgdraw_lib.f
$FLUPRO/flutil/fff  UpdateCurrentParticle.f 

$FLUPRO/flutil/ldpmqmd -m fluka PRO/usrini.o usrout.o PRO/usreou.o usrein.o  mgdraw.o magfld.o mgdraw_lib.o UpdateCurrentParticle.o -o fluka_FOOT_mag.exe

rm -rf  *.o PRO/*.o

mv fluka_FOOT_mag.exe ../

cd ../




