export ASOFTREF=@CMAKE_BINARY_DIR@/libs/
export FOOTMAIN=@CMAKE_BINARY_DIR@
export FOOTREF=$ASOFTREF/src
export FOOTLEVEL0=$FOOTMAIN/Reconstruction/level0
export FOOTFULLREC=$FOOTMAIN/Reconstruction/fullrec
export FOOTCONFIG=$FOOTFULLREC/config
export FOOTRES=$FOOTLEVEL0/results
export FOOTSIMU=$FOOTMAIN/Simulation


if [[ "$OSTYPE" == "darwin"* ]]
then
  export DYLD_LIBRARY_PATH=$ROOTSYS/lib:./:$ASOFTREF/lib:${DYLD_LIBRARY_PATH}
else
  export LD_LIBRARY_PATH=$ROOTSYS/lib:./:$ASOFTREF/lib:${LD_LIBRARY_PATH}
fi

export PATH=$PATH:$FOOTMAIN/bin
