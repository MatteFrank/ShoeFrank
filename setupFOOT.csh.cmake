setenv ASOFTREF @CMAKE_BINARY_DIR@/Libraries/
setenv FOOTMAIN @CMAKE_BINARY_DIR@

if ($OSTYPE == "darwin") then
  setenv DYLD_LIBRARY_PATH $ROOTSYS/lib:./:$ASOFTREF/lib:${DYLD_LIBRARY_PATH}
else
  setenv LD_LIBRARY_PATH $ROOTSYS/lib:./:$ASOFTREF/lib:${LD_LIBRARY_PATH}
endif

set path = (${FOOTMAIN}/bin $path)
