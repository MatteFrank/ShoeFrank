
//*********************************************  combined functions  *****************************************

void BookingBMVTX(TFile* f_out, bool onlyVTX, bool merging){
  
  if(debug>0)
    cout<<"I'm in Booking BMVTX"<<endl;  

  f_out->cd();
  TH1D *h; 
  TH2D *h2;
  char tmp_char[200];
  
  //vtx stuff
  if(!merging){
    h = new TH1D("vtx_xslope_coeff","VTX xz m for corrected positions;m;Events",1000,-0.5,0.5);
    h = new TH1D("vtx_yslope_coeff","VTX yz m coeff for corrected positions;m;Events",1000,-0.5,0.5);
    h = new TH1D("vtx_xR0_coeff","VTX xz R0 for corrected positions;R0[cm];Events",600,-3.,3.);
    h = new TH1D("vtx_yR0_coeff","VTX yz R0 for corrected positions;R0[cm];Events",600,-3.,3.);
    h = new TH1D("vtx_polar_angle","vtx polar angle distribution ;AngZ(deg);Events",400,0.,10.);
    h = new TH1D("vtx_azimuth_angle","vtx azimuth angle distribution ;Phi(deg);Events",180,0.,180.);
    h2 = new TH2D("vtx_isoplane_vtxsys","VTX tracks projection on ISO plane in VTX sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("vtx_isoplane_isosys","VTX tracks projection on ISO plane in ISO sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    
    if(onlyVTX){
      f_out->cd("..");
      return;
    }
  
    //bm stuff
    h = new TH1D("bmnhitsxevent","number of hits x event ;N of hits;Events",36,0.,36.);
    h = new TH1D("bm_polar_angle","BM polar angle distribution ;AngZ(deg);Events",400,0.,10.);
    h = new TH1D("bm_azimuth_angle","BM azimuth angle distribution ;Phi(deg);Events",180,0.,180.);
    h = new TH1D("bm_xslope_coeff","BM m_x coeff ;mx;Events",1000,-0.5,0.5);
    h = new TH1D("bm_yslope_coeff","BM m_y coeff ;my;Events",1000,-0.5,0.5);
    h = new TH1D("bm_xR0_coeff","VTX xz R0 for corrected positions;R0[cm];Events",600,-3.,3.);
    h = new TH1D("bm_yR0_coeff","VTX yz R0 for corrected positions;R0[cm];Events",600,-3.,3.);    
    h2 = new TH2D("bmisoproISOsys","BM tracks on isocenter  projections in ISO sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("bmisoproBMsys","BM tracks on isocenter projections in BM sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("bmmylar1BMsys","bm tracks on mylar1 projections in BM sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    h2 = new TH2D("bmmylar2BMsys","bm tracks on mylar2 projections in BM sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
    if(bmmcstudy==0)
      h = new TH1D("bm_realrdrift","real bm rdrift;rdrift[cm];Events",1000,0.,1.);    
  
    //combined stuff not for merging
    h = new TH1D("track_iso_diff_total","Difference for bm tracks and vtx tracks on isocenter plane;diff[cm];Events",500,0.,5.);
    h = new TH1D("track_iso_diff_x","Difference for bm tracks and vtx tracks on isocenter plane;x diff[cm];Events",500,-5.,5.);
    h = new TH1D("track_iso_diff_y","Difference for bm tracks and vtx tracks on isocenter plane;y diff[cm];Events",500,-5.,5.);
  }    
  
  //combined stuff also for merging
  h2 = new TH2D("rdrift_vs_residual","residual calculation ;rdrift[cm];residual",STBIN,0.,1.,400, -0.5,0.5 );
  h = new TH1D("space_residual_error","error events ;rdrift;Events",1000.,1.,10.);
  h = new TH1D("time_residual_error","error events ;tdrift;Events",1000.,1.,10.);
  //~ h = new TH1D("time_diff","bm event timeacq - msd event timeacq ;time;Events",500,0.,100.);
  h2 = new TH2D("pvers_mx_bmvtx","pvers mx angle for bm and vertex for selected tracks;BM Pvers mx;vtx Pverse mx ",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("pvers_my_bmvtx","pvers my angle for bm and vertex for selected tracks;BM Pvers mx;vtx Pverse my ",500,-0.05,0.05,500,-0.05,0.05);
  h2 = new TH2D("r0x_bmvtx","R0x for bm and vtx tracks correlation;BM r0x;vtx r0x",600,-3.,3.,600,-3.,3.);
  h2 = new TH2D("r0y_bmvtx","R0y for bm and vtx tracks correlation;BM r0y;vtx r0y",600,-3.,3.,600,-3.,3.);
  h = new TH1D("pvers_angle_diff","angle difference bm_track - vtx_track ;Angle(bm_track-vtx_track);Events",2000,-0.1,0.1);
  h = new TH1D("space_residual_numev","number of events for each bin in strel residual graph;bin;events",STBIN,0.,STBIN);
  h = new TH1D("space_residual_total","total space residual;x[cm];events",200,-1.,1.);
  h = new TH1D("time_residual_total","total time residual;x[cm];events",200,-1.,1.);
  
  h2 = new TH2D("old_strel_data","old FIRST strel from data ;time;rdrift",400,0.,400.,1000, 0., 1.);
  h2 = new TH2D("new_strel_rdrift","new strel ;time;rdrift",300,0.,400., 1000, 0., 1.);
  h2 = new TH2D("new_strel_time","new strel ;rdrift;time", 1000, 0., 1., 400,0.,400.);
  h2 = new TH2D("residual_strel_time","strel residuals of fitted_timestrel-MC teorical strel ;rdrift;time", 400,0.,400., 1000, -0.5, 0.5);
  h2 = new TH2D("residual_strel_rdrift","strel residuals of fitted_rdriftstrel-MC teorical strel ;rdrift;time",400,0.,400., 1000, -0.5, 0.5);
  if(bmmcstudy==0)
    h2 = new TH2D("strel_realrdrift","old FIRST strel from realrdrift ;time;rdrift",400,0.,400.,1000, 0., 1.);
  
  f_out->mkdir("Res_vs_rdrift");
  f_out->cd("Res_vs_rdrift");
  for(int i=0;i<STBIN;i++){
    sprintf(tmp_char,"strel_rdrift_%d", i);
    h = new TH1D(tmp_char,"strel residual ;Residual[cm];Events",600,-1.,1.);
  }
  f_out->cd("..");
  
  f_out->mkdir("Res_vs_tdrift");
  f_out->cd("Res_vs_tdrift");
  for(int i=0;i<STBIN;i++){
    sprintf(tmp_char,"strel_tdrift_%d", i);
    h = new TH1D(tmp_char,"strel residual ;Residual[cm];Events",600,-1.,1.);
  }
  
  f_out->cd("..");
  
  if(debug>0)
    cout<<"Booking finished"<<endl;  
  
  return;
}


void Printoutput(TFile* f_out, vector<BM_evstruct> &allbmeventin, vector<vtx_evstruct> &allvtxeventin, vector<vector<double>> &space_residual, vector<vector<double>> &time_residual, vector<vector<int>> &selected_index, bool onlyVTX){
  
  if(debug)
    cout<<"I'm in Printoutput:: allbmeventin.size():"<<allbmeventin.size()<<"  allvtxeventin.size()="<<allvtxeventin.size()<<endl;
    
  f_out->cd();
  TVector3 tmp_tvector3;
  char tmp_char[200];
  double tmp_double;

  //oldstrel_stuff
  //~ TCanvas *mcstrel=new TCanvas("mcstrel","mcstrel",800,800);
  //~ mcstrel->cd();
  TF1* first_strel_tf1_1=new TF1("first1_strel_green","1./0.78*(0.032891770+0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x)", 0., 320.);  
  TF1* first_strel_tf1_08=new TF1("first08_strel_blue","0.8/0.78*(0.032891770+0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x)", 0., 320.);  
  TF1* garfield_strel_tf1=new TF1("garfield_strel_yellow","0.00915267+0.00634507*x+2.02527e-05*x*x-7.60133e-07*x*x*x+5.55868e-09*x*x*x*x-1.68944e-11*x*x*x*x*x+1.87124e-14*x*x*x*x*x*x", 0., 320.);  
  first_strel_tf1_1->SetLineColor(3);
  first_strel_tf1_1->Write();
  first_strel_tf1_08->SetLineColor(4);
  first_strel_tf1_08->Write();
  garfield_strel_tf1->SetLineColor(41);
  garfield_strel_tf1->Write();

  //VTX events
  for(int i=0; i<allvtxeventin.size();i++){

    ((TH1D*)gDirectory->Get("vtx_xslope_coeff"))->Fill(allvtxeventin.at(i).vtx_track_pvers.X()/allvtxeventin.at(i).vtx_track_pvers.Z());
    ((TH1D*)gDirectory->Get("vtx_yslope_coeff"))->Fill(allvtxeventin.at(i).vtx_track_pvers.Y()/allvtxeventin.at(i).vtx_track_pvers.Z());
    ((TH1D*)gDirectory->Get("vtx_xR0_coeff"))->Fill(allvtxeventin.at(i).vtx_track_r0pos.X());
    ((TH1D*)gDirectory->Get("vtx_yR0_coeff"))->Fill(allvtxeventin.at(i).vtx_track_r0pos.Y());
    ((TH1D*)gDirectory->Get("vtx_polar_angle"))->Fill(allvtxeventin.at(i).vtx_track_pvers.Theta()*RAD2DEG);
    ((TH1D*)gDirectory->Get("vtx_azimuth_angle"))->Fill(allvtxeventin.at(i).vtx_track_pvers.Phi()*RAD2DEG);
    ((TH2D*)gDirectory->Get("vtx_isoplane_vtxsys"))->Fill(allvtxeventin.at(i).vtx_track_r0pos.X(), allvtxeventin.at(i).vtx_track_r0pos.Y());
    tmp_tvector3=ExtrapolateZ(allvtxeventin.at(i).vtx_track_pvers, allvtxeventin.at(i).vtx_track_r0pos, -VTXISOZ, true, true);
    ((TH2D*)gDirectory->Get("vtx_isoplane_isosys"))->Fill(allvtxeventin.at(i).vtx_track_r0pos.X(),allvtxeventin.at(i).vtx_track_r0pos.y());
  }
  
  if(debug)
    cout<<"I'm in Printoutput:: allvtxeventin finished  allvtxeventin.size()="<<allvtxeventin.size()<<endl;
  
  if(onlyVTX)
    return;
  
  //BM events
  for(int i=0; i<allbmeventin.size();i++){
    ((TH1D*)gDirectory->Get("bmnhitsxevent"))->Fill(allbmeventin.at(i).hitnum);
    ((TH1D*)gDirectory->Get("bm_polar_angle"))->Fill(allbmeventin.at(i).bm_track_pvers.Theta()*RAD2DEG);
    ((TH1D*)gDirectory->Get("bm_azimuth_angle"))->Fill(allbmeventin.at(i).bm_track_pvers.Phi()*RAD2DEG);
    ((TH1D*)gDirectory->Get("bm_xslope_coeff"))->Fill(allbmeventin.at(i).bm_track_pvers.X()/allbmeventin.at(i).bm_track_pvers.Z());
    ((TH1D*)gDirectory->Get("bm_yslope_coeff"))->Fill(allbmeventin.at(i).bm_track_pvers.Y()/allbmeventin.at(i).bm_track_pvers.Z());
    ((TH1D*)gDirectory->Get("bm_xR0_coeff"))->Fill(allbmeventin.at(i).bm_track_r0pos.X());
    ((TH1D*)gDirectory->Get("bm_yR0_coeff"))->Fill(allbmeventin.at(i).bm_track_r0pos.Y());
    tmp_tvector3=ExtrapolateZ(allbmeventin.at(i).bm_track_pvers, allbmeventin.at(i).bm_track_r0pos, -BMISOZ, true, true);
    ((TH2D*)gDirectory->Get("bmisoproISOsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
    tmp_tvector3=ExtrapolateZ(allbmeventin.at(i).bm_track_pvers, allbmeventin.at(i).bm_track_r0pos, -BMISOZ, false, true);
    ((TH2D*)gDirectory->Get("bmisoproBMsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());//dovrebbe essere = a bmisoproISOsys
     tmp_tvector3=ExtrapolateZ(allbmeventin.at(i).bm_track_pvers, allbmeventin.at(i).bm_track_r0pos, BMN_MYLAR1Z, true, true);
    ((TH2D*)gDirectory->Get("bmmylar1BMsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
    tmp_tvector3=ExtrapolateZ(allbmeventin.at(i).bm_track_pvers, allbmeventin.at(i).bm_track_r0pos, BMN_MYLAR2Z, false, true);
    ((TH2D*)gDirectory->Get("bmmylar2BMsys"))->Fill(tmp_tvector3.X(), tmp_tvector3.Y());
    
    for(int k=0;k<allbmeventin.at(i).hitnum;k++){
      ((TH2D*)gDirectory->Get("old_strel_data"))->Fill(allbmeventin.at(i).bm_hit_time[k],allbmeventin.at(i).bm_hit_rdrift[k]);
      if(bmmcstudy==0){
        ((TH1D*)gDirectory->Get("bm_realrdrift"))->Fill(allbmeventin.at(i).bm_hit_realrdrift[k]);
        ((TH2D*)gDirectory->Get("strel_realrdrift"))->Fill(allbmeventin.at(i).bm_hit_time[k],allbmeventin.at(i).bm_hit_realrdrift[k]);
      }
    }
  }
  
  if(debug)
    cout<<"I'm in Printoutput:: allbmeventin finished allbmeventin.size()="<<allbmeventin.size()<<endl;    
  
  //~ gDirectory->ls();
  //~ cout<<"ora faccio crashasre root cercando grafico che non esiste"<<endl;
  //~ ((TH1D*)gDirectory->Get("fdsjaklflkasjdlk"))->Fill(3.);
  
  //combined
  TVector3 bmproject, vtxproject;
  for(int i=0;i<selected_index.size();i++){
    //~ cout<<"i="<<i<<"  selected_index.size()="<<selected_index.size()<<"  selected_index.at(i).size()="<<selected_index.at(i).size()<<"  selected_index.at(i).at(2)="<<selected_index.at(i).at(2)<<endl;
    if(selected_index.at(i).at(2)==0){//both bm and vtx track
      //~ cout<<"provo a fillare questo:"<<allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.X()/allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Z()<<"    e questo:    "<<allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.X()/allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Z()<<"  in pvers_mx_bmvtx"<<endl;
      ((TH2D*)gDirectory->Get("pvers_mx_bmvtx"))->Fill(allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.X()/allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Z(), allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.X()/allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Z());        
      //~ cout<<"provo a filalre:"<<allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Y()/allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Z()<<" e questo:"<<allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Y()/allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Z()<<"  in pvers_my_bmvtx"<<endl;
      ((TH2D*)gDirectory->Get("pvers_my_bmvtx"))->Fill(allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Y()/allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Z(), allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Y()/allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers.Z());   
      //~ cout<<"provo a fillare:"<<allbmeventin.at(selected_index.at(i).at(0)).bm_track_r0pos.X()<<"   fe questo:"<<allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_r0pos.X()<<"  in r0x_bmvtx"<<endl;     
      ((TH2D*)gDirectory->Get("r0x_bmvtx"))->Fill(allbmeventin.at(selected_index.at(i).at(0)).bm_track_r0pos.X(), allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_r0pos.X());        
      //~ cout<<"provo a fillare:"<<allbmeventin.at(selected_index.at(i).at(0)).bm_track_r0pos.Y()<<"   fe questo:"<<allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_r0pos.Y()<<"  in r0y_bmvtx"<<endl;           
      ((TH2D*)gDirectory->Get("r0y_bmvtx"))->Fill(allbmeventin.at(selected_index.at(i).at(0)).bm_track_r0pos.Y(), allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_r0pos.Y());        
      ((TH1D*)gDirectory->Get("pvers_angle_diff"))->Fill(allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers.Angle(allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers));        
      bmproject=ExtrapolateZ(allbmeventin.at(selected_index.at(i).at(0)).bm_track_pvers, allbmeventin.at(selected_index.at(i).at(0)).bm_track_r0pos, -BMISOZ,true, true);
      vtxproject=ExtrapolateZ(allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_pvers, allvtxeventin.at(selected_index.at(i).at(1)).vtx_track_r0pos, -VTXISOZ,true, false);
      if(bmproject.Z()!=vtxproject.Z())
        cout<<"ERROR on bm/msd iso projection::bmproject.Z()="<<bmproject.Z()<<"   vtxproject.Z()="<<vtxproject.Z()<<endl;
      else{
        ((TH1D*)gDirectory->Get("track_iso_diff_y"))->Fill((bmproject-vtxproject).X());    
        ((TH1D*)gDirectory->Get("track_iso_diff_x"))->Fill((bmproject-vtxproject).Y());    
        ((TH1D*)gDirectory->Get("track_iso_diff_total"))->Fill((bmproject-vtxproject).Mag());
      }    
    }
  }
  
  cout<<"r0x_bmvtx correlation factor= "<<((TH2D*)gDirectory->Get("r0x_bmvtx"))->GetCorrelationFactor()<<endl;
  cout<<"r0y_bmvtx correlation factor= "<<((TH2D*)gDirectory->Get("r0y_bmvtx"))->GetCorrelationFactor()<<endl;

  if(debug)
    cout<<"I'm in Printoutput:: combined graph finished"<<endl;

  //old strel
  fitStrel(f_out, 0,first_strel_tf1_1, first_strel_tf1_08, garfield_strel_tf1);

  if(((TH2D*)gDirectory->Get("r0x_bmvtx"))->GetCorrelationFactor()>CORRMINIMUM || ((TH2D*)gDirectory->Get("r0y_bmvtx"))->GetCorrelationFactor()>CORRMINIMUM){
    //try to estimate the new strel with space_residual
    for(int i=0; i<STBIN;i++){
      ((TH1D*)gDirectory->Get("space_residual_numev"))->AddBinContent(i+1,space_residual.at(i).size());
      for(int k=0; k<space_residual.at(i).size();k++){
        ((TH2D*)gDirectory->Get("rdrift_vs_residual"))->Fill((double)i/STBIN,space_residual.at(i).at(k));
        sprintf(tmp_char,"Res_vs_rdrift/strel_rdrift_%d", i);
        ((TH1D*)gDirectory->Get(tmp_char))->Fill(space_residual.at(i).at(k));
        ((TH1D*)gDirectory->Get("space_residual_total"))->Fill(space_residual.at(i).at(k));
      }
    }
    for(int k=0; k<space_residual.at(STBIN).size();k++)
      ((TH1D*)gDirectory->Get("space_residual_error"))->Fill(space_residual.at(STBIN).at(k));  
    //no gaussian fit on the slice of rdrift for the moment
    int binpos;
    for(int i=0; i<allbmeventin.size();i++){
      for(int k=0;k<allbmeventin.at(i).hitnum;k++){
        binpos=(int)(allbmeventin.at(i).bm_hit_rdrift[k]*STBIN);
        if(binpos<STBIN){
          sprintf(tmp_char,"Res_vs_rdrift/strel_rdrift_%d", binpos);
          tmp_double=((TH1D*)gDirectory->Get(tmp_char))->GetMean();
          ((TH2D*)gDirectory->Get("new_strel_rdrift"))->Fill(allbmeventin.at(i).bm_hit_time[k],allbmeventin.at(i).bm_hit_rdrift[k]+tmp_double);
        }    
      }
    }
    //fit the new_strel
    fitStrel(f_out,1,first_strel_tf1_1, first_strel_tf1_08, garfield_strel_tf1);
    
    if(debug)
      cout<<"I'm in Printoutput:: estimate of the strel with the position residual finished"<<endl;  
    
  //try to estimate the new strel with TIME_residual
    for(int i=0; i<STBIN;i++){
      for(int k=0; k<time_residual.at(i).size();k++){
        sprintf(tmp_char,"Res_vs_tdrift/strel_tdrift_%d", i);
        //~ cout<<"ora riempio in "<<tmp_char<<"    il valore: "<<time_residual.at(i).at(k)<<"   time_residual.size()="<<time_residual.size()<<endl;
        ((TH1D*)gDirectory->Get(tmp_char))->Fill(time_residual.at(i).at(k));
        ((TH1D*)gDirectory->Get("time_residual_total"))->Fill(time_residual.at(i).at(k));
      }
    }
    for(int k=0; k<time_residual.at(STBIN).size();k++)
      ((TH1D*)gDirectory->Get("time_residual_error"))->Fill(time_residual.at(STBIN).at(k));  
    //no gaussian fit on the slice of rdrift for the moment
    for(int i=0; i<allbmeventin.size();i++){
      for(int k=0;k<allbmeventin.at(i).hitnum;k++){
        binpos=(int)(allbmeventin.at(i).bm_hit_time[k]*STBIN/300.);
        if(binpos<STBIN && binpos>=0){
          sprintf(tmp_char,"Res_vs_tdrift/strel_tdrift_%d", binpos);
          tmp_double=((TH1D*)gDirectory->Get(tmp_char))->GetMean();
          ((TH2D*)gDirectory->Get("new_strel_time"))->Fill(allbmeventin.at(i).bm_hit_rdrift[k]+tmp_double, allbmeventin.at(i).bm_hit_time[k]);
        }    
      }
    }
    //fit the new_strel
    fitStrel(f_out, 2,first_strel_tf1_1, first_strel_tf1_08, garfield_strel_tf1);
    
    if(debug)
      cout<<"I'm in Printoutput:: estimate of the strel with the time residual finished, Printout finished"<<endl;  
  
  
  
  }else
    cout<<endl<<"Printoutput: THE DATA ARE NOT CORRELATED ENOUGH, the program will be finished"<<endl<<endl;
         
    return;  
}


void fitStrel(TFile *f_out, const int index, TF1* first_strel_tf1_1, TF1* first_strel_tf1_08 ,TF1* garfield_strel_tf1){

  //~ TCanvas *strels=new TCanvas("strels","strels",800,800);
  //~ strels->cd();
  TProfile *prof_newstrel;
  if(index==0) 
    prof_newstrel=((TH2D*)gDirectory->Get("old_strel_data"))->ProfileX(); 
  else if(index==1) 
    prof_newstrel=((TH2D*)gDirectory->Get("new_strel_rdrift"))->ProfileX(); 
  else if(index==2)
    prof_newstrel=((TH2D*)gDirectory->Get("new_strel_time"))->ProfileY(); 
  prof_newstrel->SetLineColor(3);
  prof_newstrel->Draw();
  
  TF1 poly ("poly","pol5", 0, 400);
  prof_newstrel->Fit("poly","Q+");
  //~ f_out->SaveObjectAs(strels, f_out->GetName(),"q");
  //~ TF1 first_strel_tf1("first_strel_tf1","0.8/0.78*(0.032891770+0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x)", 0., 320.);  
  //~ TF1 garfield_strel_tf1("garfield_strel_tf1","0.00915267+0.00634507*x+2.02527e-05*x*x-7.60133e-07*x*x*x+5.55868e-09*x*x*x*x-1.68944e-11*x*x*x*x*x+1.87124e-14*x*x*x*x*x*x", 0., 320.);  
  //~ first_strel_tf1.SetLineColor(3);
  //~ garfield_strel_tf1.SetLineColor(4);
  //~ f_out->cd();
  if(index==1){
    cout<<endl<<"new strel parameters from space residuals"<<endl;
    //oldstrel_stuff
    //~ first_strel_tf1.Write();
    //~ garfield_strel_tf1.Write();
    poly.SetName("fitted_rdrift_strel");
    poly.Write();
    if(MCSTREL==8){
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_rdrift"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_rdrift"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_rdrift"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- first_strel_tf1_08->Eval((double)i));
    }else if(MCSTREL==1){
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_rdrift"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_rdrift"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_rdrift"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- first_strel_tf1_1->Eval((double)i));      
    }else{
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_rdrift"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_rdrift"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_rdrift"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- garfield_strel_tf1->Eval((double)i));     
      }   
  }else if(index==2){
    cout<<endl<<"new strel parameters from time residuals"<<endl;
    poly.SetName("fitted_time_strel");
    poly.Write();
    if(MCSTREL==8){
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_time"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_time"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_time"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- first_strel_tf1_08->Eval((double)i));
    }else if(MCSTREL==1){
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_time"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_time"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_time"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- first_strel_tf1_1->Eval((double)i));      
    }else{
      for(int i=1;i<((TH2D*)gDirectory->Get("residual_strel_time"))->GetNbinsX();i++)
        ((TH2D*)gDirectory->Get("residual_strel_time"))->Fill( ((TH1D*)gDirectory->Get("residual_strel_time"))->GetXaxis()->GetBinCenter(i) , poly.Eval((double)i)- garfield_strel_tf1->Eval((double)i));     
      }     
  }
  cout<<poly.GetParameter(0)<<" + ("<<poly.GetParameter(1)<<"*tdrift) + ("<<poly.GetParameter(2)<<"*tdrift*tdrift) + ("<<poly.GetParameter(3)<<"*tdrift*tdrift*tdrift) + ("<<poly.GetParameter(4)<<"*tdrift*tdrift*tdrift*tdrift) + ("<<poly.GetParameter(5)<<"*tdrift*tdrift*tdrift*tdrift*tdrift)"<<endl<<endl;  
  return;
}


//to be made with labo sys of ref. (with respect to isocenter) 
void EvaluateSpaceResidual(vector<vector<double>> &space_residual,vector<vector<double>> &time_residual, BM_evstruct &bmevent, vtx_evstruct &vtxevent, TABMparGeo* bmgeo){
  
  if(debug>2)
    cout<<"I'm in EvaluateSpaceResidual"<<endl;
  
  TABMntuTrackTr* dummy_tracktr=new TABMntuTrackTr();
  int binpos, timepos;
  double residual;
  TVector3 pversout, r0posout;
  int cell, view, plane;
  for(int i=0;i<bmevent.hitnum;i++){
    binpos=(int)(bmevent.bm_hit_rdrift[i]*STBIN);
    timepos=(int)(bmevent.bm_hit_time[i]*STBIN/300.);
    if(binpos>STBIN-1) 
      binpos=STBIN;
    if(timepos>STBIN-1) 
      timepos=STBIN;
    vtxpversr02global(vtxevent.vtx_track_pvers, vtxevent.vtx_track_r0pos,  pversout, r0posout);
    bmgeo->GetBMNlvc(bmevent.bm_hit_cellid[i], plane, view, cell);    
    residual=dummy_tracktr->FindRdrift(pversout, r0posout, bmgeo->GetWirePos(view, plane, bmgeo->GetSenseId(cell)), bmgeo->GetWireDir(view))-bmevent.bm_hit_rdrift[i];
    if(debug>2)
      cout<<"residual="<<residual<<"  original_rdrift="<<bmevent.bm_hit_rdrift[i]<<"  fitted_rdrift="<<dummy_tracktr->FindRdrift(pversout, r0posout, bmgeo->GetWirePos(view, plane, bmgeo->GetSenseId(cell)), bmgeo->GetWireDir(view))<<endl;
    //~ if(fabs(residual)<0.8 && binpos!=STBIN){
    if(binpos!=STBIN){
      space_residual.at(binpos).push_back(residual);
    }else{
      if(debug>0)
        cout<<"possible ERROR in EvaluateSpaceResidual space_residual??? residual="<<residual<<"  hit_id="<<bmevent.bm_hit_cellid[i]<<"  hit_rdrift="<<bmevent.bm_hit_rdrift[i]<<"  hit time="<<bmevent.bm_hit_time[i]<<endl;
      space_residual.at(STBIN).push_back(bmevent.bm_hit_rdrift[i]);
    }  
    if(timepos!=STBIN){
      time_residual.at(timepos).push_back(residual);
    }else{
      if(debug>0)
        cout<<"possible ERROR in EvaluateSpaceResidual time_residual??? residual="<<residual<<"  hit_id="<<bmevent.bm_hit_cellid[i]<<"  hit_rdrift="<<bmevent.bm_hit_rdrift[i]<<"  hit time="<<bmevent.bm_hit_time[i]<<endl;
      time_residual.at(STBIN).push_back(bmevent.bm_hit_time[i]);
    }  
  }
  
  if(debug>2)
    cout<<"EvaluateSpaceResidual finished"<<endl;  
  
  return;
}


//evaluate the projections
//pvers=particle direction, r0pos=where pvers is calculated, proposz=position where I want to project, in the local coordinate! 
TVector3 ExtrapolateZ(TVector3 pvers, TVector3 r0pos, double proposz, bool global, bool beammonitor){
  TVector3 projection, shifttoglobal;
  if(global && beammonitor){
    //~ TVector3 BMshift(BMSHIFTX, BMSHIFTY, BMSHIFTZ);
    //~ r0pos+=BMshift;
    pvers.RotateX(BMISOYZANGLE*DEG2RAD);
    pvers.RotateY(BMISOXZANGLE*DEG2RAD);
    shifttoglobal.SetXYZ(BMISOX,BMISOY,BMISOZ);
  }else if(!beammonitor){
    //~ TVector3 VTXshift(VTXSHIFTX, VTXSHIFTY, VTXSHIFTZ);
    //~ r0pos+=VTXshift;
    pvers.RotateX(VTXYZANGLE*DEG2RAD);
    pvers.RotateY(VTXXZANGLE*DEG2RAD);    
    shifttoglobal.SetXYZ(VTXISOX,VTXISOY,VTXISOZ);
  }
  projection.SetXYZ(pvers.X()/pvers.Z()*proposz+r0pos.X(),  pvers.Y()/pvers.Z()*proposz+r0pos.Y(), proposz);
  if(global)
    projection+=shifttoglobal;
  return projection;
}

//to set the pvers and r0 from vtx to global coordinate
void vtxpversr02global(const TVector3 vtxpvers, const TVector3 vtxr0pos, TVector3 &pversout, TVector3 &r0posout ){
  TVector3 VTXshift(VTXISOX, VTXISOY, 0.);
  double proposz=BMISOZ-VTXISOZ;
  pversout=vtxpvers;
  r0posout=vtxr0pos+VTXshift;
  pversout.RotateX((BMISOYZANGLE-VTXYZANGLE)*DEG2RAD);
  pversout.RotateY((BMISOXZANGLE-VTXXZANGLE)*DEG2RAD);    
  r0posout.SetXYZ(pversout.X()/pversout.Z()*proposz+r0posout.X(),  pversout.Y()/pversout.Z()*proposz+r0posout.Y(), proposz);
  return;
}

//estimate allign with projections on mylars 
//da sistemare!!!!
void Allign_estimate(){

  if(debug>0)
    cout<<"I'm in Allign_estimate"<<endl;  
  
  //BM par
  Double_t BMxrot=atan((((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2) - ((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2))/(BMN_MYLAR2Z-BMN_MYLAR1Z))*RAD2DEG;    
  Double_t BMyrot=-atan((((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)-((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1))/(BMN_MYLAR2Z-BMN_MYLAR1Z))*RAD2DEG;
  Double_t BMxtra=-(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)+((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1))/2.;  
  Double_t BMxtr_err=sqrt(pow(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(1)/sqrt(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetEntries()),2.)  + pow(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(1)/sqrt(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetEntries()),2.));  
  Double_t BMytra=-(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2)+((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2))/2.;
  Double_t BMytr_err=sqrt(pow(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetMean(2)/sqrt(((TH1D*)gDirectory->Get("bmmylar2BMsys"))->GetEntries()),2.)  +  pow(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetMean(2)/sqrt(((TH1D*)gDirectory->Get("bmmylar1BMsys"))->GetEntries()),2.));  
    
  cout<<"Beam Monitor allignment parameters:"<<endl;
  cout<<"estimated rotation around X axis= "<<BMxrot<<endl;
  cout<<"estimated rotation around Y axis= "<<BMyrot<<endl;
  cout<<"estimated translation in X="<<BMxtra<<"   +-   "<<BMxtr_err<<endl;
  cout<<"estimated translation in Y="<<BMytra<<"   +-   "<<BMytr_err<<endl<<endl;
      
  TString tmp_str("BM allign par: xrot="); tmp_str+= BMxrot; tmp_str+="  yrot="; tmp_str+=BMyrot;
  tmp_str+="  x_tra="; tmp_str+=BMxtra; tmp_str+=" +- ";  tmp_str+=BMxtr_err;
  tmp_str+="  y_tra="; tmp_str+=BMytra; tmp_str+=" +- ";  tmp_str+=BMytr_err;
  TNamed n("BM_MSD_allign_par",tmp_str.Data());
  n.Write();
  
  if(debug>0)
    cout<<"Allign_estimate finished"<<endl;
  
  return;
}


void fitPositionResidual(){
TString graphname("bm_onmsd1diff_x");
vector<TString> title_vec;
title_vec.push_back(graphname);
graphname="bm_onmsd2diff_x";
title_vec.push_back(graphname);
graphname="bm_onmsd1diff_channel_x";
title_vec.push_back(graphname);
graphname="bm_onmsd2diff_channel_x";
title_vec.push_back(graphname);
graphname="pvers_total_diff";
title_vec.push_back(graphname);

for(int i=0;i<title_vec.size();i++){
  TF1  *gaus = new TF1("gaus","gaus",-2000.,2000.);  
  gaus->SetParameter(2,((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetMean());
  gaus->SetParameter(3,((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->Fit("gaus","QMB","",((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetXaxis()->GetBinCenter(((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->FindFirstBinAbove(10)),((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetXaxis()->GetBinCenter(((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->FindLastBinAbove(10)));
  cout<<endl<<title_vec.at(i).Data()<<" fit results:"<<endl;
  cout<<"mean="<<gaus->GetParameter(1)<<" +- "<<gaus->GetParError(1)<<endl;
  cout<<"devstd="<<gaus->GetParameter(2)<<" +- "<<gaus->GetParError(2)<<endl;
  delete gaus;
}
cout<<endl;
return;  
}

void merge_graphics(TFile* infile, TFile* f_out){
  vector<TString> h2tstring, h1tstring;
  TString tmp_tstring;
  TH2D* histo2d;
  TH1D* histo1d;
  char tmp_char[200];
  
  //th2d
  tmp_tstring="rdrift_vs_residual";
  h2tstring.push_back(tmp_tstring);
  tmp_tstring="pvers_mx_bmmsd";
  h2tstring.push_back(tmp_tstring);
  tmp_tstring="old_strel";
  h2tstring.push_back(tmp_tstring);
  //th1d
  tmp_tstring="space_residual_error";
  h1tstring.push_back(tmp_tstring);
  tmp_tstring="time_residual_error";
  h1tstring.push_back(tmp_tstring);
  tmp_tstring="pvers_total_diff";
  h1tstring.push_back(tmp_tstring);
  tmp_tstring="space_residual_numev";
  h1tstring.push_back(tmp_tstring);
  tmp_tstring="space_residual_total";
  h1tstring.push_back(tmp_tstring);
  tmp_tstring="time_residual_total";
  h1tstring.push_back(tmp_tstring);
  
  //merge h1tstring and h2tstring histograms
  for(int i=0;i<h2tstring.size();i++){
    infile->cd();
    histo2d=(TH2D*)((TH2D*)gDirectory->Get(h2tstring.at(i).Data()))->Clone();
    f_out->cd();
    ((TH2D*)gDirectory->Get(h2tstring.at(i).Data()))->Add(histo2d);
  }
  for(int i=0;i<h1tstring.size();i++){
    infile->cd();
    histo1d=(TH1D*)((TH1D*)gDirectory->Get(h1tstring.at(i).Data()))->Clone();
    f_out->cd();
    ((TH1D*)gDirectory->Get(h1tstring.at(i).Data()))->Add(histo1d);
  }
  
  //merge residual histograms
  for(int i=0;i<STBIN;i++){
    sprintf(tmp_char,"Res_vs_rdrift/strel_rdrift_%d", i);
    infile->cd();
    histo1d=(TH1D*)((TH1D*)gDirectory->Get(tmp_char))->Clone();
    f_out->cd();
    ((TH1D*)gDirectory->Get(tmp_char))->Add(histo1d);
    sprintf(tmp_char,"Res_vs_tdrift/strel_tdrift_%d", i);
    infile->cd();
    histo1d=(TH1D*)((TH1D*)gDirectory->Get(tmp_char))->Clone();
    f_out->cd();
    ((TH1D*)gDirectory->Get(tmp_char))->Add(histo1d);
  }
  
  return;
}


//*************************************************** MSD STUFF  ***************************************** 
void vtxreadevent(TAVTntuTrack *vtntutrack, vtx_evstruct &vtxevent, vector<vtx_evstruct> &allvtxeventin){
  
  if(debug>2)
    cout<<"I'm in VtXreadevent"<<endl;
  
  clean_vtxevstruct(vtxevent);
  TAVTtrack* vttrack = vtntutrack->GetTrack(0);
  vtxevent.vtx_track_chi2tot=vttrack->GetChi2();
  vtxevent.vtx_track_chi2uview=vttrack->GetChi2U();
  vtxevent.vtx_track_chi2vview=vttrack->GetChi2V();
  vtxevent.vtx_track_r0pos=vttrack->GetOrigin();
  vtxevent.vtx_track_pvers=vttrack->GetSlopeZ();
  vtxevent.status=0;  
  if(debug>2){
    cout<<"msdreadevent finished"<<endl;  
    print_vtxevstruct(vtxevent);
  }

  allvtxeventin.push_back(vtxevent);
  
return;
}
  
void clean_vtxevstruct(vtx_evstruct &vtxevstruct){

  vtxevstruct.vtx_track_chi2tot=-999;
  vtxevstruct.vtx_track_chi2uview=-999;
  vtxevstruct.vtx_track_chi2vview=-999;
  vtxevstruct.vtx_track_pvers.SetXYZ(-999,-999,-999);
  vtxevstruct.vtx_track_r0pos.SetXYZ(-999,-999,-999);

return;
}
  
  
void print_vtxevstruct(vtx_evstruct &vtxevstruct){

  cout<<"print_vtxevstruct:"<<endl;
  cout<<"vtx_track_chi2tot="<<vtxevstruct.vtx_track_chi2tot<<"  vtx_track_chi2uview="<<vtxevstruct.vtx_track_chi2uview<<"  vtx_track_chi2vview="<<vtxevstruct.vtx_track_chi2vview<<endl;
  cout<<"pvers=("<<vtxevstruct.vtx_track_pvers.X()<<", "<<vtxevstruct.vtx_track_pvers.Y()<<", "<<vtxevstruct.vtx_track_pvers.Z()<<")  r0=("<<vtxevstruct.vtx_track_r0pos.X()<<", "<<vtxevstruct.vtx_track_r0pos.Y()<<", "<<vtxevstruct.vtx_track_r0pos.Z()<<")"<<endl;
  cout<<"status="<<vtxevstruct.status<<endl;

return;
}  

void fitVTXgraph(){
TString graphname("MSD_1xraw");
vector<TString> title_vec;
title_vec.push_back(graphname);
graphname="MSD_1xval";
title_vec.push_back(graphname);
graphname="MSD_1xtra";
title_vec.push_back(graphname);
graphname="MSD_1yraw";
title_vec.push_back(graphname);
graphname="MSD_1yval";
title_vec.push_back(graphname);
graphname="MSD_1ytra";
title_vec.push_back(graphname);
graphname="MSD_xzraw_coeff";
title_vec.push_back(graphname);
graphname="MSD_xzval_coeff";
title_vec.push_back(graphname);

for(int i=0;i<title_vec.size();i++){
  TF1  *gaus = new TF1("gaus","gaus",-2000.,2000.);  
  gaus->SetParameter(2,((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetMean());
  gaus->SetParameter(3,((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetStdDev());
  ((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->Fit("gaus","QMB","",((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetXaxis()->GetBinCenter(((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->FindFirstBinAbove()),((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->GetXaxis()->GetBinCenter(((TH1D*)gDirectory->Get(title_vec.at(i).Data()))->FindLastBinAbove()));
  cout<<endl<<title_vec.at(i).Data()<<" fit results:"<<endl;
  cout<<"mean="<<gaus->GetParameter(1)<<" +- "<<gaus->GetParError(1)<<endl;
  cout<<"devstd="<<gaus->GetParameter(2)<<" +- "<<gaus->GetParError(2)<<endl;
  delete gaus;
}

return;  
}

//************************************************* BM stuff *************************************************

void bmreadevent(TABMparGeo* geomap, TABMntuHit*  bmRawHit, TABMntuTrack* bmNtuTrack, BM_evstruct &bmevent, vector<BM_evstruct> &allbmeventin){
  
  if(debug>2)
    cout<<"I'm in bmreadevent"<<endl;
  
  clean_bmevstruct(bmevent, false);
  bmevent.status=bmNtuTrack->GetTrackStatus();
  TABMntuTrackTr* bmtrack = bmNtuTrack->Track(0);
  if(bmNtuTrack->GetTrackStatus()==0 || bmNtuTrack->GetTrackStatus()==5){
    int evhitnum=0;
    bmevent.bm_track_chi2=bmtrack->GetMyChi2Red();
    bmevent.bm_track_pvers=bmtrack->GetPvers();
    bmevent.bm_track_r0pos=bmtrack->GetR0();
    bmevent.hitnum=bmRawHit->GetHitsN();
    for(Int_t i=0;i<bmRawHit->GetHitsN();i++){
      TABMhit* bmhit = bmRawHit->Hit(i);
      bmevent.bm_hit_rdrift[i]=bmhit->Dist();
      bmevent.bm_hit_residual[i]=bmhit->GetResidual();
      bmevent.bm_hit_time[i]=bmhit->Tdrift();
      bmevent.bm_hit_cellid[i]=geomap->GetBMNcell(bmhit->Plane(), bmhit->View(), bmhit->Cell());
    }
  }
  
  if(debug>2){
    cout<<"bmreadevent finished"<<endl;
    print_bmevstruct(bmevent);
  }
  
  allbmeventin.push_back(bmevent);
  return;
}


void clean_bmevstruct(BM_evstruct &bmevstruct, bool forced){

  bmevstruct.hitnum=0;
  bmevstruct.bm_track_chi2=-1;
  bmevstruct.bm_track_pvers.SetXYZ(-999,-999,-999);
  bmevstruct.bm_track_r0pos.SetXYZ(-999,-999,-999);
  bmevstruct.status=-1;
  if(forced){
    for(int index=0;index<MAXBMHITNUM;index++){
      bmevstruct.bm_hit_rdrift[index]=-1;
      bmevstruct.bm_hit_residual[index]=-1;
      bmevstruct.bm_hit_time[index]=-1;
      bmevstruct.bm_hit_cellid[index]=-1;
    }
    return;
  }
  int index=0;
  while(bmevstruct.bm_hit_rdrift[index]!=-1 && index<MAXBMHITNUM){
    bmevstruct.bm_hit_rdrift[index]=-1;
    bmevstruct.bm_hit_residual[index]=-1;
    bmevstruct.bm_hit_time[index]=-1;
    bmevstruct.bm_hit_cellid[index]=-1;
    index++;
  }

return;
}


void print_bmevstruct(BM_evstruct &bmevstruct){
  cout<<"print bmevstruct"<<endl;
  cout<<"hitnum="<<bmevstruct.hitnum<<"  trackchi2="<<bmevstruct.bm_track_chi2<<"  pvers=("<<bmevstruct.bm_track_pvers.X()<<", "<<bmevstruct.bm_track_pvers.Y()<<", "<<bmevstruct.bm_track_pvers.Z()<<")  r0=("<<bmevstruct.bm_track_r0pos.X()<<", "<<bmevstruct.bm_track_r0pos.Y()<<", "<<bmevstruct.bm_track_r0pos.Z()<<"  status="<<bmevstruct.status<<endl;
  cout<<"loop on hits:"<<endl;
  for(int i=0;i<bmevstruct.hitnum;i++){
    cout<<"i="<<i<<"  cellid="<<bmevstruct.bm_hit_cellid[i]<<"  rdrift="<<bmevstruct.bm_hit_rdrift[i]<<"  residual="<<bmevstruct.bm_hit_residual[i]<<"  time="<<bmevstruct.bm_hit_time[i]<<endl;
  }  
  
return;  
}




//************************************************  BM geostuff  *******************************************
//long inefficient method in shoe, to set wire_pos and wire_dir

TVector3 BMlocaltoiso(TVector3 local){
  local.RotateX((BMISOYZANGLE)*DEG2RAD);
  local.RotateY((BMISOXZANGLE)*DEG2RAD);
  TVector3 bmshift(BMISOX, BMISOY, BMISOZ);
  return local+bmshift;
}


