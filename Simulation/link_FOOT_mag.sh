#!/bin/sh
cd ROUTINES
$FLUPRO/flutil/fff  usrini.f
$FLUPRO/flutil/fff  usrein.f
$FLUPRO/flutil/fff  usreou.f
$FLUPRO/flutil/fff  usrout.f
$FLUPRO/flutil/fff  mgdraw.f
$FLUPRO/flutil/fff  magfld.f
$FLUPRO/flutil/fff  mgdraw_lib.f
$FLUPRO/flutil/fff  UpdateCurrentParticle.f

$FLUPRO/flutil/ldpmqmd -m fluka usrini.o usrout.o usreou.o usrein.o  mgdraw.o magfld.o mgdraw_lib.o UpdateCurrentParticle.o -o fluka_FOOT_mag.exe

rm -rf  *.o 

mv fluka_FOOT_mag.exe ../

cd ../




