
#if defined(__CLING__)
   // ROOT6-specific code here ...
#include "TString.h"
#include "TAMSDparGeo.hxx"

#endif

// Convert pedestal files from standalone pgm into pedestal file for SHOE
///////////////////////////////////////////////////////////////////////////
// main
void MergeMsdPed(TString fileOutName = "TAMSD_Pedestal.cal", Int_t runNumber = 3876)
{
   Int_t numFiles = 6;

   FILE* fp = fopen(fileOutName.Data(), "w");
  
   // sigma noise level
   Double_t sigLevel[] = {6, 7, 5, 6, 5, 6};
   fprintf(fp,"# SigmaNoiseLevel\n");

   for (Int_t i = 0; i < numFiles; ++i) {
      fprintf(fp,"#sensorId: %d\n", i);
      fprintf(fp,"%.0f\n", sigLevel[i]);
   }
   fprintf(fp,"############\n");

   // pedestal values
   fprintf(fp,"#senorId stripId AsicId AsicCh Mean Sigma status\n");

   Char_t line[255];
   for (Int_t i = 0; i < numFiles; ++i) {
      Int_t stripId[640];
      Int_t AsicId[640];
      Int_t AsicCh[640];
      Float_t Mean[640];
      Float_t RSigma[640];
      Float_t Sigma[640];
      Int_t status[640];
      Float_t tmp;

      Int_t boardId = i / 2;
      Int_t sideId  = i % 2;
      TString name = Form("FOOT_%d_board-%d_side-%d.cal", runNumber, boardId, sideId);
      FILE* fpin = fopen(name.Data(), "r");
      
      // skip 18 lines
      for (Int_t k = 0; k < 18; ++k)
         fgets(line,255,fpin);
      
      Int_t sensorId = TAMSDparGeo::GetSensorId(boardId, sideId);
      fprintf(fp,"#sensorId: %d\n", sensorId);

      for (Int_t k = 0; k < 640; ++k) {
         fgets(line,255,fpin);
         sscanf(line, "%d, %d, %d, %f, %f, %f, %d, %f", &stripId[k], &AsicId[k], &AsicCh[k], &Mean[k], &RSigma[k], &Sigma[k], &status[k], &tmp);
         printf("%d %d %d %d %f %f %d\n", sensorId, stripId[k], AsicId[k], AsicCh[k], Mean[k], Sigma[k], status[k]);
         fprintf(fp, "%2d %3d %2d %2d %5.1f %3.1f %d\n", sensorId, stripId[k], AsicId[k], AsicCh[k], Mean[k], Sigma[k], status[k]);
      }
   }
   
  fclose(fp);
}

