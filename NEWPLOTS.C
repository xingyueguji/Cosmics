#include <cmath>

void NEWPLOTS( Int_t nrun=55) {
    // if full replay : fullreplay = 1;
    // if single event: nevent = event number, replay = 1, fullreplay != 1;
    // if 20k replay: replay = 20k, fullreplay != 1;
    gStyle->SetOptFit(3);
    // --------------------------------------------------------------------------------
    
    TFile *f = new TFile(Form("nps_eel108_%d.root", nrun));
    TTree *t = (TTree*) f->Get("T");
    
    // Branch declaration -------------------------------------------------------------
    
    Int_t NSampWaveForm;
    t->SetBranchAddress("Ndata.NPS.cal.fly.adcSampWaveform",&NSampWaveForm);
    Double_t SampWaveForm[300000];
    t->SetBranchAddress("NPS.cal.fly.adcSampWaveform",&SampWaveForm);
    Int_t NadcCounter;
    t->SetBranchAddress("Ndata.NPS.cal.fly.adcCounter",&NadcCounter);
    Double_t adcCounter[2000];
    t->SetBranchAddress("NPS.cal.fly.adcCounter",&adcCounter);
    Double_t Pulsetime[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseTime",&Pulsetime);
    Double_t Amp[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseAmp",&Amp);
    
    //Histogram------------------------------------------------------------------------
    TH1D *TotalHit = new TH1D("TotalHit","TotalHit_Above_4mV",100,0,100);
    TH2D *TimeBlock = new TH2D("TimeBlock","Time_vs_Block",120,0,120,100,0,400);
    TH2D *TimeBlock_Zoom = new TH2D("TimeBlock_Zoom","Time_vs_Block",120,0,120,10,100,140);
    TH1D *TotalGoodHit = new TH1D("TotalGoodHit","Good Hit",100,0,100);
    TH1D *GoodHitPerBlock = new TH1D("GoodHitPerBlock","GoodHitPerBlock",120,0,120);
    TH2D *AmpBlock = new TH2D("AmpBlock","Amp_vs_Block",120,0,120,30,0,30);
    cout << "abc" << endl;




    //Filling Histogram
    Long64_t nentries = t->GetEntries();
    Int_t counterofgoodhit = 0;
    cout <<"Total Event Number is "<< nentries << endl;
    for(Int_t i=0; i<nentries; i++){
        t->GetEntry(i);
        TotalHit->Fill(NadcCounter); // For now, just use the # of elements in adccounter to count the hit, if multiple pulses, then need to add a loop here.
        for(Int_t j=0; j<NadcCounter; j++){
            TimeBlock->Fill(adcCounter[j]+1, Pulsetime[j]);
            TimeBlock_Zoom->Fill(adcCounter[j]+1, Pulsetime[j]);
            AmpBlock->Fill(adcCounter[j]+1,Amp[j]);
            if(Pulsetime[j]<= 130 or Pulsetime[j]>= 110){
                counterofgoodhit += 1;
                GoodHitPerBlock->Fill(adcCounter[j]+1);
            }
        }
        TotalGoodHit->Fill(counterofgoodhit);
        counterofgoodhit = 0;
    }

    //Draw

    TCanvas *TimevsBlock = new TCanvas("TimevsBlock","",1600,1600);
    TimevsBlock->cd();
    TimeBlock->Draw("colz");
    TimevsBlock->SaveAs(Form("Time_vs_Block_%i.pdf",nrun));

    TimevsBlock->Clear();
    TimevsBlock->cd();
    TimeBlock_Zoom->Draw("colz");
    TimevsBlock->SaveAs(Form("Time_vs_Block_Zoom_%i.pdf",nrun));

    TCanvas *TotHits = new TCanvas("TotHits","",1600,1600);
    TotHits->cd();
    TotalHit->Draw();
    TotHits->SaveAs(Form("Total_Hits_%i.pdf",nrun));

    TotHits->Clear();
    TotHits->cd();
    TotalGoodHit->Draw();
    TotHits->SaveAs(Form("Total_Good_Hits_%i.pdf",nrun));

    TotHits->Clear();
    TotHits->cd();
    GoodHitPerBlock->Draw();
    TotHits->SaveAs(Form("Total_Good_Hits_Per_Block_%i.pdf",nrun));

    TCanvas *Amp = new TCanvas("Amp","",1600,1600);
    Amp->cd();
    AmpBlock->Draw("colz");
    Amp->SaveAs(Form("Amp_VS_Block_%i.pdf",nrun));

}