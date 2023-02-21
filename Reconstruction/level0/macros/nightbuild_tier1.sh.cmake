#! /bin/bash
#define cnaf server env

# compilers
. /cvmfs/sft.cern.ch/lcg/releases/gcc/9.3.0-6991b/x86_64-centos7/setup.sh

#python2
export PATH=/cvmfs/sft.cern.ch/lcg/releases/Python/2.7.9.p1-af54f/x86_64-slc6-gcc48-opt/bin/:$PATH

#python3
export PATH=/cvmfs/sft.cern.ch/lcg/releases/Python/3.9.6-b0f98/x86_64-centos7-gcc9-opt:$PATH

#cmake
export PATH=/cvmfs/sft.cern.ch/lcg/releases/CMake/3.18.4-2ffec/x86_64-centos7-gcc9-opt/bin:$PATH

#make
export PATH=/cvmfs/sft.cern.ch/lcg/contrib/make/4.3/Linux-x86_64/bin:/urs/bin:$PATH

#root
source /opt/exp_software/foot/root/root_install/bin/thisroot.sh

#foot
export FOOTLIBS=@CMAKE_BINARY_DIR@/libs/
export FOOTBUILD=@CMAKE_BINARY_DIR@
export FOOTSRC=@CMAKE_CURRENT_SOURCE_DIR@/../../
export FOOTLEVEL0=$FOOTMAIN/Reconstruction/level0
export FOOTRAWDATA=$FOOTBUILD/Reconstruction/level0/dataRaw
export FOOTMCDATA=$FOOTBUILD/Reconstruction/level0/dataMC

export LD_LIBRARY_PATH=$ROOTSYS/lib:./:$FOOTLIBS/lib:${LD_LIBRARY_PATH}
export PATH=$PATH:$FOOTBUILD/bin

# pull last version
cd $FOOTSRC
echo "update newgeom branch"
git pull origin newgeom_v1.0
echo " "

#build last version
echo "execute cmake & make"
cd $FOOTBUILD
cmake3 $FOOTSRC -DCMAKE_BUILD_TYPE=Debug -DANC_DIR=ON -DFILECOPY=ON
make -j4
echo " "

#run rawdata reconstruction
cd $FOOTLEVEL0
echo "Run reconstruction of raw data"
DecodeGlb -in $FOOTRAWDATA/data_test.00004306.physics_foot.daq.RAW._lb0000._FOOT-RCD._0001.data -out runGSI2021_4306_Plots_1kEvts.root -nev 1000 -exp GSI2021 -run 4306
echo " "

#Compare to reference plots
echo "Compare raw data histograms with reference"
root -q TestBenchMark.C+\(1\)
echo " "

#Run MC reconstruction
echo "Run reconstruction of MC data"
DecodeGlb -in $FOOTMCDATA/GSI2021/16O_C_400_1_shoereg.root -out runGSI2021_MC_400_Plots_1kEvts.root -nev 1000 -exp GSI2021_MC -run 400 -mc
echo " "

#Compare to reference plots
echo "Compare MC data histograms with reference"
root -q TestBenchMark.C+\(0\)

\mv nightbuild.log nightbuild.old
