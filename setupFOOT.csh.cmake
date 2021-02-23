setenv ASOFTREF @CMAKE_BINARY_DIR@/libs/
setenv FOOTMAIN @CMAKE_BINARY_DIR@
setenv FOOTLEVEL0 $FOOTMAIN/Reconstruction/level0
setenv FOOTFULLREC $FOOTMAIN/Reconstruction/fullrec
setenv FOOTRES $FOOTLEVEL0/results

if ($OSTYPE == "darwin") then
  setenv DYLD_LIBRARY_PATH $ROOTSYS/lib:./:$ASOFTREF/lib:${DYLD_LIBRARY_PATH}
else
  setenv LD_LIBRARY_PATH $ROOTSYS/lib:./:$ASOFTREF/lib:${LD_LIBRARY_PATH}
endif

set path = (${FOOTMAIN}/bin $path)
