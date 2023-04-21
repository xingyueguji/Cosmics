#include <cmath>

void cosmics( Int_t nrun=1310, Int_t nevent=0, Int_t replay = 1000, Int_t fullreplay = 1) {
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
    Double_t Amp[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseAmp",&Amp);
    Double_t Int[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseInt",&Int);
    Double_t Ped[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPed",&Ped);
    Double_t Pulsetime[2000];
    t->SetBranchAddress("NPS.cal.fly.adcSampPulseTime",&Pulsetime);
    
    //Histogram------------------------------------------------------------------------
    
    TH2F *mapping = new TH2F("mapping","NPS",30,0,30,36,0,36);
    TH1F *Waveform = new TH1F("Waveform","",82,0,82*4);
    
    TH2F *mappingAmp = new TH2F("mappingAmp","Amp Mean",30,0,30,36,0,36);
    TH2F *mappingInt = new TH2F("mappingInt","Integral Mean",30,0,30,36,0,36);
    TH2F *mappingPed = new TH2F("mappingPed","Ped Mean",30,0,30,36,0,36);
    TH2F *mappingPulsetime = new TH2F("mappingPulsetime","Pulse Time mean",30,0,30,36,0,36);
    
    TCanvas *Ampmean = new TCanvas("Ampmean","Amp",3200,3200);
    TCanvas *Intmean = new TCanvas("Intmean","integral",1600,1600);
    TCanvas *Pedmean = new TCanvas("Pedmean","Pedestal",1600,1600);
    TCanvas *Pulsetimemean = new TCanvas("Pulsetimemean","Pulse time",1600,1600);
    TCanvas *SingleAmpCan = new TCanvas("SingleAmpCan","single block amp",800,800);
    
    TCanvas *Ampsingleplot = new TCanvas("Ampsingleplot","",3200,3200);
    TH1F *h_SingleBlockAmp[1080];
    for (int n = 0; n < 1080; n++){
        h_SingleBlockAmp[n] = new TH1F(Form("h_SingleBlockAmp_%d",n),"",50,0,50);
    }
    
    
    Int_t nPMT;
    Int_t location;
    Int_t NSamp;
    Double_t SampADC;
    Double_t TimingArray[NadcCounter];
    
    
    for (int i = 1; i <= 30; i++) {
        mappingAmp->GetXaxis()->SetBinLabel(i, Form("%d", 31 - i - 1));
        mappingInt->GetXaxis()->SetBinLabel(i, Form("%d", 31 - i - 1));
        mappingPed->GetXaxis()->SetBinLabel(i, Form("%d", 31 - i - 1));
        mappingPulsetime->GetXaxis()->SetBinLabel(i, Form("%d", 31 - i - 1));
      }
    for (int i = 1; i <= 36; i++) {
        mappingAmp->GetYaxis()->SetBinLabel(i, Form("%d", i - 1));
        mappingInt->GetYaxis()->SetBinLabel(i, Form("%d", i - 1));
        mappingPed->GetYaxis()->SetBinLabel(i, Form("%d", i - 1));
        mappingPulsetime->GetYaxis()->SetBinLabel(i, Form("%d", i - 1));
      }
    
    //----------------------------------------------------------------------------------
    
    int counter;
    Long64_t nentries = t->GetEntries();
    int fullreplay1;
    
    if (fullreplay == 1){
        fullreplay1 = nentries;
    }
    else{
        fullreplay1 = replay;
    }//determine if full replay or 20k
    
    
    if(replay == 1 && fullreplay != 1){
        t->GetEntry(nevent);
        for (Int_t a=0; a<NadcCounter; a++){
            counter = int(adcCounter[a]);
            h_SingleBlockAmp[counter]->Fill(Amp[a]);
        }
    }
    else{
        for(Int_t i=0; i<fullreplay1; i++){
            t->GetEntry(i);
            for (Int_t a=0; a<NadcCounter; a++){
                counter = int(adcCounter[a]);
                h_SingleBlockAmp[counter]->Fill(Amp[a]);
                //TotIntegral[counter] += Int[a];
                //TotPulseTime[counter] += Pulsetime[a];
                //TotPedestal[counter] += Ped[a];
                
                /*cout <<"Event Number is"<< i << "PMT number is" << counter << "Amp is" << Amp[counter] << endl;
                cout << "total Amp is" << TotAmp[counter] << endl;*/
            }
        }
    }
    
    

    
    int fitrangelower;
    int fitrangehigher;
    double meanofamp;
    double fittedmeanamp[1080] = {0};
    
    for (int i = 0; i < 1080; i++){
        if(replay == 1 && fullreplay != 1){
            fittedmeanamp[i] = atof(Form("%.1f",h_SingleBlockAmp[i]->GetMean()));
        }
        else{
            meanofamp = h_SingleBlockAmp[i]->GetMean();
            cout << meanofamp << endl;
            if (meanofamp < 5 && meanofamp > 0){
                fitrangelower = 3;
            }
            else if (meanofamp == 0){
                continue;
            }
            else{
                fitrangelower = int(meanofamp) - 5;
            }
            fitrangehigher = int(meanofamp) + 10; //define fit range, fit the histogram
            TF1 *fitFunction = new TF1("fitFunction", "gaus", fitrangelower, fitrangehigher); // FIXME: change lower limit
            h_SingleBlockAmp[i]->Fit(fitFunction);
            cout << h_SingleBlockAmp[i]->GetFunction("fitFunction")->GetParameter(1) << endl;
            if (h_SingleBlockAmp[i]->GetEntries() > 100){ // exclude bad fit
                fittedmeanamp[i] = atof(Form("%.1f",h_SingleBlockAmp[i]->GetFunction("fitFunction")->GetParameter(1)));
            }
            
        }
    }
    
    
    
    for (int i=1; i<271; i++){
        Ampsingleplot->Clear();
        Ampsingleplot->Divide(2,2);
        for (int j=1; j<5; j++){
            Ampsingleplot->cd(j);
            h_SingleBlockAmp[(i-1)*4 + j - 1]->Draw();
        }
        Ampsingleplot->SaveAs(Form("~/NPS/singleblockamp/canvas_%d.pdf",i));
    }
    
    for(Int_t i=0; i<1080; i++){
        if(fittedmeanamp[i] > 0){//some histogram has too less bin to do the fitting
            mappingAmp->SetBinContent(30 - i % 30 , i/30 + 1, fittedmeanamp[i]);
        }
        
    }
    
    Ampmean->cd();
    gPad->SetGrid(1,1);
    mappingAmp->SetMarkerColor(kBlack);
    mappingAmp->Draw("colztext");
    Ampmean->SaveAs("~/NPS/Ampmapping.pdf");
    
    
}
