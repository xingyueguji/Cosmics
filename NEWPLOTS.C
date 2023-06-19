#include <cmath>

void NEWPLOTS( Int_t nrun=536) {
    // if full replay : fullreplay = 1;
    // if single event: nevent = event number, replay = 1, fullreplay != 1;
    // if 20k replay: replay = 20k, fullreplay != 1;
    gStyle->SetOptFit(3);
    // --------------------------------------------------------------------------------
    
    TFile *f = new TFile(Form("../nps_eel108_%d.root", nrun));
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

    // Get some useful variables-------------------------------------------------------

    t->GetEntry(1);
    Int_t binnumber = SampWaveForm[1];
    
    //Histogram------------------------------------------------------------------------
    TH1D *TotalHit = new TH1D("TotalHit","TotalHit_Above_4mV",100,0,100); // X = # of blocks
    TH2D *TimeBlock = new TH2D("TimeBlock","Time_vs_Block",1080,0,1080,binnumber,0,binnumber * 4); // X = Block number, Y = Timing
    TH1D *TotalGoodHit = new TH1D("TotalGoodHit","Good Hit",100,0,100); // X = # of blocks
    TH1D *GoodHitPerBlock = new TH1D("GoodHitPerBlock","GoodHitPerBlock",1080,0,1080); // X = Block number
    TH2D *AmpBlock = new TH2D("AmpBlock","Amp_vs_Block",1080,0,1080,100,0,100); // X = Block number, Y = Amp
    TH2D *PulseBlock = new TH2D("PulseBlock","#_of_Pulses_vs_PMT",1080,0,1080,10,0,10); // 300 can be changed to total pmt number, now using 300 to test it.
    TH2D *GoodPulseBlock = new TH2D("GoodPulseBlock","#_of_GOOD_Pulses_vs_PMT",1080,0,1080,10,0,10); 
    TH1D *f_WaveForm = new TH1D("f_WaveForm","Waveform_4P",binnumber,0,binnumber * 4);
    TH1D *f_GoodWaveForm = new TH1D("f_GoodWaveForm","GoodWaveform_2P",binnumber,0,binnumber * 4);
    cout << "abc" << endl;

    //Arrays
    Int_t Pulsenumber[2000] = {}; // can change 120 to 36*30
    Int_t GoodPulsenumber[2000] = {};

    //Filling Histogram
    Long64_t nentries = t->GetEntries();
    Int_t counterofgoodhit = 0;

    cout <<"Total Event Number is "<< nentries << endl;
    for(Int_t i=0; i<nentries; i++){
        t->GetEntry(i);
        if(NadcCounter!=0){
            TotalHit->Fill(NadcCounter); 
        }
        for(Int_t j=0; j<NadcCounter; j++){
            TimeBlock->Fill(adcCounter[j], Pulsetime[j]);
            AmpBlock->Fill(adcCounter[j],Amp[j]);
            Pulsenumber[Int_t(adcCounter[j])] += 1;
        }
        /*Test WaveForm
        for(Int_t n=0; n<300; n++){
            if(Pulsenumber[n] == 4){
                cout <<  "Event number is " << i << "Block number is " << n << "Pulse number is " << Pulsenumber[n] << endl; 
                break;
            }
        }
        */

       /*for(Int_t n=0; n<300; n++){
            if(GoodPulsenumber[n] == 2){
                cout <<  "Event number is " << i << "Block number is " << n << "Good Pulse number is " << GoodPulsenumber[n] << endl; 
                break;
            }
        }*/
        for(Int_t k=0; k<1080; k++){
        PulseBlock->Fill(k,Pulsenumber[k]);
        }
        for(Int_t z=0;z<1080;z++){
            Pulsenumber[z] = 0;
        }
    }

    //Get Waveform (see the comments below, only displaying 4 pulses event)
    /*t->GetEntry(2);
    for(Int_t i =0; i<NSampWaveForm; i++){
        if(Int_t(SampWaveForm[i]) == 47){
            if(Int_t(SampWaveForm[i+1]!= 100)){
                continue;
            }
            for(Int_t j=0; j<100; j++){
                f_WaveForm->SetBinContent(j+1,SampWaveForm[i+2+j]);
            }
            break;
        }
    }*/
    /*t->GetEntry(1);
    for(Int_t i =0; i<NSampWaveForm; i++){
        if(Int_t(SampWaveForm[i]) == 196){
            if(Int_t(SampWaveForm[i+1]!= 100)){
                continue;
            }
            cout << SampWaveForm[i] << " " << SampWaveForm[i+1] << endl;
            for(Int_t j=0; j<100; j++){
                f_GoodWaveForm->SetBinContent(j+1,SampWaveForm[i+2+j]);
            }
            //cout << i << endl;
            break;
        }
    }*/





    /*TCanvas *C_WaveForm = new TCanvas("C_WaveForm","Waveform",800,800);
    C_WaveForm->cd();
    f_WaveForm->Draw();
    C_WaveForm->SaveAs(Form("../NewPlots/Waveform_%i.pdf",nrun));

    C_WaveForm->Clear();
    C_WaveForm->cd();
    f_GoodWaveForm->Draw();
    C_WaveForm->SaveAs(Form("../NewPlots/GoodWaveForm_%i.pdf",nrun));*/


    // Get the mean and change zoom in region ------------

    TimeBlock->FitSlicesX(nullptr,0,-1,0,"QNR");
    TH1D *f_ZoomMean = (TH1D*)gDirectory->Get("TimeBlock_1");
    Double_t TimeMean[1080] = {};
    f_ZoomMean->Draw();
    for(Int_t i=0;i<1080;i++){
        TimeMean[i] = f_ZoomMean->GetBinContent(i+1);
    }
    Double_t Sum = 0;
    Double_t nBINs = 0;
    for(Int_t i=0;i<1080;i++){
        if(TimeMean[i]!=0 and TimeMean[i] < binnumber * 4 and TimeMean[i] > 0){
            Sum+=TimeMean[i];
            nBINs += 1;
        }
    }
    Sum = Sum / nBINs;
    TH2D *TimeBlock_Zoom = new TH2D(*TimeBlock); // X = Block number, Y = zoom in Timing
    TimeBlock_Zoom->GetYaxis()->SetRangeUser(Sum - 40,Sum + 40);// FIXME: change range


    for(Int_t i=0; i<nentries; i++){
        t->GetEntry(i);
        for(Int_t j=0; j<NadcCounter; j++){
            if(Pulsetime[j]<= Sum + 40 and Pulsetime[j]>= Sum - 40){ // FIXME: change range
                counterofgoodhit += 1;
                GoodHitPerBlock->Fill(adcCounter[j]);
                GoodPulsenumber[Int_t(adcCounter[j])] += 1;
            }
        }

        for(Int_t k=0; k<1080; k++){
        GoodPulseBlock->Fill(k,GoodPulsenumber[k]);
        }
        if(counterofgoodhit !=0){
            TotalGoodHit->Fill(counterofgoodhit);
        }
        counterofgoodhit = 0;
        for(Int_t z=0;z<1080;z++){
            GoodPulsenumber[z] = 0;
        }
    }    

    //Draw

    TCanvas *TimevsBlock = new TCanvas("TimevsBlock","",800,800);
    TimevsBlock->cd();
    TimeBlock->Draw("colz");
    TimevsBlock->SaveAs(Form("../NewPlots/Time_vs_Block_%i.pdf",nrun));

    TimevsBlock->Clear();
    TimevsBlock->cd();
    TimeBlock_Zoom->Draw("colz");
    TimevsBlock->SaveAs(Form("../NewPlots/Time_vs_Block_Zoom_%i.pdf",nrun));

    TCanvas *TotHits = new TCanvas("TotHits","",800,800);
    TotHits->cd();
    TotalHit->Draw();
    TotHits->SaveAs(Form("../NewPlots/Total_Hits_%i.pdf",nrun));

    TotHits->Clear();
    TotHits->cd();
    TotalGoodHit->Draw();
    TotHits->SaveAs(Form("../NewPlots/Total_Good_Hits_%i.pdf",nrun));

    TotHits->Clear();
    TotHits->cd();
    GoodHitPerBlock->Draw();
    TotHits->SaveAs(Form("../NewPlots/Total_Good_Hits_Per_Block_%i.pdf",nrun));

    TCanvas *C_Amp = new TCanvas("Amp","",800,800);
    C_Amp->cd();
    AmpBlock->Draw("colz");
    C_Amp->SaveAs(Form("../NewPlots/Amp_VS_Block_%i.pdf",nrun));

    TCanvas *NumPulse = new TCanvas("NumPulse","#_of_Pulses_PMT_number",800,800);
    NumPulse->cd();
    PulseBlock->Draw("colz");
    NumPulse->SaveAs(Form("../NewPlots/#Pulse_vs_PMT_%i.pdf",nrun));
    NumPulse->Clear();
    NumPulse->cd();
    GoodPulseBlock->Draw("colz");
    NumPulse->SaveAs(Form("../NewPlots/GOOD#Pulse_vs_PMT_%i.pdf",nrun));

}

//For Run 55
/*Event number is 2Block number is 47Pulse number is 4
Event number is 45Block number is 63Pulse number is 4
Event number is 47Block number is 8Pulse number is 4
Event number is 55Block number is 0Pulse number is 4
Event number is 68Block number is 27Pulse number is 4
Event number is 76Block number is 18Pulse number is 4
Event number is 118Block number is 61Pulse number is 4
Event number is 159Block number is 87Pulse number is 4
Event number is 160Block number is 61Pulse number is 4
Event number is 218Block number is 54Pulse number is 4
Event number is 227Block number is 18Pulse number is 4
Event number is 241Block number is 61Pulse number is 4
Event number is 267Block number is 87Pulse number is 4
Event number is 327Block number is 18Pulse number is 4
Event number is 491Block number is 60Pulse number is 4
Event number is 502Block number is 61Pulse number is 4
Event number is 545Block number is 18Pulse number is 4
Event number is 554Block number is 18Pulse number is 4
Event number is 578Block number is 87Pulse number is 4
Event number is 598Block number is 99Pulse number is 4
Event number is 605Block number is 18Pulse number is 4
Event number is 646Block number is 89Pulse number is 4
Event number is 672Block number is 52Pulse number is 4
Event number is 673Block number is 61Pulse number is 4
Event number is 692Block number is 58Pulse number is 4
Event number is 724Block number is 58Pulse number is 4
Event number is 731Block number is 87Pulse number is 4
Event number is 846Block number is 58Pulse number is 4
Event number is 919Block number is 52Pulse number is 4
Event number is 931Block number is 87Pulse number is 4
Event number is 965Block number is 87Pulse number is 4
Event number is 970Block number is 27Pulse number is 4
Event number is 990Block number is 18Pulse number is 4
Event number is 1082Block number is 85Pulse number is 4
Event number is 1107Block number is 196Pulse number is 4
Event number is 1128Block number is 58Pulse number is 4
Event number is 1130Block number is 61Pulse number is 4
Event number is 1135Block number is 87Pulse number is 4
Event number is 1230Block number is 101Pulse number is 4
Event number is 1234Block number is 18Pulse number is 4
Event number is 1291Block number is 61Pulse number is 4
Event number is 1340Block number is 61Pulse number is 4
Event number is 1358Block number is 61Pulse number is 4
Event number is 1365Block number is 196Pulse number is 4
Event number is 1431Block number is 196Pulse number is 4
Event number is 1462Block number is 87Pulse number is 4
Event number is 1486Block number is 60Pulse number is 4*/



//GOOD PULSE NUMBER
// NO 2 pulses event if apply strict timing cut

