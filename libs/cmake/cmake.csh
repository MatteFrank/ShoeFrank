#!/bin/tcsh

# Exemaple of cmake
# Set FOOT /path/to/FOOT  source

cmake $FOOT -DCMAKE_BUILD_TYPE=Debug -DFILECOPY=ON -DTOE_DIR=OFF -DGENFIT_DIR=OFF -DANC_DIR=OFF -DBENCHMARK=ON
