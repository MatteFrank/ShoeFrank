

#if defined(__CLING__)
#include "TString.h"

#endif


///////////////////////////////////////////////////////////////////////////
// main
void WriteItrMap(TString fileOutName = "TAITdetector.map")
{
   FILE* fp = fopen(fileOutName.Data(), "w");
   
   Int_t dataLinksN = 8;
   Int_t sensorsN = 4;
   
   fprintf(fp,"# Mapping planeId (SW) sensorId (HW)\n");
   fprintf(fp,"#\n");
   fprintf(fp,"DataLinksN: %d\n", dataLinksN);
   
   for (Int_t l = 1; l <= dataLinksN; ++l) {
      fprintf(fp,"#\n");
      fprintf(fp,"DataLink: %d\n", l);
      fprintf(fp,"SensorsN: %d\n",sensorsN);

      for (Int_t i = 0; i < sensorsN; ++i) {
         Int_t sensorId = i+4*(l-1);
         Int_t planeId = i+4*(l-1);
         fprintf(fp,"PlaneId  %2d: %2d\n", planeId, sensorId);
      }
   }
   fclose(fp);
}

