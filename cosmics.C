#include <cmath>

void cosmics( Int_t nrun=1310, Int_t nevent=0, Int_t replay = 1, Int_t fullreplay = 1) {
    
    gStyle->SetOptTitle(0);
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
    t->GetEntry(nevent);
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
    TF1 *fitFunction = new TF1("fitFunction", "gaus", 0,82*4);
    TH2F *mappingAmp = new TH2F("mappingAmp","Amp Mean",30,0,30,36,0,36);
    TH2F *mappingInt = new TH2F("mappingInt","Integral Mean",30,0,30,36,0,36);
    TH2F *mappingPed = new TH2F("mappingPed","Ped Mean",30,0,30,36,0,36);
    TH2F *mappingPulsetime = new TH2F("mappingPulsetime","Pulse Time mean",30,0,30,36,0,36);
    
    TCanvas *test = new TCanvas("test","test",800,800);
    TCanvas *Ampmean = new TCanvas("Ampmean","Amp",3200,3200);
    TCanvas *Intmean = new TCanvas("Intmean","integral",1600,1600);
    TCanvas *Pedmean = new TCanvas("Pedmean","Pedestal",1600,1600);
    TCanvas *Pulsetimemean = new TCanvas("Pulsetimemean","Pulse time",1600,1600);
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
    
    //testing only for one event and mapping parameter----------------------------------
    
    /*for (int i = 0; i < 36; i++){
        nPMT = adcCounter[i];
        //cout << nPMT << "nPMT is " << endl;
        for (int a = 0; a < NSampWaveForm; a++){
            if (SampWaveForm[a] == nPMT){
                location = a;
                cout << "location = "<< location << endl;
                break;
            }
        }
        NSamp = SampWaveForm[location+1];
        for (int n = 0; n < NSamp; n++){
            SampADC = SampWaveForm[location+2+n];
            Waveform->SetBinContent(n+1, SampADC);//cut on threshold
        }
        Waveform->Fit(fitFunction);
        //FIXME: Find a way to determine the goodness of the fit and how the threshold is applied.
        TimingArray[i] = fitFunction->GetParameter(1);
        //cout << TimingArray[i] << endl;
    }*/

    /*
    for (int i = 1; i <= mapping->GetNbinsY(); i++) {
        mapping->GetXaxis()->SetBinLabel(i, Form("%d", 31 - i - 1));
      }
    for (int i = 1; i <= mapping->GetNbinsY(); i++) {
        mapping->GetYaxis()->SetBinLabel(i, Form("%d", i - 1));
      }*/
    
    //----------------------------------------------------------------------------------
    Double_t TotAmp[1080]; // 1080 in total, 0 - 1079
    Double_t TotIntegral[1080];
    Double_t TotPulseTime[1080];
    Double_t TotPedestal[1080];
    Double_t MeanAmp[1080]; // 1080 in total, 0 - 1079
    Double_t MeanIntegral[1080];
    Double_t MeanPulseTime[1080];
    Double_t MeanPedestal[1080];
    
    for (int i = 0; i < 1080; i++){
        TotAmp[i] = 0;
        TotIntegral[i] = 0;
        TotPulseTime[i] = 0;
        TotPedestal[i] = 0;
        MeanAmp[i] = 0;
        MeanIntegral[i] = 0;
        MeanPulseTime[i] = 0;
        MeanPedestal[i] = 0;
    }
    int counter;
    Long64_t nentries = t->GetEntries();
    int fullreplay1;
    if (fullreplay == 1){
        fullreplay1 = nentries;
    }
    else{
        fullreplay1 = replay;
    }
    
    for(Int_t i=0; i<fullreplay1; i++){
        t->GetEntry(i);
        for (Int_t a=0; a<200; a++){
            if (int(adcCounter[a]) == 0 && a > 0){
                break;
            }
            counter = int(adcCounter[a]);
            TotAmp[counter] += Amp[a];
            //TotIntegral[counter] += Int[a];
            //TotPulseTime[counter] += Pulsetime[a];
            //TotPedestal[counter] += Ped[a];
            
            /*cout <<"Event Number is"<< i << "PMT number is" << counter << "Amp is" << Amp[counter] << endl;
            cout << "total Amp is" << TotAmp[counter] << endl;*/
            
        }
        
    }
    
    for(Int_t i=0; i<1080; i++){
        MeanAmp[i] = TotAmp[i]/ nentries;
        //MeanIntegral[i] = TotIntegral[i]/ nentries;
        //MeanPulseTime[i] = TotPulseTime[i]/ nentries;
        //MeanPedestal[i] = TotPedestal[i]/ nentries;
        MeanAmp[i] = atof(Form("%.1f", MeanAmp[i]));
    }
    
    for(Int_t i=0; i<1080; i++){
        cout << MeanAmp[i] << endl;
        mappingAmp->SetBinContent(30 - i % 30 , i/30 + 1, MeanAmp[i]);
    }
    
    
    
    Ampmean->cd();
    gPad->SetGrid(1,1);
    mappingAmp->SetMarkerColor(kBlack);
    mappingAmp->SetLabelSize(0.01,"Z");
    mappingAmp->Draw("colztext");
    Ampmean->SaveAs("~/NPS/test233.pdf");
    
    
    
    
    
    
    //-------------------------------------------------------------------------------------
    /*test->cd();
    Int_t xaxisposition;
    Int_t yaxisposition;
    Double_t abc = 0;
    for (int b = 0; b < 36; b++){
        if(adcCounter[b] == abc){
            break;
        }
        xaxisposition = 30 - int(adcCounter[b]) % 30 - 1;
        yaxisposition = int(adcCounter[b]) / 30;
        cout << "adcCounter is"<<adcCounter[b]<<endl;
        cout << xaxisposition << " x " << yaxisposition << " y " << endl;
        mapping->SetBinContent(xaxisposition+1,yaxisposition+1,TimingArray[b]);
        cout << TimingArray[b] << "Mean is " << endl;
    }
    mapping->SetMarkerColor(kBlack);
    mapping->Draw("colztext");
    test->SaveAs("~/NPS/test123.pdf");*/
    
    
}
