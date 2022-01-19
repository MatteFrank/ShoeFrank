/*!
 \file TAGparTools.cxx
 \brief   Implementation of TAGparTools.
 */

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TSystem.h"
#include "TGeoMatrix.h"

#include "TAGrecoManager.hxx"
#include "TAGparTools.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TAGparTools TAGparTools.hxx "TAGparTools.hxx"
  \brief tools parameters for vertex. **
*/

//! Class Imp
ClassImp(TAGparTools);

Int_t TAGparTools::fgPrecisionLevel = 6;

//______________________________________________________________________________
//! Standard constructor
TAGparTools::TAGparTools()
: TAGpara(),
  fFileStream(),
  fFileName(),
  fMatrixList(0x0),
  fCurrentPosition(0,0,0)
{
}

//______________________________________________________________________________
//! Destructor
TAGparTools::~TAGparTools()
{
   if (fMatrixList)
      delete fMatrixList;
}

//______________________________________________________________________________
//! Read one char
//!
//! \param[out] item read char
void TAGparTools::ReadItem(Char_t& item)
{
   if (fFileStream.eof()) return;
   fFileStream.get(item);
}

//______________________________________________________________________________
//! Read a string
//!
//! \param[out] item read string
void TAGparTools::ReadItem(TString& item)
{
   Int_t pos = -1;
   Char_t buf[1024];
   TString key;
   while (pos == -1) {
      if (fFileStream.eof()) break;
	  fFileStream.getline(buf, 1024);
     if (buf[0] == '/') continue;
     if (buf[0] == '#') continue;
	  key = buf;
	  pos = key.First(":");
   }
   item = key(pos+1, key.Length()-pos);   
   if(FootDebugLevel(3)) cout << item.Data() << endl;
}

//______________________________________________________________________________
//! Read a string for a given key
//!
//! \param[in] key key in string
//! \param[out] item read string
void TAGparTools::ReadItem(TString& key, TString& item)
{
  Int_t pos = -1;
  Char_t buf[1024];
  TString line;
  while (pos == -1) {
    if (fFileStream.eof()) break;
    fFileStream.getline(buf, 1024);
    if (buf[0] == '/') continue;
    if (buf[0] == '#') continue;
    if (buf[0] == '\n') continue;
    if (buf[0] == ' ') continue;
    if (buf[0] == '\0') continue;
    line = buf;
    pos = line.First(":");
  }
  
  key = line(0, pos+1);
  item = line(pos+1, line.Length()-pos);
  item = Normalize(item.Data());
}

//______________________________________________________________________________
//! Read a integer for a given key as integer
//!
//! \param[in] key key in string
//! \param[out] item read integer
void TAGparTools::ReadItem(Int_t& key, Int_t& item)
{
   TString sKey;
   TString sItem;
   
   ReadItem(sKey, sItem);
   char tmp[128];
   sscanf(sKey.Data(), "%s %d:", tmp, &key);
   
   item = sItem.Atoi();
}

//______________________________________________________________________________
//! Read a integer from a string
//!
//! \param[out] arg read integer
void TAGparTools::ReadItem(Int_t &arg)
{
   TString item;
   ReadItem(item);
   arg = item.Atoi();
   
   if(FootDebugLevel(3))
	  cout << arg << endl;
}

//______________________________________________________________________________
//! Read a float from a string
//!
//! \param[out] arg read float
void TAGparTools::ReadItem(Float_t &arg)
{
   TString item;
   ReadItem(item);
   arg = item.Atof();
  if(FootDebugLevel(3))
	  cout << arg << endl;
}

//______________________________________________________________________________
//! Read a char array from a string
//!
//! From a string of the form "i-j;k;l;m-n" returns an char array
//! containing all the integers from i to j, then k, l and then from m to n.
//! \param[out] array char array
//! \param[in] delimiter delimeter
void TAGparTools::ReadItem(TArrayC& array, const Char_t delimiter)
{
   array.Reset(0);
   
   TString key;
   ReadItem(key);
   key = Normalize(key.Data());
   if (key.Atoi() == -1)
      return;
   
   // Get substrings separated by 'delimiter'
   TObjArray* ranges = key.Tokenize(delimiter);
   
   // Finally takes each substring (which ought to be a range of the form
   // x-y), and decode it into the theList integer vector.
   for (Int_t i = 0; i < ranges->GetEntriesFast(); ++i ) {
	 
      TString& s = ((TObjString*)ranges->At(i))->String();
      FillArray(s, array);
   }
   
   delete ranges;
}

//______________________________________________________________________________
//! Fill a char array from a string
//!
//! \param[in] s string
//! \param[out] array char array
void TAGparTools::FillArray(TString& s, TArrayC& array)
{
   Int_t m1;
   Int_t m2;
   Int_t n;
   Int_t incr;
   GetRange(s.Data(),m1,m2,incr,n);
   Int_t m = m1;
   while ( n > 0 ) {
      array[m] = 1;
      m += incr;
      --n;
   }
}

//______________________________________________________________________________
//! Read a integer array from a string
//!
//! From a string of the form "i-j;k;l;m-n" returns an integer array
//! containing all the integers from i to j, then k, l and then from m to n.
//! \param[out] array integer array
//! \param[in] delimiter delimeter
void TAGparTools::ReadItem(TArrayI& array, const Char_t delimiter)
{
   TString key;
   ReadItem(key);
   key = Normalize(key.Data());
   if (key.Atoi() == -1)
      return;
   
   // Get substrings separated by 'delimiter'
   TObjArray* ranges = key.Tokenize(delimiter);
   
   // Finally takes each substring (which ought to be a range of the form
   // x-y), and decode it into the theList integer vector.
   for (Int_t i = 0; i < ranges->GetEntriesFast(); ++i ) {
      
      TString& s = ((TObjString*)ranges->At(i))->String();
      FillArray(s, array);
   }
   
   delete ranges;
}

//______________________________________________________________________________
//! Fill a integer array from a string
//!
//! \param[in] s string
//! \param[out] array integer array
void TAGparTools::FillArray(TString& s, TArrayI& array)
{
   Int_t m1;
   Int_t m2;
   Int_t n;
   Int_t incr;
   GetRange(s.Data(),m1,m2,incr,n);
   
   Int_t m = m1;
   Int_t k = 0;
   while ( n > 0 ) {
      array.Set(array.GetSize()+1);
      array[array.GetSize()-1] = m;
      m += incr;
      --n;
   }
}

//_____________________________________________________________________________
//! Read a map from string with two delimeters
//!
//! From a string of the form "i,j; k,l; m,n" returns an integer array
//! containing the pixel line i / col j, line k / col l, line m / col n
//! \param[out] map map pair/int
//! \param[in] delimiter1 delimeter1
//! \param[in] delimiter2 delimeter2
void TAGparTools::ReadItem(map<pair<int, int>, int>& map, const Char_t delimiter1, const Char_t delimiter2)
{
   TArrayC arrayLine;
   arrayLine.Set(968);
   arrayLine.Reset(0);
   
   TArrayC arrayCol;
   arrayCol.Set(968);
   arrayCol.Reset(0);
   
   TString key;
   ReadItem(key);
   key = Normalize(key.Data());
   if (key.Atoi() == -1)
      return;

   // Get substrings separated by 'delimiter2'
   TObjArray* ranges = key.Tokenize(delimiter2);
   
   // Finally takes each substring (line and column number)
   // and decode it into the theList integer vector.
   for (Int_t i = 0; i < ranges->GetEntriesFast(); ++i ) {
      TString& s       = ((TObjString*)ranges->At(i))->String();
      TObjArray* pixel = s.Tokenize(delimiter1);
      
      if (pixel->GetEntriesFast() != 2)
         Error("ReadItem()","wrong tokenize for [%s] with size %d", key.Data(), pixel->GetEntriesFast());

      TString& sl    = ((TObjString*)pixel->At(0))->String();
      TString& sc    = ((TObjString*)pixel->At(1))->String();
      FillArray(sl, arrayLine);
      FillArray(sc, arrayCol);
      
      for (Int_t l = 0; l < arrayLine.GetSize(); ++l) {
         for (Int_t c = 0; c < arrayCol.GetSize(); ++c) {
            if (arrayLine[l] == 1 && arrayCol[c] == 1) {
               pair<Int_t, Int_t> deadPixel(l, c);
               map[deadPixel] = 1;
            }
         }
      }
      arrayLine.Reset(0);
      arrayCol.Reset(0);
      
      delete pixel;
   }
   
   delete ranges;
}

//_____________________________________________________________________________
//! Read a double vector from a string for a given size with delimiter
//!
//! \param[out] coeff double vector
//! \param[in] size size of vector
//! \param[in] delimiter delimeter
//! \param[in] keyFlag flag for key in string
void TAGparTools::ReadItem(Double_t* coeff, Int_t size,  const Char_t delimiter, Bool_t keyFlag)
{
   TString key;
   if (keyFlag)
     TAGparTools::ReadItem(key);
   else {
     Char_t buf[255];
     do {
       fFileStream.getline(buf, 255);
       if (fFileStream.eof()) return;
       key = buf;
     } while (buf[0] == '/' || buf[0] == '\0' || buf[0] == '#');
   }
  
   if (key.IsNull()) return;
   
   TObjArray* list = key.Tokenize(delimiter);
   if (list->GetEntries() != size)
	  Error("ReadItem()","wrong tokenize for [%s] with size %d", key.Data(), size);
   
   for (Int_t k = 0; k < list->GetEntries(); k++) {
	  TObjString* obj = (TObjString*)list->At(k);
	  TString item = obj->GetString();
     item =  Normalize(item);
      Int_t pos = item.First(" ");
      if (pos == -1)
         pos = item.Length();
      TString value(item(0, pos));      
	  coeff[k] = item.Atof();
   }
   
   if(FootDebugLevel(3)) {
	  for (Int_t i = 0; i < list->GetEntries(); ++i) {
		 cout << coeff[i] << " " ;      
	  }
	  cout << endl;
   }
   delete list;
}

//______________________________________________________________________________
//! Read a vector from a string
//!
//! \param[out] arg read vector
void TAGparTools::ReadVector3(TVector3 &arg)
{
   Double_t* co = new Double_t[3];
   ReadItem(co, 3, ':');
   arg.SetXYZ(co[0], co[1], co[2]); 
   delete[] co;
}

//______________________________________________________________________________
//! reads a string of a given max length from configuration file
//! The strings is expected to be contained by double quotes : "
//!
//! \param[out] aString read string
void TAGparTools::ReadStrings(TString& aString)
{
   TString key;
   TAGparTools::ReadItem(key);

   if (key.IsNull()) return;

   Int_t pos = key.First('"');
   Int_t end = key.Last('"');
   
   if (end == -1 || pos == -1 || end-pos-1 > 255) {
	  cout << "TAGparTools: problem with reading file, missing \" or line too long" << endl;
	  return;
   }
   
    aString = key(pos+1, end-pos-1);
   
   if(FootDebugLevel(3))
	  cout << aString << endl;
}

//______________________________________________________________________________
//! reads a string between "" followed by integers separated by delimiter1 Integers are separated by delimiter2
//!
//! \param[out] aString read string
//! \param[out] array read array
//! \param[in] delimiter1 delimeter1
//! \param[in] delimiter2 delimeter2
void TAGparTools::ReadStringsInts(TString& aString, TArrayI& array, const Char_t delimiter1, const Char_t delimiter2)
{
   Char_t buf[512];
   TString key;
   do {
      if (fFileStream.eof()) return ;//break;
      fFileStream.getline(buf, 512);
      key = buf;
   } while (buf[0] == '/');

   if (key.IsNull()) return;
   
   Int_t len = key.Length();
   Int_t pos = key.First('"');
   Int_t end = key.Last('"');
   if (end == -1 || pos == -1 || end-pos-1 > 512) {
      cout << "TAGparTools: problem with reading file, missing \" or line too long" << endl;
      return;
   }
   
   aString = key(pos+1, end-pos-1);
   pos = key.Last(delimiter1);
   TString range = key(pos+1, len-pos);

   TObjArray* ranges = range.Tokenize(delimiter2);
   Int_t n = ranges->GetEntriesFast();;
   array.Set(n);
   
   for (Int_t i = 0; i < n; ++i ) {
      TString& s = ((TObjString*)ranges->At(i))->String();
      Int_t r = s.Atoi();
      array[i] = r;
   }
   
   if(FootDebugLevel(3))
      cout << range << endl;
   
   if(FootDebugLevel(3))
      cout << aString << endl;
}

//______________________________________________________________________________
//! From a string of the form "m-n" returns a range (begin,end),
//! its ordering (incr=+-1) and its size (abs(begin-end)+1)
//!
//! \param[in] cstr string
//! \param[out] begin begin of range
//! \param[out] end end of range
//! \param[out] incr increment in range
//! \param[out] n number of items in range
void TAGparTools::GetRange(const char* cstr, Int_t& begin, Int_t& end, Int_t& incr, Int_t& n)
{
   TString str(cstr);
   
   incr = 1;
   Ssiz_t pos = str.First('-');
   
   if ( pos < 0 ) {
	  begin = str.Atoi();
	  end = -1;
	  n = 1;
   } else {
	  begin = str.Atoi();
	  end = TString(str(pos+1,str.Length()-pos)).Atoi();
	  if ( begin > end ) {
		 incr = -1;
		 n = begin-end+1;
	  } else {
		 n = end-begin+1;
	  }    
   }
}

//_____________________________________________________________________________
//! Remove multiple blanks, and blanks in the begining/end.
//!
//! \param[in] line string to normalize
TString TAGparTools::Normalize(const char* line)
{
   TString rv(line);
   
   if ( rv.Length() <= 0 ) return TString();
   
   while ( rv[0] == ' ' || rv == '\t')
	  rv.Remove(0,1);
   
   while ( rv[rv.Length()-1] == ' ' || rv[rv.Length()-1] == '\t')
	  rv.Remove(rv.Length()-1,1);
   
   Ssiz_t i(0);
   bool kill = false;
   
   for ( i = 0; i < rv.Length(); ++i ) {
	  if ( rv[i] == ' ' || rv[i] == '\t') {
		 if (kill) {
			rv.Remove(i,1);
			--i;
		 } else
			kill = true;
	  } else 
		 kill = false;
   }
   
   return rv;
}

//______________________________________________________________________________
//! Open file
//!
//! \param[in] name file name
Bool_t TAGparTools::Open(const TString& name) 
{
   // openning file
   gSystem->ExpandPathName(name);
   
   fFileStream.open(name.Data());
   if (!fFileStream) {
	  Error("Open()", "failed to open file '%s'", name.Data());
	  return false;
   }
   
   fFileName = name;
   
   return true;
}

//______________________________________________________________________________
//! Eof check
Bool_t TAGparTools::Eof()
{
   // closing file
   return fFileStream.eof();
}

//______________________________________________________________________________
//! Close file
Bool_t TAGparTools::Close()
{
   // closing file
   fFileStream.close();
   
   return true;
}

// transformation
//_____________________________________________________________________________
//! Transformation from master to local framework
//!
//! \param[in] detID detector id
//! \param[in] xg X position in master framework
//! \param[in] yg Y position in master framework
//! \param[in] zg Z position in master framework
//! \param[out] xl X position in local framework
//! \param[out] yl Y position in local framework
//! \param[out] zl Z position in local framework
void TAGparTools::MasterToLocal(Int_t detID,
                                     Double_t xg, Double_t yg, Double_t zg,
                                     Double_t& xl, Double_t& yl, Double_t& zl) const
{
   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {0., 0., 0.};
   Double_t global[3] = {xg, yg, zg};
   
   mat->MasterToLocal(global, local);
   xl = local[0];
   yl = local[1];
   zl = local[2];
}

//_____________________________________________________________________________
//! Transformation from master to local framework
//!
//! \param[in] detID detector id
//! \param[in] glob position in master framework
//! \return local position in local framework
TVector3 TAGparTools::MasterToLocal(Int_t detID, TVector3& glob) const
{

   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {0., 0., 0.};
   Double_t global[3] = {glob.X(), glob.Y(), glob.Z()};
   
   mat->MasterToLocal(global, local);
   TVector3 pos(local[0], local[1], local[2]);
   
   return pos;
}

//_____________________________________________________________________________
//! Transformation from master to local framework for vector (no translation)
//!
//! \param[in] detID detector id
//! \param[in] glob position in master framework
//! \return local position in local framework
TVector3 TAGparTools::MasterToLocalVect(Int_t detID, TVector3& glob) const
{
   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {0., 0., 0.};
   Double_t global[3] = {glob.X(), glob.Y(), glob.Z()};
   
   mat->MasterToLocalVect(global, local);
   TVector3 pos(local[0], local[1], local[2]);
   
   return pos;
}

//_____________________________________________________________________________
//! Transformation from local to master framework
//!
//! \param[in] detID sensor id
//! \param[in] xl X position in local framework
//! \param[in] yl Y position in local framework
//! \param[in] zl Z position in local framework
//! \param[out] xg X position in master framework
//! \param[out] yg Y position in master framework
//! \param[out] zg Z position in master framework
void TAGparTools::LocalToMaster(Int_t detID,
                                     Double_t xl, Double_t yl, Double_t zl,
                                     Double_t& xg, Double_t& yg, Double_t& zg) const
{
   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {xl, yl, zl};
   Double_t global[3] = {0., 0., 0.};
   
   mat->LocalToMaster(local, global);
   xg = global[0];
   yg = global[1];
   zg = global[2];
}

//_____________________________________________________________________________
//! Transformation from local to master framework
//!
//! \param[in] detID sensor id
//! \param[in] loc position in local framework
//! \return position in master framework
TVector3 TAGparTools::LocalToMaster(Int_t detID, TVector3& loc) const
{
   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {loc.X(), loc.Y(), loc.Z()};
   Double_t global[3] = {0., 0., 0.};
   
   mat->LocalToMaster(local, global);
   TVector3 pos(global[0], global[1], global[2]);
   
   return pos;
}

//_____________________________________________________________________________
//! Transformation from local to master framework for vector (no translation)
//!
//! \param[in] detID sensor id
//! \param[in] loc position in local framework
//! \return position in master framework
TVector3 TAGparTools::LocalToMasterVect(Int_t detID, TVector3& loc) const
{
   TGeoHMatrix* mat = static_cast<TGeoHMatrix*> ( fMatrixList->At(detID) );
   Double_t local[3]  = {loc.X(), loc.Y(), loc.Z()};
   Double_t global[3] = {0., 0., 0.};
   
   mat->LocalToMasterVect(local, global);
   TVector3 pos(global[0], global[1], global[2]);
   
   return pos;
}

//_____________________________________________________________________________
//! Add H matrix at a given index
//!
//! \param[in] mat matrix to add
//! \param[in] idx index in the list
void TAGparTools::AddTransMatrix(TGeoHMatrix* mat, Int_t idx)
{
   if (idx == -1)
      fMatrixList->Add(mat);
   else {
      TGeoHMatrix* oldMat = GetTransfo(idx);
      if (oldMat)
         RemoveTransMatrix(oldMat);
      fMatrixList->AddAt(mat, idx);
   }
}

//_____________________________________________________________________________
//! Remove H matrix in list
//!
//! \param[in] mat matrix to remove
void TAGparTools::RemoveTransMatrix(TGeoHMatrix* mat)
{
   if (!fMatrixList->Remove(mat))
      printf("Cannot remove matrix");
}

//_____________________________________________________________________________
//! Get transformation at a given index
//!
//! \param[in] idx index in the list
TGeoHMatrix* TAGparTools::GetTransfo(Int_t idx)
{
   if (idx < 0 || idx >= fMatrixList->Capacity()) {
      Warning("GetTransfo()","Wrong detector id number: %d ", idx);
      return 0x0;
   }
   
   return (TGeoHMatrix*)fMatrixList->At(idx);
}

//_____________________________________________________________________________
//! Get combinaison transformation at a given index
//!
//! \param[in] idx index in the list
TGeoCombiTrans* TAGparTools::GetCombiTransfo(Int_t idx)
{
   if (idx < 0 || idx >= fMatrixList->Capacity()) {
      Warning("GetTransfo()","Wrong detector id number: %d ", idx);
      return 0x0;
   }
   
   TGeoHMatrix* hm = (TGeoHMatrix*)fMatrixList->At(idx);
   if (hm == 0x0)
      return 0x0;
   
   Double_t* mat   = hm->GetRotationMatrix();
   Double_t* dis   = hm->GetTranslation();
   
   TGeoRotation rot;
   rot.SetMatrix(mat);
   
   TGeoTranslation trans;
   trans.SetTranslation(dis[0], dis[1], dis[2]);
   
   return  new TGeoCombiTrans(trans, rot);
}

//_____________________________________________________________________________
//! Set up matrices
//!
//! \param[in] size size of matrices
void TAGparTools::SetupMatrices(Int_t size)
{
   fMatrixList = new TObjArray(size);
   fMatrixList->SetOwner(true);
   fMatrixList->Clear();
}

//_____________________________________________________________________________
//! Print Fluka Card
//!
//! \param[in] fTitle file name
//! \param[in] fWHAT1 what1 string
//! \param[in] fWHAT2 what2 string
//! \param[in] fWHAT3 what3 string
//! \param[in] fWHAT4 what4 string
//! \param[in] fWHAT5 what5 string
//! \param[in] fWHAT6 what6 string
//! \param[in] fSDUM fSDUM string
string  TAGparTools::PrintCard(TString fTitle, TString fWHAT1, TString fWHAT2, TString fWHAT3,
		 TString fWHAT4, TString fWHAT5, TString fWHAT6, TString fSDUM) {
  
  stringstream fLine;
	
  if (fTitle.Sizeof() != 10) fTitle.Resize(10);
  if (fSDUM.Sizeof() != 10) fSDUM.Resize(10);
  if (fWHAT1.Sizeof() > 10) fWHAT1.Resize(10);
  if (fWHAT2.Sizeof() > 10) fWHAT2.Resize(10);
  if (fWHAT3.Sizeof() > 10) fWHAT3.Resize(10);
  if (fWHAT4.Sizeof() > 10) fWHAT4.Resize(10);
  if (fWHAT5.Sizeof() > 10) fWHAT5.Resize(10);
  if (fWHAT6.Sizeof() > 10) fWHAT6.Resize(10);

  fLine << setw(10) << fTitle << setw(10) << fWHAT1 << setw(10) << fWHAT2
	<< setw(10) << fWHAT3 << setw(10) << fWHAT4 << setw(10) << fWHAT5
	<< setw(10) << fWHAT6 << setw(10) << fSDUM;
	
  return fLine.str();
  
}

//_____________________________________________________________________________
//! Get cross region
//!
//! \param[in] regname region name
Int_t  TAGparTools::GetCrossReg(TString &regname)
{
  return gTAGroot->CurrentRunInfo().GetRegion(regname);
}

//____________________________________________________________________________
//! Tokenize
//!
//! \param[in] str string
//! \param[in] delimiters delimiter
//! \return a string vector
vector<string>  TAGparTools::Tokenize(const string str, const string delimiters)
{
   vector<string> tokens;
   // Skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of(delimiters, 0);
   // Find first "non-delimiter".
   string::size_type pos     = str.find(delimiters, lastPos);
   
   while (string::npos != pos || string::npos != lastPos) {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find(delimiters, lastPos);
   }
   
   return tokens;
}
