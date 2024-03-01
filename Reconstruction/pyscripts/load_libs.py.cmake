#!/usr/bin/env python3

print("********************************************************************")
print("                     Load libraries for @PROJECT_NAME@ \n")
print("********************************************************************")

from ROOT import gSystem

# comes from configure
gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAG@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMC@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TADI@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TADAQ@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAAPI@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAST@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAIR@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TABM@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAVT@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAIT@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMSD@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TATW@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TACA@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAED@.so")


opt = "@GENFIT_DIR@"
if opt == "ON" :
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@GENFIT_LIBRARIES@.so")
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAGF@.so")

opt = "@TOE_DIR@"
if opt == "ON":
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TATOE@.so")

gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAFO@.so")
gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TANA@.so")

opt = "@ANC_DIR@"
if opt == "ON" :
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAPL@.so")
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TACE@.so")
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TANE@.so")
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAPXI@.so")
   gSystem.Load("@CMAKE_LIBRARY_OUTPUT_DIRECTORY@/lib@TAMP@.so")

