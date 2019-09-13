#!/bin/sh
cd ROUTINES
$FLUPRO/flutil/fff  PRO/usrini.f
$FLUPRO/flutil/fff  usrein.f
$FLUPRO/flutil/fff  PRO/usreou.f
$FLUPRO/flutil/fff  usrout.f
$FLUPRO/flutil/fff  mgdraw.f
$FLUPRO/flutil/fff  mgdraw_lib.f
$FLUPRO/flutil/fff  UpdateCurrentParticle.f

$FLUPRO/flutil/ldpmqmd -m fluka PRO/usrini.o usrout.o PRO/usreou.o usrein.o mgdraw.o mgdraw_lib.o UpdateCurrentParticle.o -o fluka_FOOT.exe

rm *.o  PRO/*.o

mv fluka_FOOT.exe ../

cd ../




