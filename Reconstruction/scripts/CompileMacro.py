#!/usr/local/bin/python3
# macro to compile all macros in Reconstruction/macros


import os

os.environ["DYLD_LIBRARY_PATH"] = "/Users/cfinck/Hadron/FOOT/build/libs//lib"
path1 = "/Users/cfinck/Hadron/FOOT/trunk/Reconstruction/macros/"

for filename in os.listdir(path1):
   cmd = 'root -q -e ' + '\'.L ' + filename + '+' '\''
   print(cmd)
   os.system(cmd)
