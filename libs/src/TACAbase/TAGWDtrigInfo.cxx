#include <sys/time.h>
#include <math.h>

#include "TAGWDtrigInfo.hxx"

ClassImp(TAGWDtrigInfo);

TString  TAGWDtrigInfo::fgkBranchName = "WDtrigInfo.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGWDtrigInfo::TAGWDtrigInfo()
: TAGdata()
{

  fWDruntime=0;
  fWDprevruntime=0;
  fTriggerID=-1;
  for(int i=0;i<NMAXTRIG;i++){
    fTriggersStatus[i]=0;
    fTriggersCounter[i]=0;
    fTriggersPrevCounter[i]=0;
  }
  for(int i=0;i<NMONTRIG;i++){
    for(int j=0;j<NCLK;j++){
      fTriggersTiming[i][j]=0;
    }
  }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGWDtrigInfo::~TAGWDtrigInfo()
{
}


void TAGWDtrigInfo::AddInfo(int tbo, int triggerID, int nbanks, vector<uint32_t> words)
{

  //  fTriggerID = triggerID;

  
  int TGEN[NMAXTRIG+1];  
  
  int iW=0;


  for(int ibank=0;ibank<nbanks;ibank++){
    if(words.at(iW) == TRGI_BANK_HEADER){
      iW++;
      int size =  words.at(iW);
      //printf("TRGI size%08x\n",words.at(iW));
      int cnt=0;
      iW++;
      for(int i=0;i<size;i++){
	int word= words.at(iW);
	//printf("TRGI word%08x\n",words.at(iW));
	switch(i){
	case 0:
	  fTriggerID= word & 0xff;
	  break;
	case 1:
	  for(int iTrig=0;iTrig<32;iTrig++){
	    int bit = (word >> iTrig) &0x1;
	    fTriggersStatus[iTrig] = bit;
	  }
	  break;
	case 2:
	  for(int iTrig=0;iTrig<32;iTrig++){
	    int bit = (word >> iTrig) &0x1;
	    fTriggersStatus[iTrig+32] = bit;
	  }
	  break;
	case 3:
	  fWDruntime = (double)(word*1E-6);
	  break;
	case 5:
	  fWDtrigNum =word;
	  break;
	default:
	  break;
	}
	iW++;
      }
    }else if(words.at(iW) == TGEN_BANK_HEADER){
      iW++;
      int size = words.at(iW);;
      //printf("TGEN size%08x\n",words.at(iW));
      iW++;
      for(int i=0;i<size;i++){
	TGEN[i] = words.at(iW);
	//printf("iw::%d  TGEN word%08x\n",iW, words.at(iW));
	iW++;
      }
    }else if(words.at(iW) ==  TRGC_BANK_HEADER){
      iW++;
      int size = words.at(iW);
      //printf("TRCG size%08x\n",words.at(iW));
      iW++;
      for(int i=0;i<size;i++){
	fTriggersCounter[i] = words.at(iW);
	iW++;
      }
    }
  }
  
  
  
  uint64_t TriggerGenerationBin[NCLK];
  for(unsigned int ibin = 0; ibin<32; ibin++){
    TriggerGenerationBin[(ibin + 32 - TGEN[0]) % 32] = TGEN[ibin+1]; // bit 31:0
  }
  for(unsigned int ibin = 0; ibin<NCLK; ibin++){
    TriggerGenerationBin[(ibin + 32 - TGEN[0]) % 32] = (((uint64_t)TGEN[ibin+33])<<32) | TriggerGenerationBin[(ibin + 32 - TGEN[0]) % 32]; // bit 63:32
  }
  
  uint64_t iTriggerGenerationBin = 0;
  uint64_t TrigOr = 0;
  for(unsigned int iclktick=0; iclktick<NCLK; iclktick++){
    iTriggerGenerationBin = TriggerGenerationBin[iclktick];
    TrigOr = TrigOr | iTriggerGenerationBin;
    int bit[NMONTRIG]={0,1,2,3,4,5,6,7,8,9,10,11,12,63};
    for(int ialgo=0; ialgo<NMONTRIG; ialgo++){
      if(((iTriggerGenerationBin >> bit[ialgo])&0x1 )== 1){
	fTriggersTiming[ialgo][iclktick] = 1;
      }else{
	fTriggersTiming[ialgo][iclktick] = 0;
      }
    }
    TriggerGenerationBin[iclktick] = 0; //auto reset ouput
    /*
     TriggerGenerationBin >> 0)  // MARGOR
     TriggerGenerationBin >> 1)  // MARG MAJ
     TriggerGenerationBin >> 2)  // MARG MAJ REG
     TriggerGenerationBin >> 3)  // TOFTRG
     TriggerGenerationBin >> 4)  // CALOOR
     TriggerGenerationBin >> 5)  // NEUTRON
     TriggerGenerationBin >> 6)  // INTERSPILL TRG?
     TriggerGenerationBin >> 7)  // TRIGBACKTRG ?
     TriggerGenerationBin >> 8)  // FRAGTOFREG
     TriggerGenerationBin >> 9)  // FRAGVETO
     TriggerGenerationBin >>10)  // FRAGTOFROMEREG
     TriggerGenerationBin >>11)  // FRAGVETOROME
     TriggerGenerationBin >>12)  // FRAGTOFREGT1
     TriggerGenerationBin >>63)  // TRIGGER
    */
  }

  
  if(fWDtrigNum>0){
    fMajRate = (fTriggersCounter[40]-fTriggersPrevCounter[40])/(fWDruntime-fWDprevruntime);
  }

  // cout << "t::" << fWDruntime << "  rate::" << fMajRate << "  counter::" << fTriggersCounter[40] << endl;
  
  for(int i=0;i<NMAXTRIG;i++)fTriggersPrevCounter[i] = fTriggersCounter[i];
  fWDprevruntime= fWDruntime;
    
  return;
  
}



//------------------------------------------+-----------------------------------
//! Clear.
void TAGWDtrigInfo::Clear(Option_t*)
{

  TAGdata::Clear();

  fTriggerID=-1;
  for(int i=0;i<NMAXTRIG;i++)fTriggersStatus[i]=0;
  for(int i=0;i<NMONTRIG;i++){
    for(int j=0;j<NCLK;j++){
    fTriggersTiming[i][j]=0;
    }
  }
  
}


/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TAGWDtrigInfo::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGWDtrigInfo"
  << endl;
}

