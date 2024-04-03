#!/bin/tcsh

# Exemaple of cmake
# Set FOOT /path/to/FOOT  source

#with Geant4 v11
cmake -DGeant4_DIR=$G4_BUILD11 $FOOT -DANC_DIR=ON  -DFILECOPY=ON

#with all options
cmake $FOOT -DCMAKE_BUILD_TYPE=Release -DFILECOPY=ON -DTOE_DIR=OFF -DGENFIT_DIR=OFF -DANC_DIR=ON
