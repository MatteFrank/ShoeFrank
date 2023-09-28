#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TChain.h>
#include <TMacro.h>
#include <iostream>
#include <fstream>
#include <TFileMerger.h>

#include "TAMCntuPart.hxx"
#include "TAMCntuHit.hxx"

#include "TASTparGeo.hxx"
#include "TASTntuHit.hxx"

#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"

#include "TATWparGeo.hxx"
#include "TATWntuPoint.hxx"

#include "TAGcampaignManager.hxx"
#include "TAGgeoTrafo.hxx"

#endif

// main
//! \param[in] nameFileList name of the file with the list of .root files to be merged
//! \param[in] forced set to true to skip all the runinfo checks
void MergeShoeOutput(TString nameFileList = "filelist.txt", bool forced = false)

{

    TString nameOut("mergedTree.root");
    string line;
    ifstream myfile(nameFileList.Data());
    TTree::SetMaxTreeSize( 1000000000000LL ); // 1 TB
    if (myfile.is_open())
    {   
        // gDebug = 5;
        string filename;
        TFile *file_out = new TFile(nameOut, "RECREATE");
        int processedfile = 0;
        TFileMerger *merger=new TFileMerger();
        merger->OutputFile(nameOut);
        // merger->AddObjectNames("runinfo");    
        TAGrunInfo *firstruninfo;   
        vector<Short_t> runnumbers;     
        while (myfile.good() && !myfile.eof())
        {
            //   getline(myfile,filename);
            myfile >> filename;
            cout << "processedfile=" << processedfile << "  adding:" << filename << endl;
            TString currfilename(filename);
            TFile *file_in = new TFile(currfilename);
            if (file_in->IsOpen() == false)
            {
                cout << "ERROR: I cannot open the input file:" << filename << " I'll skip this file" << endl;
                continue;
            }
            TAGrunInfo *currruninfo = (TAGrunInfo *)(file_in->Get("runinfo;1"));
            if (processedfile == 0)
            {
                firstruninfo = (TAGrunInfo *)(currruninfo->Clone("firstruninfo"));
                runnumbers.push_back(firstruninfo->RunNumber());
            }else{
                Int_t status = firstruninfo->IsEqualExceptRunnumber(*currruninfo);
                if (status == -1)
                {
                    cout << "the runinfo of the first and the current file are different" << endl;
                    if (!forced)
                    {
                        cout << "I'll skip this file" << endl;
                        continue;
                    }
                }else
                  runnumbers.push_back(((TAGrunInfo *)(file_in->Get("runinfo;1")))->RunNumber());
            }
            TTree *tree = (TTree *)file_in->Get("tree");
            if (tree == nullptr)
            {
                tree = (TTree *)file_in->Get("EventTree");
                if (tree == nullptr)
                {
                    cout << "ERROR: your input file: " << filename << " do not contains a tree, I'll skip this file" << endl;
                    continue;
                }
            }
            merger->AddFile(currfilename);
            processedfile++;
        }
        merger->PartialMerge(TFileMerger::kAll| TFileMerger::kSkipListed);
        myfile.close();
        TString fullrunname="number of runs included in this file:";
        for(Int_t i=0;i<runnumbers.size();i++){
          fullrunname+=" ";
          fullrunname+=runnumbers.at(i);
        }
        TFile *file_reout = new TFile(nameOut,"update");
        TMacro m("fullrunname");
        m.AddLine(fullrunname);
        m.Write();
        file_reout->Close();
    }else{
        cout << "Unable to open " << nameFileList.Data() << endl;
        cout << "This program will be ended" << endl;
        return;
    }


}
