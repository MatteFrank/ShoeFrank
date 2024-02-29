#!/usr/bin/env python3

import load_libs
import sys
from ROOT import TAGrecoManager
from ROOT import GlobalAna
from ROOT import TAGrecoManager

argv = sys.argv

fileOut = ""
fileIn = ""
exp = ""
nTotEv = -1
nSkipEv = -1
runNb = -1
mc = False
   
for i in range(0, len(argv)):
    if argv[i] == "-fileOut":
      i += 1;
      fileOut =str(argv[i]);
    if argv[i] == "-in":
      i += 1;
      fileIn = str(argv[i])
    if argv[i] == "-exp":
      i += 1;
      exp = str(argv[++i])
    if argv[i] == "-nev":
      i += 1;
      nTotEv = int(argv[++i])
    if argv[i] == "-nsk":
      i += 1;
      nSkipEv = int(argv[++i])
    if argv[i] == "-run":
       i += 1;
       runNb = int(argv[++i])
    if argv[i] == "-mc":
        mc = True;
      
TAGrecoManager.Instance(exp)
TAGrecoManager.GetPar().FromFile()
TAGrecoManager.GetPar().Print()

glbAna = GlobalAna(exp, runNb, fileIn, fileOut, mc)
glbAna.BeforeEventLoop()

if nSkipEv > 0:
   glbAna.GoEvent(nSkipEv)
   
glbAna.LoopEvent(nTotEv)
glbAna.AfterEventLoop()
