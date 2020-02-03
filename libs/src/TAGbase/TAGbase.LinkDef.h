
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TAGactTreeReader;
#pragma link C++ class TAGactTreeReaderBranch;

#pragma link C++ class TAGactTreeWriter;
#pragma link C++ class TAGactTreeWriterBranch;
#pragma link C++ class TAGaction+;

#pragma link C++ class TAGactionFile+;

#pragma link C++ class TAGactNtuCluster2D;
#pragma link C++ class TAGactNtuCluster1D;

#pragma link C++ class TAGdata;
#pragma link C++ function operator<<(ostream&,const TAGdata&);

#pragma link C++ class TAGcluster;
#pragma link C++ class TAGpoint;
#pragma link C++ class TAGntuPoint;
#pragma link C++ class TAGtrack;
#pragma link C++ class TAGntuGlbTrack;

#pragma link C++ class TAGdataDsc+;

#pragma link C++ class TAGpadGroup+;
#pragma link C++ class TAGview+;

#pragma link C++ class TAGbaseDigitizer+;

#pragma link C++ class TAGeventId-;
#pragma link C++ function operator==(const TAGeventId&,const TAGeventId&);

#pragma link C++ class TAGgeoTrafo+;

#pragma link C++ class TAGparTools+;

#pragma link C++ class TAGnamed;
#pragma link C++ function operator<<(ostream&,const TAGnamed&);

#pragma link C++ class TAGobject;
#pragma link C++ function operator<<(ostream&,const TAGobject&);

#pragma link C++ class TAGpara;
#pragma link C++ function operator<<(ostream&,const TAGpara&);

#pragma link C++ class TAGparaDsc+;

#pragma link C++ class TAGroot;
#pragma link C++ global gTAGroot;

#pragma link C++ class TAGrunInfo+;
#pragma link C++ function operator==(const TAGrunInfo&,const TAGrunInfo&);

#pragma link C++ class TAGtimestamp-;
#pragma link C++ function operator-(const TAGtimestamp&,const TAGtimestamp&);

#pragma link C++ class TAGbaseMaterials+;
#pragma link C++ class TAGmaterials+;
#pragma link C++ class TAGionisMaterials+;

#pragma link C++ class TAGlineFit+;
#pragma link C++ class TAGconfigFile+;

#pragma link C++ class ControlPlotsRepository+;
#pragma link C++ class ControlPlotInfo+;
#pragma link C++ class MagicSkills+;
#pragma link C++ class GlobalPar+;

#endif
