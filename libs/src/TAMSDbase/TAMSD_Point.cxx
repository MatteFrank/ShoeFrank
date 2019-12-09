



#include "TString.h"
#include "TClonesArray.h"

#include "TAMSD_Point.hxx"





ClassImp(TAMSD_Point) // Description of Single Detector TAMSD_Point 




//______________________________________________________________________________
//  build a point
TAMSD_Point::TAMSD_Point( int layer, int iCol, int iRow, TVector3 pos )   {
   
	m_layer = layer;

	m_columnHitID = iCol;
	m_rowHitID	  = iRow;

	m_position = pos;

	// to be done
	// m_column = m_columnHit->GetStrip();
	// m_row = m_rowHit->GetStrip();

	// da fare 
	// m_de
	// m_time

	Initialise();

}





//______________________________________________________________________________
//  
void TAMSD_Point::Initialise() {

	m_posMC = TVector3( 0, 0, 0 );
	m_momMC = TVector3( 0, 0, 0 );

	m_isMC = false;

	// take the detector geometry
	// m_geometry = (TAMSDparGeo*) gTAGroot->FindParaDsc("TAMSDGeo", "TAMSDparGeo")->Object();

	// da fare
	// m_column = m_columnHit->GetStrip();
	// m_row = m_rowHit->GetStrip();


	// m_de = m_columnHit->GetEnergyLoss() + m_rowHit->GetEnergyLoss();
	// m_time =  m_columnHit->GetTime();

	// if ( !m_isTrueGhost ) {
	// 	m_posMC = ( m_columnHit->GetMCPosition_detectorFrame() + m_rowHit->GetMCPosition_detectorFrame() )*0.5;
	// 	m_momMC = ( m_columnHit->GetMCMomentum_detectorFrame() + m_rowHit->GetMCMomentum_detectorFrame() )*0.5;
	// }

}



void TAMSD_Point::SetGeneratedParticle ( int colGenPart, int rowGenPart )         {

	m_isMC = true;
	m_columnParticleID = colGenPart;
	m_rawParticleID    = rowGenPart;


	if ( m_columnParticleID == m_rawParticleID )   	{
		m_isTrueGhost = false;
		m_ParticleID = m_columnParticleID;
	}
    else 	{
    	m_isTrueGhost = true;
    	m_ParticleID = -1;
    }

}






//______________________________________________________________________________
void TAMSD_Point::TrueGhostWarning() {
	if ( !m_isTrueGhost )	return;
	cout << "WARNING :: TAMSD_Point::TrueGhostWarning()  -->  you are asking for a single MC value of a ghost point."<< endl;
	exit(0);
}



//______________________________________________________________________________
int TAMSD_Point::GetGenPartID() {
	TrueGhostWarning();
	return m_ParticleID;
}



//______________________________________________________________________________
TVector3 TAMSD_Point::GetPosition_detectorFrame() { 
	// TrueGhostWarning();
	return m_position; 
}


//______________________________________________________________________________
TVector3 TAMSD_Point::GetPosition_footFrame() { 
	// TrueGhostWarning();
	// TVector3 glob = m_position;
	// m_geometry->Local2Global( &glob ); 
	// return glob; 
};


//______________________________________________________________________________
TVector3 TAMSD_Point::GetMCPosition_detectorFrame() { 
	TrueGhostWarning();
	return m_posMC; 
};


//______________________________________________________________________________
TVector3 TAMSD_Point::GetMCPosition_footFrame() { 
	// TrueGhostWarning();
	// TVector3 glob = m_posMC;
	// m_geometry->Local2Global( &glob ); 
	// return glob; 
};


//______________________________________________________________________________
TVector3 TAMSD_Point::GetMCMomentum_detectorFrame() { 
	TrueGhostWarning();
	return m_momMC; 
};


//______________________________________________________________________________
TVector3 TAMSD_Point::GetMCMomentum_footFrame() { 
	// TrueGhostWarning();
	// TVector3 globP = m_momMC;
	// m_geometry->Local2Global_RotationOnly( &globP ); 
	// return globP; 
};



























