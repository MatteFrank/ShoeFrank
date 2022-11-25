#include "TAGwaveCatcher.hxx"
#include "TAxis.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include <vector>

ClassImp(TAGwaveCatcher);

//------------------------------------------+-----------------------------------
TAGwaveCatcher::TAGwaveCatcher()
 : TObject(),
   fSamplesN(0),
   fVectT(0x0),
   fVectA(0x0)
{
}

//------------------------------------------+-----------------------------------
void TAGwaveCatcher::Reserve(Int_t sample)
{
   fSamplesN = sample;
   fVectT = new float[sample];
   fVectA = new float[sample];
}

//------------------------------------------+-----------------------------------
void TAGwaveCatcher::Clear(Option_t * /*option*/)
{
   memset(fVectT, 0, sizeof(float)*fSamplesN);
   memset(fVectA, 0, sizeof(float)*fSamplesN);
}

//------------------------------------------+-----------------------------------
TAGwaveCatcher::TAGwaveCatcher(const TAGwaveCatcher &other)
{
   fChannelId = other.fChannelId;
   fSamplesN  = other.fSamplesN;
   
   fVectT = new float[other.fSamplesN];
   fVectA = new float[other.fSamplesN];
   
   memcpy(fVectT, other.fVectT, sizeof(float)*fSamplesN);
   memcpy(fVectA, other.fVectA, sizeof(float)*fSamplesN);

}

//------------------------------------------+-----------------------------------
void TAGwaveCatcher::SetWave(const TAGwaveCatcher &other)
{
   fChannelId = other.fChannelId;
   fSamplesN  = other.fSamplesN;
   
   fVectT = new float[other.fSamplesN];
   fVectA = new float[other.fSamplesN];
   
   memcpy(fVectT, other.fVectT, sizeof(float)*fSamplesN);
   memcpy(fVectA, other.fVectA, sizeof(float)*fSamplesN);
}

//------------------------------------------+-----------------------------------
TAGwaveCatcher::~TAGwaveCatcher()
{
  delete[] fVectT;
  delete[] fVectA;
}


