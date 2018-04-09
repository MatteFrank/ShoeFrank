

g++ -std=c++11 -Wall -O2 \
-lMinuit -lGeom -lGeomPainter -lEve -lRGL -lGed -lEG -lTreePlayer -L $FOOTMAIN//libs/GenFit/lib -lgenfit2	\
$FOOTMAIN/libs/lib/libTAGbase.so $FOOTMAIN/libs/lib/libTAGmclib.so 		\
$FOOTMAIN/libs/lib/libTABMbase.so $FOOTMAIN/libs/lib/libTAIRbase.so $FOOTMAIN/libs/lib/libTAITbase.so 		\
$FOOTMAIN/libs/lib/libTATWbase.so $FOOTMAIN/libs/lib/libTACAbase.so $FOOTMAIN/libs/lib/libTAMSDbase.so 		\
$FOOTMAIN/libs/lib/libTAGfoot.so $FOOTMAIN/libs/lib/libTAVTbase.so 		\
-I $FOOTREF/TAIRbase	\
-I $FOOTREF/TABMbase	\
-I $FOOTREF/TAVTbase	\
-I $FOOTREF/TAITbase	\
-I $FOOTREF/TAMSDbase	\
-I $FOOTREF/TATWbase	\
-I $FOOTREF/TACAbase	\
-I $FOOTREF/TAGbase		\
-I $FOOTREF/TAGfoot		\
-I $FOOTMAIN/Simulation	\
-I $FOOTMAIN/libs/GenFit/include \
-I $FOOTMAIN/libs/GenFit/trackReps/include \
-I $FOOTMAIN/libs/GenFit/fields/include \
-I $FOOTMAIN/libs/GenFit/measurements/include \
 $FOOTREF/TAVTbase/TAVTparGeo.cxx $FOOTREF/TAITbase/TAITparGeo.cxx $FOOTREF/TAGfoot/Materials.cxx       \
 $FOOTMAIN/Simulation/MakeGeo.cxx 	\
 -o $FOOTMAIN/Simulation/makeGeo `root-config --cflags --libs` 

