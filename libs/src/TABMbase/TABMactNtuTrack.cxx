/*!
  \file
  \version $Id: TABMactNtuTrack.cxx,v 1.12 2003/07/08 18:58:35 mueller Exp $
  \brief   Implementation of TABMactNtuTrack.
*/

#include "TABMactNtuTrack.hxx"

using namespace std;
/*!
  \class TABMactNtuTrack TABMactNtuTrack.hxx "TABMactNtuTrack.hxx"
  \brief Track builder for Beam Monitor. **
*/

ClassImp(TABMactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuTrack::TABMactNtuTrack(const char* name,
                                 TAGdataDsc* dscntutrk,
                                 TAGdataDsc* dscnturaw,
                                 TAGparaDsc* dscbmgeo,
                                 TAGparaDsc* dscbmcon,
                                 TAGparaDsc* dsctggeo)
  : TAGaction(name, "TABMactNtuTrack - Track finder for BM"),
    fpNtuTrk(dscntutrk),
    fpNtuHit(dscnturaw),
    fpBMGeo(dscbmgeo),
    fpBMCon(dscbmcon),
    fpTGgeo(dsctggeo)
{
  if (FootDebugLevel(1))
   cout<<"TABMactNtuTrack::default constructor::Creating the Beam Monitor Track ntuplizer"<<endl;
  AddDataOut(fpNtuTrk, "TABMntuTrack");
  AddDataIn(fpNtuHit,  "TABMntuRaw");
  AddPara(fpBMGeo,  "TABMparGeo");
  AddPara(fpBMCon,  "TABMparCon");

  p_bmcon = (TABMparCon*) fpBMCon->Object();
  p_bmgeo = (TABMparGeo*) fpBMGeo->Object();
  p_tggeo = (TAGparGeo*)  fpTGgeo->Object();
   
  TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
  TVector3 tgCenter = geoTrafo->GetTGCenter();
  TVector3 bmCenter = geoTrafo->GetBMCenter();
  
  Float_t tgThick = p_tggeo->GetTargetPar().Size[2];
  
  TVector3 targetPos(tgCenter[0], tgCenter[1], -bmCenter[2]+tgCenter[2]+tgThick/2.);
  p_bmgeo->SetTarget(targetPos);
  
  tmp_trackTr=new TABMntuTrackTr();

}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuTrack::~TABMactNtuTrack()
{ 
delete tmp_trackTr;    
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TABMactNtuTrack::CreateHistogram()
{
   DeleteHistogram();
   
   fpHisMap = new TH2F("BM_Track_bmMap","BM - Position of the track at target center", 250, -3., 3.,250 , -3, 3);
   AddHistogram(fpHisMap);
   fpHisMylar12d = new TH2F("BM_Track_bmmylar1_2d","BM - Position of the track on the BM center plane", 500, -3., 3.,500 , -3., 3.);
   AddHistogram(fpHisMylar12d);   
   fpHisMylar22d = new TH2F("BM_Track_bmmylar2_2d","BM - Position of the track on the BM center plane", 500, -3., 3.,500 , -3., 3.);
   AddHistogram(fpHisMylar22d);   
   fpResTot = new TH2F("BM_Track_bm_residual_tot","Residual vs Rdrift; Residual [cm]; Measured rdrift [cm]", 6000, -0.3, 0.3,250 , 0., 1.);
   AddHistogram(fpResTot);  
   fpHisTrackStatus = new TH1I("BM_Track_track_status","Track status; -3=prefitstatuscut -2=maxhitcut__-1=minhitcut__0=ok__1/2=firedV/Uplane__3=hitrejected__4=noconv__5=chi2cut; Events", 15, -5, 10);
   AddHistogram(fpHisTrackStatus);  
   fpHisNhitTrack = new TH1I("BM_Trak_nhitsxtrack","number of hits x track; ; Events", 30, 0, 30);
   AddHistogram(fpHisNhitTrack);  
   fpHisPrefitStatus = new TH1I("BM_Track_prefit_status","bm prefit status; ; Events", 6, -3, 3);
   AddHistogram(fpHisPrefitStatus);  
   fpHisNite = new TH1I("BM_Track_nite","Number of iteration of the tracking method; Number of iteration; Events", p_bmcon->GetNumIte(), 0,p_bmcon->GetNumIte() );
   AddHistogram(fpHisNite);  
   fpHisChi2Red = new TH1F("BM_Track_chi2red","chi2red", 1000, 0., 100.);
   AddHistogram(fpHisChi2Red);   
   fpHisTrackEff = new TH1F("BM_Track_FitEff","Efficiency with pivot-probe method on FITTED tracks", 110, 0., 1.1);
   AddHistogram(fpHisTrackEff);   

   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuTrack::Action()
{  
  TABMntuTrack* p_ntutrk = (TABMntuTrack*) fpNtuTrk->Object();
  p_nturaw = (TABMntuRaw*)   fpNtuHit->Object();
  
  p_ntutrk->Clear();//maybe useless? 

  Int_t i_nhit = p_nturaw->GetHitsN();

   //NB.: If the preselection reject the event no track will be saved     
  
  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack::Action::start"<<endl;  

  //parameters  
  Double_t tmp_double, res;
  Bool_t converged;
  TABMntuTrackTr best_trackTr;
  Int_t hit_view, tmp_int, tmp_cellx=1, tmp_celly=1, tracknum=1;
  TMatrixDSym hitCov(7);
  TVectorD hitCoords(7);
  vector<vector<Int_t>> hitxplane(p_bmgeo->GetLayersN()*2); //number of hit for every bm plane (plane should be 12 in BM)
  Int_t firedPlane=p_bmgeo->GetLayersN()*2; //number of plane fired
  Int_t firedUview=p_bmgeo->GetLayersN();//number of U view plane fired
  Int_t firedVview=p_bmgeo->GetLayersN();//number of V view plane fired
  Int_t firedSingleUview=0;//number of U view plane fired with only 1 hit
  Int_t firedSingleVview=0;//number of V view plane fired with only 1 hit
  Bool_t tmp_bool;  
  vector<Int_t> singlehittrack;  

  //COUNTER FOR NUMBER OF POSSIBLE TRACKS:
  for(Int_t i_h = 0; i_h < i_nhit; i_h++) {
    p_hit = p_nturaw->Hit(i_h);
    if(FootDebugLevel(3))
      cout<<"hit="<<i_h<<" plane="<<p_hit->Plane()<<"  view="<<p_hit->View()<<"  cell="<<p_hit->Cell()<<"  wireplane="<<p_bmgeo->GetWirePlane(p_hit->Plane(),p_hit->View())<<"  rdrift="<<p_hit->Dist()<<"  isfake="<<p_hit->GetIsFake()<<endl;
     //~ if (p_bmgeo->GetWirePlane(p_hit->Plane(),p_hit->View()) >= 0)
      hitxplane.at(p_bmgeo->GetWirePlane(p_hit->Plane(),p_hit->View())).push_back(i_h);
  }
  //calculate number of possible tracks (tracknum), the number of the plane with at least one hit for each view (firedUview/firedVview) and for both the views (firedPlane)
  for(Int_t j = 0; j < hitxplane.size(); j++) {  
    if(hitxplane.at(j).size()!=0){
      tracknum*=hitxplane.at(j).size();
      if(hitxplane.at(j).size()==1){
        singlehittrack.push_back(hitxplane.at(j).at(0));
        if(j%2==0)
          firedSingleUview++;
        else
          firedSingleVview++;
      }
    }else{
      firedPlane--;
      if(j%2==0)
        firedUview--;
      else
        firedVview--;   
    }
  }
  
  Int_t rejhit=i_nhit-firedPlane;
  if(rejhit<0)
    cout<<"ERROR!!!! in TABMactNtuTrack::rejhit<0   i_nhit="<<i_nhit<<"  firedPlane="<<firedPlane<<endl;//provv
  
  if(FootDebugLevel(1))
    cout<<"TABMactNtuTrack:: tracknum="<<tracknum<<"  firedPlane="<<firedPlane<<"  firedUview="<<firedUview<<"  firedVview="<<firedVview<<endl;    
  
  //*************APPLY SOME CUTS************
  if(firedUview<p_bmcon->GetPlanehitcut() || firedVview<p_bmcon->GetPlanehitcut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::WARNING!!::no possible track!!: firedUview="<<firedUview<<"  firedVview="<<firedVview<<"   planehitcut="<<p_bmcon->GetPlanehitcut()<<endl;
    if(firedUview<p_bmcon->GetPlanehitcut())
      p_ntutrk->GetTrackStatus()=1;
    else
      p_ntutrk->GetTrackStatus()=2;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());  
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else if(i_nhit>=p_bmcon->GetMaxnhit_cut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::WARNING!!::the number of hits is too high:  number of hit="<<i_nhit<<"  Maxhitcut="<<p_bmcon->GetMaxnhit_cut()<<endl;
    p_ntutrk->GetTrackStatus()=-2;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());  
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else if(i_nhit<=p_bmcon->GetMinnhit_cut()){
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::WARNING!!::the number of hits is too low:  number of hit="<<i_nhit<<"  Minhitcut="<<p_bmcon->GetMinnhit_cut()<<endl;
    p_ntutrk->GetTrackStatus()=-1;
    if(ValidHistogram())
      fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());    
    fpNtuTrk->SetBit(kValid);
    return kTRUE;
  }else
    p_ntutrk->GetTrackStatus()=-1000;
  
  //print hitxplane
  if(FootDebugLevel(2)){  
    cout<<"TABMactNtuTrack::print hitxplane"<<endl;  
    Print_matrix(hitxplane);    
  }
  
  vector<vector<Int_t>> hitxtrack; 
  Int_t best_index; //the position in hitxtrack matrix of the best tracktr
  Int_t prunedhit;//element to be pruned from the track 
  Int_t prefit_status=-2;
//*************************************************NEW LEAST CHI2 TRACKING*************************************
 if(p_bmcon->GetFitterIndex()!=0) {
    
    //prefit only on the single hits
    if(firedSingleUview>2 && firedSingleVview>2){
      tmp_trackTr->Clean();
      EstimateFIRSTTrackPar(singlehittrack, firedSingleUview, firedSingleVview);
      Chi2Fit(singlehittrack, prunedhit, converged);
      if((firedSingleUview+firedSingleVview)<(firedUview+firedVview)){  //tracks with more hit on the same view and plane
        SortFirstDoubleHits(hitxplane, hitxtrack);
        prefit_status=1;
        if(p_bmcon->GetPrefitEnable()==2){
          p_ntutrk->GetTrackStatus()=-3;
          if(ValidHistogram())
            fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());  
          fpNtuTrk->SetBit(kValid);
          return kTRUE;
        }
      }else{
        hitxtrack.push_back(singlehittrack);//no double hits
        prefit_status=0;
      }
    }else{
      prefit_status=-1;
      ChargeAllTracks(hitxtrack,hitxplane, tracknum, firedPlane);
      if(p_bmcon->GetPrefitEnable()==2){
        p_ntutrk->GetTrackStatus()=-3;
        if(ValidHistogram())
          fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());  
        fpNtuTrk->SetBit(kValid);
        return kTRUE;
      }
    }
    
    if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack:: End of prefit_status="<<prefit_status<<endl;
        
    best_index=0;
    
    for(Int_t i=0;i<hitxtrack.size();i++){
     if(FootDebugLevel(2))
      cout<<"TABMactNtuTrack:: loop on hitxtrack: i="<<i<<endl; 
  
      if(prefit_status!=0 || (prefit_status==0 && i>0)){
        tmp_trackTr->Clean();
        EstimateFIRSTTrackPar(hitxtrack.at(i), firedUview, firedVview);
        Chi2Fit(hitxtrack.at(i),prunedhit, converged);
      }else{
        best_trackTr=*tmp_trackTr;
        best_index=i;        
      }
      
      if((tmp_trackTr->GetChi2Red() < best_trackTr.GetChi2Red()) || i==0){
        best_trackTr=*tmp_trackTr;
        best_index=i;
      }
      
      if(prunedhit>=0){//charge prune hits
        if((i_nhit-hitxtrack.at(i).size()-1)>p_bmcon->GetRejmaxcut())
          ChargePrunedTrack(prunedhit, firedUview, firedVview, hitxtrack, i);
        else if(FootDebugLevel(2))
          cout<<"TABMactNtuTrack:: prunedhit cannot added to hitxtrack due to Rejmaxcut:  hitxtrack index="<<i<<"  i_nhit="<<i_nhit<<"  hitxtrack.at(i).size()="<<hitxtrack.at(i).size()<<"  prunedhit="<<prunedhit<<"  rejhitmax="<<p_bmcon->GetRejmaxcut()<<endl;
      }
    }//end of the loop on hitxtrack
    best_trackTr.SetPrefitStatus(prefit_status);  //save and update the best_track
  }

  if(FootDebugLevel(1))  
    cout<<"TABMactNtuTrack:end of tracking"<<endl;
  
  if(best_trackTr.GetNhit()!=0) {
    TABMntuTrackTr* trk = new((*(p_ntutrk->GetListOfTracks()))[p_ntutrk->GetTracksN()]) TABMntuTrackTr(best_trackTr);
    p_ntutrk->GetTrackStatus()= (best_trackTr.GetChi2Red()>=p_bmcon->GetChi2Redcut())? 5:0;
    Int_t hit_fittedplane[12]={0};
    //flag the hit of the best track
    for(Int_t i=0;i<hitxtrack.at(best_index).size();i++){
      p_hit = p_nturaw->Hit(hitxtrack.at(best_index).at(i));    
      p_hit->SetIsSelected(true);
      if (ValidHistogram())  
        fpResTot->Fill(p_hit->GetResidual(),p_hit->Dist());    
      if(p_hit->GetIsSelected() && p_ntutrk->GetTrackStatus()==0){
        if(p_hit->View()==0)
          hit_fittedplane[p_hit->Plane()]++;
        else
          hit_fittedplane[p_hit->Plane()+6]++;      
      }
    }//end of loop on hits
    
    //evaluate eff_fittedplane
    if(p_ntutrk->GetTrackStatus()==0){
      Int_t eff_fittedplane_probe=0, eff_fittedplane_pivot=0; 
      //view==0
      if(hit_fittedplane[0]>0 && hit_fittedplane[2]>0 && hit_fittedplane[4]>0){
        eff_fittedplane_pivot++;
        if(hit_fittedplane[1]>0 && hit_fittedplane[3]>0)
          eff_fittedplane_probe++;
      }
      if(hit_fittedplane[1]>0 && hit_fittedplane[3]>0 && hit_fittedplane[5]>0){
        eff_fittedplane_pivot++;
        if(hit_fittedplane[2]>0 && hit_fittedplane[4]>0)
          eff_fittedplane_probe++;
      }
      //view==1
      if(hit_fittedplane[6]>0 && hit_fittedplane[8]>0 && hit_fittedplane[10]>0){
        eff_fittedplane_pivot++;
        if(hit_fittedplane[7]>0 && hit_fittedplane[9]>0)
          eff_fittedplane_probe++;
      }
      if(hit_fittedplane[7]>0 && hit_fittedplane[9]>0 && hit_fittedplane[11]>0){
        eff_fittedplane_pivot++;
        if(hit_fittedplane[8]>0 && hit_fittedplane[10]>0)
          eff_fittedplane_probe++;
      }
    trk->SetEffFittedPlane( (eff_fittedplane_pivot==0) ?  -1 : (Double_t) eff_fittedplane_probe/eff_fittedplane_pivot);
    }
    
    if (ValidHistogram()){
       //~ fpHisR02d->Fill(trk->GetR0()[0],trk->GetR0()[1]);
       TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
       Float_t posZ = geoTrafo->FromGlobalToBMLocal(TVector3(0,0,0)).Z();
       TVector3 pos = trk->PointAtLocalZ(posZ);
       fpHisMap->Fill(pos[0], pos[1]);
       pos=trk->PointAtLocalZ(p_bmgeo->GetMylar1().Z());
       fpHisMylar12d->Fill(pos[0], pos[1]);
       pos=trk->PointAtLocalZ(p_bmgeo->GetMylar2().Z());
       fpHisMylar22d->Fill(pos[0], pos[1]);
       fpHisChi2Red->Fill(trk->GetChi2Red());
       fpHisNhitTrack->Fill(trk->GetNhit());
       fpHisNite->Fill(trk->GetNite());
       fpHisPrefitStatus->Fill(prefit_status);
       if(trk->GetEffFittedPlane()>=0)
         fpHisTrackEff->Fill(trk->GetEffFittedPlane());
     }
  }else if(converged==false)
    p_ntutrk->GetTrackStatus()=4;
  else
    cout<<"ERROR in TABMactNtuTrack :: track converged lost!"<<endl;
  
  if(ValidHistogram())
    fpHisTrackStatus->Fill(p_ntutrk->GetTrackStatus());  

  fpNtuTrk->SetBit(kValid);

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::Action()::done"<<endl;

  return kTRUE;
}



void TABMactNtuTrack::ChargePrunedTrack(const Int_t prunedhit, Int_t &firedUview, Int_t &firedVview, vector< vector<Int_t> > &hitxtrack, const Int_t index){

  //check if the pruned track can pass the plane cuts
  p_hit=p_nturaw->Hit(hitxtrack.at(index).at(prunedhit));
  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::a track has to be pruned:  prunedhit="<<prunedhit<<"  hitxtrack.at(index)[prunedhit]="<<hitxtrack.at(index).at(prunedhit)<<endl<<"hit_view of the pruned hit="<<p_hit->View()<<"firedUview="<<firedUview<<"  firedVview="<<firedVview<<"  planehitcut="<<p_bmcon->GetPlanehitcut()<<"  new number_of_hit="<<hitxtrack.at(index).size()-1<<"  minhitcut="<<p_bmcon->GetMinnhit_cut()<<endl;          
  //if it can pass, add the pruned track to hitxtrack         
  if(((p_hit->View()==0 && (firedUview-1)>=p_bmcon->GetPlanehitcut()) || (p_hit->View()==1 && (firedVview-1)>=p_bmcon->GetPlanehitcut())) && (hitxtrack.at(index).size()-1)>=p_bmcon->GetMinnhit_cut()){
    vector<Int_t> tmp_vec_int=hitxtrack.at(index);
    if(FootDebugLevel(2)){
      cout<<"Before the pruning:"<<endl;
      for(Int_t kk=0;kk<tmp_vec_int.size();kk++)
        cout<<tmp_vec_int.at(kk)<<" ";
      cout<<endl;  
    }
    tmp_vec_int.erase(tmp_vec_int.begin()+prunedhit);
    if(FootDebugLevel(2)){
      cout<<"After the pruning:"<<endl;
      for(Int_t kk=0;kk<tmp_vec_int.size();kk++)
        cout<<tmp_vec_int.at(kk)<<" ";
      cout<<endl;
    }
    hitxtrack.push_back(tmp_vec_int);
  }else if(FootDebugLevel(2))
    cout<<"this pruning cannot be made for the planehitcut or the minnhitcut"<<endl;
  return;
}  
  
  
//to print matrix
void TABMactNtuTrack::Print_matrix(vector<vector<int>>& vec){
  for(int i=0;i<vec.size();i++){
    for(int j=0;j<vec.at(i).size();j++)
      cout<<vec.at(i).at(j)<<" ";
    cout<<endl;
    }  
  cout<<endl;  
return;  
}


void TABMactNtuTrack::ChargeAllTracks(vector< vector<Int_t> > &hitxtrack,vector< vector<Int_t> > &hitxplane, Int_t tracknum, Int_t firedPlane) {
  hitxtrack.clear();
  hitxtrack.resize(tracknum);
  for(Int_t j=0; j<tracknum; j++)
    hitxtrack.at(j).resize(firedPlane);  
  
  Int_t block=1, planeindex=0, shift, tmp_int;
  for(Int_t i=0;i<p_bmgeo->GetLayersN()*2;i++){
    if(hitxplane.at(i).size()>0){
      tmp_int=0;
      shift=0;
      block*=hitxplane.at(i).size();
      while(tmp_int<tracknum){
        for(Int_t k=0;k<hitxplane.at(i).size();k++){
          for(Int_t j=tmp_int;j<tracknum/block+shift;j++){
            hitxtrack.at(tmp_int).at(planeindex)=hitxplane.at(i).at(k);
            tmp_int++;
          }
          shift=tmp_int;
        }
      }
      planeindex++;
    }
  }
  
  return;
}     




Int_t TABMactNtuTrack::EstimateFIRSTTrackPar(vector<Int_t> &singlehittrack, Int_t &firedUview, Int_t &firedVview){
  
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar::begin of EstimateFIRSTTrackPar"<<endl;
  
  //charge hits and vectors
  vector<Double_t> Ycentro_U, Xcentro_V, Zcentro_U, Zcentro_V, R_U, R_V;
  firedUview=0;
  firedVview=0;
  for(Int_t i_h = 0; i_h <singlehittrack.size() ; i_h++) {
    p_hit = p_nturaw->Hit(singlehittrack.at(i_h));

     if(p_hit->View()==0){//here the inversion in p_bmgeo->Z and Ycentro is right
        Ycentro_U.push_back(p_bmgeo->GetWireZ(p_bmgeo->GetSenseId(p_hit->Cell()),p_hit->Plane(),p_hit->View()));
        Zcentro_U.push_back(p_bmgeo->GetWireY(p_bmgeo->GetSenseId(p_hit->Cell()),p_hit->Plane(),p_hit->View()));
      R_U.push_back(p_hit->Dist());
      firedUview++;
    }else{
      Xcentro_V.push_back(p_bmgeo->GetWireZ(p_bmgeo->GetSenseId(p_hit->Cell()),p_hit->Plane(),p_hit->View()));
      Zcentro_V.push_back(p_bmgeo->GetWireX(p_bmgeo->GetSenseId(p_hit->Cell()),p_hit->Plane(),p_hit->View()));
      R_V.push_back(p_hit->Dist());
      firedVview++;
    }
  }//end of charging Xcentro_V etc,  
  
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar:: end of charging Xcentro_V etc."<<endl;
  
  // estimate track line in the U view ( namely y-z plane)  start to find the 4 tangent of the 2 farer hits
  Int_t ii_max=-1, jj_max=-1;
  Double_t dist_max=0., dist_curr;
  TG_STRUCT tan_max;
  for (Int_t ii = 1; ii<firedUview;ii++){
    for(Int_t jj=0;jj<ii;jj++ ){
      dist_curr = sqrt( (Ycentro_U.at(ii)-Ycentro_U.at(jj))*(Ycentro_U.at(ii)-Ycentro_U.at(jj)) + (Zcentro_U.at(ii)-Zcentro_U.at(jj))*(Zcentro_U.at(ii)-Zcentro_U.at(jj)));
      if(FootDebugLevel(4))
    	  cout <<"ii="<<ii<<" jj="<<jj<<" yc[ii]="<<Ycentro_U.at(ii)<<" zc[ii]="<<Zcentro_U.at(ii)<<" yc[jj]="<<Ycentro_U.at(jj)<<" ,zc[jj]="<<Zcentro_U.at(jj)<<endl;
      if(dist_curr>dist_max) {
        dist_max = dist_curr;
        ii_max = ii;
        jj_max = jj;
        tan_max = Circles2Tangents(Ycentro_U.at(ii), Zcentro_U.at(ii), R_U.at(ii), Ycentro_U.at(jj), Zcentro_U.at(jj), R_U.at(jj));
      }
    }
  }   
  if(FootDebugLevel(4)) { 
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar::more distant cells= "<<ii_max<<" "<<jj_max<<endl;
    for(Int_t hh=0;hh<4;hh++)
      cout<<" m= "<<tan_max.mm[hh]<<" q= "<<tan_max.qq[hh]<<endl;
  }
  Double_t min_sum_scarto=10000000., sum_scarto, distanza,p_U(0), q_U(0), p_V(0), q_V(0);   
  Int_t tmp_int;
  for(int hh=0;hh<4;hh++){
    sum_scarto = 0.;
    if(FootDebugLevel(4))  
      cout<<"tangent # "<<hh<<endl;
    for (int ii = 0; ii<firedUview;ii++){
      if((ii!=ii_max)&&(ii!=jj_max)){
        distanza = fabs(Zcentro_U.at(ii)-tan_max.mm[hh]*Ycentro_U.at(ii) - tan_max.qq[hh])/sqrt(1. + tan_max.mm[hh]*tan_max.mm[hh]);
        sum_scarto +=(distanza - R_U.at(ii))*(distanza - R_U.at(ii));
        if(FootDebugLevel(3))
          cout<<"plane="<<ii<<" distance="<<distanza<<"  R_U="<<R_U.at(ii)<<" distance - R_U.at(ii)="<<distanza - R_U.at(ii)<<"  sum_scarto="<<sum_scarto<<endl;
      }
    }
    if(sum_scarto<min_sum_scarto){
      min_sum_scarto = sum_scarto;
      p_U = tan_max.mm[hh];
      q_U = tan_max.qq[hh];
      tmp_int=hh;
    }
  }
  
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar:: estimated track line in the U view p_U="<<p_U<<"  q_U="<<q_U<<"  best tangent="<<tmp_int<<endl;   
  
  //  estimate track line in the V view ( namely x-z plane)  
  dist_max=0;
  ii_max=-1;
  jj_max=-1;
  /* start to find the 4 tangent of the 2 farest hits */
  for (int ii = 1; ii<firedVview;ii++){
    for(int jj=0;jj<ii;jj++ ){
      dist_curr = sqrt((Xcentro_V.at(ii)-Xcentro_V.at(jj))*(Xcentro_V.at(ii)-Xcentro_V.at(jj)) + (Zcentro_V[ii]-Zcentro_V[jj])*(Zcentro_V[ii]-Zcentro_V[jj]));
      if(FootDebugLevel(4))  
    	  cout <<"ii="<<ii<<"  ,jj="<<jj<<"  xV[ii]="<<Xcentro_V.at(ii)<<" zV[ii]="<<Zcentro_V[ii]<<"  xV[jj]="<<Xcentro_V.at(jj)<<"  zV[jj]="<<Zcentro_V[jj]<<endl;
      if(dist_curr>dist_max) {
        dist_max = dist_curr;
        ii_max = ii;
        jj_max = jj;
        tan_max = Circles2Tangents(Xcentro_V.at(ii),Zcentro_V[ii], R_V.at(ii), Xcentro_V.at(jj), Zcentro_V[jj], R_V.at(jj));
      }
    }
  }
  if(FootDebugLevel(4)) {
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar::more distant cells V view= "<<ii_max<<" "<<jj_max<<endl;
    for(int hh=0;hh<4;hh++)
      cout<<"m="<<tan_max.mm[hh]<<"  q="<<tan_max.qq[hh]<<endl;
  }
  min_sum_scarto = 10000000.;
  for(int hh=0;hh<4;hh++){
    sum_scarto = 0.;
    if(FootDebugLevel(4))   cout<<"tangent # "<<hh<<endl;
    for (int ii = 0; ii<firedVview;ii++){
      if((ii!=ii_max)&&(ii!=jj_max)){
        distanza = fabs(Zcentro_V[ii]-tan_max.mm[hh]*Xcentro_V.at(ii) - tan_max.qq[hh])/sqrt(1. + tan_max.mm[hh]*tan_max.mm[hh]);
        sum_scarto +=(distanza - R_V.at(ii))*(distanza - R_V.at(ii));
        if(FootDebugLevel(4)) {
          cout<<"plane= "<<ii<<" distance="<<distanza<<"  R_V="<<R_V.at(ii)<<"  (distance - R_V.at(ii))="<<(distanza - R_V.at(ii))<<"  sum_scarto="<<sum_scarto<<endl;
        }
      }
    }
    if(sum_scarto<min_sum_scarto){
      min_sum_scarto = sum_scarto;
      p_V = tan_max.mm[hh];
      q_V = tan_max.qq[hh];
      tmp_int=hh;
    }
  }  
  
  if(FootDebugLevel(4))
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar:: estimated track line in the V view, p_V="<<p_V<<"  q_V="<<q_V<<"  best tangent="<<tmp_int<<endl;  

  tmp_trackTr->SetNhit(singlehittrack.size());
  TVector3 tmp_tvector3(p_V,p_U,1.);
  tmp_tvector3.SetMag(1.);
  tmp_trackTr->SetPvers(tmp_tvector3);
  tmp_trackTr->SetR0(q_V,q_U,0.);
  if(FootDebugLevel(4))
    cout<<"R0=("<<tmp_trackTr->GetR0().X()<<", "<<tmp_trackTr->GetR0().Y()<<", "<<tmp_trackTr->GetR0().Z()<<")   Pvers=("<<tmp_trackTr->GetPvers().X()<<", "<<tmp_trackTr->GetPvers().Y()<<", "<<tmp_trackTr->GetPvers().Z()<<")"<<endl;

  //calculate the residual of all the points (aka ComputeDataAll)
  ComputeDataAll(singlehittrack);
  
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::EstimateFIRSTTrackPar::ended"<<endl;
  
  return 0;
}


//Given 2 circles in the x,y plane return the 4 tangents
TG_STRUCT TABMactNtuTrack::Circles2Tangents(Double_t xc1, Double_t yc1, Double_t r1, Double_t xc2, Double_t yc2, Double_t r2){
  TG_STRUCT tan_par;
  for (int ii=0;ii<4;ii++){ /* clean up */
    tan_par.mm[ii] =0. ;
    tan_par.qq[ii] =0. ;
  }
  /*    cout<<"I'm in Circles2Tangets"<<endl;
  cout<<"xc1= "<<xc1<<" yc1= "<<yc1<<" r1= "<<r1<<"  xc2= "<<xc2<<" yc1= "<<
  yc2<<" r2= "<<r2<<endl; */

  Double_t aa,bb,cc, dete, qpiu1, qmeno1, qpiu2, qmeno2;
  cc = (r1*r1 -2*r1*r2 +r2*r2 - yc2*yc2 + 2*yc1*yc2 -yc1*yc1);
  bb = 2*(yc1-yc2)*(xc1-xc2);
  aa = (r1*r1 -2*r1*r2 +r2*r2 -xc1*xc1 +2*xc1*xc2 - xc2*xc2);
  dete = bb*bb -4*aa*cc;
  if(dete>0) {
    tan_par.mm[0] = (-bb + sqrt(dete))/2/aa;
    tan_par.mm[1] = (-bb - sqrt(dete))/2/aa;
  }
  cc = (r1*r1 +2*r1*r2 +r2*r2 - yc2*yc2 + 2*yc1*yc2 -yc1*yc1);
  bb = 2*(yc1-yc2)*(xc1-xc2);
  aa = (r1*r1 +2*r1*r2 +r2*r2 -xc1*xc1 +2*xc1*xc2 - xc2*xc2);
  dete = bb*bb -4*aa*cc;
  
  if(dete>0) {
    tan_par.mm[2] = (-bb + sqrt(dete))/2/aa;
    tan_par.mm[3] = (-bb - sqrt(dete))/2/aa;
  }
  
  for(int hh=0;hh<4;hh++){
    qpiu1 = yc1 -tan_par.mm[hh]*xc1 + r1*sqrt(1+ tan_par.mm[hh]*tan_par.mm[hh]);
    qmeno1 = yc1 -tan_par.mm[hh]*xc1 - r1*sqrt(1+ tan_par.mm[hh]*tan_par.mm[hh]);
    qpiu2 = yc2 -tan_par.mm[hh]*xc2 + r2*sqrt(1+ tan_par.mm[hh]*tan_par.mm[hh]);
    qmeno2 = yc2 -tan_par.mm[hh]*xc2 - r2*sqrt(1+ tan_par.mm[hh]*tan_par.mm[hh]);

    if( (fabs(qpiu1-qpiu2)<.001) || (fabs(qpiu1-qmeno2)<0.001) ){ 
      tan_par.qq[hh] = qpiu1; 
    }
    else if( (fabs(qmeno1-qmeno2)<0.001)||(fabs(qmeno1-qpiu2)<0.001)){ 
      tan_par.qq[hh] = qmeno1; 
    }
  }
  
  return tan_par;
}



void TABMactNtuTrack::Chi2Fit(vector<Int_t> &singlehittrack, Int_t &prunedhit, Bool_t converged){
  
  TMatrixD AA, VV; 
  TVectorD alpha, Dy, Eta;

  alpha.ResizeTo(4);
  Eta.ResizeTo(4);
  Eta.Zero();
    
  Dy.ResizeTo(tmp_trackTr->GetNhit());
  alpha(0) = tmp_trackTr->GetR0().X();
  alpha(1) = tmp_trackTr->GetR0().Y();
  alpha(2) = tmp_trackTr->GetPvers().X();
  alpha(3) = tmp_trackTr->GetPvers().Y();
  AA.ResizeTo(tmp_trackTr->GetNhit(),4);
  VV.ResizeTo(tmp_trackTr->GetNhit(),tmp_trackTr->GetNhit());
  
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::Chi2Fit::start with computeDy and VV"<<endl;
  
  //compute Dy and VV:
  ComputeDy(singlehittrack, Dy);
  ComputeVV(singlehittrack, VV);
  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::Chi2Fit::computechiqua"<<endl;
  Double_t old_chi2;
  Int_t worst_hit=((p_nturaw->GetHitsN()-singlehittrack.size()-1)>p_bmcon->GetRejmaxcut()) ? -1: -2;
  for (Int_t hh=0; hh<p_bmcon->GetNumIte();hh++){
     if(FootDebugLevel(3))
    Info("Action()"," Iteration number %d\n",hh);
    old_chi2=tmp_trackTr->GetChi2Red();
    ComputeDy(singlehittrack, Dy);
    ComputeAA(singlehittrack, alpha, AA);
    if(Mini(tmp_trackTr->GetNhit(), AA, VV, Dy, Eta)){
      Update(singlehittrack, Dy, alpha, Eta, VV, worst_hit); 
      if(converged==false)
        converged=true;
      if(FootDebugLevel(3))
        cout<<"TABMactNtuTrack::Chi2Fit::iteration number="<<hh<<"  chi2red="<<tmp_trackTr->GetChi2Red()<<"  old_chi2="<<old_chi2<<endl;
      if(tmp_trackTr->GetChi2Red()>old_chi2){
        tmp_trackTr->SetNite(hh);
        break;
      }
    }else{
      if(FootDebugLevel(1))
      cout<<"TABMactNtuTrack::Chi2Fit:: Mini not possible"<<hh<<endl;
      break;
    }
  }
  if(tmp_trackTr->GetNite()==0)
    tmp_trackTr->SetNite(p_bmcon->GetNumIte()-1);

  ComputeDataAll(singlehittrack);
    
  //charge prunedhit
  prunedhit=-1;
  if(tmp_trackTr->GetChi2Red() > p_bmcon->GetChi2Redcut() && worst_hit>-1)
    prunedhit=worst_hit;  

  if(FootDebugLevel(3))
    cout<<"TABMactNtuTrack::Chi2Fit::end; track parameters: alpha(0)="<<alpha(0)<<"  alpha(1)="<<alpha(1)<<"  alpha(2)="<<alpha(2)<<"  alpha(3)="<<alpha(3)<<endl;  
  
  return;
}


void TABMactNtuTrack::ComputeDy(vector<Int_t> &singlehittrack, TVectorD &Dy)
{
  Dy.ResizeTo(tmp_trackTr->GetNhit());
  for (Int_t ifilo=0;ifilo<singlehittrack.size();ifilo++){
    p_hit = p_nturaw->Hit(singlehittrack.at(ifilo));
    Dy(ifilo) =p_hit->GetResidual(); 
  }
  
  //~ cout<<"Printing Dy vector="<<endl;
  //~ Dy.Print();
  return ;
}


void TABMactNtuTrack::ComputeVV(vector<Int_t> &singlehittrack, TMatrixD &VV) {
  
  VV.ResizeTo(tmp_trackTr->GetNhit(),tmp_trackTr->GetNhit());
  VV = 0.;
  for(Int_t ii = 0; ii<singlehittrack.size();ii++){
    p_hit = p_nturaw->Hit(ii);
    VV(ii,ii) =  1./p_hit->GetSigma()/p_hit->GetSigma();
  }
  return;
}

Double_t TABMactNtuTrack::ComputeChiQua(TVectorD &Dy, TMatrixD &VV) {
  TVectorD Aus; 
  Aus.ResizeTo(Dy.GetNrows());
  Aus = VV*Dy;

  return Dy*Aus;
}


//  numerical computation of the Df/Dalpha derivatives
void TABMactNtuTrack::ComputeAA(vector<Int_t> &singlehittrack, TVectorD &alpha, TMatrixD &AA){
    
  AA.ResizeTo(tmp_trackTr->GetNhit(),4);
  TVectorD parametri_moved(4);
  Int_t tmp_int;

  for(Int_t ii = 0; ii<4;ii++){
    parametri_moved = alpha;
    if(parametri_moved(ii)!=0.)
	    parametri_moved(ii) *= (1.+p_bmcon->GetParMove());
    else
      parametri_moved(ii) = p_bmcon->GetParMove();
    
    if(FootDebugLevel(3)){
      cout<<"ii="<<ii<<" old parameters:"<<endl;
      tmp_trackTr->PrintR0Pvers();
    }
    
    tmp_trackTr->NewSet(parametri_moved); //tmp_trackTr should have new R0 and Pvers 
    
    if(FootDebugLevel(3)){
      cout<<"new parameters:"<<endl;
      tmp_trackTr->PrintR0Pvers();
    }
    //    cout<<"print out of alpha with incremented parameter # "<<ii<<endl;
    //    parametri_moved.Print();
    
    //ComputeDataAll and charge AA
    for(Int_t j=0;j<singlehittrack.size();j++){
      p_hit=p_nturaw->Hit(singlehittrack.at(j));
      AA(j,ii)=(p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(),true) - p_hit->Dist())/(parametri_moved(ii)-alpha(ii));
      //~ cout<<"j="<<j<<" ii="<<ii<<"  AA="<<AA(j,ii)<<"  dist0="<<p_hit->Dist()<<"  newDist="<<p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(), true)<<endl;
    }
  }

  tmp_trackTr->NewSet(alpha);
  ComputeDataAll(singlehittrack);

  return;
}

void TABMactNtuTrack::ComputeDataAll(vector<Int_t> &singlehittrack){
  Double_t res, chi2red=0.;
  for(Int_t i=0;i<singlehittrack.size();i++){
    p_hit=p_nturaw->Hit(singlehittrack.at(i));
    res=p_hit->Dist()-p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(),true);
    p_hit->SetResidual(res);
    //~ cout<<"res="<<res<<"  hit->View="<<p_hit->View()<<"  plane="<<p_hit->Plane()<<"  cell="<<p_hit->Cell()<<endl;
    p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
    chi2red+=p_hit->GetChi2();
    //~ cout<<"residui che calcolo ="<<p_hit->GetResidual()<<"  dist="<<p_hit->Dist()<<"  newdist="<<p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(),true)<<endl;//provv
  }
  if(singlehittrack.size()>4)
    tmp_trackTr->SetChi2Red(chi2red/((Double_t)(singlehittrack.size()-4)));
  else
    tmp_trackTr->SetChi2Red(1000.);
  return;  
}

Int_t TABMactNtuTrack::Mini(Int_t nmeas,TMatrixD &AA, TMatrixD &VV, TVectorD &Dy, TVectorD &Eta) {
  TMatrixD B, AAt, VVa;
  VVa.ResizeTo(4,4);
  AAt.ResizeTo(4,nmeas);
  B.ResizeTo(4,nmeas);
  
  AAt.Transpose(AA);
  VVa = AAt*VV*AA;
  //~ cout<<"VVa.Determinant()="<<VVa.Determinant()<<"  VV.Determinant()="<<VV.Determinant()<<endl;
  if(VVa.Determinant()>0.0001 && VVa.IsValid()){
    Eta.ResizeTo(4);
    VVa.Invert();
    B = VVa*AAt*VV;
    Eta = B*Dy;
    return 1;
  }
  
  return 0;
}


void TABMactNtuTrack::Update(vector<Int_t> &singlehittrack, TVectorD Dy, TVectorD& alpha, TVectorD &Eta, TMatrixD &VV, Int_t &worst_hit) {

  //~ TVectorD Aus;
  //~ Aus.ResizeTo(Dy.GetNrows());

  alpha = Eta + alpha;
  if(fabs(alpha(2))>1 || TMath::IsNaN(alpha(2))) alpha(2) = 0;
  if(fabs(alpha(3))>1 || TMath::IsNaN(alpha(3))) alpha(3) = 0;
  tmp_trackTr->NewSet(alpha);
  ComputeDataAll(singlehittrack);
  ComputeDy(singlehittrack, Dy);
  //~ Aus = VV*Dy;
  //~ tmp_trackTr->SetChi2Red((Dy*Aus)/(Double_t)(tmp_trackTr->GetNhit()-4.));

  //set the hits chi2 contribution and the worst_hit
  if(worst_hit!=-2){
    Double_t worst_cont=-1.;
    for(int ii=0;ii<singlehittrack.size();ii++){
      p_hit = p_nturaw->Hit(singlehittrack.at(ii));
      if(p_hit->GetChi2()>worst_cont){
        worst_cont=p_hit->GetChi2();
        worst_hit=ii;
      }
    }
  }

  return;
}

 
Int_t TABMactNtuTrack::SortFirstDoubleHits(vector< vector<Int_t> > &hitxplane, vector< vector<Int_t> > &hitxtrack){
  
  if(FootDebugLevel(4))
      cout<<"TABMactNtuTrack::SortFirstDobuleHits start"<<endl;
  
  vector<Int_t> bigtrackhits;
  Double_t mindist, provv_dist;
  Int_t minhit_index;
  for(Int_t i=0; i<hitxplane.size();i++){
    if(hitxplane.at(i).size()==1)
      bigtrackhits.push_back(hitxplane.at(i).at(0));
    else if(hitxplane.at(i).size()>1){
      mindist=10000000.;
      for(Int_t k=0;k<hitxplane.at(i).size();k++){
        p_hit=p_nturaw->Hit(hitxplane.at(i).at(k));
        provv_dist=fabs(p_hit->Dist() - p_bmgeo->FindRdrift(tmp_trackTr->GetR0(), tmp_trackTr->GetPvers(), p_hit->GetA0(), p_hit->GetWvers(),true));
        if(provv_dist<mindist){
          mindist=provv_dist;
          minhit_index=k;
        }
      }
      bigtrackhits.push_back(hitxplane.at(i).at(minhit_index));
    }  
  }
  
  hitxtrack.push_back(bigtrackhits);
  
  if(FootDebugLevel(4)){
    cout<<"TABMactNtuTrack::SortFirstDobuleHits end, bigtrackhits=";
    for(Int_t i=0; i<bigtrackhits.size(); i++)
      cout<<bigtrackhits.at(i)<<"  ";
    cout<<endl;  
  }
  return 0;
}




