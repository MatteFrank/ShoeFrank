#include "MultiTrackCheck.hxx"


using namespace std;



//----------------------------------------------------------------------------------------------------
MultiTrackCheck::MultiTrackCheck() {


  m_Plotter = ControlPlotsRepository::GetControlObject("Track");
  
}

void MultiTrackCheck::Initialize( EVENT_STRUCT* evStr ) {

  debug_multi = 0;
  doMinDist = false;
  doVertex = false;
  //////// Detector Hit //////////////////
  TAGdataDsc* _vtraw    = gTAGroot->FindDataDsc("vtRaw",  "TAVTntuRaw");
  myn_vtraw = (TAVTntuRaw*) _vtraw->Object();

  // TAGdataDsc* _vtclus   = gTAGroot->FindDataDsc("vtClus",  "TAVTntuCluster");
  // myn_vtclus = (TAVTntuRaw*) _vtclus->Object();
  
  
  // take the VT geometry object
  //myp_vtgeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());
  m_VT_geo = shared_ptr<TAVTparGeo> ( (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );

  
  TAGdataDsc* _itraw    = gTAGroot->FindDataDsc("itRaw",  "TAITntuRaw");
  myn_itraw = (TAITntuRaw*) _itraw->Object();
   // take the IT geometry object
  //myp_itgeo    = new TAGparaDsc("itGeo", new TAITparGeo());
  m_IT_geo = shared_ptr<TAITparGeo> ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );
  
  TAGdataDsc* _msdraw   = gTAGroot->FindDataDsc("msdRaw", "TAMSDntuRaw");
  myn_msdraw = (TAMSDntuRaw*) _msdraw->Object();

   //////// MCtrack //////////////////
  // TAGdataDsc* _mceve    = new TAGdataDsc("myn_mceve", new TAMCntuEve());
  // mya_mceve    = new TAMCactNtuEve("mya_mceve", myn_mceve, evStr);

  distanceok = 0;
  distancetot = 0;
  pos1 = 0;
  pos2 = 0;
  pos3 = 0;
  pos4 = 0;
  badchi2truetrack =0;
  tottrack=0;
  tottrackwithatleastonehitperdet=0;
  tottrackwith9hits =0;
  Chi2ofallgoodtrack = 0;
  Chi2oftruetrack = 0;
  Chi2goodwithtruetrack =0;
  goodchi2truetrack =0;
}

void MultiTrackCheck::Process( Long64_t jentry ){
  
//  //cout << " EVENT EVENT  EVENT "  << endl; 
//  // Hit collection per det
//  m_VT_hitCollection.clear();
//  m_IT_hitCollection.clear();
//  m_MSD_hitCollection.clear();
//  m_VT_partIDColl.clear();
//  // Frag info container
//  m_FragIdxs.clear();
//  m_Frag_hitCollection.clear();
//  m_Frag_indexCollection.clear();
//  // Track list with 9 hits
//  m_Trcks9hits.clear();
//  m_Trcks9index.clear();
//  //VT for fit
//  firstVThitlist.clear();
//  secondVThitlist.clear();
//  thirdVThitlist.clear();
//  lastVThitlist.clear();
//  //IT for fit
//  firstIThitlist.clear();
//  secondIThitlist.clear();
//  //MSD for fit
//  firstMSDhitlist.clear();
//  secondMSDhitlist.clear();
//  thirdMSDhitlist.clear();
//  //P0 and P1 for Vertexing
//  m_p0_zx.clear();
//  m_p0_zy.clear();
//  m_p1_zx.clear();
//  m_p1_zy.clear();
//  
//  int ntrackonVT = 0;     
//  int ntrackonIT = 0;  
//  int ntrackonMSD = 0;
//  TwoTrackEvent = false;
//
//
//  // // save pixels in the collection
//   TAVTparGeo* vtxGeo = (TAVTparGeo*) gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object();
//
//  for (int nSensor = 0; nSensor <  vtxGeo->GetSensorsN(); nSensor++) {	// over all sensors
//    for (int nPx = 0; nPx <  myn_vtraw->GetPixelsN( nSensor ); nPx++){ 		// over all pixels for each sensor
//      
//      TAVTntuHit* vt_hit = myn_vtraw->GetPixel( nSensor, nPx);
//      
//      tmpVT_partID =  vt_hit->m_genPartID;
//      //  cout << " tmpVT_partID  " << tmpVT_partID  << endl;
//      
//      if(vt_hit->GetMCPosition_footFrame().Z() < 0.7) ntrackonVT++;
//      
//
//      // get true  coord
//      TVector3 vtpos = vt_hit->GetMCPosition_footFrame();
//      
//            
//      // get pixel coord
//      TVector3 vtpixelPos = m_VT_geo->GetPixelPos_footFrame( vt_hit->GetLayer(), vt_hit->GetPixelColumn(), vt_hit->GetPixelLine() );
//      
//      
//      // control plot histograms for VT
//      m_Plotter->SetMultiTrackPlots("VT", &vtpos, &vtpixelPos);
//      
//
//      m_VT_hitCollection.push_back(vt_hit);
//      m_VT_partIDColl.push_back(tmpVT_partID);
//
//      
//    }
//  }
//  
//
//
//  // // VT cluster
//    
//  // TAVTntuCluster* vtclus = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus", "TAVTntuCluster")->Object();
//  // // for (int nSensor = 0; nSensor < ntup->GetSensorsN(); nSensor++) {   // over all sensors
//  // for (int nSensor = 0; nSensor < 4; nSensor++) {   // over all sensors
//    
//  //   cout << "N vertex pixel in sensor " << nSensor << ": " << vtclus->GetClustersN( nSensor ) << endl;
//    
//  //   for (int nPx = 0; nPx < vtclus->GetClustersN( nSensor ); nPx++)  {     // over all pixels for each sensor
//  //     TClonesArray* arra = vtclus->GetCluster( nSensor, nPx )->GetListOfPixels();
//  //     for ( int n=0; n<arra->GetEntries(); n++ ) {
//  // 	//TVector3  vt_clus = ( (TAVTntuHit*) arra->At(n) )->GetPixelPosition_footFrame();
//  // 	m_VT_hitCollection.push_back((TAVTntuHit*) arra->At(n));
//  //     }
//  //   }
//  // }
//  
//  
//   TAITparGeo* itGeo = (TAITparGeo*) gTAGroot->FindParaDsc(TAITparGeo::GetItDefParaName(), "TAITparGeo")->Object();
//
//  
//  for (int nSensor = 0; nSensor < itGeo->GetSensorsN(); nSensor++) {	// over all sensors
//    
//    for (int nPx = 0; nPx < myn_itraw->GetPixelsN( nSensor ); nPx++){ 		// over all pixels for each sensor
//      
//      TAITntuHit* it_hit = (TAITntuHit*)myn_itraw->GetPixel( nSensor, nPx );
//      
//      if((it_hit->GetMCPosition_footFrame().Z() > 13.3 && it_hit->GetMCPosition_footFrame().Z() < 13.5) || (it_hit->GetMCPosition_footFrame().Z() > 14.3 && it_hit->GetMCPosition_footFrame().Z() < 14.5))  ntrackonIT++;
//      
//      
//
//
//       // get true  coord
//      TVector3 itpos = it_hit->GetMCPosition_footFrame();
//
//      // get pixel coord
//      TVector3  itpixelPos = m_IT_geo->GetPixelPos_footFrame( it_hit->GetSensorID(), it_hit->GetPixelColumn(), it_hit->GetPixelLine() );
//
//
//
//      m_Plotter->SetMultiTrackPlots("IT", &itpos, &itpixelPos);
// 
//
//      m_IT_hitCollection.push_back(it_hit);
//
//    }
//    
//}
//
//
//
//
//  // IT cluster 
//  // TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus", "TAITntuCluster")->Object();
//  // //for (int nSensor = 0; nSensor < ntup->GetSensorsN(); nSensor++) {   // over all sensors
//    
//  //   for (int nSensor = 0; nSensor < 32; nSensor++) {   // over all sensors
//
//  //     cout << "N vertex pixel in sensor " << nSensor << ": " << itclus->GetClustersN( nSensor ) << endl;
//      
//  //   for (int nPx = 0; nPx < itclus->GetClustersN( nSensor ); nPx++)  {     // over all pixels for each sensor
//      
//  //     TClonesArray* arra = itclus->GetCluster( nSensor, nPx )->GetListOfPixels();
//  //     for ( int n=0; n<arra->GetEntries(); n++ ) {
//  // 	//TVector3 vPos = ( (TAITntuHit*) arra->At(n) )->GetPixelPosition_footFrame();
//  // 	m_IT_hitCollection.push_back((TAITntuHit*) arra->At(n));
//	
//  //     }
//  //   }
//  // }  
//  
//  
//  
//
//
//
//  
//   
//   for (int i = 0; i < myn_msdraw->GetPixelsN(0); i++)  {                    
//     TAMSDntuHit* msd_hit = myn_msdraw->GetPixel(0,i);       
//     
//   //  if(msd_hit->GetMCPosition_Global().Z() < 27.0) 	ntrackonMSD++;
//     
//    
//   
//      // get true  coord
//    // TVector3 msdpos = msd_hit->GetMCPosition_Global();
//     
//     
//   //  m_Plotter->SetMultiTrackPlots("MSD", &msdpos, &msdpos); //only true MC info for now
//	     
//     m_MSD_hitCollection.push_back(msd_hit);
//
//
//   }
//   
//  
//   
//   //// Read track ntuple variable//// 
//   //TAMCntuEve*  p_ntumceve = (TAMCntuEve*)   myn_mceve->GenerateObject();
//   TAMCntuEve*  p_ntumceve  = (TAMCntuEve*) gTAGroot->FindDataDsc("myn_mceve", "TAMCntuEve")->Object();
//   int ntrackMC =  p_ntumceve->GetHitN();
//   // cout << " ntrackMC  " << ntrackMC << endl; 
//   tottrack += ntrackMC;
//   
//   m_Plotter->SetMultiTrackInfo("TrackInfo",ntrackonVT, ntrackonIT, ntrackonMSD, ntrackMC);
//   
//   for(int i=0; i<ntrackMC; i++){
//     TAMCntuEveHit *myTrack = p_ntumceve->Hit(i);
//     int index = i;
//     m_Frag_indexCollection.push_back(index);
//     m_Frag_hitCollection.push_back(myTrack);  
//   }
//
//   // cout << " m_Frag_indexCollection.size()  " <<  m_Frag_indexCollection.size() << endl; 
//   // cout << " m_Frag_hitCollection.size()    " <<  m_Frag_hitCollection.size() << endl; 
//    // cout << " m_VT_hitCollection.size()     " <<   m_VT_hitCollection.size() << endl;
//    // cout << " m_IT_hitCollection.size()     " <<   m_IT_hitCollection.size() << endl;
//    // cout << " m_MSD_hitCollection.size()   " <<  m_MSD_hitCollection.size() << endl;
//     
//   for (int i=0; i<m_Frag_hitCollection.size(); i++){
//       
//     int nVT = 0;     
//     int nIT = 0;  
//     int nMSD = 0;  
//     
//
//     for(int j=0; j<m_VT_hitCollection.size(); j++){
//       // int tmp_vtid = m_VT_hitCollection.at(j)->GetMCid()-1;  
//       // int tmp_vtid_mgen = m_VT_hitCollection.at(j)->m_genPartID;
//       int tmp_vtid_mgen = m_VT_hitCollection.at(j)->GetMCid();
//       // cout << " tmp_vtid  " << tmp_vtid  << endl;
//       //  cout << " tmp_vtid_mgen  " << tmp_vtid_mgen  << endl;
//       // cout << " m_Frag_indexCollection.at(i)  " << m_Frag_indexCollection.at(i)  << endl;
//       if(tmp_vtid_mgen == m_Frag_indexCollection.at(i)){
//	 
//	 if (debug_multi > 0) cout<<" Vt hit associated to part "<< i << " That is a:: "<< m_Frag_hitCollection.at(i)->FlukaID() <<"  and has charge, mass:: "<< m_Frag_hitCollection.at(i)->Chg()<<" "<<m_Frag_hitCollection.at(i)->Mass()<<" "<<endl; 
//
//	 nVT++;
//       }
//       
//     }
//     
//     for(int k=0; k<m_IT_hitCollection.size(); k++){
//       // int tmp_itid = m_IT_hitCollection.at(k)->GetMCid()-1;
//       // int tmp_itid_mgen = m_IT_hitCollection.at(k)->m_genPartID;
//       int tmp_itid_mgen = m_IT_hitCollection.at(k)->GetMCid();
//       //if(tmp_itid == m_Frag_indexCollection.at(i)){
//	 if(tmp_itid_mgen == m_Frag_indexCollection.at(i)){
//	 
//	 if (debug_multi > 0) cout<<" It hit associated to part "<< i << " That is a:: "<< m_Frag_hitCollection.at(i)->FlukaID() <<"  and has charge, mass:: "<< m_Frag_hitCollection.at(i)->Chg()<<" "<<m_Frag_hitCollection.at(i)->Mass()<<" "<<endl; 
//	 
//	 nIT++;
//	 
//       }
//     }
//         
//     for(int z=0; z<m_MSD_hitCollection.size(); z++){
//       // int tmp_msdid = m_MSD_hitCollection.at(z)->GetMCid()-1;
//        int tmp_msdid_mgen = m_MSD_hitCollection.at(z)->m_genPartID;
//       //       int tmp_msdid_mgen = m_MSD_hitCollection.at(z)->GetGenPartID();
//   	if (tmp_msdid_mgen == m_Frag_indexCollection.at(i))
//   	  {
//	    
//	    if (debug_multi > 0) cout<<" MSD hit associated to part "<< i << " That is a:: "<< m_Frag_hitCollection.at(i)->FlukaID() <<"  and has charge, mass:: "<< m_Frag_hitCollection.at(i)->Chg()<<" "<<m_Frag_hitCollection.at(i)->Mass()<<" "<<endl; 
//	    
//	    nMSD++;
//   	  }
//     }
//
//     
//     m_Plotter->SetMultiTrackHitInfo( "HitInfo", nVT, nIT, nMSD);
//     
//     int nTrackingHit = nVT + nIT + nMSD;
//     //     if (debug_multi > 0)   cout << " Nhit per track  " <<  nVT + nIT + nMSD << endl;
//
//     //  cout << " Nhit per track  " <<  nVT << " + " << nIT << " + " << nMSD << endl;
//     
//     
//     if(nVT>0 && nIT>0 && nMSD>0) tottrackwithatleastonehitperdet++;
//     
//     if (nTrackingHit == 9){
//       
//       tottrackwith9hits++;
//       m_Trcks9hits.push_back(m_Frag_hitCollection.at(i));
//       m_Trcks9index.push_back(i);
//     } 
//   }
//
//   if (debug_multi > 0){
//     cout << " m_Trcks9hits  size " <<  m_Trcks9hits.size() << endl;
//     cout << " m_Trcks9index size " <<  m_Trcks9index.size() << endl;
//   }
//  
//  
//      // cout << " m_VT_hitCollection.size()  " << m_VT_hitCollection.size()  << endl;
//   for (int i =0; i< m_Trcks9index.size(); i++) 
//     {
//       // cout << "index  "  << i << "  charge   " << m_Frag_hitCollection.at(m_Trcks9index.at(i))->Chg() <<endl;
//       //
//
//       // cout << "  charge  m_Trcks9hits   " << m_Trcks9hits.at(i)->Chg() <<endl;
//       // cout << "  mass  m_Trcks9hits     " << m_Trcks9hits.at(i)->Mass() <<endl;
//       
//       m_VT_Trackpos.clear();
//       m_IT_Trackpos.clear();
//       m_MSD_Trackpos.clear();
//       // cout << " TRACK TRACK TRACK "  << endl; 
//       
//
//       
//       for(int j=0; j<m_VT_hitCollection.size(); j++){
//	 if ( m_Trcks9index.at(i) == m_VT_hitCollection.at(j)->m_genPartID)
//	   { 
//	     
//	     TAVTntuHit* p_hit = m_VT_hitCollection.at(j);
//	     
//	     // get true  coord
//	     TVector3 vtpos = p_hit->GetMCPosition_footFrame();
//
//	     // m_VT_Trackpos.push_back(vtpos);
//	     
//	     // get pixel coord
//	     TVector3 vtpixelPos = m_VT_geo->GetPixelPos_footFrame( p_hit->GetLayer(), p_hit->GetPixelColumn(), p_hit->GetPixelLine() );
//	     
//	     //m_VT_Trackpos.push_back(vtpixelPos);
//	     
//
//	     //	     TVector3 vtpixelPos_clus = p_hit->GetPixelPosition_footFrame();
//
//	     
//	     m_VT_Trackpos.push_back(vtpixelPos);
//	     
//	     // control plot histograms for VT
//	        m_Plotter->SetMultiTrackPlots("VT_TRACK9HITS", &vtpos, &vtpixelPos);
//	     
//	     
//	   }
//       }
//       
//       for(int k=0; k<m_IT_hitCollection.size(); k++){
//	 if ( m_Trcks9index.at(i) == m_IT_hitCollection.at(k)->m_genPartID)
//	   { 
//	    
//	     TAITntuHit* p_hit = m_IT_hitCollection.at(k);
//	     
//	     // get true  coord
//	     TVector3 itpos = p_hit->GetMCPosition_footFrame();
//
//	     // get pixel coord
//	     TVector3  itpixelPos = m_IT_geo->GetPixelPos_footFrame( p_hit->GetSensorID(), p_hit->GetPixelColumn(), p_hit->GetPixelLine() );
//
//	     //m_IT_Trackpos.push_back(itpos);
//	     // m_IT_Trackpos.push_back(itpixelPos);
//	     
//	     
//	     //TVector3 itpixelPos_clus = p_hit->GetPixelPosition_footFrame();
//	     
//	     
//	      m_IT_Trackpos.push_back(itpixelPos);
//
//
//
//	      
//	      m_Plotter->SetMultiTrackPlots("IT_TRACK9HITS", &itpos, &itpixelPos);
// 
//	   }
//	 
//       }
//       
//       for(int z=0; z<m_MSD_hitCollection.size(); z++){
//	 if (m_Trcks9index.at(i) == m_MSD_hitCollection.at(z)->m_genPartID)
//	   {
//	     
//	     
////	     TAMSDntuHit* p_hit = m_MSD_hitCollection.at(z);
//
//	     // get true  coord
////	     TVector3 msdpos = p_hit->GetMCPosition_Global();
////	     m_MSD_Trackpos.push_back(msdpos);
//	     
////	     m_Plotter->SetMultiTrackPlots("MSD_TRACK9HITS", &msdpos, &msdpos); //only true MC info for now
//	     
//	    
//	   }
//	 
//       }
//
//       if (debug_multi == -1){
//	 cout <<  " HITS per track" << endl;
//	 cout <<  " m_VT_Trackpos.size() " << m_VT_Trackpos.size() << endl;
//	 cout <<  " m_IT_Trackpos.size() " << m_IT_Trackpos.size() << endl;
//	 cout <<  " m_MSD_Trackpos.size() " << m_MSD_Trackpos.size() << endl;
//       }
//       
//     
//       m_DetTrackpos["VT"] = m_VT_Trackpos;
//       m_DetTrackpos["IT"] = m_IT_Trackpos;
//       m_DetTrackpos["MSD"] = m_MSD_Trackpos;
//       
//	   
//       
//       for ( map< string, vector <TVector3>>::iterator it = m_DetTrackpos.begin(); it != m_DetTrackpos.end(); it++ ) {       
//	 
//       	 if (debug_multi > 1) cout << " (*it).first  " << (*it).first << "   (*it).second.size()   " << (*it).second.size() <<endl;
//       	 if ((*it).first == "VT" && (*it).second.size()==4){
//	   
//       	   for (int i =0; i<(*it).second.size(); i++){
//	     
//	     if(i==0) firstVThitlist.push_back((*it).second.at(i));
//	     else if(i==1)secondVThitlist.push_back((*it).second.at(i));
//	     else if(i==2)thirdVThitlist.push_back((*it).second.at(i));
//	     else if(i==3)lastVThitlist.push_back((*it).second.at(i));
//	   }
//	 }  
//	 else if ((*it).first == "IT" && (*it).second.size()==2){
//
//	   for (int j =0; j<(*it).second.size(); j++){
//	     
//	     if(j==0) firstIThitlist.push_back((*it).second.at(j));
//	     else if(j==1)secondIThitlist.push_back((*it).second.at(j));
//	     
//	     
//	   }
//	 }
//	 else if ((*it).first == "MSD" && (*it).second.size()==3){
//
//	   for (int z =0; z<(*it).second.size(); z++){
//	     if(z==0) firstMSDhitlist.push_back((*it).second.at(z));
//	     else if(z==1)secondMSDhitlist.push_back((*it).second.at(z));
//	     else if(z==2)thirdMSDhitlist.push_back((*it).second.at(z));
//	     
//	   }
//	   
//	 }
//	 
//       }
//       
//     }
//   
//   
//   if (firstVThitlist.size() == 2)
//     {
//       //     cout << " EVENT with 2 tracks  " << endl;
//       
//       TwoTrackEvent = true;
//     }
//   EfficiencyChi2Fit();
//   
//   if (doMinDist)  EfficiencyMinimumDistance();
//
//   if ( doVertex && TwoTrackEvent){
//     //  cout << "Z x   " << endl;
//     Double_t zx_Vertex = VertexZ(m_p0_zx, m_p1_zx);
//     //cout << "Z y " << endl;   
//     Double_t zy_Vertex = VertexZ(m_p0_zy, m_p1_zy);
//
//     // vector<double> v(m_p0_zx.size());
//     // v = {zx_Vertex, zy_Vertex};
//     // double Zmean = TMath::Mean(m_p0_zx.size(), &v[0]);
//   
//     // cout << "Zmean   " << Zmean << endl;
//
//     m_Plotter->SetVertexCoord("Vertex", zx_Vertex, zy_Vertex);
//
//
//   }
//      
//   
   
   
   
   
}

void MultiTrackCheck::Finalize(){
  
  
  cout << "Tottrackwith9hits/Tottrack  " << tottrackwith9hits << " /  " << tottrack << "   " << float(tottrackwith9hits)/float(tottrack)  << endl;
  cout << "Tottrackwith9hits/Tottrackwithatleastonehitperdet  " <<  tottrackwith9hits << " / " << tottrackwithatleastonehitperdet << "   "<< float(tottrackwith9hits)/float(tottrackwithatleastonehitperdet)  << endl;
  
  cout << "Efficiency one chi2 < 5.0 and the other < 10.0   " << float(Chi2goodwithtruetrack)/float(Chi2ofallgoodtrack) << endl;

  cout << "Efficiency of total minimum distance method " << float(distanceok)/float(distancetot)  << endl;


}

void MultiTrackCheck::EfficiencyChi2Fit(){
  Int_t n1 = 4;
  Double_t *x1 = new Double_t[n1];
  Double_t *y1 = new Double_t[n1];
  Double_t *z1 = new Double_t[n1];
  Double_t *e1 = new Double_t[n1];
  
  for (unsigned int i=0; i<firstVThitlist.size(); i++){   
    
    x1[0]=firstVThitlist.at(i).x();
    y1[0]=firstVThitlist.at(i).y();
    z1[0]=firstVThitlist.at(i).z();
    e1[0]=0.0006;   //Manually set
    
    for (unsigned int j=0; j<secondVThitlist.size(); j++){
      x1[1]=secondVThitlist.at(j).x();
      y1[1]=secondVThitlist.at(j).y();
      z1[1]=secondVThitlist.at(j).z();
      e1[1]=0.0006;   //Manually set
      
      for (unsigned int z=0; z<thirdVThitlist.size(); z++){
	
	x1[2]=thirdVThitlist.at(z).x();
	y1[2]=thirdVThitlist.at(z).y();
	z1[2]=thirdVThitlist.at(z).z();
	e1[2]=0.0006;   //Manually set
	
	for (unsigned int k=0; k<lastVThitlist.size(); k++){
	  
	  x1[3]=lastVThitlist.at(k).x();
	  y1[3]=lastVThitlist.at(k).y();
	  z1[3]=lastVThitlist.at(k).z();
	  e1[3]=0.0006;   //Manually set
	  
	  TGraphErrors *gre3 = new TGraphErrors(n1, z1, x1, 0, e1);
	  TGraphErrors *gre4 = new TGraphErrors(n1, z1, y1, 0, e1);
	  gre3->Fit("pol1","q");
	  gre4->Fit("pol1","q");
	  
	  auto f3 = gre3->GetFunction("pol1");
	  auto f4 = gre4->GetFunction("pol1");
	  
	  Double_t chi2all_1 = f3->GetChisquare();
	  Double_t chi2all_2 = f4->GetChisquare();
	  

	  
	  if ((i==j)&& (j==z)&& (z==k)){
	    
	    string fragtype = build_string(m_Trcks9hits.at(i)->GetCharge()) + "__" + build_string(round(m_Trcks9hits.at(i)->GetMass()/UMA));

	    m_Plotter->SetMultiTrackChi2PerFrag("VT",  chi2all_1, chi2all_2, fragtype);
	    Truetrack = true;
	    
	    

	  }else{
	    Truetrack = false;
	  }
	  
	  if (Truetrack && TwoTrackEvent) {

	    Double_t p0_zx = f3->GetParameter(0);
	    Double_t p1_zx = f3->GetParameter(1);

	    
	    m_p0_zx.push_back(p0_zx);
	    m_p1_zx.push_back(p1_zx);
	    
	    Double_t p0_zy = f4->GetParameter(0);
	    Double_t p1_zy = f4->GetParameter(1);
	    	    
	    
	    m_p0_zy.push_back(p0_zy); 
	    m_p1_zy.push_back(p1_zy);

	    

	  }

	  m_Plotter->SetMultiTrackChi2("VT",  chi2all_1, chi2all_2, Truetrack);

	  if (((chi2all_1< 5.0) && (chi2all_2 < 10.0)) || ((chi2all_1< 10.0) && (chi2all_2 < 5.0)) ){
	    Chi2ofallgoodtrack++;
	    
	    if (Truetrack)Chi2goodwithtruetrack++;
	  }
	  
	  
	  gre3->Delete();
	  gre4->Delete();
	  
	  
	}
	
      }
      
      
    }
    
  }
}



void MultiTrackCheck::EfficiencyMinimumDistance(){

  for (unsigned int i=0; i<firstVThitlist.size(); i++){
    double distance = 2.0;
    
    for (unsigned int j=0; j<secondVThitlist.size(); j++){
      
      TVector3 difference21 = secondVThitlist.at(j) - firstVThitlist.at(i); 
      double tmp_magnitude21 = difference21.Mag();
      
      for (unsigned int z=0; z<thirdVThitlist.size(); z++){
	
	TVector3 difference32 = thirdVThitlist.at(z) - secondVThitlist.at(j); 
	double tmp_magnitude32 = difference32.Mag();
	
	for (unsigned int k=0; k<lastVThitlist.size(); k++){
	  
     	     TVector3 difference43 = lastVThitlist.at(k) - thirdVThitlist.at(z); 
     	     double tmp_magnitude43 = difference43.Mag();
	     
	     
     	     double tmp_magnitude =  tmp_magnitude21 +  tmp_magnitude32 +  tmp_magnitude43;
	     
	 	     
     	     if (tmp_magnitude <= distance){
	       
     	       distance = tmp_magnitude;
	       
     	       pos1 = i;
     	       pos2 = j;
     	       pos3 = z;
     	       pos4 = k;
     	     }
     	     else{
     	       continue;}
	     
	}
      }
	 
    }
    
    if ((pos1 == pos2) && (pos2 == pos3) && (pos3 == pos4) ){
      //	       cout <<  " i, j, z, k :  " << pos1 <<"  "<< pos2 <<"  "<< pos3 << "  "<< pos4 <<endl;
      distanceok++;
    }
    
  }
  
  distancetot += firstVThitlist.size();
}


 double MultiTrackCheck::VertexZ( vector<double> m_p0,  vector<double> m_p1){ 

	    // cout << " m_p0.at(0) " << m_p0.at(0) << endl;
	    // cout << " m_p0.at(1) " << m_p0.at(1) << endl;

	    // cout << " m_p1.at(0) " << m_p1.at(0) << endl;
	    // cout << " m_p1.at(1) " << m_p1.at(1) << endl;

	    double b = m_p0.at(0) - m_p0.at(1);
	    double a = m_p1.at(0) - m_p1.at(1);

	    double zcoord = 999;
	    
	    if (a!=0) zcoord = -(b/a);
	    

	    

	    return zcoord;
 }
