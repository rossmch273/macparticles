{
    #include <sys/stat.h>

    using namespace ROOT;
    using namespace ROOT::RDF;

    //load file containing data on the pi+
    TFile *particleFile = new TFile("../particleData/particle-data/pip-data.root", "READ");

    //extract rec_tree from file this contains the information we need
    TTree *recTree = (TTree*)particleFile->Get("rec_tree");

    //link the tree of particle data to the models trained with different variables
    
    //recTree->AddFriend("training_calculated", "../training-models.root");
    recTree->AddFriend("training_detected", "../pip-training-models.root");
    recTree->AddFriend("training_truth", "../pip-training-models.root");

    //link the tree containing the corrections to the file
    recTree->AddFriend("correctionDNN", "DNNs/model7/pip/corrections.root");

    //create dataframe from rec_tree
    RDataFrame particleData(*(recTree));

    auto colNames = particleData.GetColumnNames();

    for (auto &&colName : colNames){
        std::cout << colName << std::endl;
    }

  

    auto correction = [](double a, float b){
        return a*b;
    };

    auto product = [](float a, double b){
        return (double)a*b;
    };

    auto correctionData = particleData.Define("detected_reweight", product, {"training_detected.kerasDNN_fast_weight","training_detected.fast_weight"}).Define("truth_reweight", product, {"training_truth.kerasDNN_fast_weight","training_truth.fast_weight"}).
                                        Define("corrected_weight", correction, {"detected_reweight","correctionDNN.truth-correction"});

    auto d1 = correctionData.Display({"corrected_weight", "detected_reweight", "truth_reweight"}, 10);
    d1->Print();

    
    auto detCorrectP = correctionData.Filter("corrected_weight>gRandom->Uniform()").Histo1D( TH1DModel{"corrected p", "true correction p", 100, 0,10}, {"detP"});
    auto truP = correctionData.Filter("training_truth.kerasDNN_fast_weight*training_truth.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"truth p", "true correction p", 100, 0,10}, {"truP"});
    auto detP = correctionData.Filter("training_detected.kerasDNN_fast_weight*training_detected.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"det p", "true correction p", 100, 0, 10}, {"detP"});

    
    auto detCorrectPhi = correctionData.Filter("corrected_weight>gRandom->Uniform()").Histo1D( TH1DModel{"corrected p", "true correction phi", 100, -4,4}, {"detPhi"});
    auto truPhi = correctionData.Filter("training_truth.kerasDNN_fast_weight*training_truth.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"truth p", "true correction phi", 100, -4,4}, {"truPhi"});
    auto detPhi = correctionData.Filter("training_detected.kerasDNN_fast_weight*training_detected.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"det p", "true correction phi", 100, -4,4}, {"detPhi"});

    auto detCorrectTheta = correctionData.Filter("corrected_weight>gRandom->Uniform()").Histo1D( TH1DModel{"corrected p", "true correction theta", 100, 0, 2}, {"detTheta"});
    auto truTheta = correctionData.Filter("training_truth.kerasDNN_fast_weight*training_truth.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"truth p", "true correction theta", 100, 0,2}, {"truTheta"});
    auto detTheta = correctionData.Filter("training_detected.kerasDNN_fast_weight*training_detected.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"det p", "true correction theta", 100, 0,2}, {"detTheta"});

    //P
    TH1D *correct_tru_P = new TH1D();
    correct_tru_P = (TH1D*)detCorrectP->Clone();
    correct_tru_P->Divide(truP.GetPtr());

    TH1D *det_tru_P = new TH1D();
    det_tru_P = (TH1D*)detP->Clone();
    det_tru_P->Divide(truP.GetPtr());

    //Theta
    TH1D *correct_tru_Theta = new TH1D();
    correct_tru_Theta = (TH1D*)detCorrectTheta->Clone();
    correct_tru_Theta->Divide(truTheta.GetPtr());

    TH1D *det_tru_Theta = new TH1D();
    det_tru_Theta = (TH1D*)detTheta->Clone();
    det_tru_Theta->Divide(truTheta.GetPtr());

    //Phi
    TH1D *correct_tru_Phi = new TH1D();
    correct_tru_Phi = (TH1D*)detCorrectPhi->Clone();
    correct_tru_Phi->Divide(truPhi.GetPtr());

    TH1D *det_tru_Phi = new TH1D();
    det_tru_Phi = (TH1D*)detPhi->Clone();
    det_tru_Phi->Divide(truPhi.GetPtr());




    



    TCanvas *c1 = new TCanvas();
    TLegend *leg1 = new TLegend();

    detP->SetLineWidth(3);
    truP->SetLineWidth(3);
    detCorrectP->SetLineWidth(3);
    detP->SetLineColor(3);
    truP->SetLineColor(4);
    detCorrectP->SetLineColor(2);

    truP->Draw();
    detCorrectP->Draw("sames");
    detP->Draw("sames");

    leg1->AddEntry(truP.GetPtr(), "Tru");
    leg1->AddEntry(detP.GetPtr(), "Det");
    leg1->AddEntry(detCorrectP.GetPtr(), "Det Corrected");

    leg1->Draw();

    c1->Print("plots/correction-plots/pip/momentum.pdf");

    TCanvas *c2 = new TCanvas();
    TLegend *leg2 = new TLegend();

    detPhi->SetLineWidth(3);
    truPhi->SetLineWidth(3);
    detCorrectPhi->SetLineWidth(3);
    detPhi->SetLineColor(3);
    truPhi->SetLineColor(4);
    detCorrectPhi->SetLineColor(2);

    truPhi->Draw();
    detCorrectPhi->Draw("sames");
    detPhi->Draw("sames");

    leg2->AddEntry(truPhi.GetPtr(), "Tru");
    leg2->AddEntry(detPhi.GetPtr(), "Det");
    leg2->AddEntry(detCorrectPhi.GetPtr(), "Det Corrected");

    leg2->Draw();

    c2->Print("plots/correction-plots/pip/phi.pdf");

    TCanvas *c3 = new TCanvas();
    TLegend *leg3 = new TLegend();

    detTheta->SetLineWidth(3);
    truTheta->SetLineWidth(3);
    detCorrectTheta->SetLineWidth(3);
    detTheta->SetLineColor(3);
    truTheta->SetLineColor(4);
    detCorrectTheta->SetLineColor(2);

    truTheta->Draw();
    detCorrectTheta->Draw("sames");
    detTheta->Draw("sames");

    leg3->AddEntry(truTheta.GetPtr(), "Tru");
    leg3->AddEntry(detTheta.GetPtr(), "Det");
    leg3->AddEntry(detCorrectTheta.GetPtr(), "Det Corrected");

    leg3->Draw();

    c3->Print("plots/correction-plots/pip/theta.pdf");

    TCanvas *c4 = new TCanvas();
    TLegend *leg4 = new TLegend();

    det_tru_P->SetLineColor(2);
    det_tru_P->SetLineWidth(3);
    det_tru_P->SetMaximum(2);
    det_tru_P->SetTitle("P ratio");
    correct_tru_P->SetTitle("P ratio");
    correct_tru_P->SetMaximum(2);
    correct_tru_P->SetLineWidth(3);
    correct_tru_P->Draw();
    det_tru_P->Draw("sames");

    leg4->AddEntry(det_tru_P, "detected/truth");
    leg4->AddEntry(correct_tru_P, "corrected/truth");

    leg4->Draw();

    c4->Print("plots/correction-plots/pip/pRatio.pdf");

    TCanvas *c5 = new TCanvas();
    TLegend *leg5 = new TLegend();

    det_tru_Theta->SetLineColor(2);
    det_tru_Theta->SetLineWidth(3);
    det_tru_Theta->SetMaximum(4);
    correct_tru_Theta->SetMaximum(4);
    correct_tru_Theta->SetLineWidth(3);
    det_tru_Theta->SetTitle("Theta ratio");
    correct_tru_Theta->SetTitle("Theta ratio");
    correct_tru_Theta->Draw();
    det_tru_Theta->Draw("sames");

    leg5->AddEntry(det_tru_Theta, "detected/truth");
    leg5->AddEntry(correct_tru_Theta, "corrected/truth");

    leg5->Draw();

    c5->Print("plots/correction-plots/pip/thetaRatio.pdf");

    TCanvas *c6 = new TCanvas();
    TLegend *leg6 = new TLegend();

    det_tru_Phi->SetLineColor(2);
    det_tru_Phi->SetLineWidth(3);
    det_tru_Phi->SetMaximum(2);
    correct_tru_Phi->SetMaximum(2);
    correct_tru_Phi->SetLineWidth(3);
    det_tru_Phi->SetTitle("Phi ratio");
    correct_tru_Phi->SetTitle("Phi ratio");
    correct_tru_Phi->Draw();
    det_tru_Phi->Draw("sames");

    leg6->AddEntry(det_tru_Phi, "detected/truth");
    leg6->AddEntry(correct_tru_Phi, "corrected/truth");

    leg6->Draw();

    c6->Print("plots/correction-plots/pip/phiRatio.pdf");



}

