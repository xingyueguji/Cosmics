#include <cmath>

void NEWPLOTS( Int_t nrun=55) {
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
    
    //Histogram------------------------------------------------------------------------
    TH1D *TotalHit = new TH1D("TotalHit","TotalHit_Above_4mV",100,0,100);
    TH2D *TimeBlock = new TH2D("TimeBlock","Time_vs_Block",120,0,120,100,0,400);
    TH2D *TimeBlock_Zoom = new TH2D("TimeBlock_Zoom","Time_vs_Block",120,0,120,10,100,140);
    TH1D *TotalGoodHit = new TH1D("TotalGoodHit","Good Hit",100,0,100);
    TH1D *GoodHitPerBlock = new TH1D("GoodHitPerBlock","GoodHitPerBlock",120,0,120);
    TH2D *AmpBlock = new TH2D("AmpBlock","Amp_vs_Block",120,0,120,30,0,30);
    TH2D *PulseBlock = new TH2D("PulseBlock","#_of_Pulses_vs_PMT",300,0,300,10,0,10); // 300 can be changed to total pmt number, now using 300 to test it.
    TH2D *GoodPulseBlock = new TH2D("GoodPulseBlock","#_of_GOOD_Pulses_vs_PMT",300,0,300,10,0,10);
    TH1D *f_WaveForm = new TH1D("f_WaveForm","Waveform_4P",100,0,400);
    TH1D *f_GoodWaveForm = new TH1D("f_GoodWaveForm","GoodWaveform_2P",100,0,400);
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
        TotalHit->Fill(NadcCounter); 

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


        /*Test WaveForm
        for(Int_t n=0; n<300; n++){
            if(Pulsenumber[n] == 4){
                cout <<  "Event number is " << i << "Block number is " << n << "Pulse number is " << Pulsenumber[n] << endl; 
                break;
            }
        }
        */

       for(Int_t n=0; n<300; n++){
            if(GoodPulsenumber[n] == 2){
                cout <<  "Event number is " << i << "Block number is " << n << "Good Pulse number is " << GoodPulsenumber[n] << endl; 
                break;
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
            GoodPulsenumber[z] = 0;
        }
    }

    //Get Waveform (see the comments below, only displaying 4 pulses event)
    t->GetEntry(2);
    //cout << SampWaveForm[4692] <<" "<<  SampWaveForm[4693] << endl;
    for(Int_t i =0; i<NSampWaveForm; i++){
        if(Int_t(SampWaveForm[i]) == 47){
            if(Int_t(SampWaveForm[i+1]!= 100)){
                continue;
            }
            for(Int_t j=0; j<100; j++){
                f_WaveForm->SetBinContent(j+1,SampWaveForm[i+2+j]);
            }
            //cout << i << endl;
            break;
        }
    }
    t->GetEntry(1);
    for(Int_t i =0; i<NSampWaveForm; i++){
        if(Int_t(SampWaveForm[i]) == 196){
            if(Int_t(SampWaveForm[i+1]!= 100)){
                continue;
            }
            for(Int_t j=0; j<100; j++){
                f_GoodWaveForm->SetBinContent(j+1,SampWaveForm[i+2+j]);
            }
            //cout << i << endl;
            break;
        }
    }





    TCanvas *C_WaveForm = new TCanvas("C_WaveForm","Waveform",800,800);
    C_WaveForm->cd();
    f_WaveForm->Draw();
    C_WaveForm->SaveAs(Form("../Waveform_%i.pdf",nrun));

    C_Waveform->Clear();
    C_Waveform->cd();
    f_GoodWaveForm->Draw();
    C_WaveForm->SaveAs(Form("../GoodWaveForm_%i.pdf",nrun));



    

    //Draw
/*
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
    NumPulse->SaveAs(Form("GOOD#Pulse_vs_PMT_%i.pdf",nrun));*/

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

/*Event number is 1Block number is 196Good Pulse number is 2
Event number is 2Block number is 48Good Pulse number is 2
Event number is 11Block number is 196Good Pulse number is 2
Event number is 24Block number is 196Good Pulse number is 2
Event number is 26Block number is 196Good Pulse number is 2
Event number is 35Block number is 61Good Pulse number is 2
Event number is 45Block number is 26Good Pulse number is 2
Event number is 47Block number is 3Good Pulse number is 2
Event number is 55Block number is 24Good Pulse number is 2
Event number is 58Block number is 196Good Pulse number is 2
Event number is 68Block number is 16Good Pulse number is 2
Event number is 69Block number is 196Good Pulse number is 2
Event number is 76Block number is 17Good Pulse number is 2
Event number is 85Block number is 196Good Pulse number is 2
Event number is 88Block number is 196Good Pulse number is 2
Event number is 92Block number is 196Good Pulse number is 2
Event number is 94Block number is 196Good Pulse number is 2
Event number is 97Block number is 196Good Pulse number is 2
Event number is 118Block number is 54Good Pulse number is 2
Event number is 144Block number is 196Good Pulse number is 2
Event number is 145Block number is 196Good Pulse number is 2
Event number is 156Block number is 196Good Pulse number is 2
Event number is 160Block number is 27Good Pulse number is 2
Event number is 161Block number is 196Good Pulse number is 2
Event number is 163Block number is 55Good Pulse number is 2
Event number is 165Block number is 196Good Pulse number is 2
Event number is 218Block number is 47Good Pulse number is 2
Event number is 220Block number is 196Good Pulse number is 2
Event number is 227Block number is 16Good Pulse number is 2
Event number is 232Block number is 196Good Pulse number is 2
Event number is 241Block number is 48Good Pulse number is 2
Event number is 244Block number is 196Good Pulse number is 2
Event number is 250Block number is 196Good Pulse number is 2
Event number is 256Block number is 196Good Pulse number is 2
Event number is 267Block number is 58Good Pulse number is 2
Event number is 272Block number is 196Good Pulse number is 2
Event number is 306Block number is 196Good Pulse number is 2
Event number is 308Block number is 196Good Pulse number is 2
Event number is 314Block number is 196Good Pulse number is 2
Event number is 320Block number is 196Good Pulse number is 2
Event number is 327Block number is 20Good Pulse number is 2
Event number is 331Block number is 196Good Pulse number is 2
Event number is 334Block number is 196Good Pulse number is 2
Event number is 336Block number is 196Good Pulse number is 2
Event number is 338Block number is 196Good Pulse number is 2
Event number is 343Block number is 196Good Pulse number is 2
Event number is 347Block number is 196Good Pulse number is 2
Event number is 354Block number is 196Good Pulse number is 2
Event number is 356Block number is 196Good Pulse number is 2
Event number is 360Block number is 106Good Pulse number is 2
Event number is 372Block number is 196Good Pulse number is 2
Event number is 382Block number is 196Good Pulse number is 2
Event number is 389Block number is 196Good Pulse number is 2
Event number is 392Block number is 79Good Pulse number is 2
Event number is 411Block number is 57Good Pulse number is 2
Event number is 412Block number is 196Good Pulse number is 2
Event number is 414Block number is 196Good Pulse number is 2
Event number is 421Block number is 99Good Pulse number is 2
Event number is 426Block number is 18Good Pulse number is 2
Event number is 433Block number is 93Good Pulse number is 2
Event number is 435Block number is 196Good Pulse number is 2
Event number is 444Block number is 196Good Pulse number is 2
Event number is 449Block number is 196Good Pulse number is 2
Event number is 454Block number is 196Good Pulse number is 2
Event number is 456Block number is 196Good Pulse number is 2
Event number is 491Block number is 53Good Pulse number is 2
Event number is 502Block number is 51Good Pulse number is 2
Event number is 506Block number is 196Good Pulse number is 2
Event number is 512Block number is 196Good Pulse number is 2
Event number is 526Block number is 196Good Pulse number is 2
Event number is 529Block number is 196Good Pulse number is 2
Event number is 545Block number is 23Good Pulse number is 2
Event number is 554Block number is 24Good Pulse number is 2
Event number is 562Block number is 196Good Pulse number is 2
Event number is 563Block number is 196Good Pulse number is 2
Event number is 564Block number is 53Good Pulse number is 2
Event number is 567Block number is 196Good Pulse number is 2
Event number is 576Block number is 196Good Pulse number is 2
Event number is 578Block number is 61Good Pulse number is 2
Event number is 595Block number is 34Good Pulse number is 2
Event number is 605Block number is 19Good Pulse number is 2
Event number is 610Block number is 196Good Pulse number is 2
Event number is 626Block number is 196Good Pulse number is 2
Event number is 633Block number is 196Good Pulse number is 2
Event number is 648Block number is 196Good Pulse number is 2
Event number is 652Block number is 196Good Pulse number is 2
Event number is 656Block number is 27Good Pulse number is 2
Event number is 672Block number is 27Good Pulse number is 2
Event number is 673Block number is 27Good Pulse number is 2
Event number is 676Block number is 196Good Pulse number is 2
Event number is 690Block number is 196Good Pulse number is 2
Event number is 692Block number is 51Good Pulse number is 2
Event number is 695Block number is 196Good Pulse number is 2
Event number is 703Block number is 196Good Pulse number is 2
Event number is 706Block number is 99Good Pulse number is 2
Event number is 724Block number is 55Good Pulse number is 2
Event number is 726Block number is 196Good Pulse number is 2
Event number is 731Block number is 61Good Pulse number is 2
Event number is 757Block number is 37Good Pulse number is 2
Event number is 775Block number is 196Good Pulse number is 2
Event number is 776Block number is 196Good Pulse number is 2
Event number is 779Block number is 196Good Pulse number is 2
Event number is 788Block number is 61Good Pulse number is 2
Event number is 790Block number is 196Good Pulse number is 2
Event number is 798Block number is 33Good Pulse number is 2
Event number is 806Block number is 196Good Pulse number is 2
Event number is 808Block number is 196Good Pulse number is 2
Event number is 814Block number is 89Good Pulse number is 2
Event number is 817Block number is 196Good Pulse number is 2
Event number is 819Block number is 58Good Pulse number is 2
Event number is 823Block number is 196Good Pulse number is 2
Event number is 828Block number is 196Good Pulse number is 2
Event number is 830Block number is 196Good Pulse number is 2
Event number is 835Block number is 196Good Pulse number is 2
Event number is 846Block number is 78Good Pulse number is 2
Event number is 849Block number is 196Good Pulse number is 2
Event number is 857Block number is 196Good Pulse number is 2
Event number is 859Block number is 196Good Pulse number is 2
Event number is 869Block number is 196Good Pulse number is 2
Event number is 873Block number is 196Good Pulse number is 2
Event number is 888Block number is 196Good Pulse number is 2
Event number is 891Block number is 196Good Pulse number is 2
Event number is 906Block number is 196Good Pulse number is 2
Event number is 907Block number is 196Good Pulse number is 2
Event number is 914Block number is 196Good Pulse number is 2
Event number is 919Block number is 16Good Pulse number is 2
Event number is 926Block number is 196Good Pulse number is 2
Event number is 931Block number is 79Good Pulse number is 2
Event number is 946Block number is 196Good Pulse number is 2
Event number is 966Block number is 196Good Pulse number is 2
Event number is 970Block number is 52Good Pulse number is 2
Event number is 971Block number is 196Good Pulse number is 2
Event number is 990Block number is 21Good Pulse number is 2
Event number is 1000Block number is 196Good Pulse number is 2
Event number is 1035Block number is 196Good Pulse number is 2
Event number is 1037Block number is 85Good Pulse number is 2
Event number is 1048Block number is 85Good Pulse number is 2
Event number is 1071Block number is 196Good Pulse number is 2
Event number is 1073Block number is 196Good Pulse number is 2
Event number is 1075Block number is 196Good Pulse number is 2
Event number is 1081Block number is 196Good Pulse number is 2
Event number is 1082Block number is 78Good Pulse number is 2
Event number is 1090Block number is 196Good Pulse number is 2
Event number is 1091Block number is 196Good Pulse number is 2
Event number is 1092Block number is 196Good Pulse number is 2
Event number is 1096Block number is 99Good Pulse number is 2
Event number is 1108Block number is 196Good Pulse number is 2
Event number is 1110Block number is 196Good Pulse number is 2
Event number is 1124Block number is 196Good Pulse number is 2
Event number is 1128Block number is 59Good Pulse number is 2
Event number is 1130Block number is 58Good Pulse number is 2
Event number is 1135Block number is 57Good Pulse number is 2
Event number is 1142Block number is 85Good Pulse number is 2
Event number is 1154Block number is 196Good Pulse number is 2
Event number is 1160Block number is 196Good Pulse number is 2
Event number is 1162Block number is 93Good Pulse number is 2
Event number is 1164Block number is 196Good Pulse number is 2
Event number is 1191Block number is 196Good Pulse number is 2
Event number is 1198Block number is 196Good Pulse number is 2
Event number is 1218Block number is 96Good Pulse number is 2
Event number is 1220Block number is 196Good Pulse number is 2
Event number is 1221Block number is 196Good Pulse number is 2
Event number is 1222Block number is 196Good Pulse number is 2
Event number is 1230Block number is 63Good Pulse number is 2
Event number is 1234Block number is 16Good Pulse number is 2
Event number is 1242Block number is 78Good Pulse number is 2
Event number is 1243Block number is 196Good Pulse number is 2
Event number is 1251Block number is 196Good Pulse number is 2
Event number is 1258Block number is 196Good Pulse number is 2
Event number is 1266Block number is 196Good Pulse number is 2
Event number is 1269Block number is 196Good Pulse number is 2
Event number is 1271Block number is 196Good Pulse number is 2
Event number is 1273Block number is 196Good Pulse number is 2
Event number is 1274Block number is 196Good Pulse number is 2
Event number is 1281Block number is 196Good Pulse number is 2
Event number is 1289Block number is 78Good Pulse number is 2
Event number is 1291Block number is 24Good Pulse number is 2
Event number is 1309Block number is 196Good Pulse number is 2
Event number is 1314Block number is 196Good Pulse number is 2
Event number is 1316Block number is 61Good Pulse number is 2
Event number is 1321Block number is 196Good Pulse number is 2
Event number is 1324Block number is 8Good Pulse number is 2
Event number is 1328Block number is 196Good Pulse number is 2
Event number is 1331Block number is 196Good Pulse number is 2
Event number is 1333Block number is 196Good Pulse number is 2
Event number is 1336Block number is 196Good Pulse number is 2
Event number is 1340Block number is 51Good Pulse number is 2
Event number is 1342Block number is 196Good Pulse number is 2
Event number is 1343Block number is 196Good Pulse number is 2
Event number is 1345Block number is 196Good Pulse number is 2
Event number is 1357Block number is 196Good Pulse number is 2
Event number is 1358Block number is 18Good Pulse number is 2
Event number is 1359Block number is 61Good Pulse number is 2
Event number is 1363Block number is 196Good Pulse number is 2
Event number is 1364Block number is 196Good Pulse number is 2
Event number is 1376Block number is 196Good Pulse number is 2
Event number is 1394Block number is 196Good Pulse number is 2
Event number is 1398Block number is 196Good Pulse number is 2
Event number is 1399Block number is 196Good Pulse number is 2
Event number is 1400Block number is 196Good Pulse number is 2
Event number is 1404Block number is 196Good Pulse number is 2
Event number is 1408Block number is 196Good Pulse number is 2
Event number is 1414Block number is 196Good Pulse number is 2
Event number is 1422Block number is 196Good Pulse number is 2
Event number is 1428Block number is 93Good Pulse number is 2
Event number is 1440Block number is 196Good Pulse number is 2
Event number is 1472Block number is 196Good Pulse number is 2
Event number is 1478Block number is 85Good Pulse number is 2
Event number is 1480Block number is 196Good Pulse number is 2
Event number is 1482Block number is 196Good Pulse number is 2
Event number is 1486Block number is 57Good Pulse number is 2
Event number is 1493Block number is 196Good Pulse number is 2*/