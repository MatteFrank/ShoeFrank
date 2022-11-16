
#ifndef TAGwaveCatcher_HXX
#define TAGwaveCatcher_HXX

#include "TObject.h"

#include <vector>

using namespace std;

// this class holds the waveform, channel id of the WD and the BoardId
class TAGwaveCatcher : public TObject
{
   
public:
   TAGwaveCatcher();
   ~TAGwaveCatcher();
   TAGwaveCatcher(const TAGwaveCatcher &other);
   
   void SetWave(const TAGwaveCatcher &other);

   void Clear(Option_t * option ="");
   
   void Reserve(Int_t sample);
   
   void SetChannelId(Int_t c)      { fChannelId = c;     }
   void SetSamplesN(int value)     { fSamplesN = value;  }
   
   Int_t GetChannelId()      const { return fChannelId;  }
   Int_t GetSamplesN()       const { return fSamplesN;   }
   
   float& GetVectT(Int_t i)        { return fVectT[i];   }
   float& GetVectA(Int_t i)        { return fVectA[i];   }

   float* GetVectT()               { return fVectT;      }
   float* GetVectA()               { return fVectA;      }

private:
   Int_t  fChannelId;
   Int_t  fSamplesN;
   
   float* fVectT; // [fSamplesN]
   float* fVectA; // [fSamplesN]
   
   ClassDef(TAGwaveCatcher,1);
};

#endif
