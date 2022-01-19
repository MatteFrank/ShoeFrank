#ifndef _TAGparTools_HXX
#define _TAGparTools_HXX
/*!
  \file TAGparTools.hxx
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
   //! parameter file
   ifstream fFileStream;         //! parameter file
   TString  fFileName;           ///< name of the file

    //! list of transformation matrices  (rotation+translation for each crystal)
   TObjArray* fMatrixList;       //! list of transformation matrices  (rotation+translation for each crystal)
   TVector3   fCurrentPosition;  ///< current position
   
public:
   TAGparTools();
   virtual ~TAGparTools();
   
   //! Open file
   Bool_t  Open(const TString& name);
   // EOF status
   Bool_t  Eof();
   // Close file
   Bool_t  Close();
   // Read string item
   void    ReadItem(TString& item);
   // Read string item with a key
   void    ReadItem(TString& key, TString& item);
   // Read integer item with a key
   void    ReadItem(Int_t& key, Int_t& item);
   // Read char item
   void    ReadItem(Char_t& item);
   // Read integer item
   void    ReadItem(Int_t &arg);
   // Read float item
   void    ReadItem(Float_t &arg);
   // Read char array with a delimiter in string
   void    ReadItem(TArrayC& array, const Char_t delimiter = ';');
   // Read integer array with a delimiter in string
   void    ReadItem(TArrayI& array, const Char_t delimiter = ';');
   // Read integer array and strings with two delimiters in string
   void    ReadStringsInts(TString& aString, TArrayI& array,  const Char_t delimiter1 = ':', const Char_t delimiter2 = ';');
   // Read double array with a delimiter in string with or w/o key
   void    ReadItem(Double_t* coeff, Int_t size, const Char_t delimiter = ' ', Bool_t keyFlag = true);
   // Read pair map with two delimiters in string
   void    ReadItem(map< pair<int, int>, int>& map, const Char_t delimiter1 = ',', const Char_t delimiter2 = ';');
   //Read TVector3
   void    ReadVector3(TVector3 &arg);
   //Read string
   void    ReadStrings(TString& aString);
   // Get range from a string
   void    GetRange(const char* str, Int_t& begin, Int_t& end, Int_t& incr, Int_t& n);
   // Fill char array from string
   void    FillArray(TString& s, TArrayC& array);
   // Fill integer array from string
   void    FillArray(TString& s, TArrayI& array);
   // Normalize (remove double blank)
   TString Normalize(const char* line);
   
   // Set up transformation matrices
   void    SetupMatrices(Int_t size);

   //! Get file name
   TString GetFileName() const { return fFileName; }
   
   // Transform point from the global detector reference frame
   // to the local sensor reference frame of the detection id
   void            MasterToLocal(Int_t detID,  Double_t xg, Double_t yg, Double_t zg,
                                   Double_t& xl, Double_t& yl, Double_t& zl) const;
   // Master to local with vector
   TVector3        MasterToLocal(Int_t detID, TVector3& glob) const;
   // Master to local vector with vector
   TVector3        MasterToLocalVect(Int_t detID, TVector3& glob) const;
   
   // Transform point from the local reference frame
   // of the detection id to the global reference frame
   void            LocalToMaster(Int_t detID,  Double_t xl, Double_t yl, Double_t zl,
                                   Double_t& xg, Double_t& yg, Double_t& zg) const;
   // Local to master with vector
   TVector3        LocalToMaster(Int_t detID, TVector3& loc) const;
   // Local to master vector with vector
   TVector3        LocalToMasterVect(Int_t detID, TVector3& loc) const;

   // Add matrxi transformation
   void            AddTransMatrix(TGeoHMatrix* mat, Int_t idx = -1);
   // Remove matrix transformation
   void            RemoveTransMatrix(TGeoHMatrix* mat);
   // Get matrix transformation
   TGeoHMatrix*    GetTransfo(Int_t idx);
   // Get matrix tranformation for volume placing
   TGeoCombiTrans* GetCombiTransfo(Int_t idx);

   // Print card for Fluka
   string          PrintCard(TString fTitle, TString fWHAT1, TString fWHAT2, TString fWHAT3,
                             TString fWHAT4, TString fWHAT5, TString fWHAT6, TString fSDUM);
   // Get Fluka crossing regions
   Int_t           GetCrossReg(TString &regname);
  
public:
   //! Get precision level for cout
   static Int_t GetPrecisionLevel()            { return fgPrecisionLevel;  }
   //! Set precision level for cout
   static void  SetPrecisionLevel(Int_t level) { fgPrecisionLevel = level; }
   // Tokenize string
   static vector<string> Tokenize(const string str, const string delimiters);
   
protected:
   static Int_t fgPrecisionLevel; ///< Precision level for cout

   ClassDef(TAGparTools,2)
};

#endif
