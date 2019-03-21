

#include "TTree.h"

#include "LocalRecoData.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TABMntuRaw.hxx"



ClassImp(LocalRecoData)




//__________________________________________________________
LocalRecoData::LocalRecoData(TString fileNameIn, TString fileNameout)
 : BaseLocalReco(fileNameIn, fileNameout),
   fTree(0x0),
   fActEvtReader(0x0)
{
   fEvtStruct = new EVENT_STRUCT;
}




//__________________________________________________________
LocalRecoData::~LocalRecoData()
{
   // default destructor
   // if (fActEvtReader) delete fActEvtReader;
   if (fActEvtReader) delete fActEvtReader;
}




//__________________________________________________________
void LocalRecoData::LoopEvent(Int_t nEvents)
{
   // if (nEvents == 0)
   //    nEvents = fTree->GetEntries();
   
   // if (nEvents > fTree->GetEntries())
   //    nEvents = fTree->GetEntries();
   
   // for (Long64_t ientry = 0; ientry < nEvents; ientry++) {
      
   //    fTree->GetEntry(ientry);
      
   //    if(ientry % 100 == 0)
   //       cout<<" Loaded Event:: " << ientry << endl;
      
   //    if (!fTAGroot->NextEvent()) break;
   // }
}




//__________________________________________________________
void LocalRecoData::CreateRawAction() {
   
   
   m_daqEvent    = new TAGdataDsc("daqEvt", new TAGdaqEvent());
   TString parFileName;
   
   m_parGeo_TG = new TAGparaDsc(TAGparGeo::GetDefParaName(), new TAGparGeo());
   TAGparGeo* fpParGeo = (TAGparGeo*)m_parGeo_TG->Object();
   fpParGeo->FromFile();   

   if (GlobalPar::GetPar()->IncludeST()) {
     m_parMap_ST = new TAGparaDsc("parMap_ST", new TASTparMap());
     TASTparMap* fpParMapSt = (TASTparMap*) m_parMap_ST->Object();
     parFileName="./geomaps/tr_ch.map";
     fpParMapSt->FromFile(parFileName);
     
     m_datRaw_ST = new TAGdataDsc("datRaw_ST", new TASTdatRaw()); 
     m_actDatRaw_ST = new TASTactDatRaw( "actDatRaw_ST", m_datRaw_ST,m_daqEvent,m_parMap_ST);
   }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      m_datRaw_BM = new TAGdataDsc("datRaw_BM", new TABMdatRaw()); 
      m_ntuRaw_BM = new TAGdataDsc("ntuRaw_BM", new TABMntuRaw()); 
      m_ntuTrack_BM = new TAGdataDsc("ntuTrack_BM", new TABMntuTrack()); 
  
     m_parGeo_BM    = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
     TABMparGeo* fpParGeoBm   = (TABMparGeo*) m_parGeo_BM->Object();
     fpParGeoBm->FromFile();
  
      m_parCon_BM  = new TAGparaDsc("parCon_BM", new TABMparCon());
      TABMparCon* fpParConfBm = (TABMparCon*)m_parCon_BM->Object();
      parFileName = "./config/beammonitor.cfg";
      fpParConfBm->FromFile(parFileName.Data());
      parFileName = "./config/bmreso_vs_r.root";
      fpParConfBm->LoadReso(parFileName);
   
      m_parMap_BM  = new TAGparaDsc("parMap_BM", new TABMparMap());
      TABMparMap*  fpParMap_BM = (TABMparMap*)m_parMap_BM->Object();
      parFileName = "./geomaps/";
      parFileName += fpParConfBm->GetParmapfile();
      fpParMap_BM->FromFile(parFileName.Data(), fpParGeoBm);
    

      // TABMactVmeReader // later...
      m_actDatRaw_BM = new TABMactDatRaw( "actDatRaw_BM", m_datRaw_BM, m_daqEvent, m_parMap_BM, m_parCon_BM, m_parGeo_BM, m_datRaw_ST);
      m_actNtuRaw_BM = new TABMactNtuRaw( "actNtuRaw_BM", m_ntuRaw_BM, m_datRaw_BM, m_datRaw_ST, m_parGeo_BM, m_parCon_BM );
      m_actNtuTrack_BM = new TABMactNtuTrack( "actNtuTrack_BM",m_ntuTrack_BM, m_ntuRaw_BM,  m_parGeo_BM, m_parCon_BM, m_parGeo_TG );
      
      if (fFlagHisto) {
         // m_actDatRaw->CreateHistogram();
         m_actNtuRaw_BM->CreateHistogram();
         // m_actNtuTrack->CreateHistogram();
      }

   }
   
   if (GlobalPar::GetPar()->IncludeVertex()) {   }
   
   
   if(GlobalPar::GetPar()->IncludeTW()) {}
   
   if(GlobalPar::GetPar()->IncludeCA()) {}
   


}

//__________________________________________________________
void LocalRecoData::OpenFileIn()
{

   fActEvtReader = new TAGactDaqReader("data_test.00001201.physics_foot.daq.RAW._lb0000._EB-RCD._0001.data.moved");
   // fActEvtReader = new TAGactDaqReader(fileNameIn);

   // fActEvtReader->SetupChannel(m_daqEvent);
   fActEvtReader->Open(GetName());

   // fActEvtReader = new TFile(GetName());
   // fTree = (TTree*)fActEvtReader->Get("EventTree");
   
   // Evento *ev  = new Evento();
   // ev->FindBranches(fTree, fEvtStruct);
}

//__________________________________________________________
void LocalRecoData::SetRawHistogramDir()
{

   // BM
   if (GlobalPar::GetPar()->IncludeBM())
      m_actNtuRaw_BM->SetHistogramDir((TDirectory*)fActEvtWriter->File());

}

//__________________________________________________________
void LocalRecoData::CloseFileIn()
{
   fActEvtReader->Close();
}

//__________________________________________________________
void LocalRecoData::AddRawRequiredItem()  {

   // fTAGroot->AddRequiredItem("eveActNtuMc");
   
   // if (GlobalPar::GetPar()->IncludeST())
   //    AddRequiredMcItemSt();
   
   if (GlobalPar::GetPar()->IncludeBM())
      AddRequiredMcItemBm();
   
   
   // if (GlobalPar::GetPar()->IncludeTW())
   //    AddRequiredMcItemTw();

}
// //__________________________________________________________
// void LocalRecoData::AddRequiredMcItemSt()
// {
//    fTAGroot->AddRequiredItem("stActNtuMc");
// }

//__________________________________________________________
void LocalRecoData::AddRequiredMcItemBm()
{
   fTAGroot->AddRequiredItem("bmActNtuMc");
}


// //__________________________________________________________
// void LocalRecoData::AddRequiredMcItemTw()
// {
//    fTAGroot->AddRequiredItem("twActNtuMc");
// }


//__________________________________________________________
void LocalRecoData::SetTreeBranches(){

   BaseLocalReco::SetTreeBranches();
   
   // fActEvtWriter->SetupElementBranch(fpNtuMcEve, TAMCntuEve::GetBranchName());

   // if (GlobalPar::GetPar()->IncludeST()) {
   //    fActEvtWriter->SetupElementBranch(fpNtuRawSt, TASTntuRaw::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcSt, TAMCntuHit::GetStcBranchName());
   // }
   
   if (GlobalPar::GetPar()->IncludeBM()) {
      // fActEvtWriter->SetupElementBranch(fpNtuRawBm, TABMntuRaw::GetBranchName());
      // fActEvtWriter->SetupElementBranch(fpNtuMcBm, TAMCntuHit::GetBmBranchName());
   }
   
   // if (GlobalPar::GetPar()->IncludeVertex()) {
   //    if (fFlagHits)
   //       fActEvtWriter->SetupElementBranch(fpNtuRawVtx, TAVTntuRaw::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcVt, TAMCntuHit::GetVtxBranchName());
   // }
   
   // if (GlobalPar::GetPar()->IncludeInnerTracker()) {
   //    if (fFlagHits)
   //       fActEvtWriter->SetupElementBranch(fpNtuRawIt, TAITntuRaw::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcIt, TAMCntuHit::GetItrBranchName());
   // }
   
   // if (GlobalPar::GetPar()->IncludeMSD()) {
   //    if (fFlagHits)
   //       fActEvtWriter->SetupElementBranch(fpNtuRawMsd, TAMSDntuRaw::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcMsd, TAMCntuHit::GetMsdBranchName());
   // }
   
   // if (GlobalPar::GetPar()->IncludeTW()) {
   //    if (fFlagHits)
   //       fActEvtWriter->SetupElementBranch(fpNtuRawTw, TATW_ContainerHit::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcTw, TAMCntuHit::GetTofBranchName());
   // }
   
   // if (GlobalPar::GetPar()->IncludeCA()) {
   //    if (fFlagHits)
   //       fActEvtWriter->SetupElementBranch(fpNtuRawCa, TACAntuRaw::GetBranchName());
   //    fActEvtWriter->SetupElementBranch(fpNtuMcCa, TAMCntuHit::GetCalBranchName());
   // }
}

