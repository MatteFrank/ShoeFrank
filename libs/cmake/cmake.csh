#!/bin/tcsh

# Exemaple of cmake
# Set FOOT /path/to/FOOT  source

cmake -DGeant4_DIR=$G4_BUILD10 $FOOT -DCMAKE_BUILD_TYPE=Debug
cmake $FOOT -DCMAKE_BUILD_TYPE=Debug -DFILECOPY=ON -DTOE_DIR=OFF
