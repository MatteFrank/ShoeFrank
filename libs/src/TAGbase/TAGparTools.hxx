#ifndef _TAGparTools_HXX
#define _TAGparTools_HXX
/*!
  \file
  \brief   Declaration of TAGparTools.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TArrayC.h"
#include "TArrayI.h"
#include "TVector3.h"
#include "TObjArray.h"

#include "TAGpara.hxx"
using namespace std;

class TGeoHMatrix;
class TGeoCombiTrans;
//##############################################################################

class TAGparTools : public TAGpara {
      
protected:
   ifstream fFileStream; // parameter file
   TString  fFileName;   // name of the file

   TObjArray* fMatrixList;       //! list of transformation matrices  (rotation+translation for each crystal)
   TVector3   fCurrentPosition;  // current position
   
public:
   TAGparTools();
   virtual ~TAGparTools();
   
   Bool_t  Open(const TString& name);
   Bool_t  Eof();
   Bool_t  Close();
   void    ReadItem(TString& item);
   void    ReadItem(TString& key, TString& item);
   void    ReadItem(Char_t& item);
   void    ReadItem(Int_t &arg);
   void    ReadItem(Float_t &arg);
   void    ReadItem(TArrayC& array, const Char_t delimiter = ';');
   void    ReadItem(TArrayI& array, const Char_t delimiter = ';');
   void    ReadStringsInts(TString& aString, TArrayI& array,  const Char_t delimiter1 = ':', const Char_t delimiter2 = ';');
   void    ReadItem(Double_t* coeff, Int_t size, const Char_t delimiter = ' ', Bool_t keyFlag = true);
   void    ReadItem(map< pair<int, int>, int>& map, const Char_t delimiter1 = ',', const Char_t delimiter2 = ';');
   void    ReadVector3(TVector3 &arg);
   void    ReadStrings(TString& aString);
   void    GetRange(const char* str, Int_t& begin, Int_t& end, Int_t& incr, Int_t& n);
   void    FillArray(TString& s, TArrayC& array);
   void    FillArray(TString& s, TArrayI& array);

   TString Normalize(const char* line);
   
   void    SetupMatrices(Int_t size);

   //! Get file name
   TString GetFileName() const { return fFileName; }
   
   //! Transform point from the global detector reference frame
   //! to the local sensor reference frame of the detection id
   void            MasterToLocal(Int_t detID,  Double_t xg, Double_t yg, Double_t zg,
                                   Double_t& xl, Double_t& yl, Double_t& zl) const;
   
   TVector3        MasterToLocal(Int_t detID, TVector3& glob) const;
   TVector3        MasterToLocalVect(Int_t detID, TVector3& glob) const;
   
   //! Transform point from the local reference frame
   //! of the detection id to the global reference frame
   void            LocalToMaster(Int_t detID,  Double_t xl, Double_t yl, Double_t zl,
                                   Double_t& xg, Double_t& yg, Double_t& zg) const;
   
   TVector3        LocalToMaster(Int_t detID, TVector3& loc) const;
   TVector3        LocalToMasterVect(Int_t detID, TVector3& loc) const;

   //! Add matrxi transformation
   void            AddTransMatrix(TGeoHMatrix* mat, Int_t idx = -1);
   //! Remove matrix transformation
   void            RemoveTransMatrix(TGeoHMatrix* mat);
   //! Get matrix transformation
   TGeoHMatrix*    GetTransfo(Int_t idx);
   //! Get matrix tranformation for volume placing
   TGeoCombiTrans* GetCombiTransfo(Int_t idx);

  //! Print card for Fluka
   string          PrintCard(TString fTitle, TString fWHAT1, TString fWHAT2, TString fWHAT3,
                             TString fWHAT4, TString fWHAT5, TString fWHAT6, TString fSDUM);
public:
   static Int_t GetPrecisionLevel()            { return fgPrecisionLevel;  }
   static void  SetPrecisionLevel(Int_t level) { fgPrecisionLevel = level; }

protected:
   static Int_t fgPrecisionLevel;

   ClassDef(TAGparTools,2)
};

#endif
