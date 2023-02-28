export ASOFTREF=@CMAKE_BINARY_DIR@/libs/
export FOOTMAIN=@CMAKE_BINARY_DIR@


if [[ "$OSTYPE" == "darwin"* ]]
then
  export DYLD_LIBRARY_PATH=$ROOTSYS/lib:./:$ASOFTREF/lib:${DYLD_LIBRARY_PATH}
else
  export LD_LIBRARY_PATH=$ROOTSYS/lib:./:$ASOFTREF/lib:${LD_LIBRARY_PATH}
fi

export PATH=$PATH:$FOOTMAIN/bin
