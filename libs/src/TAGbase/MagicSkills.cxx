#include "MagicSkills.hxx"


ofstream note;

//____________________________________________________________________________
vector<string>  Tokenize(const string str,
                      const string delimiters)
{


  vector<string> tokens;
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find(delimiters, lastPos);

    }
    return tokens;
}

// vector<string> Tokenize(string inString, string divisor ) {
  
//   char *c=new char[inString.size()+1];
//   copy(inString.begin(), inString.end(), c);
//   c[inString.size()] = '\0';
  
//   vector<string> out;
//   char *token;	
//   token = strtok( c, divisor.c_str() );
//   while( token != NULL ) {
//     out.push_back( token );
//     token = strtok( NULL, divisor.c_str() );
//   }
//   delete c;
//   delete token;
//   return out;
// }




//____________________________________________________________________________
void RemoveSpace( string* s ) {
  s->erase( ::remove_if(s->begin(), s->end(), ::isspace), s->end() );
}




//____________________________________________________________________________
int ThinkPositive(int x) {
  if (x>0)
    return x;
  else
    return 0;
}

int ForinosRule(double d) {

  if (d == 0)
    return 0;
  
  int x = trunc(d);

  double y = d;
  int n = 0;
  
  if ( x == 0 ) {
    
    while( x == 0 && n < 15 ) {
      n++;
      y *= 10;
      x = trunc(y);
    }
    if (n==15) return 0;
    if (x != 1) return n;
    
    n++;
    return n;
    
  }
  
  if (x<10) {
    n++;
    return n;
  }

  if (x>100) {
    do {
      y = y/10;
      x = trunc(y);
      n++;
    }  while ( x>100 );
    n--;
    return -n;
  }
  
  return 0;
}

double Round( double in, int precision ) {
  double x;
  double exp = pow(10, precision);
  double y = in * exp;
  x = (double)round(y);
  return x/exp;
//return in;
}

void Traspose(TH2F* h2) {
  TH2F * hInv = new TH2F();
  hInv = (TH2F*)h2->Clone();
  hInv->Reset();
  for (int i=0; i<h2->GetNbinsX(); i++) { //true -> reco
    for (int j=0; j<h2->GetNbinsY(); j++) { //reco -> true
      hInv->SetBinContent(j+1, i+1, h2->GetBinContent(i+1, j+1) );
      hInv->SetBinError(j+1, i+1, h2->GetBinError(i+1, j+1) );	  
    }
  }
  if( h2 ) *h2 = *hInv;
  else h2 = (TH2F*) hInv->Clone();
  delete hInv;
}





  

//Replace part of "original" if it founds "erase" with "add". Otherwise return input string.
string StrReplace(string original, string erase, string add) {
  
  int found = original.find(erase);
  if ( (size_t)found != string::npos ) {
    int cutLength = erase.size();
    original.replace( found, cutLength, add );
    return original;
  }
  else {
    cout<<"not found "<<erase<<" in "<<original<<endl;
    return original;
  }
}


bool isInt(double x) {
  int y = (int)trunc(x);
  if (x != y)
    return false;
  else 
    return true;
}

void StdLegend( TLegend* fLegend ) {
  fLegend->SetNColumns(1);
  fLegend->SetFillColor(0);
  fLegend->SetFillStyle(0);
  fLegend->SetBorderSize(0);
  fLegend->SetTextFont(72);
  fLegend->SetTextSize(0.025);
}

void ColorList () {
  cout<<"kRed "<<kRed<<endl;
  cout<<"kOrange "<<kOrange<<endl;
  cout<<"kYellow "<<kYellow<<endl;
  cout<<"kSpring "<<kSpring<<endl;
  cout<<"kGreen "<<kGreen<<endl;
  cout<<"kTeal "<<kTeal<<endl;
  cout<<"kCyan "<<kCyan<<endl;
  cout<<"kAzure "<<kAzure<<endl;
  cout<<"kBlue "<<kBlue<<endl;
  cout<<"kViolet "<<kViolet<<endl;
  cout<<"kMagenta "<<kMagenta<<endl;
  cout<<"kPink "<<kPink<<endl;
  cout<<"kWhite "<<kWhite<<endl;
  cout<<"kBlack "<<kBlack<<endl;
  cout<<"kGray "<<kGray<<endl;

}

/********************************************************/
double mcm (vector<double> x) {
/********************************************************/
    int m = -666;
    if ( !isInt(x[0] * 10) ){
      cout<<"Fucking binning on my ass! Rebin better, sono'a'bitch!!"<<endl;
      return -666;
    }
    int a = (int)(x[0] * 10);
    for (int i=1; i<( int)x.size(); i++) {
      if ( !isInt(x[i] * 10) ) {
	cout<<"Fucking binning on my ass! Rebin better, sono'a'bitch!!"<<endl;
	return -666;
      }
      int b = (int)(x[i] * 10);
      if ( a < b ) {
	int c = a;
	a=b;
	b=c;
      }
      for (int n=1; n<500; n++) {
	m = a*n;
	if ( m % b == 0 ) break;
      }
      a=m;
    }
    return m/10;
  };


  
void SmartCopy( const TH1F * h_in, TH1F *&h_out )
{
	cout << "SmartCopy1d: input histogram name = " << h_in->GetName() << ", output histogram address " << h_out << endl;
	if( h_out )
	{
		//cout << "SmartCopy1d: h_out has been identified as non-zero\n"; 
		(*h_out) = *h_in;
	}

	else 
	{
		//cout << "SmartCopy1d: h_out has been identified as null pointer\n";
		h_out = new TH1F( *h_in );
	}
}  
  
void SmartCopy( const TH2F * h_in, TH2F *&h_out )
{
	cout << "SmartCopy2d: input histogram name = " << h_in->GetName() << ", output histogram address " << h_out << endl;
	if( h_out )
	{
		cout << "SmartCopy2d: h_out has been identified as non-zero\n"; 
		(*h_out) = *h_in;
	}
	else 
	{
		cout << "SmartCopy2d: h_out has been identified as null pointer\n";
		h_out = new TH2F( *h_in );
		cout << "SmartCopy2d: h_out address is now " << h_out << endl;
	}
}  
 

void DivideByBinWidth( TH1F * h )
{
	int nbins = h->GetNbinsX();
	float width, content, error;
	for( int i = 1; i <= nbins; i++ )
	{
		width = h->GetBinWidth( i );
		content = h->GetBinContent( i );
		error = h->GetBinError( i );
		h->SetBinContent( i, content / width );
		h->SetBinError( i, error / width );
	}
} 
  
  
 string MakeLatexArray( float up, float down )
{
  char out[100];
  sprintf( out, "$\\begin{array}{c}+%.2f\\\\-%.2f\\end{array}$", Round(fabs(up), 2), Round( fabs(down), 2));
  return string( out );
}


void GetPlusAndMinus( float& p, float& m )
{
  float up, down;
  if( p * m > 0 ) //same sign
  {
     if( p > 0 )
     {
       up = ( fabs(p) > fabs(m) ? p : m );
       down = 0.;
     }
     else
     {
       down = ( fabs(p) > fabs(m) ? p : m );
       up = 0.;
       
     }
  }
  else //opposite sign
  {
     if( p > m ) 
     {
       up = p;
       down = m;
     }
     else
     {
       up = m;
       down = p;
     }
     
  }
  p = fabs(up);
  m = fabs(down);
}


void MirrorRebin( TH1F * h_in )
{
//   cout << "MirrorRebin: h_in->GetNbinsX() = " << h_in->GetNbinsX() << endl;
  int nbins = h_in->GetNbinsX();
  int newbins = nbins/2;
//   const TArrayD * bins_old = h_in->GetXaxis()->GetXbins();
//  const double * bins_old = h_in->GetXaxis()->GetXbins()->GetArray();
  //cout << "bins_old->GetSize() " << bins_old->GetSize()  << endl;
  float * bins_new = new float[ newbins+1 ];
  for( int i = 1; i <= newbins; i++ )
  { 
    bins_new[i-1] = h_in->GetBinLowEdge( newbins + i); 
     cout << "MirrorRebin: new bin " << i-1 << ", old bin " << nbins/2+i << endl;
     cout << "bins_new["<<i-1<<"] = " << bins_new[i-1] << endl;
  }
    bins_new[0] = 0.;
  bins_new[newbins] = bins_new[ newbins-1] + h_in->GetBinWidth( nbins );
//   cout << "bins_new["<<newbins<< "] " <<bins_new[newbins] << endl;
  TH1F * h_out = new TH1F( ( string(h_in->GetName()) + "_mirrored" ).c_str(), ( string(h_in->GetTitle()) + " mirrored" ).c_str(), newbins, bins_new );
  float value, err;
  for( int i = 1; i <= newbins; i++ )
  {
    value = h_in->GetBinContent(i);
    value += h_in->GetBinContent( nbins+1 -i );
    err = h_in->GetBinError(i) * h_in->GetBinError(i);
    err += h_in->GetBinError( nbins+1 -i ) * h_in->GetBinError( nbins+1 -i );
    h_out->SetBinContent( newbins+1 - i, value );
    h_out->SetBinError( newbins+1 - i, sqrt(err) );
  }
  cout << "MirrorRebin: Integral before: " << h_in->Integral() << ", integral after " << h_out->Integral() << endl;
  SmartCopy( h_out, h_in );
  delete h_out;
  delete [] bins_new;
}

void MirrorRebin( TH2F * h_in )
{
  int nbins = h_in->GetNbinsX();
  int newbins = nbins/2;
//   const TArrayD * bins_old = h_in->GetXaxis()->GetXbins();
  float * bins_new = new float[ newbins+1 ];

  for( int i = 1; i <= newbins; i++ )
  { 
    bins_new[i-1] = h_in->GetBinLowEdge( newbins + i); 
     cout << "MirrorRebin: new bin " << i-1 << ", old bin " << nbins/2+i << endl;
     cout << "bins_new["<<i-1<<"] = " << bins_new[i-1] << endl;
  }
    bins_new[0] = 0.;
  bins_new[newbins] = bins_new[ newbins-1] + h_in->GetBinWidth( nbins );
  TH2F * h_out = new TH2F( ( string(h_in->GetName()) + "_mirrored" ).c_str() , ( string(h_in->GetTitle()) + " mirrored" ).c_str(), newbins, bins_new, newbins, bins_new );
  float value, err;
  for( int i = 1; i <= newbins; i++ )
  {
     for( int j = 1; j <= newbins; j++ )
     {
      value = h_in->GetBinContent(i, j); //quadrante in basso a sinistra
      value += h_in->GetBinContent( nbins+1 -i, j  ); //quadrante in basso a destra
      value += h_in->GetBinContent(i, nbins+1-j); //quadrante in alto a sinistra
      value += h_in->GetBinContent( nbins+1 -i, nbins+1-j  ); //quadrante in alto a destra
      
      
      
      err = h_in->GetBinError(i, j) * h_in->GetBinError(i, j);
      err += h_in->GetBinError( nbins+1 -i, j ) * h_in->GetBinError( nbins+1 -i, j );
      err += h_in->GetBinError(i, nbins+1 -j) * h_in->GetBinError(i, nbins+1 -j);
      err += h_in->GetBinError( nbins+1 -i, nbins+1 -j ) * h_in->GetBinError( nbins+1 -i, nbins+1 -j );      
      h_out->SetBinContent( newbins+1 - i, newbins+1-j, value );
      h_out->SetBinError( newbins+1 - i, newbins+1-j, sqrt(err) );
     }
  }
  
  SmartCopy( h_out, h_in );
  delete h_out;
  delete [] bins_new;
}