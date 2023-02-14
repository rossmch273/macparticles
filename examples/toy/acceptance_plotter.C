{
    /*This script reproduces the acceptance plots that are autoamtically produced when we perform the acceptance training and then the 
    reweight training.*/
    
    using namespace ROOT;
    using namespace ROOT::RDF;

    TFile *particleFile = new TFile("pip-data.root", "READ");

    TTree *recTree = (TTree*)particleFile->Get("rec_tree");

    recTree->AddFriend("kerasDNN", "fast_sim/acceptance/pi+/training_acceptances.root");
    recTree->AddFriend("BDT", "fast_sim/reweight/pi+/training_acceptances.root");

    RDataFrame particleData(*(recTree));

    auto colNames = particleData.GetColumnNames();

    for (auto &&colName : colNames) cout << colName << endl;
    
    //Define histograms
    auto HistToyP = particleData.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyP", "", 100, 0, 10},{"truP"});
    auto HistDNNP = particleData.Filter("kerasDNN.fast_accept==1").Histo1D( TH1DModel{"DNNP", "", 100, 0, 10},{"calcP"});
    auto HistBDTP = particleData.Filter("BDT.fast_weight*kerasDNN.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"BDTP", "", 100, 0, 10},{"calcP"});

    auto HistToyTheta = particleData.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyTheta", "", 100, 0, 2},{"truTheta"});
    auto HistDNNTheta = particleData.Filter("kerasDNN.fast_accept==1").Histo1D( TH1DModel{"toyTheta", "", 100, 0, 2},{"calcTheta"});
    auto HistBDTTheta = particleData.Filter("BDT.fast_weight*kerasDNN.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"toyTheta", "", 100, 0, 2},{"calcTheta"});

    auto HistToyPhi = particleData.Filter("accepted_pip==1").Histo1D( TH1DModel{"toyPhi", "", 100, -4, 4},{"truPhi"});
    auto HistDNNPhi = particleData.Filter("kerasDNN.fast_accept==1").Histo1D( TH1DModel{"toyPhi", "", 100, -4, 4},{"calcPhi"});
    auto HistBDTPhi = particleData.Filter("BDT.fast_weight*kerasDNN.fast_weight>gRandom->Uniform()").Histo1D( TH1DModel{"toyP", "", 100, -4, 4},{"calcPhi"});


    TCanvas *c1 = new TCanvas();


    c1->Divide(3,1);

    c1->cd(1);
    HistToyP->SetLineColor(4);
    HistToyP->SetLineWidth(2);
    HistToyP->Draw();

    HistBDTP->SetLineColor(3);
    HistBDTP->SetLineWidth(2);
    HistBDTP->Draw("sames");

    HistDNNP->SetLineWidth(2);
    HistDNNP->SetLineColor(2);
    HistDNNP->Draw("sames");

    TLegend *leg1 = new TLegend();
    leg1->AddEntry(HistToyP.GetPtr(), "toy", "l");
    leg1->AddEntry(HistDNNP.GetPtr(), "DNN", "l");
    leg1->AddEntry(HistBDTP.GetPtr(), "BDT", "l");
    leg1->Draw();

    c1->cd(2);
    HistToyTheta->SetLineColor(4);
    HistToyTheta->SetLineWidth(2);
    HistToyTheta->Draw();

    HistBDTTheta->SetLineColor(3);
    HistBDTTheta->SetLineWidth(2);
    HistBDTTheta->Draw("sames");

    HistDNNTheta->SetLineWidth(2);
    HistDNNTheta->SetLineColor(2);
    HistDNNTheta->Draw("sames");

    TLegend *leg2 = new TLegend();
    leg2->AddEntry(HistToyTheta.GetPtr(), "toy", "l");
    leg2->AddEntry(HistDNNTheta.GetPtr(), "DNN", "l");
    leg2->AddEntry(HistBDTTheta.GetPtr(), "BDT", "l");
    leg2->Draw();

    c1->cd(3);
    HistToyPhi->SetLineColor(4);
    HistToyPhi->SetLineWidth(2);
    HistToyPhi->Draw();

    HistBDTPhi->SetLineColor(3);
    HistBDTPhi->SetLineWidth(2);
    HistBDTPhi->Draw("sames");

    HistDNNPhi->SetLineWidth(2);
    HistDNNPhi->SetLineColor(2);
    HistDNNPhi->Draw("sames");

    TLegend *leg3 = new TLegend();
    leg3->AddEntry(HistToyPhi.GetPtr(), "toy", "l");
    leg3->AddEntry(HistDNNPhi.GetPtr(), "DNN", "l");
    leg3->AddEntry(HistBDTPhi.GetPtr(), "BDT", "l");
    leg3->Draw();






}