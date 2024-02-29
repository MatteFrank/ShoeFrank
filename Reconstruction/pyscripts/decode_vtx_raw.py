#!/usr/bin/env python3

import load_libs
import sys
from ROOT import TAGroot
from ROOT import TAGrecoManager
from ROOT import TAGcampaignManager
from ROOT import TAGactTreeWriter
from ROOT import TAGactDaqReader
from ROOT import TAGgeoTrafo

from ROOT import TAGdaqEvent
from ROOT import TAVTntuHit
from ROOT import TAVTntuCluster
from ROOT import TAVTntuTrack

from ROOT import TAVTactNtuHit
from ROOT import TAVTactNtuCluster
from ROOT import TAVTactNtuTrackF

from ROOT import TAGparaDsc
from ROOT import TAGdataDsc
from ROOT import TAVTparMap
from ROOT import TAVTparGeo
from ROOT import TAVTparConf

from ROOT import TString
from ROOT import TStopwatch


### main
argv = sys.argv

outFileName = ""
filename = ""
expName = ""
nMaxEvts = -1
runNumber = -1
   
for i in range(0, len(argv)):
    if argv[i] == "-out":
      i += 1
      outFileName =str(argv[i])
    if argv[i] == "-in":
      i += 1
      filename = str(argv[i])
    if argv[i] == "-exp":
      i += 1
      expName = str(argv[++i])
    if argv[i] == "-nev":
      i += 1
      nMaxEvts = int(argv[++i])
    if argv[i] == "-run":
       i += 1
       runNumber = int(argv[++i])
  
  
print(filename, outFileName, expName, runNumber, nMaxEvts)
TAGrecoManager.Instance(expName)
TAGrecoManager.GetPar().FromFile()
TAGrecoManager.GetPar().Print()

tagr = TAGroot()

campManager = TAGcampaignManager(expName)
campManager.FromFile()

geoTrafo =  TAGgeoTrafo()
parFileName = campManager.GetCurGeoFile(TAGgeoTrafo.GetBaseName(), runNumber)
geoTrafo.FromFile(parFileName)

outFile = TAGactTreeWriter("outFile")

vtMap =  TAGparaDsc("vtMap", TAVTparMap())
map   = vtMap.Object()
parFileName = campManager.GetCurMapFile(TAVTparGeo.GetBaseName(), runNumber)
map.FromFile(parFileName)

vtGeo    =  TAGparaDsc("vtGeo",  TAVTparGeo())
geomap   = vtGeo.Object()
parFileName = campManager.GetCurGeoFile(TAVTparGeo.GetBaseName(), runNumber)
geomap.FromFile(parFileName)

vtConf  = TAGparaDsc("vtConf", TAVTparConf())
parconf = vtConf.Object()
parFileName = campManager.GetCurConfFile(TAVTparGeo.GetBaseName(), runNumber)
parconf.FromFile(parFileName)

vtDaq    =  TAGdataDsc("vtDaq",  TAGdaqEvent())
vtNtu    =  TAGdataDsc("vtNtu",  TAVTntuHit())
vtClus   =  TAGdataDsc("vtClus",  TAVTntuCluster())
vtTrck   =  TAGdataDsc("vtTrck",  TAVTntuTrack())

daqActReader  = TAGactDaqReader("daqActReader", vtDaq)

vtActRaw  =  TAVTactNtuHit("vtActRaw", vtNtu, vtDaq, vtGeo, vtConf, vtMap)
vtActRaw.CreateHistogram()

vtActClus = TAVTactNtuCluster("vtActClus", vtNtu, vtClus, vtConf, vtGeo)
vtActClus.CreateHistogram()

vtActTrck = TAVTactNtuTrackF("vtActTrck", vtClus, vtTrck, vtConf, vtGeo)
vtActTrck.CreateHistogram()

#outFile.SetupElementBranch(vtNtu, "vtrh.");
#outFile.SetupElementBranch(vtClus, "vtclus.");
#outFile.SetupElementBranch(vtTrck, "vttrack.");

daqActReader.Open(filename)

tagr.AddRequiredItem(daqActReader)
tagr.AddRequiredItem(vtActRaw)
tagr.AddRequiredItem(vtActClus)
#   tagr.AddRequiredItem(vtActTrck)
tagr.AddRequiredItem(outFile)

tagr.Print()

if outFile.Open(outFileName, "RECREATE") :
   exit(0)

vtActRaw.SetHistogramDir(outFile.File())
vtActClus.SetHistogramDir(outFile.File())
vtActTrck.SetHistogramDir(outFile.File())

print(" Beginning the Event Loop ")
tagr.BeginEventLoop()

watch = TStopwatch()
watch.Start()

nEvents = 0
while tagr.NextEvent() :

   nEvents += 1
   if nEvents % 100 == 0:
      print("Event: ", nEvents)

   if nEvents == nMaxEvts:
      break

tagr.EndEventLoop()

outFile.Print()
outFile.Close()
