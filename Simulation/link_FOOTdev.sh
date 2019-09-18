#!/bin/sh
cd ROUTINES
$FLUKA/flutil/fff  DEV/usrini.f
$FLUKA/flutil/fff  usrein.f
$FLUKA/flutil/fff  DEV/usreou.f
$FLUKA/flutil/fff  usrout.f
$FLUKA/flutil/fff  mgdraw.f
$FLUKA/flutil/fff  mgdraw_lib.f
$FLUKA/flutil/fff  UpdateCurrentParticle.f

$FLUKA/flutil/ldpm3qmd -m fluka DEV/usrini.o usrout.o DEV/usreou.o usrein.o  mgdraw.o  mgdraw_lib.o UpdateCurrentParticle.o -o fluka_FOOTdev.exe

rm *.o DEV/*.o

mv fluka_FOOT.exe ../

cd ../




