#ifndef GlobalTrackingStudies_hxx
#define GlobalTrackingStudies_hxx

#include <TGeoManager.h>
#include <TGeoMedium.h>
#include <TGeoMaterial.h>

#include <TGeoMaterialInterface.h>
#include <ConstField.h>

#include <KalmanFitterRefTrack.h>
#include <KalmanFitter.h>
#include <KalmanFitterInfo.h>
#include <KalmanFitStatus.h>
#include <DAF.h>
#include <RKTrackRep.h>
#include <Track.h>

#include <Exception.h>
#include <FieldManager.h>
#include <StateOnPlane.h>

#include <TrackPoint.h>
#include <MaterialEffects.h>
#include <TMatrixDSym.h>
#include <TMatrixD.h>


#include <EventDisplay.h>

#include "WireMeasurement.h"
#include "PlanarMeasurement.h"
#include "SpacepointMeasurement.h"
#include "SharedPlanePtr.h"

#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#include <TRandom3.h>

#include <TVector3.h>
#include <vector>

#include <TMath.h>

#include "TAGparGeo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTactNtuClusterF.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuHit.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuHit.hxx"
#include "TAMSDntuCluster.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

#include "TAMCntuPart.hxx"

#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"

#include "TAGrecoManager.hxx"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include "TAGgeoTrafo.hxx"
#include "TAGaction.hxx"
#include "TAMCntuHit.hxx"
#include "TAVTactBaseNtuTrack.hxx"
#include "TAVTactNtuVertex.hxx"
#include "TAVTactNtuTrackF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuVertexPD.hxx"



using namespace std;
using namespace genfit;

#define build_string(expr)						\
  (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


typedef vector<genfit::AbsMeasurement*> MeasurementVector;

class GlobalTrackingStudies : public TAGaction {

public:


	GlobalTrackingStudies(const char* name);
	~GlobalTrackingStudies() {};

   void   CreateHistogram();
	Bool_t Action();

	void Finalize();


private:

	TH1F* m_histo_nClusXTrk;
	TH1F* m_histo_mcGoodTrk;

	TH1F* m_histo_MeanNPix_inClusPerTrk[8];
	TH1F* m_histo_MaxDeltaNPix_inClusPerTrk[8];

	TH1F* m_histo_DeltaX_VtIt_straight[8];
	TH1F* m_histo_DeltaY_VtIt_straight[8];
	TH2F* m_histo_DeltaXDeltaY_VtIt_straight[8];
	string m_kalmanOutputDir;

	int m_moreThanOnePartPerCluster;
	int m_allNoiseTrack;

};

#endif
