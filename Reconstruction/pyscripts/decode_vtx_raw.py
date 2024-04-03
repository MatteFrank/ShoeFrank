#!/usr/bin/env python3

# import libraries
import load_libs
import sys

# import required classes
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


### ################# ################# ###
argv = sys.argv

outFileName = ""
filename    = ""
expName     = ""
nMaxEvts    = -1
runNumber   = -1
   
# looking for arguments
for i in range(0, len(argv)):
    if argv[i] == "-out":
      i += 1
      outFileName =str(argv[i])
    if argv[i] == "-in":
      i += 1
      filename = str(argv[i])
    if argv[i] == "-exp":
      i += 1
      expName = str(argv[i])
    if argv[i] == "-nev":
      i += 1
      nMaxEvts = int(argv[i])
    if argv[i] == "-run":
       i += 1
       runNumber = int(argv[i])
  
  
# instanciate reco manager
TAGrecoManager.Instance(expName)
TAGrecoManager.GetPar().FromFile()
TAGrecoManager.GetPar().Print()

ntu = TAGrecoManager.GetPar().IsSaveTree()

# instanciate TAG root
tagr = TAGroot()

# instanciate campaign manager
campManager = TAGcampaignManager(expName)
campManager.FromFile()

# instanciate FOOT geo transformation
geoTrafo    = TAGgeoTrafo()
parFileName = campManager.GetCurGeoFile(TAGgeoTrafo.GetBaseName(), runNumber)
geoTrafo.FromFile(parFileName)

# out put file
outFile = TAGactTreeWriter("outFile")

# open map, config and geo files
vtMap       = TAGparaDsc(TAVTparMap())
map         = vtMap.Object()
parFileName = campManager.GetCurMapFile(TAVTparGeo.GetBaseName(), runNumber)
map.FromFile(parFileName)

vtGeo       =  TAGparaDsc(TAVTparGeo())
geomap      = vtGeo.Object()
parFileName = campManager.GetCurGeoFile(TAVTparGeo.GetBaseName(), runNumber)
geomap.FromFile(parFileName)
sensorsN    = geomap.GetSensorsN()

vtConf      = TAGparaDsc(TAVTparConf())
parconf     = vtConf.Object()
parFileName = campManager.GetCurConfFile(TAVTparGeo.GetBaseName(), runNumber)
parconf.FromFile(parFileName)

# define containers
vtDaq  = TAGdataDsc(TAGdaqEvent())
vtNtu  = TAGdataDsc(TAVTntuHit(sensorsN))
vtClus = TAGdataDsc(TAVTntuCluster(sensorsN))
vtTrck = TAGdataDsc(TAVTntuTrack())

# input file
name         = tagr.DefaultActionName("TAGactDaqReader");
daqActReader = TAGactDaqReader(name, vtDaq)

# define actions
name      = tagr.DefaultActionName("TAVTactNtuHit");
vtActRaw  = TAVTactNtuHit(name, vtNtu, vtDaq, vtGeo, vtConf, vtMap)
vtActRaw.CreateHistogram()

name      = tagr.DefaultActionName("TAVTactNtuCluster");
vtActClus = TAVTactNtuCluster(name, vtNtu, vtClus, vtConf, vtGeo)
vtActClus.CreateHistogram()

name      = tagr.DefaultActionName("TAVTactNtuTrackF");
vtActTrck = TAVTactNtuTrackF(name, vtClus, vtTrck, vtConf, vtGeo)
vtActTrck.CreateHistogram()

# save in tree
if ntu :
   outFile.SetupElementBranch(vtNtu)
   outFile.SetupElementBranch(vtClus)
   outFile.SetupElementBranch(vtTrck)

##########################################

# open input file
daqActReader.Open(filename)

# required actions
tagr.AddRequiredItem(daqActReader)
tagr.AddRequiredItem(vtActRaw)
tagr.AddRequiredItem(vtActClus)
#   tagr.AddRequiredItem(vtActTrck)
tagr.AddRequiredItem(outFile)

tagr.Print()

# open output file
if outFile.Open(outFileName, "RECREATE") :
   exit(0)

# set directory for histos
vtActRaw.SetHistogramDir(outFile.File())
vtActClus.SetHistogramDir(outFile.File())
vtActTrck.SetHistogramDir(outFile.File())

# loop oever events
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
