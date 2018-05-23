
//  This script shows how to read with CERN-ROOT the wroot.root
// file containing some histos and a TTree produced by
// the wroot.cpp program.

// It is used in the inlib/exlib pages.

void rroot() {

  TFile* f = new TFile("wroot.root");
  if(!f) {
    std::cout << "TFile wroot.root not found." << std::endl;
    return;
  }

  TCanvas* plotter = new TCanvas("canvas","",10,10,800,800);
  plotter->Divide(2,2);  

  ////////////////////////////////////////////////////////
  /// histos /////////////////////////////////////////////
  ////////////////////////////////////////////////////////
 {TDirectory* dir = (TDirectory*)f->Get("histo");
  if(!dir) {
    std::cout << "TDirectory histos not found." << std::endl;    
    return;
  }
  TH1D* hrg = (TH1D*)dir->Get("rg");
  if(!hrg) {
    std::cout << "TH1D rg not found." << std::endl;    
    return;
  }
  Stat_t hstats[TH1::kNstat];
  hrg->GetStats(hstats);
  ::printf("TH1d : rg : sumw %g, sumw2 %g, sumwx %g, sumwx2 %g\n",hstats[0],hstats[1],hstats[2],hstats[3]);
  plotter->cd(1);
  hrg->Draw();

  TProfile* hprof = (TProfile*)dir->Get("prof");
  if(!hprof) {
    std::cout << "TProfile prof not found." << std::endl;    
    return;
  }
  Stat_t stats[TH1::kNstat];
  hprof->GetStats(stats);
  ::printf("TProfile : prof: sumw %g, sumw2 %g, sumwx %g, sumwx2 %g, sumwy %g, sumwy2 %g\n",
      stats[0],stats[1],stats[2],stats[3],stats[4],stats[5]);
  ::printf("TProfile : prof: mean %g, RMS %g\n",hprof->GetMean(),hprof->GetRMS());
  TArrayD* _fSumw2 = hprof->GetSumw2();
  Double_t Sw2 = 0;
 {Int_t binn = _fSumw2->fN-2;
  for(unsigned int ibin=1;ibin<=binn;ibin++) Sw2 += (*_fSumw2)[ibin];}
  ::printf("TProfile : prof : _fSumw2 %d, sum %g, Sw2 %g\n",_fSumw2->fN,_fSumw2->GetSum(),Sw2);
  plotter->cd(2);
  hprof->Draw();

  TH2D* hrgbw = (TH2D*)dir->Get("rgbw");
  if(!hrgbw) {
    std::cout << "TH2D rgbw not found." << std::endl;    
    return;
  }
  plotter->cd(3);
  hrgbw->Draw();}

  ////////////////////////////////////////////////////////
  /// TTree produced with wroot::ntuple //////////////////
  ////////////////////////////////////////////////////////
 {TH1D* hrg = new TH1D("hrg","Rand gauss",100,-5,5);

  TTree* tree = (TTree*)f->Get("rg_rbw");
  if(!tree) {
    std::cout << "TTree rg_rbw not found." << std::endl;    
    return;
  }

 {TObjArray* brs = tree->GetListOfBranches();
  for(int i=0;i<brs->GetEntries();i++) {
    TBranch* b = (TBranch*)brs->At(i);
    //std::cout << "branch : " << b->GetName() << std::endl;    
    b->Print();
  }}

  double rgauss;
  tree->SetBranchAddress("rgauss",&rgauss);

  int num = tree->GetEntries();  
  std::cout << "number of events = " << num << std::endl;

  for(int i=0;i<num;i++) {
    tree->GetEntry(i);
    hrg->Fill(rgauss,1);
  }

  plotter->cd(4);
  hrg->Draw();
  
  }

  plotter->Update();

}
