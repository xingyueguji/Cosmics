void Task1( Int_t nrun=1310,Double_t nblock = 3, Int_t nevent = 0) {

  gStyle->SetOptTitle(1);
  // --------------------------------------------------------------------------------

  TFile *f = new TFile(Form("nps_eel108_%d.root", nrun));
  TTree *t = (TTree*) f->Get("T");

  Int_t NSampWaveForm;
  t->SetBranchAddress("Ndata.NPS.cal.fly.adcSampWaveform",&NSampWaveForm) ;
  Double_t SampWaveForm[300000];
  t->SetBranchAddress("NPS.cal.fly.adcSampWaveform",&SampWaveForm) ;


  TH1F *h_SampWaveForm = new TH1F("h_SampWaveForm",Form("NPS_nrun_%d_nevent_%d_nblock_%f",nrun,nevent,nblock),82,0,82*4);
  //histogram with 82 bins, bin width = 4ns
    h_SampWaveForm->SetLineColor(2);
    h_SampWaveForm->SetLineWidth(2);
  Int_t SampPMT;
  Int_t NSamp;
  Double_t SampADC;
  
  Long64_t nentries = t->GetEntries();
  TCanvas *CanSamp= new TCanvas("CanSamp","NPS Sample",800,800);
    CanSamp->Divide(1,1);
    
  // --------------------------------------------------------------------------------
    Int_t ns = 0;
    Int_t location = 0;
    cout<<"abc"<<endl;
    
    t->GetEntry(nevent);
    
    for(Int_t i = 0; i< NSampWaveForm; i++){
        if(SampWaveForm[i] == nblock){
            location = i;
            cout<<location<<endl;
            break;
        }
    }
    //locate the position of nblock since currently SampWaveForm stored all the blocks

      SampPMT=SampWaveForm[location++];
        cout<<"We are at "<<SampPMT<<endl;
      NSamp=SampWaveForm[location++];
    cout<<"Nsamp "<< NSamp << endl;
      for (Int_t n=0;n<NSamp;n++) {
    SampADC=SampWaveForm[location++];
    h_SampWaveForm->SetBinContent(n+1,SampADC);
          cout<<"SampADC"<< SampADC << endl;
      }
    
    //filling the histogram
    
    
    h_SampWaveForm->GetXaxis()->SetTitle("Time(4ns each bin)");
    h_SampWaveForm->GetYaxis()->SetTitle("ADC value (mV)");
    int xx = int(nblock);
    h_SampWaveForm->SetTitle(Form("NPS_nrun_%d_nevent_%d_nblock_%d",nrun,nevent,xx));



    CanSamp->cd(1);
    h_SampWaveForm->SetMarkerStyle(2);
    h_SampWaveForm->Fit("gaus");
    h_SampWaveForm->Draw("P");
    
    CanSamp->SaveAs(Form("~/NPS/Singleblock_run_%d_event_%d.pdf",nrun,nevent));
}
