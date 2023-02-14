#!/bin/sh
TARGET_MACHINE=`uname`
if [ -z "$FLUPRO" ]; then
    echo "$FLUPRO is not defined"
    if [ "$TARGET_MACHINE" = "Darwin" ] ; then
	say "fluka directory is not defined!"
    fi
    exit
else
    echo "FLUKA directory is" $FLUPRO
fi
GFOR=`gfortran -dumpversion`
GFORN=`gfortran -dumpversion | awk '{s=substr($1,0,2)}{print s}'`
echo "gfortran version is: " $GFOR
if [ `echo $GFORN` -lt 8 ]; then
    echo "gfortran version is not a good one (<8)"
    if [ "$TARGET_MACHINE" = "Darwin" ] ; then
	say "gfortran version is not a good one!"
    fi
    exit
fi
FLUVER=`grep version $FLUPRO/Version.tag`
FLUVER20=`echo $FLUVER | grep 2020`
FLUVER21=`echo $FLUPRO | grep 2021`
if [ -n "$FLUVER21" ]; then
    echo "FLUKA version is a 2021 one"
    echo $FLUVER
    cd ROUTINES
    $FLUPRO/flutil/fff  PRO/usrini.f
    $FLUPRO/flutil/fff  usrein.f
    $FLUPRO/flutil/fff  PRO/usreou.f
    $FLUPRO/flutil/fff  usrout.f
    $FLUPRO/flutil/fff  magfld2021.f
    $FLUPRO/flutil/fff  mgdraw.f
    $FLUPRO/flutil/fff  mgdraw_lib.f
    $FLUPRO/flutil/fff  source.f
    $FLUPRO/flutil/fff  UpdateCurrentParticle.f
    
    $FLUPRO/flutil/ldpmqmdnw -m fluka PRO/usrini.o usrout.o PRO/usreou.o usrein.o magfld2021.o mgdraw.o mgdraw_lib.o source.o UpdateCurrentParticle.o -o fluka_FOOT_mag.exe

    rm *.o  PRO/*.o
    if [ "$TARGET_MACHINE" = "Darwin" ] ; then
	rm -rf *.dSYM
    elif [ "$TARGET_MACHINE" = "Linux" ] ; then
	mv *.map ../
    fi
    mv fluka_FOOT_mag.exe ../
    cd ../
elif [ -n "$FLUVER20" ]; then
    echo "FLUKA version is a 2020 one"
    echo $FLUVER
    cd ROUTINES
    $FLUPRO/flutil/fff  PRO/usrini.f
    $FLUPRO/flutil/fff  usrein.f
    $FLUPRO/flutil/fff  PRO/usreou.f
    $FLUPRO/flutil/fff  usrout.f
    $FLUPRO/flutil/fff  magfld2020.f
    $FLUPRO/flutil/fff  mgdraw.f
    $FLUPRO/flutil/fff  mgdraw_lib.f
    $FLUPRO/flutil/fff  source.f
    $FLUPRO/flutil/fff  UpdateCurrentParticle.f
    
    $FLUPRO/flutil/ldpmqmd -m fluka PRO/usrini.o usrout.o PRO/usreou.o usrein.o magfld2020.o mgdraw.o mgdraw_lib.o source.o UpdateCurrentParticle.o -o fluka_FOOT_mag.exe

    rm *.o  PRO/*.o
    if [ "$TARGET_MACHINE" = "Darwin" ] ; then
	rm -rf *.dSYM
    elif [ "$TARGET_MACHINE" = "Linux" ] ; then
	mv *.map ../
    fi    
    mv fluka_FOOT_mag.exe ../    
    cd ../
else
    echo "Unrecognized FLUKA version"
    if [ "$TARGET_MACHINE" = "Darwin" ] ; then
	say "fluka version is unrecognizable"
    fi
    exit
fi
echo "Build of FLUKA executable is complete"
if [ "$TARGET_MACHINE" = "Darwin" ] ; then
    say "Build of fluka executable is complete!"
fi
exit
