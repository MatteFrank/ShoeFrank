#! /bin/tcsh
#define env
setenv FOOTLIBS @CMAKE_BINARY_DIR@/libs/
setenv FOOTBUILD @CMAKE_BINARY_DIR@
setenv FOOTSRC @CMAKE_CURRENT_SOURCE_DIR@/../
setenv FOOTREC $FOOTBUILD/Reconstruction/
setenv FOOTRAWDATA $FOOTBUILD/Reconstruction/dataRaw
setenv FOOTMCDATA $FOOTBUILD/Reconstruction/dataMC

if ($OSTYPE == "darwin") then
  setenv DYLD_LIBRARY_PATH $ROOTSYS/lib:./:$FOOTLIBS/lib:${DYLD_LIBRARY_PATH}
else
  setenv LD_LIBRARY_PATH $ROOTSYS/lib:./:$FOOTLIBS/lib:${LD_LIBRARY_PATH}
endif

set path = ($FOOTBUILD/bin $path)

# pull last version
cd $FOOTSRC
echo "update newgeom branch"
git pull origin newgeom_v1.0
echo " "

#build last version
echo "execute cmake & make"
cd $FOOTBUILD
cmake $FOOTSRC -DCMAKE_BUILD_TYPE=Debug -DANC_DIR=ON -DFILECOPY=ON
make -j4
echo " "

#run rawdata reconstruction
cd $FOOTREC
echo "Run reconstruction of raw data"
\cp ./config/GSI2021/TAMSDdetector.cfg TAMSDdetector_old.cfg
\cp ./config/GSI2021/FootGlobal.par FootGlobal_old.par
sed 's/TrackingFlag:               0/TrackingFlag:               1/' ./config/GSI2021/TAMSDdetector.cfg > TAMSDdetector_new.cfg
sed 's/IncludeStraight: n/IncludeStraight: y/' ./config/GSI2021/FootGlobal.par > FootGlobal_new.par
\mv  TAMSDdetector_new.cfg ./config/GSI2021/TAMSDdetector.cfg
\mv   FootGlobal_new.par ./config/GSI2021/FootGlobal.par
DecodeGlb -in $FOOTRAWDATA/data_test.00004306.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data -out runGSI2021_4306_Plots_1kEvts.root -nev 1000 -exp GSI2021 -run 4306
\mv TAMSDdetector_old.cfg ./config/GSI2021TAMSDdetector.cfg
\mv FootGlobal_old.par ./config/GSI2021/FootGlobal.par
echo " "

#Compare to reference plots
echo "Compare raw data histograms with reference"
root -q TestBenchMark.C+\(1\)
echo " "

#Run MC reconstruction
echo "Run reconstruction of MC data"
\cp ./config/GSI2021_MC/TAMSDdetector.cfg TAMSDdetector_old.cfg
\cp ./config/GSI2021_MC/FootGlobal.par FootGlobal_old.par
sed 's/TrackingFlag:               0/TrackingFlag:               1/' ./config/GSI2021/TAMSDdetector.cfg > TAMSDdetector_new.cfg
sed 's/IncludeStraight: n/IncludeStraight: y/' ./config/GSI2021/FootGlobal.par > FootGlobal_new.par
\mv  TAMSDdetector_new.cfg ./config/GSI2021_MC/TAMSDdetector.cfg
\mv   FootGlobal_new.par ./config/GSI2021_MC/FootGlobal.par
DecodeGlb -in $FOOTMCDATA/GSI2021/16O_C_400_1_shoereg.root -out runGSI2021_MC_400_Plots_1kEvts.root -nev 1000 -exp GSI2021_MC -run 400 -mc
\mv TAMSDdetector_old.cfg ./config/GSI2021_MCTAMSDdetector.cfg
\mv FootGlobal_old.par ./config/GSI2021_MC/FootGlobal.par
echo " "

#Compare to reference plots
echo "Compare MC data histograms with reference"
root -q TestBenchMark.C+\(0\)

\mv nightbuild.new nightbuild.old
\mv nightbuild.log nightbuild.new
\cp  nightbuild.new ~/

 ssh sbgli  -o BatchMode=yes 'cat nightbuild.new | mail -s "shoe night build" cfinck@iphc.cnrs.fr'
 
\rm  ~/nightbuild.new

