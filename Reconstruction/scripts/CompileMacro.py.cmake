#!/usr/local/bin/python3
# macro to compile all macros in Reconstruction/macros
#
# usage: python3 CompileMacro.py > & compile.txt &

import os

os.environ["DYLD_LIBRARY_PATH"] = "@CMAKE_BINARY_DIR@/libs/lib"
os.environ["LD_LIBRARY_PATH"] = "@CMAKE_BINARY_DIR@/libs/lib"
path1 = "@CMAKE_CURRENT_SOURCE_DIR@/macros/"

for filename in os.listdir(path1):
   cmd = 'root -q -e ' + '\'.L ' + filename + '+' '\''
   print(cmd)
   os.system(cmd)
