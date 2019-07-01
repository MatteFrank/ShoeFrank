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

  static TTree *RootTree = 0;
  static Evento *pEv = 0;

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


  RootTree = new TTree("EventTree","gsimay");

  RootTree->Branch("EventNumber",&eve.EventNumber,"EventNumber/I");
  RootTree->Branch("TRn",&eve.TRn,"TRn/I");
  RootTree->Branch("TRpaid",&eve.TRpaid,"TRpaid[TRn]/I");
  RootTree->Branch("TRgen",&eve.TRgen,"TRgen[TRn]/I");
  RootTree->Branch("TRcha",&eve.TRcha,"TRcha[TRn]/I");
  RootTree->Branch("TRreg",&eve.TRreg,"TRreg[TRn]/I");
  RootTree->Branch("TRbar",&eve.TRbar,"TRbar[TRn]/I");
  RootTree->Branch("TRdead",&eve.TRdead,"TRdead[TRn]/I");
  RootTree->Branch("TRfid",&eve.TRfid,"TRfid[TRn]/I");
  RootTree->Branch("TRix",&eve.TRix,"TRix[TRn]/D");
  RootTree->Branch("TRiy",&eve.TRiy,"TRiy[TRn]/D");
  RootTree->Branch("TRiz",&eve.TRiz,"TRiz[TRn]/D");
  RootTree->Branch("TRfx",&eve.TRfx,"TRfx[TRn]/D");
  RootTree->Branch("TRfy",&eve.TRfy,"TRfy[TRn]/D");
  RootTree->Branch("TRfz",&eve.TRfz,"TRfz[TRn]/D");
  RootTree->Branch("TRipx",&eve.TRipx,"TRipx[TRn]/D");
  RootTree->Branch("TRipy",&eve.TRipy,"TRipy[TRn]/D");
  RootTree->Branch("TRipz",&eve.TRipz,"TRipz[TRn]/D");
  RootTree->Branch("TRfpx",&eve.TRfpx,"TRfpx[TRn]/D");
  RootTree->Branch("TRfpy",&eve.TRfpy,"TRfpy[TRn]/D");
  RootTree->Branch("TRfpz",&eve.TRfpz,"TRfpz[TRn]/D");
  RootTree->Branch("TRmass",&eve.TRmass,"TRmass[TRn]/D");
  RootTree->Branch("TRtime",&eve.TRtime,"TRtime[TRn]/D");
  RootTree->Branch("TRtof",&eve.TRtof,"TRtof[TRn]/D");
  RootTree->Branch("TRtrlen",&eve.TRtrlen,"TRtrlen[TRn]/D");
  
  RootTree->Branch("STCn",&eve.STCn,"STCn/I");
  RootTree->Branch("STCid",&eve.STCid,"STCid[STCn]/I");
  RootTree->Branch("STCxin",&eve.STCxin,"STCxin[STCn]/D");
  RootTree->Branch("STCyin",&eve.STCyin,"STCyin[STCn]/D");
  RootTree->Branch("STCzin",&eve.STCzin,"STCzin[STCn]/D");
  RootTree->Branch("STCpxin",&eve.STCpxin,"STCpxin[STCn]/D");
  RootTree->Branch("STCpyin",&eve.STCpyin,"STCpyin[STCn]/D");
  RootTree->Branch("STCpzin",&eve.STCpzin,"STCpzin[STCn]/D");
  RootTree->Branch("STCxout",&eve.STCxout,"STCxout[STCn]/D");
  RootTree->Branch("STCyout",&eve.STCyout,"STCyout[STCn]/D");
  RootTree->Branch("STCzout",&eve.STCzout,"STCzout[STCn]/D");
  RootTree->Branch("STCpxout",&eve.STCpxout,"STCpxout[STCn]/D");
  RootTree->Branch("STCpyout",&eve.STCpyout,"STCpyout[STCn]/D");
  RootTree->Branch("STCpzout",&eve.STCpzout,"STCpzout[STCn]/D");
  RootTree->Branch("STCde",&eve.STCde,"STCde[STCn]/D");
  RootTree->Branch("STCal",&eve.STCal,"STCal[STCn]/D");
  RootTree->Branch("STCtim",&eve.STCtim,"STCtim[STCn]/D");
  
  RootTree->Branch("BMNn",&eve.BMNn,"BMNn/I");
  RootTree->Branch("BMNid",&eve.BMNid,"BMNid[BMNn]/I");
  RootTree->Branch("BMNilay",&eve.BMNilay,"BMNilay[BMNn]/I");
  RootTree->Branch("BMNiview",&eve.BMNiview,"BMNiview[BMNn]/I");
  RootTree->Branch("BMNicell",&eve.BMNicell,"BMNicell[BMNn]/I");
  RootTree->Branch("BMNxin",&eve.BMNxin,"BMNxin[BMNn]/D");
  RootTree->Branch("BMNyin",&eve.BMNyin,"BMNyin[BMNn]/D");
  RootTree->Branch("BMNzin",&eve.BMNzin,"BMNzin[BMNn]/D");
  RootTree->Branch("BMNpxin",&eve.BMNpxin,"BMNpxin[BMNn]/D");
  RootTree->Branch("BMNpyin",&eve.BMNpyin,"BMNpyin[BMNn]/D");
  RootTree->Branch("BMNpzin",&eve.BMNpzin,"BMNpzin[BMNn]/D");
  RootTree->Branch("BMNxout",&eve.BMNxout,"BMNxout[BMNn]/D");
  RootTree->Branch("BMNyout",&eve.BMNyout,"BMNyout[BMNn]/D");
  RootTree->Branch("BMNzout",&eve.BMNzout,"BMNzout[BMNn]/D");
  RootTree->Branch("BMNpxout",&eve.BMNpxout,"BMNpxout[BMNn]/D");
  RootTree->Branch("BMNpyout",&eve.BMNpyout,"BMNpyout[BMNn]/D");
  RootTree->Branch("BMNpzout",&eve.BMNpzout,"BMNpzout[BMNn]/D");
  RootTree->Branch("BMNde",&eve.BMNde,"BMNde[BMNn]/D");
  RootTree->Branch("BMNal",&eve.BMNal,"BMNal[BMNn]/D");
  RootTree->Branch("BMNtim",&eve.BMNtim,"BMNtim[BMNn]/D");
  
  RootTree->Branch("VTXn",&eve.VTXn,"VTXn/I");
  RootTree->Branch("VTXid",&eve.VTXid,"VTXid[VTXn]/I");
  RootTree->Branch("VTXilay",&eve.VTXilay,"VTXilay[VTXn]/I");
  RootTree->Branch("VTXxin",&eve.VTXxin,"VTXxin[VTXn]/D");
  RootTree->Branch("VTXyin",&eve.VTXyin,"VTXyin[VTXn]/D");
  RootTree->Branch("VTXzin",&eve.VTXzin,"VTXzin[VTXn]/D");
  RootTree->Branch("VTXpxin",&eve.VTXpxin,"VTXpxin[VTXn]/D");
  RootTree->Branch("VTXpyin",&eve.VTXpyin,"VTXpyin[VTXn]/D");
  RootTree->Branch("VTXpzin",&eve.VTXpzin,"VTXpzin[VTXn]/D");
  RootTree->Branch("VTXxout",&eve.VTXxout,"VTXxout[VTXn]/D");
  RootTree->Branch("VTXyout",&eve.VTXyout,"VTXyout[VTXn]/D");
  RootTree->Branch("VTXzout",&eve.VTXzout,"VTXzout[VTXn]/D");
  RootTree->Branch("VTXpxout",&eve.VTXpxout,"VTXpxout[VTXn]/D");
  RootTree->Branch("VTXpyout",&eve.VTXpyout,"VTXpyout[VTXn]/D");
  RootTree->Branch("VTXpzout",&eve.VTXpzout,"VTXpzout[VTXn]/D");
  RootTree->Branch("VTXde",&eve.VTXde,"VTXde[VTXn]/D");
  RootTree->Branch("VTXal",&eve.VTXal,"VTXal[VTXn]/D");
  RootTree->Branch("VTXtim",&eve.VTXtim,"VTXVTXtim[VTXn]/D");

  RootTree->Branch("ITRn",&eve.ITRn,"ITRn/I");
  RootTree->Branch("ITRid",&eve.ITRid,"ITRid[ITRn]/I");
  RootTree->Branch("ITRisens",&eve.ITRisens,"ITRisens[ITRn]/I");
  RootTree->Branch("ITRxin",&eve.ITRxin,"ITRxin[ITRn]/D");
  RootTree->Branch("ITRyin",&eve.ITRyin,"ITRyin[ITRn]/D");
  RootTree->Branch("ITRzin",&eve.ITRzin,"ITRzin[ITRn]/D");
  RootTree->Branch("ITRpxin",&eve.ITRpxin,"ITRpxin[ITRn]/D");
  RootTree->Branch("ITRpyin",&eve.ITRpyin,"ITRpyin[ITRn]/D");
  RootTree->Branch("ITRpzin",&eve.ITRpzin,"ITRpzin[ITRn]/D");
  RootTree->Branch("ITRxout",&eve.ITRxout,"ITRxout[ITRn]/D");
  RootTree->Branch("ITRyout",&eve.ITRyout,"ITRyout[ITRn]/D");
  RootTree->Branch("ITRzout",&eve.ITRzout,"ITRzout[ITRn]/D");
  RootTree->Branch("ITRpxout",&eve.ITRpxout,"ITRpxout[ITRn]/D");
  RootTree->Branch("ITRpyout",&eve.ITRpyout,"ITRpyout[ITRn]/D");
  RootTree->Branch("ITRpzout",&eve.ITRpzout,"ITRpzout[ITRn]/D");
  RootTree->Branch("ITRde",&eve.ITRde,"ITRde[ITRn]/D");
  RootTree->Branch("ITRal",&eve.ITRal,"ITRal[ITRn]/D");
  RootTree->Branch("ITRtim",&eve.ITRtim,"ITRITRtim[ITRn]/D");

  RootTree->Branch("MSDn",&eve.MSDn,"MSDn/I");
  RootTree->Branch("MSDid",&eve.MSDid,"MSDid[MSDn]/I");
  RootTree->Branch("MSDilay",&eve.MSDilay,"MSDilay[MSDn]/I");
  RootTree->Branch("MSDxin",&eve.MSDxin,"MSDxin[MSDn]/D");
  RootTree->Branch("MSDyin",&eve.MSDyin,"MSDyin[MSDn]/D");
  RootTree->Branch("MSDzin",&eve.MSDzin,"MSDzin[MSDn]/D");
  RootTree->Branch("MSDpxin",&eve.MSDpxin,"MSDpxin[MSDn]/D");
  RootTree->Branch("MSDpyin",&eve.MSDpyin,"MSDpyin[MSDn]/D");
  RootTree->Branch("MSDpzin",&eve.MSDpzin,"MSDpzin[MSDn]/D");
  RootTree->Branch("MSDxout",&eve.MSDxout,"MSDxout[MSDn]/D");
  RootTree->Branch("MSDyout",&eve.MSDyout,"MSDyout[MSDn]/D");
  RootTree->Branch("MSDzout",&eve.MSDzout,"MSDzout[MSDn]/D");
  RootTree->Branch("MSDpxout",&eve.MSDpxout,"MSDpxout[MSDn]/D");
  RootTree->Branch("MSDpyout",&eve.MSDpyout,"MSDpyout[MSDn]/D");
  RootTree->Branch("MSDpzout",&eve.MSDpzout,"MSDpzout[MSDn]/D");
  RootTree->Branch("MSDde",&eve.MSDde,"MSDde[MSDn]/D");
  RootTree->Branch("MSDal",&eve.MSDal,"MSDal[MSDn]/D");
  RootTree->Branch("MSDtim",&eve.MSDtim,"MSDtim[MSDn]/D");

  RootTree->Branch("SCNn",&eve.SCNn,"SCNn/I");
  RootTree->Branch("SCNid",&eve.SCNid,"SCNid[SCNn]/I");
  RootTree->Branch("SCNibar",&eve.SCNibar,"SCNibar[SCNn]/I");
  RootTree->Branch("SCNiview",&eve.SCNiview,"SCNiview[SCNn]/I");
  RootTree->Branch("SCNxin",&eve.SCNxin,"SCNxin[SCNn]/D");
  RootTree->Branch("SCNyin",&eve.SCNyin,"SCNyin[SCNn]/D");
  RootTree->Branch("SCNzin",&eve.SCNzin,"SCNzin[SCNn]/D");
  RootTree->Branch("SCNpxin",&eve.SCNpxin,"SCNpxin[SCNn]/D");
  RootTree->Branch("SCNpyin",&eve.SCNpyin,"SCNpyin[SCNn]/D");
  RootTree->Branch("SCNpzin",&eve.SCNpzin,"SCNpzin[SCNn]/D");
  RootTree->Branch("SCNxout",&eve.SCNxout,"SCNxout[SCNn]/D");
  RootTree->Branch("SCNyout",&eve.SCNyout,"SCNyout[SCNn]/D");
  RootTree->Branch("SCNzout",&eve.SCNzout,"SCNzout[SCNn]/D");
  RootTree->Branch("SCNpxout",&eve.SCNpxout,"SCNpxout[SCNn]/D");
  RootTree->Branch("SCNpyout",&eve.SCNpyout,"SCNpyout[SCNn]/D");
  RootTree->Branch("SCNpzout",&eve.SCNpzout,"SCNpzout[SCNn]/D");
  RootTree->Branch("SCNde",&eve.SCNde,"SCNde[SCNn]/D");
  RootTree->Branch("SCNal",&eve.SCNal,"SCNal[SCNn]/D");
  RootTree->Branch("SCNtim",&eve.SCNtim,"SCNtim[SCNn]/D");
  
  RootTree->Branch("CALn",&eve.CALn,"CALn/I");
  RootTree->Branch("CALid",&eve.CALid,"CALid[CALn]/I");
  RootTree->Branch("CALicry",&eve.CALicry,"CALicry[CALn]/I");
  RootTree->Branch("CALxin",&eve.CALxin,"CALxin[CALn]/D");
  RootTree->Branch("CALyin",&eve.CALyin,"CALyin[CALn]/D");
  RootTree->Branch("CALzin",&eve.CALzin,"CALzin[CALn]/D");
  RootTree->Branch("CALpxin",&eve.CALpxin,"CALpxin[CALn]/D");
  RootTree->Branch("CALpyin",&eve.CALpyin,"CALpyin[CALn]/D");
  RootTree->Branch("CALpzin",&eve.CALpzin,"CALpzin[CALn]/D");
  RootTree->Branch("CALxout",&eve.CALxout,"CALxout[CALn]/D");
  RootTree->Branch("CALyout",&eve.CALyout,"CALyout[CALn]/D");
  RootTree->Branch("CALzout",&eve.CALzout,"CALzout[CALn]/D");
  RootTree->Branch("CALpxout",&eve.CALpxout,"CALpxout[CALn]/D");
  RootTree->Branch("CALpyout",&eve.CALpyout,"CALpyout[CALn]/D");
  RootTree->Branch("CALpzout",&eve.CALpzout,"CALpzout[CALn]/D");
  RootTree->Branch("CALde",&eve.CALde,"CALde[CALn]/D");
  RootTree->Branch("CALal",&eve.CALal,"CALal[CALn]/D");
  RootTree->Branch("CALtim",&eve.CALtim,"CALtim[CALn]/D");

  RootTree->Branch("CROSSn",&eve.CROSSn,"CROSSn/I");
  RootTree->Branch("CROSSid",&eve.CROSSid,"CROSSid[CROSSn]/I");
  RootTree->Branch("CROSSnreg",&eve.CROSSnreg,"CROSSnreg[CROSSn]/I");
  RootTree->Branch("CROSSnregold",&eve.CROSSnregold,"CROSSnregold[CROSSn]/I");
  RootTree->Branch("CROSSx",&eve.CROSSx,"CROSSx[CROSSn]/D");
  RootTree->Branch("CROSSy",&eve.CROSSy,"CROSSy[CROSSn]/D");
  RootTree->Branch("CROSSz",&eve.CROSSz,"CROSSz[CROSSn]/D");
  RootTree->Branch("CROSSpx",&eve.CROSSpx,"CROSSpx[CROSSn]/D");
  RootTree->Branch("CROSSpy",&eve.CROSSpy,"CROSSpy[CROSSn]/D");
  RootTree->Branch("CROSSpz",&eve.CROSSpz,"CROSSpz[CROSSn]/D");
  RootTree->Branch("CROSSm",&eve.CROSSm,"CROSSm[CROSSn]/D");
  RootTree->Branch("CROSSch",&eve.CROSSch,"CROSSch[CROSSn]/D");
  RootTree->Branch("CROSSt",&eve.CROSSt,"CROSSt[CROSSn]/D");

  pEv = new Evento();

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
      status = pEv->Clean();

      //	leggo l'header

      nread= fscanf(pfile,"%d %d %d %d %d %d %d %d %d %d \n",&eve.EventNumber,
		    &eve.TRn,&eve.STCn,&eve.BMNn,&eve.VTXn,&eve.ITRn,&eve.MSDn,
		    &eve.SCNn,&eve.CALn,&eve.CROSSn);
      if(nread!=10){
	cout<<"ReadError in ev header section: nread = "<<nread<<
	  " instead of 10; ev= "<<NumProcessed<<endl;
	ReadError = true;
      }
      
      //	leggo il common della cinematica

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
	  if(nread!=23){
	    cout<<"ReadError in kine section: nread = "<<nread<<
	      " instead of 23; ev= "<<NumProcessed<<endl;
	    
	    ReadError= true;
	    break;
	  }
	}
      }

      //	leggo i rilasci nello start counter

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
	  if(nread!=16){
	    ReadError= true;
	    cout<<"ReadError in STC: nread = "<<nread<<
	      " instead of 16; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }

      //	leggo i rilasci nella prima drift chamber

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
	  if(nread!=19){
	    ReadError = true;
	    cout<<"ReadError in BMN section: nread = "<<nread<<
	      " instead of 19; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }
      
      //	leggo i rilasci nel vertice

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
	  if(nread!=17){
	    ReadError = true;
	    cout<<"ReadError in VTX section: nread = "<<nread<<
	      " instead of 17; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }

      //	leggo i rilasci nell'inner tracker

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
	  if(nread!=17){
	    ReadError = true;
	    cout<<"ReadError in ITR section: nread = "<<nread<<
	      " instead of 19; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }


      //	leggo i rilasci nella seconda drift chamber

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
	  if(nread!=17){
	    ReadError = true;
	    cout<<"ReadError in MSD section: nread = "<<nread<<
	      " instead of 17; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }

      //	leggo i rilasci nello scint

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
	  if(nread!=18){
	    ReadError= true;
	    cout<<"ReadError in SCN: nread = "<<nread<<
	      " instead of 18; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }

      //	leggo i rilasci nel calorimetro

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
	  if(nread!=17){
	    ReadError= true;
	    cout<<"ReadError in pixel CAL section: nread = "<<nread<<
	      " instead of 17; ev= "<<NumProcessed<<endl;
	    break;
	  }
	}
      }

      //	leggo i boundary crossing

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
	  RootTree->Fill() ;
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
  RootTree->Write();
  
  f_out->Close();
  cout<<" total number of event safely converted= "<<NumProcessed<<endl;

  return 0;
  
}
