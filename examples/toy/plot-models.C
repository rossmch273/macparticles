{

    /*This code is for plotting the momentum variables for each of the three types of training data
        NOTE: There may be an error in this code in how we filter for the acceptance (still need to clarify this)*/
        
    #include <sys/stat.h>

    using namespace ROOT;
    using namespace ROOT::RDF;

    //DEFINE LAMBDA FUNCTIONS
    auto fast_toy_histograms= [](ROOT::RDF::RResultPtr<TH1D> histo_fast, ROOT::RDF::RResultPtr<TH1D> histo_toy, const char histoFast_label[], const char histoToy_label[], 
                                    const char plot_title[], const char XaxisLabel[], const char filepath[])
    {

        TCanvas *c1 = new TCanvas();

        histo_fast->SetTitle(plot_title);
        histo_fast->GetXaxis()->SetTitle(XaxisLabel);
        histo_fast->GetXaxis()->SetTitleSize(0.05);
        histo_fast->GetXaxis()->SetLabelSize(0.05);
        histo_fast->GetYaxis()->SetLabelSize(0.05); 
        histo_fast->SetLineWidth(2);
        histo_fast->SetLineColor(2);
        histo_fast->Draw();

        histo_toy->SetLineWidth(2);
        histo_toy->SetLineColor(4);
        histo_toy->Draw("sames");

        gPad->Update();
        auto *st = (TPaveStats*)histo_fast->FindObject("stats");
        st->SetY1NDC(0.5);
        st->SetY2NDC(0.7);
        
        TLegend *leg1 = new TLegend();
        leg1->AddEntry(histo_fast.GetPtr(), histoFast_label, "l");
        leg1->AddEntry(histo_toy.GetPtr(), histoToy_label, "l");
        leg1->Draw();

        //print canvas to file
        c1->Print(filepath);
    };


    auto model_compare = [](ROOT::RDF::RResultPtr<TH1D> histo_calculated, ROOT::RDF::RResultPtr<TH1D> histo_detected, ROOT::RDF::RResultPtr<TH1D> histo_truth, 
                            ROOT::RDF::RResultPtr<TH1D> histo_toy, const char XaxisLabel[], const char filename[])
    {
        TCanvas *c1 = new TCanvas();

        histo_calculated->SetTitle("Comparison of Models");
        histo_calculated->GetXaxis()->SetTitle(XaxisLabel);
        histo_calculated->GetXaxis()->SetTitleSize(0.05);
        histo_calculated->GetXaxis()->SetLabelSize(0.05);
        histo_calculated->GetYaxis()->SetLabelSize(0.05); 
        histo_calculated->SetLineWidth(2);
        histo_calculated->SetLineColor(2);
        histo_calculated->Draw();
        
        histo_toy->SetLineWidth(2);
        histo_toy->Draw("sames");

        histo_detected->SetLineWidth(2);
        histo_detected->SetLineColor(3);
        histo_detected->Draw("sames");

        histo_truth->SetLineWidth(2);
        histo_truth->SetLineColor(6);
        histo_truth->Draw("sames");

        gPad->Update();
        auto *st_calculated = (TPaveStats*)histo_calculated->FindObject("stats");
        st_calculated->SetY1NDC(0.55);
        st_calculated->SetY2NDC(0.75);

        auto *st_detected = (TPaveStats*)histo_detected->FindObject("stats");
        st_detected->SetY1NDC(0.35);
        st_detected->SetY2NDC(0.55);

        auto *st_truth = (TPaveStats*)histo_truth->FindObject("stats");
        st_truth->SetY1NDC(0.15);
        st_truth->SetY2NDC(0.35);

        TLegend *leg1 = new TLegend();
        leg1->AddEntry(histo_calculated.GetPtr(), "fast-calculated", "l");
        leg1->AddEntry(histo_detected.GetPtr(), "fast-detected", "l");
        leg1->AddEntry(histo_truth.GetPtr(), "fast-truth", "l");
        leg1->AddEntry(histo_toy.GetPtr(), "toy", "l");

        leg1->Draw();

        c1->Print(filename);



    };

    //CODE

    //want to read data stored in training-types file into a dataframe
    auto calculated = RDataFrame("training_calculated", "training-types.root");
    auto detected = RDataFrame("training_detected", "training-types.root");
    auto truth = RDataFrame("training_truth", "training-types.root");

    //generate P histograms
    auto his_calculated_fastP = calculated.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"calculated_fastP"," ",100,0,10},{"calcP"});
    auto his_detected_fastP = detected.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"detected_fastP"," ",100,0,10},{"detP"});
    auto his_truth_fastP = truth.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"truth_fastP"," ",100,0,10},{"truP"});
    auto his_toy_P = calculated.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyP"," ",100,0,10},{"truP"});

    //generate Theta histograms
    auto his_calculated_fastTheta = calculated.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"calculated_fastTheta"," ",100,0,2},{"calcTheta"});
    auto his_detected_fastTheta = detected.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"detected_fastTheta"," ",100,0,2},{"detTheta"});
    auto his_truth_fastTheta = truth.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"truth_fastTheta"," ",100,0,2},{"truTheta"});
    auto his_toy_Theta = calculated.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyTheta"," ",100,0,2},{"truTheta"});

    //generate Phi histograms
    auto his_calculated_fastPhi = calculated.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"calculated_fastPhi"," ",100,-4,4},{"calcPhi"});
    auto his_detected_fastPhi = detected.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"detected_fastPhi"," ",100,-4,4},{"detPhi"});
    auto his_truth_fastPhi = truth.Filter("kerasDNN_fast_accept==1").Histo1D( TH1DModel{"truth_fastPhi"," ",100,-4,4},{"truPhi"});
    auto his_toy_Phi = calculated.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyPhi"," ",100,-4,4},{"truPhi"});

    
    mkdir("AcceptancePlots", 0777);

    //Generate plots for fast model trained with calculated variables against toy model
    mkdir("AcceptancePlots/calculated", 0777);

    fast_toy_histograms(his_calculated_fastP, his_toy_P, "fast-calculated", "toy", "Plot of fast sim acceptance with calculated variables against the toy model", "P",
                        "AcceptancePlots/calculated/momentum.png");
    fast_toy_histograms(his_calculated_fastTheta, his_toy_Theta, "fast-calculated", "toy", "Plot of fast sim acceptance with calculated variables against the toy model", "#theta",
                         "AcceptancePlots/calculated/theta.png");
    fast_toy_histograms(his_calculated_fastPhi, his_toy_Phi, "fast-calculated", "toy", "Plot of fast sim acceptance with calculated variables against the toy model", "#phi",
                        "AcceptancePlots/calculated/phi.png");

    

    //Generate plots for fast model trained with detected variables against toy model
    mkdir("AcceptancePlots/detected", 0777);

    fast_toy_histograms(his_detected_fastP, his_toy_P, "fast-detected", "toy", "Plot of fast sim acceptance with detected variables against the toy model", "P",
                        "AcceptancePlots/detected/momentum.png");
    fast_toy_histograms(his_detected_fastTheta, his_toy_Theta, "fast-detected", "toy", "Plot of fast sim acceptance with detected variables against the toy model", "#theta",
                        "AcceptancePlots/detected/theta.png");
    fast_toy_histograms(his_detected_fastPhi, his_toy_Phi, "fast-detected", "toy", "Plot of fast sim acceptance with detected variables against the toy model", "#phi",
                        "AcceptancePlots/detected/phi.png");

    //Generate plots for fast model trained with truth variables against toy model
    mkdir("AcceptancePlots/truth", 0777);

    fast_toy_histograms(his_truth_fastP, his_toy_P, "fast-truth", "toy", "Plot of fast sim acceptance with truth variables against the toy model", "P",
                        "AcceptancePlots/truth/momentum.png");
    fast_toy_histograms(his_truth_fastTheta, his_toy_Theta, "fast-truth", "toy", "Plot of fast sim acceptance with truth variables against the toy model", "#theta",
                        "AcceptancePlots/truth/theta.png");
    fast_toy_histograms(his_truth_fastPhi, his_toy_Phi, "fast-truth", "toy", "Plot of fast sim acceptance with truth variables against the toy model", "#phi",
                        "AcceptancePlots/truth/phi.png");

    //plot other permutations of models for comparisons
    
    //calc-det
    mkdir("AcceptancePlots/calc-det", 0777);
    fast_toy_histograms(his_calculated_fastP, his_detected_fastP, "fast-calculated", "fast-detected", "fast-calc vs fast-det", "P", "AcceptancePlots/calc-det/momentum.png");
    fast_toy_histograms(his_calculated_fastTheta, his_detected_fastTheta, "fast-calculated", "fast-detected", "fast-calc vs fast-det", "#theta", 
                        "AcceptancePlots/calc-det/theta.png");
    fast_toy_histograms(his_calculated_fastPhi, his_detected_fastPhi, "fast-calculated", "fast-detected", "fast-calc vs fast-det", "#phi", "AcceptancePlots/calc-det/phi.png");

    //calc-ru
    mkdir("AcceptancePlots/calc-tru", 0777);
    fast_toy_histograms(his_calculated_fastP, his_truth_fastP, "fast-calculated", "fast-truth", "fast-calc vs fast-truth", "P", "AcceptancePlots/calc-tru/momentum.png");
    fast_toy_histograms(his_calculated_fastTheta, his_truth_fastTheta, "fast-calculated", "fast-truth", "fast-calc vs fast-truth", "#theta", 
                        "AcceptancePlots/calc-tru/theta.png");
    fast_toy_histograms(his_calculated_fastPhi, his_truth_fastPhi, "fast-calculated", "fast-truth", "fast-calc vs fast-truth", "#phi", "AcceptancePlots/calc-tru/phi.png");

    //det-tru
    mkdir("AcceptancePlots/det-tru", 0777);
    fast_toy_histograms(his_detected_fastP, his_truth_fastP, "fast-detected", "fast-truth", "fast-det vs fast-truth", "P", "AcceptancePlots/det-tru/momentum.png");
    fast_toy_histograms(his_detected_fastTheta, his_truth_fastTheta, "fast-detected", "fast-truth", "fast-det vs fast-truth", "#theta", 
                        "AcceptancePlots/det-tru/theta.png");
    fast_toy_histograms(his_detected_fastPhi, his_truth_fastPhi, "fast-detected", "fast-truth", "fast-det vs fast-truth", "#phi", "AcceptancePlots/det-tru/phi.png");

    //plot comparison between all of the models
    model_compare(his_calculated_fastP, his_detected_fastP, his_truth_fastP, his_toy_P, "P", "AcceptancePlots/momentum-compare.png");
    model_compare(his_calculated_fastTheta, his_detected_fastTheta, his_truth_fastTheta, his_toy_Theta, "#theta", "AcceptancePlots/theta-compare.png");
    model_compare(his_calculated_fastPhi, his_detected_fastPhi, his_truth_fastPhi, his_toy_Phi, "P", "AcceptancePlots/phi-compare.png");





    
    
    
    


}