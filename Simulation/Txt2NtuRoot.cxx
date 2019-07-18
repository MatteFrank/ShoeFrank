#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TString.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>

#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "Evento.hxx"
#include "TAMCevent.hxx"

using namespace std;

int main(int argc, char *argv[])
{
   
   int status = 0, iL=0, NumProcessed=0, numfiles = 0, nread=0;
   TString outname("Out.root"), inname("In.txt");
   vector<TString> infiles; TString tmpSin;
   
   ifstream lista_file;
   char fname[200],linea[200];
   FILE *pfile;
   bool ReadError = false;
   int maxevpro = 1000000000;
   int fragtrig=0;
   double Ethreshold = 0;
   
   static TTree *rootTree = 0;
   
   EVENT_STRUCT eve;
   
   ifstream infile;
   
   
   for (int i = 0; i < argc; i++){
      if(strcmp(argv[i],"-in") == 0) {
         inname = TString(argv[++i]);
      }
      if(strcmp(argv[i],"-out") == 0) {
         outname = TString(argv[++i]);
      }
      if(strcmp(argv[i],"-nev") == 0) {
         maxevpro = atoi(argv[++i]);
      }
      if(strcmp(argv[i],"-iL") == 0) { iL = 1; }
      if(strcmp(argv[i],"-help") == 0) {
         cout<<"Conversion of fluka TXT file : usage -> Txt2Root [opts] "<<endl;
         cout<<" possible opts are:"<<endl;
         cout<<"   -in  file    : [def=In.txt] Root input file"<<endl;
         cout<<"   -out  file   : [def=Out.root] Root output file"<<endl;
         cout<<"   -sel selw    : [def=0] select ev: 1*dc + 10*lyso "<<endl;
         cout<<"   -iL        : [def=none] input file is a list of files"<<endl;
         cout<<"   -nev        : [def=Inf] Max no. of events to process"<<endl;
         return 1;
      }
   }
   
   if(iL==0) {
      infiles.push_back(inname);
   } else {
      lista_file.open(inname.Data());
      cout<<"Processing data from "<<inname.Data()<<" LIST file!"<<endl;
      while (lista_file.getline(linea, 200, '\n')) {
         sscanf(linea,"%s",fname);
         cout<<"Adding "<<fname<<" to the list of files to be processed!"<<endl;
         tmpSin= fname;
         infiles.push_back(tmpSin);
      }
      lista_file.close();
   }
   numfiles = infiles.size();
   
   TFile *f_out = new TFile(outname,"RECREATE");
   f_out->cd();
   rootTree = new TTree("EventTree","gsimay");
   
   // Event
   TAMCevent* event = new TAMCevent();
   event->SetBranches(rootTree);
   
   //  rootTree->Branch("CROSSn",&eve.CROSSn,"CROSSn/I");
   //  rootTree->Branch("CROSSid",&eve.CROSSid,"CROSSid[CROSSn]/I");
   //  rootTree->Branch("CROSSnreg",&eve.CROSSnreg,"CROSSnreg[CROSSn]/I");
   //  rootTree->Branch("CROSSnregold",&eve.CROSSnregold,"CROSSnregold[CROSSn]/I");
   //  rootTree->Branch("CROSSx",&eve.CROSSx,"CROSSx[CROSSn]/D");
   //  rootTree->Branch("CROSSy",&eve.CROSSy,"CROSSy[CROSSn]/D");
   //  rootTree->Branch("CROSSz",&eve.CROSSz,"CROSSz[CROSSn]/D");
   //  rootTree->Branch("CROSSpx",&eve.CROSSpx,"CROSSpx[CROSSn]/D");
   //  rootTree->Branch("CROSSpy",&eve.CROSSpy,"CROSSpy[CROSSn]/D");
   //  rootTree->Branch("CROSSpz",&eve.CROSSpz,"CROSSpz[CROSSn]/D");
   //  rootTree->Branch("CROSSm",&eve.CROSSm,"CROSSm[CROSSn]/D");
   //  rootTree->Branch("CROSSch",&eve.CROSSch,"CROSSch[CROSSn]/D");
   //  rootTree->Branch("CROSSt",&eve.CROSSt,"CROSSt[CROSSn]/D");
   
   //    loop sui file della lista ( if any)
   
   for(int idfile=0; idfile<numfiles;idfile++){
      cout<<endl<<"Now processing data from "<<infiles.at(idfile)<<" file!"<<endl;
      ReadError = false;
      
      pfile = fopen(infiles.at(idfile),"r");
      
      nread= fscanf(pfile,"%d %lf\n",&fragtrig,&Ethreshold);
      if(nread!=2){
         ReadError= true;
         cout<<"Wrong run header: read Error!! exiting  "<<endl;
         return 1;
      }
      
      //	  loop sugli eventi del file
      
      while((!feof(pfile))&&(!ReadError)){
         NumProcessed++;
         if (maxevpro>0) {
            if (NumProcessed>maxevpro) break;
         }
         
         eve.EventNumber = 0;
         eve.TRn         = 0;
         eve.STCn        = 0;
         eve.BMNn        = 0;
         eve.VTXn        = 0;
         eve.ITRn        = 0;
         eve.MSDn        = 0;
         eve.SCNn        = 0;
         eve.CALn        = 0;
         eve.CROSSn      = 0;
         
         if(NumProcessed%10000==0){ cout<<"# event = "<<NumProcessed<<endl;}
         status = event->Clean();
         
         //	header
         
         nread= fscanf(pfile,"%d %d %d %d %d %d %d %d %d %d \n",&eve.EventNumber,
                       &eve.TRn,&eve.STCn,&eve.BMNn,&eve.VTXn,&eve.ITRn,&eve.MSDn,
                       &eve.SCNn,&eve.CALn,&eve.CROSSn);
         
         event->SetEvent(eve.EventNumber);
         
         if(nread!=10){
            cout<<"ReadError in ev header section: nread = "<<nread<<
            " instead of 10; ev= "<<NumProcessed<<endl;
            ReadError = true;
         }
         
         //	track cinematics
         
         if(!ReadError){
            for(int jj =0;jj<eve.TRn;jj++){
               nread = fscanf(pfile,
                              "%d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n ",
                              &eve.TRpaid[jj],&eve.TRgen[jj],&eve.TRcha[jj],
                              &eve.TRreg[jj],&eve.TRbar[jj],&eve.TRdead[jj],
                              &eve.TRfid[jj],&eve.TRix[jj],
                              &eve.TRiy[jj],&eve.TRiz[jj],&eve.TRfx[jj],&eve.TRfy[jj],
                              &eve.TRfz[jj],&eve.TRipx[jj],&eve.TRipy[jj],&eve.TRipz[jj],
                              &eve.TRfpx[jj],&eve.TRfpy[jj],&eve.TRfpz[jj],&eve.TRmass[jj],
                              &eve.TRtime[jj],&eve.TRtof[jj],&eve.TRtrlen[jj]);
               
               event->AddPart(eve.TRpaid[jj], eve.TRgen[jj], eve.TRcha[jj], eve.TRreg[jj], eve.TRbar[jj],
                              eve.TRdead[jj], eve.TRfid[jj],
                              TVector3(eve.TRix[jj], eve.TRiy[jj], eve.TRiy[jj]),
                              TVector3(eve.TRfx[jj], eve.TRfy[jj], eve.TRfy[jj]),
                              TVector3(eve.TRipx[jj], eve.TRipy[jj], eve.TRipy[jj]),
                              TVector3(eve.TRfpx[jj], eve.TRfpy[jj], eve.TRfpy[jj]),
                              eve.TRmass[jj], eve.TRtof[jj], eve.TRtime[jj], eve.TRtrlen[jj]);
               
               if(nread!=23){
                  cout<<"ReadError in kine section: nread = "<<nread<<
                  " instead of 23; ev= "<<NumProcessed<<endl;
                  
                  ReadError= true;
                  break;
               }
            }
         }
         
         //	Fill start counter
         
         if(!ReadError){
            for(int jj=0; jj<eve.STCn;jj++){
               nread= fscanf(pfile,
                             "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                             &eve.STCid[jj],
                             &eve.STCxin[jj],&eve.STCyin[jj],&eve.STCzin[jj],
                             &eve.STCxout[jj],&eve.STCyout[jj],&eve.STCzout[jj],
                             &eve.STCpxin[jj],&eve.STCpyin[jj],&eve.STCpzin[jj],
                             &eve.STCpxout[jj],&eve.STCpyout[jj],&eve.STCpzout[jj],
                             &eve.STCde[jj],&eve.STCal[jj],&eve.STCtim[jj]);
               
               event->AddSTC(eve.STCid[jj],
                             TVector3(eve.STCxin[jj], eve.STCyin[jj], eve.STCzin[jj]),
                             TVector3(eve.STCxout[jj], eve.STCyout[jj], eve.STCzout[jj]),
                             TVector3(eve.STCpxin[jj], eve.STCpyin[jj], eve.STCpzin[jj]),
                             TVector3(eve.STCpxout[jj], eve.STCpyout[jj], eve.STCpzout[jj]),
                             eve.STCde[jj], eve.STCal[jj], eve.STCtim[jj], 0);
               
               if(nread!=16){
                  ReadError= true;
                  cout<<"ReadError in STC: nread = "<<nread<<
                  " instead of 16; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	Fill beam monitor
         
         if(!ReadError){
            
            for(int jj=0; jj<eve.BMNn;jj++){
               nread = fscanf(pfile,
                              "%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                              &eve.BMNid[jj],&eve.BMNiview[jj],&eve.BMNilay[jj],
                              &eve.BMNicell[jj],
                              &eve.BMNxin[jj],&eve.BMNyin[jj],&eve.BMNzin[jj],
                              &eve.BMNxout[jj],&eve.BMNyout[jj],&eve.BMNzout[jj],
                              &eve.BMNpxin[jj],&eve.BMNpyin[jj],&eve.BMNpzin[jj],
                              &eve.BMNpxout[jj],&eve.BMNpyout[jj],&eve.BMNpzout[jj],
                              &eve.BMNde[jj],&eve.BMNal[jj],&eve.BMNtim[jj]);
               
                event->AddBMN(eve.BMNid[jj], eve.BMNilay[jj], eve.BMNiview[jj], eve.BMNicell[jj],
                              TVector3(eve.BMNxin[jj], eve.BMNyin[jj], eve.BMNzin[jj]),
                              TVector3(eve.BMNxout[jj], eve.BMNyout[jj], eve.BMNzout[jj]),
                              TVector3(eve.BMNpxin[jj], eve.BMNpyin[jj], eve.BMNpzin[jj]),
                              TVector3(eve.BMNpxout[jj], eve.BMNpyout[jj], eve.BMNpzout[jj]),
                              eve.BMNde[jj], eve.BMNal[jj], eve.BMNtim[jj], 0);
               
               if(nread!=19){
                  ReadError = true;
                  cout<<"ReadError in BMN section: nread = "<<nread<<
                  " instead of 19; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	Fill vertex
         
         if(!ReadError){
            
            for(int jj=0; jj<eve.VTXn;jj++){
               nread = fscanf(pfile,
                              "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                              &eve.VTXid[jj],&eve.VTXilay[jj],
                              &eve.VTXxin[jj],&eve.VTXyin[jj],&eve.VTXzin[jj],
                              &eve.VTXxout[jj],&eve.VTXyout[jj],&eve.VTXzout[jj],
                              &eve.VTXpxin[jj],&eve.VTXpyin[jj],&eve.VTXpzin[jj],
                              &eve.VTXpxout[jj],&eve.VTXpyout[jj],&eve.VTXpzout[jj],
                              &eve.VTXde[jj],&eve.VTXal[jj],&eve.VTXtim[jj]);
               
               event->AddVTX(eve.VTXid[jj], eve.VTXilay[jj],
                             TVector3(eve.VTXxin[jj], eve.VTXyin[jj], eve.VTXzin[jj]),
                             TVector3(eve.VTXxout[jj], eve.VTXyout[jj], eve.VTXzout[jj]),
                             TVector3(eve.VTXpxin[jj], eve.VTXpyin[jj], eve.VTXpzin[jj]),
                             TVector3(eve.VTXpxout[jj], eve.VTXpyout[jj], eve.VTXpzout[jj]),
                             eve.VTXde[jj], eve.VTXal[jj], eve.VTXtim[jj], 0);

               if(nread!=17){
                  ReadError = true;
                  cout<<"ReadError in VTX section: nread = "<<nread<<
                  " instead of 17; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	Fill inner tracker
         
         if(!ReadError){
            
            for(int jj=0; jj<eve.ITRn;jj++){
               nread = fscanf(pfile,
                              "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                              &eve.ITRid[jj],&eve.ITRisens[jj],
                              &eve.ITRxin[jj],&eve.ITRyin[jj],&eve.ITRzin[jj],
                              &eve.ITRxout[jj],&eve.ITRyout[jj],&eve.ITRzout[jj],
                              &eve.ITRpxin[jj],&eve.ITRpyin[jj],&eve.ITRpzin[jj],
                              &eve.ITRpxout[jj],&eve.ITRpyout[jj],&eve.ITRpzout[jj],
                              &eve.ITRde[jj],&eve.ITRal[jj],&eve.ITRtim[jj]);
               
               event->AddITR(eve.ITRid[jj], eve.ITRisens[jj], -1, -1,
                             TVector3(eve.ITRxin[jj], eve.ITRyin[jj], eve.ITRzin[jj]),
                             TVector3(eve.ITRxout[jj], eve.ITRyout[jj], eve.ITRzout[jj]),
                             TVector3(eve.ITRpxin[jj], eve.ITRpyin[jj], eve.ITRpzin[jj]),
                             TVector3(eve.ITRpxout[jj], eve.ITRpyout[jj], eve.ITRpzout[jj]),
                             eve.ITRde[jj], eve.ITRal[jj], eve.ITRtim[jj], 0);
               
               if(nread!=17){
                  ReadError = true;
                  cout<<"ReadError in ITR section: nread = "<<nread<<
                  " instead of 19; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         
         //	Fill multi strip detector
         
         if(!ReadError){
            
            for(int jj=0; jj<eve.MSDn;jj++){
               nread = fscanf(pfile,
                              "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                              &eve.MSDid[jj],&eve.MSDilay[jj],
                              &eve.MSDxin[jj],&eve.MSDyin[jj],&eve.MSDzin[jj],
                              &eve.MSDxout[jj],&eve.MSDyout[jj],&eve.MSDzout[jj],
                              &eve.MSDpxin[jj],&eve.MSDpyin[jj],&eve.MSDpzin[jj],
                              &eve.MSDpxout[jj],&eve.MSDpyout[jj],&eve.MSDpzout[jj],
                              &eve.MSDde[jj],&eve.MSDal[jj],&eve.MSDtim[jj]);
               
               event->AddMSD(eve.MSDid[jj], eve.MSDilay[jj], -1, -1,
                             TVector3(eve.MSDxin[jj], eve.MSDyin[jj], eve.MSDzin[jj]),
                             TVector3(eve.MSDxout[jj], eve.MSDyout[jj], eve.MSDzout[jj]),
                             TVector3(eve.MSDpxin[jj], eve.MSDpyin[jj], eve.MSDpzin[jj]),
                             TVector3(eve.MSDpxout[jj], eve.MSDpyout[jj], eve.MSDpzout[jj]),
                             eve.MSDde[jj], eve.MSDal[jj], eve.MSDtim[jj], 0);
               
               if(nread!=17){
                  ReadError = true;
                  cout<<"ReadError in MSD section: nread = "<<nread<<
                  " instead of 17; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	Fill scint
         
         if(!ReadError){
            for(int jj=0; jj<eve.SCNn;jj++){
               nread= fscanf(pfile,"%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                             &eve.SCNid[jj],&eve.SCNibar[jj],
                             &eve.SCNiview[jj],&eve.SCNxin[jj],
                             &eve.SCNyin[jj],&eve.SCNzin[jj],&eve.SCNxout[jj],
                             &eve.SCNyout[jj],&eve.SCNzout[jj],&eve.SCNpxin[jj],
                             &eve.SCNpyin[jj],&eve.SCNpzin[jj],&eve.SCNpxout[jj],
                             &eve.SCNpyout[jj],&eve.SCNpzout[jj],&eve.SCNde[jj],
                             &eve.SCNal[jj],&eve.SCNtim[jj]);
               
               event->AddTW(eve.SCNid[jj], eve.SCNibar[jj], eve.SCNiview[jj],
                            TVector3(eve.SCNxin[jj], eve.SCNyin[jj], eve.SCNzin[jj]),
                            TVector3(eve.SCNxout[jj], eve.SCNyout[jj], eve.SCNzout[jj]),
                            TVector3(eve.SCNpxin[jj], eve.SCNpyin[jj], eve.SCNpzin[jj]),
                            TVector3(eve.SCNpxout[jj], eve.SCNpyout[jj], eve.SCNpzout[jj]),
                            eve.SCNde[jj], eve.SCNal[jj], eve.SCNtim[jj], 0);
               
               if(nread!=18){
                  ReadError= true;
                  cout<<"ReadError in SCN: nread = "<<nread<<
                  " instead of 18; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	Fill calorimeter
         
         if(!ReadError){
            for(int jj=0; jj<eve.CALn;jj++){
               nread= fscanf(pfile,
                             "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                             &eve.CALid[jj],&eve.CALicry[jj],
                             &eve.CALxin[jj],&eve.CALyin[jj],&eve.CALzin[jj],
                             &eve.CALxout[jj],&eve.CALyout[jj],&eve.CALzout[jj],
                             &eve.CALpxin[jj],&eve.CALpyin[jj],&eve.CALpzin[jj],
                             &eve.CALpxout[jj],&eve.CALpyout[jj],&eve.CALpzout[jj],
                             &eve.CALde[jj],&eve.CALal[jj],&eve.CALtim[jj]);
               
               event->AddCAL(eve.CALid[jj], eve.CALicry[jj],
                             TVector3(eve.CALxin[jj], eve.CALyin[jj], eve.CALzin[jj]),
                             TVector3(eve.CALxout[jj], eve.CALyout[jj], eve.CALzout[jj]),
                             TVector3(eve.CALpxin[jj], eve.CALpyin[jj], eve.CALpzin[jj]),
                             TVector3(eve.CALpxout[jj], eve.CALpyout[jj], eve.CALpzout[jj]),
                             eve.CALde[jj], eve.CALal[jj], eve.CALtim[jj], 0);
               
               if(nread!=17){
                  ReadError= true;
                  cout<<"ReadError in pixel CAL section: nread = "<<nread<<
                  " instead of 17; ev= "<<NumProcessed<<endl;
                  break;
               }
            }
         }
         
         //	fill boundary crossing (not fill in G4)
         
         if(!ReadError){
            for(int jj=0; jj<eve.CROSSn;jj++){
               nread = fscanf(pfile,"%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf \n",
                              &eve.CROSSid[jj],&eve.CROSSnreg[jj],&eve.CROSSnregold[jj],&eve.CROSSx[jj],
                              &eve.CROSSy[jj],&eve.CROSSz[jj],&eve.CROSSpx[jj],
                              &eve.CROSSpy[jj],&eve.CROSSpz[jj],&eve.CROSSm[jj],
                              &eve.CROSSch[jj],&eve.CROSSt[jj]);
               if(nread!=12){
                  cout<<"ReadError in CROSS section: nread = "<<nread<<
                  " instead of 12; ev= "<<NumProcessed<<endl;
                  ReadError = true;
                  break;
               }
            }
         }
         
         
         if( ReadError){
            break;
         }
         else{
            if((eve.TRn<=MAXTR)&&(eve.STCn<=MAXSTC)&&(eve.BMNn<=MAXBMN)&&(eve.VTXn<=MAXVTX)
               &&(eve.ITRn<=MAXITR)&&(eve.SCNn<=MAXSCN)&&(eve.CALn<=MAXCAL)
               &&(eve.MSDn<=MAXMSD)&&(eve.CROSSn<=MAXCROSS)){
               rootTree->Fill() ;
            }
            else{
               cout<<ReadError<<" "<<eve.EventNumber<<" "<<eve.TRn<<" "<<eve.STCn
               <<" "<<eve.BMNn<<" "<<eve.VTXn<<" "<<eve.ITRn<<" "<<eve.MSDn
               <<" "<<eve.SCNn<<" "<<eve.CALn<<" "<<eve.CROSSn<<endl;
            }
         }
         
      }
      
      fclose(pfile);
   }
   rootTree->Write();
   
   f_out->Close();
   cout<<" total number of event safely converted= "<<NumProcessed<<endl;
   
   return 0;
   
}
