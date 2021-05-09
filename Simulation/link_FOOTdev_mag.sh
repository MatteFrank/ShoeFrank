#!/bin/sh
cd ROUTINES
$FLUKA/flutil/fff  DEV/usrini.f
$FLUKA/flutil/fff  usrein.f
$FLUKA/flutil/fff  DEV/usreou.f
$FLUKA/flutil/fff  usrout.f
$FLUKA/flutil/fff  mgdraw.f
$FLUKA/flutil/fff  magfld.f
$FLUKA/flutil/fff  mgdraw_lib.f
$FLUKA/flutil/fff  source.f
$FLUKA/flutil/fff  UpdateCurrentParticle.f

$FLUKA/flutil/ldpm3qmd -m fluka DEV/usrini.o usrout.o DEV/usreou.o usrein.o  mgdraw.o magfld.o mgdraw_lib.o source.o UpdateCurrentParticle.o -o fluka_FOOTdev_mag.exe

rm -rf  *.o DEV/*.o

mv fluka_FOOTdev_mag.exe ../

cd ../




