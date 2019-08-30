

#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TSystem.h"
#include "TGeoMatrix.h"

#include "GlobalPar.hxx"
#include "TAGparTools.hxx"

//##############################################################################

/*!
  \class TAGparTools TAGparTools.hxx "TAGparTools.hxx"
  \brief tools parameters for vertex. **
*/

ClassImp(TAGparTools);

//______________________________________________________________________________
TAGparTools::TAGparTools()
: TAGpara(),
  fFileStream(),
  fFileName(),
  fMatrixList(0x0),
  fCurrentPosition(0,0,0)
{
   // Standard constructor
}

//______________________________________________________________________________
TAGparTools::~TAGparTools()
{
   // Destructor
   if (fMatrixList)
      delete fMatrixList;
}

//_____________________________________________________________________________
void TAGparTools::ReadItem(TString& item)
{
   Int_t pos = -1;
   Char_t buf[255];
   TString key;
   while (pos == -1) {
      if (fFileStream.eof()) break;
	  fFileStream.getline(buf, 255);
     if (buf[0] == '/') continue;
	  key = buf;
	  pos = key.First(":");
   }
   item = key(pos+1, key.Length()-pos);   
   if(FootDebugLevel(3)) cout << item.Data() << endl;
}

//_____________________________________________________________________________
void TAGparTools::ReadItem(Int_t &arg)
{
   TString item;
   ReadItem(item);
   arg = item.Atoi();
   
   if(FootDebugLevel(3))
	  cout << arg << endl;
}

//_____________________________________________________________________________
void TAGparTools::ReadItem(Float_t &arg)
{
   TString item;
   ReadItem(item);
   arg = item.Atof();
  if(FootDebugLevel(3))
	  cout << arg << endl;
}

//_____________________________________________________________________________
void TAGparTools::ReadItem(TArrayC& array, const Char_t delimiter)
{
   // From a string of the form "i-j;k;l;m-n" returns an integer array
   // containing all the integers from i to j, then k, l and then from m to n.
   
   array.Set(3000);
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

//_____________________________________________________________________________
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

//_____________________________________________________________________________
void TAGparTools::ReadItem(map<pair<int, int>, int>& map, const Char_t delimiter1, const Char_t delimiter2)
{
   // From a string of the form "i,j; k,l; m,n" returns an integer array
   // containing the pixel line i / col j, line k / col l, line m / col n
   
   TArrayC arrayLine;
   arrayLine.Set(960);
   arrayLine.Reset(0);
   
   TArrayC arrayCol;
   arrayCol.Set(960);
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
void TAGparTools::ReadItem(Float_t* coeff, Int_t size,  const Char_t delimiter)
{
   TString key;
   TAGparTools::ReadItem(key);
   
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

//_____________________________________________________________________________
void TAGparTools::ReadVector3(TVector3 &arg)
{
   Float_t* co = new Float_t[3];
   ReadItem(co, 3, ':');
   arg.SetXYZ(co[0], co[1], co[2]); 
   delete[] co;
}

//_____________________________________________________________________________
void TAGparTools::ReadStrings(TString& aString)
{
   
   // reads a string of a given max length from configuration file
   // The strings is expected to be contained by double quotes : "
   
   TString key;
   TAGparTools::ReadItem(key);
   
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

//_____________________________________________________________________________
void TAGparTools::GetRange(const char* cstr, Int_t& begin, Int_t& end, Int_t& incr, Int_t& n)
{
   // From a string of the form "m-n" returns a range (begin,end),
   // its ordering (incr=+-1) and its size (abs(begin-end)+1)
   
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
TString TAGparTools::Normalize(const char* line)
{
   // Remove multiple blanks, and blanks in the begining/end.
   
   TString rv(line);
   
   if ( rv.Length() <= 0 ) return TString();
   
   while ( rv[0] == ' ' )
	  rv.Remove(0,1);
   
   while ( rv[rv.Length()-1] == ' ' )
	  rv.Remove(rv.Length()-1,1);
   
   Ssiz_t i(0);
   bool kill = false;
   
   for ( i = 0; i < rv.Length(); ++i ) {
	  if ( rv[i] == ' ' ) {
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
Bool_t TAGparTools::Eof()
{
   // closing file
   return fFileStream.eof();
}

//______________________________________________________________________________
Bool_t TAGparTools::Close()
{
   // closing file
   fFileStream.close();
   
   return true;
}

// transformation
//_____________________________________________________________________________
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
void TAGparTools::RemoveTransMatrix(TGeoHMatrix* mat)
{
   if (!fMatrixList->Remove(mat))
      printf("Cannot remove matrix");
}

//_____________________________________________________________________________
TGeoHMatrix* TAGparTools::GetTransfo(Int_t idx)
{
   if (idx < 0 || idx >= fMatrixList->Capacity()) {
      Warning("GetTransfo()","Wrong detector id number: %d ", idx);
      return 0x0;
   }
   
   return (TGeoHMatrix*)fMatrixList->At(idx);
}

//_____________________________________________________________________________
void TAGparTools::SetupMatrices(Int_t size)
{
   fMatrixList = new TObjArray(size);
   fMatrixList->SetOwner(true);
}



//_____________________________________________________________________________
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
