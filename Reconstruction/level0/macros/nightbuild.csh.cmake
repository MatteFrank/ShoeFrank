#! /bin/tcsh
setenv FOOTLIBS @CMAKE_BINARY_DIR@/libs/
setenv FOOTBUILD @CMAKE_BINARY_DIR@
setenv FOOTSRC @CMAKE_CURRENT_SOURCE_DIR@/../../
setenv FOOTLEVEL0 $FOOTBUILD/Reconstruction/level0

if ($OSTYPE == "darwin") then
  setenv DYLD_LIBRARY_PATH $ROOTSYS/lib:./:$FOOTLIBS/lib:${DYLD_LIBRARY_PATH}
else
  setenv LD_LIBRARY_PATH $ROOTSYS/lib:./:$FOOTLIBS/lib:${LD_LIBRARY_PATH}
endif

cd $FOOTSRC
git pull origin newgeom_v1.0

cd $FOOTBUILD
cmake $FOOTSRC -DCMAKE_BUILD_TYPE=Debug -DANC_DIR=ON -DFILECOPY=ON
make -j4

cd $FOOTLEVEL0
DecodeGlb -in dataRaw/data_test.00004306.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data -out runGSI2021_4306_Plots_1kEvts.root -nev 1000 -exp GSI2021 -run 4306

root -q TestBenchMark.C+
