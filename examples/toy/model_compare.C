{
    #include <sys/stat.h>

    using namespace ROOT;
    using namespace ROOT::RDF;

    //load file containing data on the pi+
    TFile *particleFile = new TFile("particleData/particle-data/pip-data.root", "READ");

    //extract rec_tree from file this contains the information we need
    TTree *recTree = (TTree*)particleFile->Get("rec_tree");

    //link the tree of particle data to the models trained with different variables
    
    recTree->AddFriend("training_calculated", "training-models.root");
    recTree->AddFriend("training_detected", "training-models.root");
    recTree->AddFriend("training_truth", "training-models.root");
    
    
    //create dataframe from rec_tree
    RDataFrame particleData(*(recTree));

    auto colNames = particleData.GetColumnNames();

    for (auto &&colName : colNames){
        std::cout << colName << std::endl;
    }


    //define lambda function to calculate ratios between columns in dataframe
    auto columnRatio = [](float column1Value, float column2Value)
    {
        if(column2Value==0){
            return (float)0.0;
        }
        else{
            return (float)column1Value/column2Value;
        }
    };

    auto reweightRatio = [](float dnnValue1, double bdtValue1, float dnnValue2, double bdtValue2)
    {
        if(dnnValue2==0 || bdtValue2==0 ){
            return (float)0.0;
        }
        else{
            float ratio = (dnnValue1*bdtValue1)/(dnnValue2*bdtValue2);
            return ratio ;
        }
    };
    
    

    //generate columns containing the ratios of the plots
    
    //generate columns containing the ratios of the plots
    auto particleDataRatios = particleData.Define("calcDetRatioDNN", columnRatio,{"training_calculated.kerasDNN_fast_weight","training_detected.kerasDNN_fast_weight"}).
                                Define("calcTruRatioDNN", columnRatio, {"training_calculated.kerasDNN_fast_weight", "training_truth.kerasDNN_fast_weight"}).
                                Define("truDetRatioDNN", columnRatio, {"training_truth.kerasDNN_fast_weight", "training_detected.kerasDNN_fast_weight"}).
                                Define("calcDetRatioReweight", reweightRatio, {"training_calculated.kerasDNN_fast_weight", "training_calculated.fast_weight", "training_detected.kerasDNN_fast_weight","training_detected.fast_weight"}).
                                Define("calcTruRatioReweight", reweightRatio, {"training_calculated.kerasDNN_fast_weight", "training_calculated.fast_weight", "training_truth.kerasDNN_fast_weight","training_truth.fast_weight"}).
                                Define("truDetRatioReweight", reweightRatio, {"training_truth.kerasDNN_fast_weight","training_truth.fast_weight", "training_detected.kerasDNN_fast_weight", "training_detected.fast_weight"});

    //GENERATE HISTOGRAMS OF FAST WEIGHT RATIOS
    auto histCalcDetRatioDNN = particleDataRatios.Histo1D( TH1DModel{"calculated/detected", "calculated/detected DNN",100,-2,2},{"calcDetRatioDNN"});

    auto histCalcTruRatioDNN = particleDataRatios.Histo1D( TH1DModel{"calculated/truth", "calculated/truth DNN",100,-2,2},{"calcTruRatioDNN"});

    auto histTruDetRatioDNN = particleDataRatios.Histo1D( TH1DModel{"truth/detected", "truth/detected DNN",100,-2,2},{"truDetRatioDNN"});

    auto histCalcDetRatioReweight = particleDataRatios.Histo1D( TH1DModel{"calculated/detected", "calculated/detected Reweighted",100,-2,2},{"calcDetRatioReweight"});

    auto histCalcTruRatioReweight = particleDataRatios.Histo1D( TH1DModel{"calculated/truth", "calculated/truth Reweighted",100,-2,2},{"calcTruRatioReweight"});

    auto histTruDetRatioReweight = particleDataRatios.Histo1D( TH1DModel{"truth/detected", "truth/detected Reweighted",100,-2,2},{"truDetRatioReweight"});

    //GENERATE 2D HISTOGRAMS OF FAST WEIGHT RATIOS AGAINST P, THETA, PHI

    // Calc/Det
    auto histoCalcDetDNN_P = particleDataRatios.Histo2D( TH2DModel{"calcDetDNN_P", "DNN: calc/det vs p", 100, 0, 10, 100, 0, 2}, "truP", "calcDetRatioDNN");
    auto histoCalcDetDNN_Theta = particleDataRatios.Histo2D( TH2DModel{"calcDetDNN_P", "DNN: calc/det vs #theta", 100, 0, 2, 100, 0, 2}, "truTheta", "calcDetRatioDNN");
    auto histoCalcDetDNN_Phi = particleDataRatios.Histo2D( TH2DModel{"calcDetDNN_P", "DNN: calc/det vs #phi", 100, -4, 4, 100, 0, 2}, "truPhi", "calcDetRatioDNN");

    // Calc/Tru
    auto histoCalcTruDNN_P = particleDataRatios.Histo2D( TH2DModel{"calcTruDNN_P", "DNN: calc/tru vs p", 100, 0, 10, 100, 0, 2}, "truP", "calcTruRatioDNN");
    auto histoCalcTruDNN_Theta = particleDataRatios.Histo2D( TH2DModel{"calcTruDNN_P", "calc/tru vs #theta", 100, 0, 2, 100, 0, 2}, "truTheta", "calcTruRatioDNN");
    auto histoCalcTruDNN_Phi = particleDataRatios.Histo2D( TH2DModel{"calcTruDNN_P", "calc/tru vs #phi", 100, -4, 4, 100, 0, 2}, "truPhi", "calcTruRatioDNN");

    // Tru/Det
    auto histoTruDetDNN_P = particleDataRatios.Histo2D( TH2DModel{"truDetDNN_P", "tru/det vs p", 100, 0, 10, 100, 0, 2}, "truP", "truDetRatioDNN");
    auto histoTruDetDNN_Theta = particleDataRatios.Histo2D( TH2DModel{"truDetDNN_P", "tru/det vs #theta", 100, 0, 2, 100, 0, 2}, "truTheta", "truDetRatioDNN");
    auto histoTruDetDNN_Phi = particleDataRatios.Histo2D( TH2DModel{"truDetDNN_P", "tru/det vs #phi", 100, -4, 4, 100, 0, 2}, "truPhi", "truDetRatioDNN");
    

  



    //OUPUT 1D HISTOGRAMS OF FAST WEIGHTS

    // Comparison
    TCanvas *c1 = new TCanvas();
    TLegend *leg = new TLegend();

    histCalcDetRatioDNN->SetLineWidth(3);
    histCalcDetRatioDNN->SetLineColor(4);
    histCalcDetRatioDNN->Draw();

    histCalcTruRatioDNN->SetLineWidth(3);
    histCalcTruRatioDNN->SetLineColor(2);
    histCalcTruRatioDNN->Draw("same");

    histTruDetRatioDNN->SetLineWidth(3);
    histTruDetRatioDNN->SetLineColor(1);
    histTruDetRatioDNN->Draw("same");

    leg->AddEntry(histCalcDetRatioDNN.GetPtr(), "calculated/detected", "l");
    leg->AddEntry(histCalcTruRatioDNN.GetPtr(), "calculated/truth", "l");
    leg->AddEntry(histTruDetRatioDNN.GetPtr(), "truth/detected", "l");

    leg->Draw();

    c1->Print("model-comparison/plots/fast-weight-ratios/DNN/combined.png");

    // Calc/Det DNN
    TCanvas *c2 = new TCanvas();
    histCalcDetRatioDNN->SetLineWidth(3);
    histCalcDetRatioDNN->SetLineColor(4);
    histCalcDetRatioDNN->Draw();

    c2->Print("model-comparison/plots/fast-weight-ratios/DNN/calculated-detected.png");

    // Calc/Tru DNN
    TCanvas *c3 = new TCanvas();
    histCalcTruRatioDNN->SetLineWidth(3);
    histCalcTruRatioDNN->SetLineColor(2);
    histCalcTruRatioDNN->Draw();

    c3->Print("model-comparison/plots/fast-weight-ratios/DNN/calculated-truth.png");

    // Tru/Det DNN
    TCanvas *c4 = new TCanvas();
    histTruDetRatioDNN->SetLineWidth(3);
    histTruDetRatioDNN->SetLineColor(1);
    histTruDetRatioDNN->Draw();

    c4->Print("model-comparison/plots/fast-weight-ratios/DNN/truth-detected.png");

    // Calc/Det Reweight
    TCanvas *c5 = new TCanvas();

    histCalcDetRatioReweight->SetLineWidth(3);
    histCalcDetRatioReweight->SetLineColor(4);
    histCalcDetRatioReweight->Draw();

    c5->Print("model-comparison/plots/fast-weight-ratios/reweight/calculated-detected.png");

    // Calc/Tru Reweight
    TCanvas *c6 = new TCanvas();

    histCalcTruRatioReweight->SetLineWidth(3);
    histCalcTruRatioReweight->SetLineColor(2);
    histCalcTruRatioReweight->Draw();

    c6->Print("model-comparison/plots/fast-weight-ratios/reweight/calculated-truth.png");

    TCanvas *c7 = new TCanvas();

    histTruDetRatioReweight->SetLineWidth(3);
    histTruDetRatioReweight->SetLineColor(1);
    histTruDetRatioReweight->Draw();

    c7->Print("model-comparison/plots/fast-weight-ratios/reweight/truth-detected.png");

    TCanvas *c8 = new TCanvas();
    histCalcDetRatioReweight->SetLineWidth(3);
    histCalcDetRatioReweight->SetLineColor(4);
    histCalcDetRatioReweight->Draw();

    histCalcTruRatioReweight->SetLineWidth(3);
    histCalcTruRatioReweight->SetLineColor(2);
    histCalcTruRatioReweight->Draw("same");

    histTruDetRatioReweight->SetLineWidth(3);
    histTruDetRatioReweight->SetLineColor(1);
    histTruDetRatioReweight->Draw("same");

    TLegend *leg1 = new TLegend();

    leg1->AddEntry(histCalcDetRatioDNN.GetPtr(), "calculated/detected", "l");
    leg1->AddEntry(histCalcTruRatioDNN.GetPtr(), "calculated/truth", "l");
    leg1->AddEntry(histTruDetRatioDNN.GetPtr(), "truth/detected", "l");

    leg1->Draw();

    c8->Print("model-comparison/plots/fast-weight-ratios/reweight/comparison.png");

    TCanvas *c9 = new TCanvas();
    histCalcDetRatioReweight->SetLineWidth(3);
    histCalcDetRatioReweight->SetLineColor(4);
    histCalcDetRatioReweight->Draw();

    histCalcDetRatioDNN->SetLineWidth(3);
    histCalcDetRatioDNN->SetLineColor(2);
    histCalcDetRatioDNN->Draw("same");

    TLegend *leg9 = new TLegend();

    leg9->AddEntry(histCalcDetRatioDNN.GetPtr(), "DNN", "l");
    leg9->AddEntry(histCalcDetRatioReweight.GetPtr(), "Reweight", "l");

    leg9->Draw();

    c9->Print("model-comparison/plots/fast-weight-ratios/reweight/calculated-detected-comparison.png");

        
    TCanvas *c10= new TCanvas();
    histCalcTruRatioReweight->SetLineWidth(3);
    histCalcTruRatioReweight->SetLineColor(4);
    histCalcTruRatioReweight->Draw();

    histCalcTruRatioDNN->SetLineWidth(3);
    histCalcTruRatioDNN->SetLineColor(2);
    histCalcTruRatioDNN->Draw("same");

    TLegend *leg10 = new TLegend();

    leg10->AddEntry(histCalcTruRatioDNN.GetPtr(), "DNN", "l");
    leg10->AddEntry(histCalcTruRatioReweight.GetPtr(), "Reweight", "l");
    leg10->Draw();

    c10->Print("model-comparison/plots/fast-weight-ratios/reweight/calculated-truth-comparison.png");

    TCanvas *c11 = new TCanvas();
    histTruDetRatioReweight->SetLineWidth(3);
    histTruDetRatioReweight->SetLineColor(4);
    histTruDetRatioReweight->Draw();

    histTruDetRatioDNN->SetLineWidth(3);
    histTruDetRatioDNN->SetLineColor(2);
    histTruDetRatioDNN->Draw("same");

    TLegend *leg11 = new TLegend();

    leg11->AddEntry(histTruDetRatioDNN.GetPtr(), "DNN", "l");
    leg11->AddEntry(histTruDetRatioReweight.GetPtr(), "Reweight", "l");

    leg11->Draw();

    c11->Print("model-comparison/plots/fast-weight-ratios/reweight/truth-detected-comparison.png");



    //OUPUT 2D HISTOGRAMS

    TCanvas *c12 = new TCanvas();
    gStyle->SetPalette(kRainBow);
    histoCalcDetDNN_P->Draw();

    c12->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-detected-P.png");

    TCanvas *c13 = new TCanvas();
    histoCalcDetDNN_Theta->Draw();
    c13->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-detected-theta.png");

    TCanvas *c14 = new TCanvas ();
    histoCalcDetDNN_Phi->Draw();
    c14->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-detected-phi.png");

    TCanvas *c15 = new TCanvas();
    gStyle->SetPalette(kRainBow);
    histoCalcTruDNN_P->Draw();

    c15->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-truth-P.png");

    TCanvas *c16 = new TCanvas();
    histoCalcTruDNN_Theta->Draw();
    c16->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-truth-theta.png");

    TCanvas *c17 = new TCanvas ();
    histoCalcTruDNN_Phi->Draw();
    c17->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-truth-phi.png");
    
    TCanvas *c18 = new TCanvas();
    gStyle->SetPalette(kRainBow);
    histoTruDetDNN_P->Draw();

    c18->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/truth-detected-P.png");

    TCanvas *c19 = new TCanvas();
    histoTruDetDNN_Theta->Draw();
    c19->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/truth-detected-theta.png");

    TCanvas *c20 = new TCanvas ();
    histoTruDetDNN_Phi->Draw();
    c20->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/truth-detected-phi.png");
    
    
    
    
    
    
    /*
    c12->Divide(3,1);

    c12->cd(1);
    histoCalcDetDNN_P->Draw("colz");

    c12->cd(2);
    histoCalcDetDNN_Theta->Draw("colz");
    
    c12->cd(3);
    histoCalcDetDNN_Phi->Draw("colz");

    c12->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-detected.png");

    TCanvas *c13 = new TCanvas();
    gStyle->SetPalette(kRainBow);
    c13->Divide(3,1);

    c13->cd(1);
    histoCalcTruDNN_P->Draw("colz");

    c13->cd(2);
    histoCalcTruDNN_Theta->Draw("colz");
    
    c13->cd(3);
    histoCalcTruDNN_Phi->Draw("colz");

    c13->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/calculated-truth.png");

    TCanvas *c14 = new TCanvas();
    gStyle->SetPalette(kRainBow);
    c14->Divide(3,1);

    c14->cd(1);
    histoTruDetDNN_P->Draw("colz");

    c14->cd(2);
    histoTruDetDNN_Theta->Draw("colz");
    
    c14->cd(3);
    histoTruDetDNN_Phi->Draw("colz");

    c14->Print("model-comparison/plots/fast-weight-ratios/2D/DNN/truth-detected.png");
    */













    


  




}