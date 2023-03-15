

void computeSTeff(const char *filename){

  TFile *file = new TFile(filename,"READ");
  file->cd();


  TH1D *hEff = ((TH1D*)gDirectory->Get("ST/stEff"));

  double eff[8];
  for(int ch=0;ch<8;ch++){
    eff[ch] = hEff->GetBinContent(ch+1)/(double)hEff->GetBinContent(9);
    cout<< "eff channel " << ch << "::" << eff[ch] << endl;
  }

  vector<vector<int>> comb;
  vector<int> tmpcomb;
  for(int i=0;i<8;i++){
    for(int j=0;j<8 && (j!=i);j++){
      for(int k=0;k<8 && ((k!=i)&&(k!=j));k++){
	for(int l=0;l<8  && ((l!=i)&&(l!=j)&&(l!=k));l++){
	  for(int m=0;m<8  && ((m!=i)&&(m!=j)&&(m!=k)&&(m!=l));m++){
	    cout << "5-->" << i<< j << k << l << m << endl;
	    tmpcomb.push_back(i); tmpcomb.push_back(j);
	    tmpcomb.push_back(k); tmpcomb.push_back(l);
	    tmpcomb.push_back(m);
	    comb.push_back(tmpcomb);
	    tmpcomb.clear();
	    for(int n=0;n<8  && ((n!=i)&&(n!=j)&&(n!=k)&&(n!=l)&&(n!=m));n++){
	      cout << "6-->" << i<< j << k << l << m  << n << endl;
	      tmpcomb.push_back(i); tmpcomb.push_back(j);
	      tmpcomb.push_back(k); tmpcomb.push_back(l);
	      tmpcomb.push_back(m); tmpcomb.push_back(n);
	      comb.push_back(tmpcomb);
	      tmpcomb.clear();
	      for(int o=0;o<8  && ((o!=i)&&(o!=j)&&(o!=k)&&(o!=l)&&(o!=m)&&(o!=n));o++){
		cout << "7-->" << i<< j << k << l << m  << n << o << endl;
		tmpcomb.push_back(i); tmpcomb.push_back(j);
		tmpcomb.push_back(k); tmpcomb.push_back(l);
		tmpcomb.push_back(m); tmpcomb.push_back(n);
		tmpcomb.push_back(o);
		comb.push_back(tmpcomb);
		tmpcomb.clear();
	      }
	    }
	  }
	}
      }
    }
  }

  for(int ch=0;ch<8;ch++)tmpcomb.push_back(ch);
  comb.push_back(tmpcomb);
  tmpcomb.clear();

  
  double tmpeff,trgeff=0;
  for(int i=0;i<comb.size();i++){
    tmpeff=1;
    for(int ch=0;ch<8;ch++){
      if(find(comb.at(i).begin(), comb.at(i).end(), ch)!=comb.at(i).end()){
	tmpeff*=eff[ch];
      }else{
	tmpeff*=(1-eff[ch]);
      }
    }
    for(int ch=0;ch<comb.at(i).size();ch++)cout << comb.at(i).at(ch);
    trgeff+=tmpeff;
    cout << "  tmpeff::" << tmpeff << endl;
  }
  
  cout << endl << endl <<  "trgeff::" << trgeff << endl;













}
