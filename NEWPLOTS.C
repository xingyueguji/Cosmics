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
    TH2D *PulseBlock = new TH2D("PulseBlock","#_of_Pulses_vs_PMT",300,0,300,10,0,10); // 300 can be changed to total pmt number, now using 300 to test it.
    TH2D *GoodPulseBlock = new TH2D("GoodPulseBlock","#_of_GOOD_Pulses_vs_PMT",300,0,300,10,0,10);
    cout << "abc" << endl;

    //Arrays
    Int_t Pulsenumber[300] = {}; // can change 120 to 36*30
    Int_t GoodPulsenumber[300] = {};

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
            Pulsenumber[Int_t(adcCounter[j])] += 1;
            if(Pulsetime[j]<= 130 or Pulsetime[j]>= 110){
                counterofgoodhit += 1;
                GoodHitPerBlock->Fill(adcCounter[j]+1);
                GoodPulsenumber[Int_t(adcCounter[j])] += 1;
            }
        }
        for(Int_t k=0; k<120; k++){
        PulseBlock->Fill(k+1,Pulsenumber[k]+1);
        GoodPulseBlock->Fill(k+1,GoodPulsenumber[k]+1);
        }
        TotalGoodHit->Fill(counterofgoodhit);
        counterofgoodhit = 0;
        for(Int_t z=0;z<300;z++){
            Pulsenumber[z] = 0;
        }
    }

    

    //Draw

    TCanvas *TimevsBlock = new TCanvas("TimevsBlock","",800,800);
    TimevsBlock->cd();
    TimeBlock->Draw("colz");
    TimevsBlock->SaveAs(Form("Time_vs_Block_%i.pdf",nrun));

    TimevsBlock->Clear();
    TimevsBlock->cd();
    TimeBlock_Zoom->Draw("colz");
    TimevsBlock->SaveAs(Form("Time_vs_Block_Zoom_%i.pdf",nrun));

    TCanvas *TotHits = new TCanvas("TotHits","",800,800);
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

    TCanvas *C_Amp = new TCanvas("Amp","",800,800);
    C_Amp->cd();
    AmpBlock->Draw("colz");
    C_Amp->SaveAs(Form("Amp_VS_Block_%i.pdf",nrun));

    TCanvas *NumPulse = new TCanvas("NumPulse","#_of_Pulses_PMT_number",800,800);
    NumPulse->cd();
    PulseBlock->Draw("colz");
    NumPulse->SaveAs(Form("#Pulse_vs_PMT_%i.pdf",nrun));
    NumPulse->Clear();
    NumPulse->cd();
    GoodPulseBlock->Draw("colz");
    NumPulse->SaveAs(Form("GOOD#Pulse_vs_PMT_%i.pdf",nrun));


}
