#include <cmath>

void cosmics( Int_t nrun=1355, Int_t nevent=20, Int_t replay = 1, Int_t fullreplay = 1) {
    // if full replay : fullreplay = 1;
    // if single event: nevent = event number, replay = 1, fullreplay != 1;
    // if 20k replay: replay = 20k, fullreplay != 1;
    
    
    
    //FIXME: add # of pulses above threshold for the entire run per PMT.
    //Also the # of PMT being written in coda file but below threshold.
    //trigger threshold and analysis threshold.
    // # of pulses per event per PMT
    // if 2 pulses how the amp array got filled.
    // check def file how the histogram is created and filled.
    // cc julie in the email. 
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
    TCanvas *Intmean = new TCanvas("Intmean","integral",3200,3200);
    TCanvas *Pedmean = new TCanvas("Pedmean","Pedestal",3200,3200);
    TCanvas *Pulsetimemean = new TCanvas("Pulsetimemean","Pulse time",3200,3200);
    
    TCanvas *Ampsingleplot = new TCanvas("Ampsingleplot","",3200,3200);
    TCanvas *Intsingleplot = new TCanvas("Intsingleplot","",3200,3200);
    TCanvas *Pedsingleplot = new TCanvas("Pedsingleplot","",3200,3200);
    TCanvas *Pulsetimesingleplot = new TCanvas("Pulsetimesingleplot","",3200,3200);
    
    TH1F *h_SingleBlockAmp[1080];
    TH1F *h_SingleBlockInt[1080];
    TH1F *h_SingleBlockTime[1080];
    TH1F *h_SingleBlockPed[1080];
    
    for (int n = 0; n < 1080; n++){
        h_SingleBlockAmp[n] = new TH1F(Form("h_SingleBlockAmp_%d",n),"",50,0,50);
        h_SingleBlockInt[n] = new TH1F(Form("h_SingleBlockInt_%d",n),"",50,0,20);//range to be determined
        h_SingleBlockTime[n] = new TH1F(Form("h_SingleBlockTime_%d",n),"",50,0,250);
        h_SingleBlockPed[n] = new TH1F(Form("h_SingleBlockPed_%d",n),"",50,0,120);
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
            h_SingleBlockInt[counter]->Fill(Int[a]);
            h_SingleBlockTime[counter]->Fill(Pulsetime[a]);
            h_SingleBlockPed[counter]->Fill(Ped[a]);
        }
    }
    else{
        for(Int_t i=0; i<fullreplay1; i++){
            t->GetEntry(i);
            if(i % 10000 == 0){
                cout << "Processing event "<< i << endl;
            }
            for (Int_t a=0; a<NadcCounter; a++){
                counter = int(adcCounter[a]);
                h_SingleBlockAmp[counter]->Fill(Amp[a]);
                h_SingleBlockInt[counter]->Fill(Int[a]);
                h_SingleBlockTime[counter]->Fill(Pulsetime[a]);
                h_SingleBlockPed[counter]->Fill(Ped[a]);
            }
        }
    }
    
    

    
    int fitrangelower;
    int fitrangehigher;
    double meanofamp;
    double meanofint;
    double meanofped;
    double meanoftime;
    
    int fitlowerint;
    int fithigherint;
    int fitlowerped;
    int fithigherped;
    int fitlowertime;
    int fithighertime;
    
    double fittedmeanamp[1080] = {0};
    double fittedmeanint[1080] = {0};
    double fittedmeanped[1080] = {0};
    double fittedmeantime[1080] = {0};
    
    for (int i = 0; i < 1080; i++){
        if(replay == 1 && fullreplay != 1){
            fittedmeanamp[i] = atof(Form("%.1f",h_SingleBlockAmp[i]->GetMean()));
            fittedmeanint[i] = atof(Form("%.1f",h_SingleBlockInt[i]->GetMean()));
            fittedmeanped[i] = atof(Form("%.1f",h_SingleBlockPed[i]->GetMean()));
            fittedmeantime[i] = atof(Form("%.1f",h_SingleBlockTime[i]->GetMean()));
        }
        else{
            meanofamp = h_SingleBlockAmp[i]->GetMean();
            meanofint = h_SingleBlockInt[i]->GetMean();
            meanofped = h_SingleBlockPed[i]->GetMean();
            meanoftime = h_SingleBlockTime[i]->GetMean();
            if (meanofamp < 5 && meanofamp > 0){
                fitrangelower = 3;
            }
            /*else if (meanofamp == 0){ // see what's the result of fitting empty histogram
                continue;
            }*/
            else{
                fitrangelower = int(meanofamp) - 5;
            }
            /*if (meanofint < 5 && meanofint > 0){ //FIXME: change parameter
                fitlowerint = 3;
            }

            else{
                fitlowerint = int(meanofint) - 5;
            }
            if (meanofped < 5 && meanofped > 0){
                fitlowerped = 3;
            }

            else{
                fitlowerped = int(meanofped) - 5;
            }
            if (meanoftime < 5 && meanoftime > 0){
                fitlowertime = 3;
            }
            else{
                fitlowertime = int(meanoftime) - 5;
            }*/
            
            fitrangehigher = int(meanofamp) + 10; //define fit range, fit the histogram  // +- RMS of the histogram
            fithigherped = int(meanofint) + 10; //FIXME: CHANGE PARAMETER
            fithighertime = int(meanoftime) + 10;
            fithigherint = int(meanofint) + 10;
            TF1 *fitFunction = new TF1("fitFunction", "gaus", fitrangelower, fitrangehigher); // FIXME: change lower limit
            TF1 *fitFunctionint = new TF1("fitFunctionint","gaus",2,14);
            TF1 *fitFunctionPed = new TF1("fitFunctionPed","gaus",80,100);
            TF1 *fitFunctionTime = new TF1("fitFunctionTime","gaus",70,100); //FIXME: change parameter
            
            h_SingleBlockAmp[i]->Fit(fitFunction);
            h_SingleBlockInt[i]->Fit(fitFunctionint);
            h_SingleBlockPed[i]->Fit(fitFunctionPed);
            h_SingleBlockTime[i]->Fit(fitFunctionTime);
            
            if (h_SingleBlockAmp[i]->GetEntries() > 100){ // exclude bad fit
                fittedmeanamp[i] = atof(Form("%.1f",h_SingleBlockAmp[i]->GetFunction("fitFunction")->GetParameter(1)));
            }
            if (h_SingleBlockInt[i]->GetEntries() > 100){ // exclude bad fit
                fittedmeanint[i] = atof(Form("%.1f",h_SingleBlockInt[i]->GetFunction("fitFunctionint")->GetParameter(1)));
            }
            if (h_SingleBlockPed[i]->GetEntries() > 100){ // exclude bad fit
                fittedmeanped[i] = atof(Form("%.1f",h_SingleBlockPed[i]->GetFunction("fitFunctionPed")->GetParameter(1)));
            }
            if (h_SingleBlockTime[i]->GetEntries() > 100){ // exclude bad fit
                fittedmeantime[i] = atof(Form("%.1f",h_SingleBlockTime[i]->GetFunction("fitFunctionTime")->GetParameter(1)));
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
    
    for (int i=1; i<271; i++){
        Intsingleplot->Clear();
        Intsingleplot->Divide(2,2);
        for (int j=1; j<5; j++){
            Intsingleplot->cd(j);
            h_SingleBlockInt[(i-1)*4 + j - 1]->Draw();
        }
        Intsingleplot->SaveAs(Form("~/NPS/singleblockint/canvas_%d.pdf",i));
    }
    
    for (int i=1; i<271; i++){
        Pedsingleplot->Clear();
        Pedsingleplot->Divide(2,2);
        for (int j=1; j<5; j++){
            Pedsingleplot->cd(j);
            h_SingleBlockPed[(i-1)*4 + j - 1]->Draw();
        }
        Pedsingleplot->SaveAs(Form("~/NPS/singleblockped/canvas_%d.pdf",i));
    }
    for (int i=1; i<271; i++){
        Pulsetimesingleplot->Clear();
        Pulsetimesingleplot->Divide(2,2);
        for (int j=1; j<5; j++){
            Pulsetimesingleplot->cd(j);
            h_SingleBlockTime[(i-1)*4 + j - 1]->Draw();
        }
        Pulsetimesingleplot->SaveAs(Form("~/NPS/singleblocktime/canvas_%d.pdf",i));
    }
    
    
    
    
    for(Int_t i=0; i<1080; i++){
        if(fittedmeanamp[i] > 0){//some histogram has too less bin to do the fitting // Check the mapping
            mappingAmp->SetBinContent(30 - i % 30 , i/30 + 1, fittedmeanamp[i]);
        }
        if(fittedmeanint[i]>0){
            mappingInt->SetBinContent(30 - i % 30 , i/30 + 1, fittedmeanint[i]);
        }
        if(fittedmeanped[i]>0){
            mappingPed->SetBinContent(30 - i % 30 , i/30 + 1, fittedmeanped[i]);
        }
        if(fittedmeantime[i]>0){
            mappingPulsetime->SetBinContent(30 - i % 30 , i/30 + 1, fittedmeantime[i]);
        }
    }
    
    Ampmean->cd();
    gPad->SetGrid(1,1);
    mappingAmp->SetMarkerColor(kBlack);
    mappingAmp->Draw("colztext");
    Ampmean->SaveAs(Form("~/NPS/Ampmapping_run_%d_fullreplay_%d.pdf",nrun,fullreplay));
    
    Intmean->cd();
    gPad->SetGrid(1,1);
    mappingInt->SetMarkerColor(kBlack);
    mappingInt->Draw("colztext");
    Intmean->SaveAs(Form("~/NPS/Intmapping_run_%d_fullreplay_%d.pdf",nrun,fullreplay));
    
    Pedmean->cd();
    gPad->SetGrid(1,1);
    mappingPed->SetMarkerColor(kBlack);
    mappingPed->Draw("colztext");
    Pedmean->SaveAs(Form("~/NPS/Pedmapping_run_%d_fullreplay_%d.pdf",nrun,fullreplay));
    
    Pulsetimemean->cd();
    gPad->SetGrid(1,1);
    mappingPulsetime->SetMarkerColor(kBlack);
    mappingPulsetime->Draw("colztext");
    Pulsetimemean->SaveAs(Form("~/NPS/Timemapping_run_%d_fullreplay_%d.pdf",nrun,fullreplay));
    
    
}
