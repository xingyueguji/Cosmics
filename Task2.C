void Task2( Int_t nrun=582, Int_t nblock=181, Int_t nevent = 123) {

  gStyle->SetOptTitle(1);
  // --------------------------------------------------------------------------------

  TFile *f = new TFile(Form("nps_eel108_%d.root", nrun));
  TTree *t = (TTree*) f->Get("T");

  Int_t NSampWaveForm;
  t->SetBranchAddress("Ndata.NPS.cal.fly.adcSampWaveform",&NSampWaveForm) ;
  Double_t SampWaveForm[300000];
  t->SetBranchAddress("NPS.cal.fly.adcSampWaveform",&SampWaveForm) ;

    Int_t NofChannel;
    t->SetBranchAddress("Ndata.NPS.cal.fly.adcCounter",&NofChannel);
    Double_t Channel[2000];
    t->SetBranchAddress("NPS.cal.fly.adcCounter",&Channel);
    Double_t Amp[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseAmp",&Amp);
    Double_t Int[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseInt",&Int);
    Double_t Ped[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPed",&Ped);
    Double_t Pulsetime[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseTime",&Pulsetime);

  TH1F *h_amp = new TH1F("h_amp",Form("Amplitude_run_%d",nrun),100,0,150);
    TH1F *h_int = new TH1F("h_int",Form("Integral_run_%d",nrun),100,0,100);
    TH1F *h_ped = new TH1F("h_ped",Form("Pedestal_run_%d",nrun),500,0,150);
    TH1F *h_pulsetime = new TH1F("h_pulsetime",Form("Pulse Time_run_%d",nrun),100,0,250);
    
    h_amp->GetXaxis()->SetTitle(Form("Pulse Amplistude from PMT_%d_(mV)",nblock));
    h_int->GetXaxis()->SetTitle(Form("Pulse Integral from PMT_%d_(pC)",nblock));
    h_ped->GetXaxis()->SetTitle(Form("Pulse Pedestal from PMT_%d_(mV)",nblock));
    h_pulsetime->GetXaxis()->SetTitle(Form("Pulse Time from PMT_%d_(ns)",nblock));
    
    
    TH1F *h_amp_total = new TH1F("h_amp_total","",600,0,600);
      TH1F *h_int_total = new TH1F("h_int_total","",600,0,600);
      TH1F *h_ped_total = new TH1F("h_ped_total","",600,0,600);
      TH1F *h_pulsetime_total = new TH1F("h_pulsetime_total","",600,0,600);
    
  
  Long64_t nentries = t->GetEntries();
  TCanvas *CanSamp= new TCanvas("CanSamp","",1600,1600);
    CanSamp->Divide(2,2);
    TCanvas *Summary = new TCanvas("Summary","",1600,1600);
    Summary->Divide(2,2);
    
  // --------------------------------------------------------------------------------
    for(Int_t i=0; i<nentries; i++){
        t->GetEntry(i);
        if (i%10000==0) cout << " Entry = " << i << endl;
        
        for (Int_t ii = 0; ii < NofChannel; ii++){
            if(Channel[ii] == nblock){
                h_amp->Fill(Amp[ii]);
                h_int->Fill(Int[ii]);
                h_ped->Fill(Ped[ii]);
                h_pulsetime->Fill(Pulsetime[ii]);
            }
        }
    }
    t->GetEntry(nevent);
    
    for (Int_t ii = 0; ii < NofChannel; ii++){
        h_amp_total->SetBinContent(Channel[ii],Amp[ii]);
        h_int_total->SetBinContent(Channel[ii],Int[ii]);
        h_pulsetime_total->SetBinContent(Channel[ii],Pulsetime[ii]);
        h_ped_total->SetBinContent(Channel[ii],Ped[ii]);
    }
    



    CanSamp->cd(1);
    h_amp->Draw();
    CanSamp->cd(2);
    h_int->Draw();
    CanSamp->cd(3);
    h_ped->Draw();
    CanSamp->cd(4);
    h_pulsetime->Draw();
    
    Summary->cd(1);
    h_amp_total->Draw();
    Summary->cd(2);
    h_int_total->Draw();
    Summary->cd(3);
    h_ped_total->Draw();
    Summary->cd(4);
    h_pulsetime_total->Draw();
    
    
    CanSamp->SaveAs(Form("~/NPS/multiplotsperPMT_run_%d_channel_%d.pdf",nrun,nblock));
    Summary->SaveAs(Form("~/NPS/Summary_run_%d_nevent_%d.pdf",nrun,nevent));
}
