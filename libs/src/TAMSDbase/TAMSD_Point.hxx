
#ifndef _TAMSD_Point_HXX
#define _TAMSD_Point_HXX

// ROOT classes
#include "TObject.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

// #include "TAMSDdatRaw.hxx"
// #include "TAMSDntuHit.hxx"

// all 3 needed to take from gTagROOT
#include "TAGroot.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"

// #include "TAMCntuEve.hxx"
// #include "TAGntuMCeveHit.hxx"


// class TAVTrawHit;

/** TAMSD_Point class is the scintillator hit reconstructed by the intersection of 2 hits on a 
	column and a row bar respectively
 
	Created in 2018 by Matteo Franchini franchinim@bo.infn.it

	All the coordinates are in cm and in the detector reference frame, i.e. the center
	is the center of the detector.

*/
/*------------------------------------------+---------------------------------*/



class TAMSD_Point : public TObject {
   
private:

	// TAMSDparGeo* m_geometry;

	TVector3 m_position;
	int m_layer;
	int m_column;
	int m_row;

	int m_columnHitID;
	int m_rowHitID;

	int m_columnParticleID;
	int m_rawParticleID;
	int m_ParticleID;

	bool m_isMC;
	bool m_isTrueGhost;
	// bool m_isGhost;

	double m_de;                          // energy loss in the scintillator bar
	double m_time;                        // for the moment I take the column time



	// currently the exists for the non-TrueGhost
	// for the non-TrueGhost, average of the col and row values
	// maybe I should do instead? :
	// for the trueGhost, pos = col_MCx, row_MCy, average of z
	// for the trueGhost, mom = MC momentum average
	TVector3 m_posMC;                     // MC hit position = track hitting the scint.
	TVector3 m_momMC;                     // MC momentum of the hit

	void TrueGhostWarning();



public:

	TAMSD_Point() {};
	TAMSD_Point( int layer, int iCol, int iRow, TVector3 pos );
	~TAMSD_Point() {};

	void Initialise();


	// all the Set methods 
	// void SetIsGhost();


	//    All the Get methods
	TVector3 GetPosition_detectorFrame();
	TVector3 GetPosition_footFrame();

	int GetLayer()          { return m_layer; };
	int GetColumn()         { return m_column; };
	int GetRow()            { return m_row; };

	void SetGeneratedParticle ( int colGenP, int rowGenP );

	bool IsMC()             { return m_isMC; };
	bool IsTrueGhost()      { return m_isTrueGhost; };

	int GetColumnHitID()     { return m_columnHitID; };
	int GetRowHitID()        { return m_rowHitID; };

	
	double GetEnergyLoss()          { return m_de; };
	double GetTime()                { return m_time; };

	// float IsGhost()         { return m_isGhost; };

	int                 GetGenPartID();

	TVector3          GetMCPosition_detectorFrame();
	TVector3          GetMCPosition_footFrame();

	TVector3           GetMCMomentum_detectorFrame();
	TVector3           GetMCMomentum_footFrame();

	
	ClassDef(TAMSD_Point,3)                            
};

//##############################################################################



#endif























