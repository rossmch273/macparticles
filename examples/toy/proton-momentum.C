{
    using namespace ROOT;
    using namespace ROOT::RDF;

    //get data from root file and input into a dataframe
    auto df = RDataFrame("tree", "toy_reaction.root");

    //filter the database for the cases where both the pions are detected
    auto data = df.Filter("accepted_pip==1 && accepted_pim==1");

    //Define a lambda function that will reconstruct the components of the 4-momentum of the pions using the p,theta, phi variables
    
    double m1 = 0.13957; //charged pion mass

    auto pion_4mom= [&m1](double pmag1, double theta1, double phi1)
    {
        auto px = pmag1*sin(theta1)*cos(phi1);
        auto py = pmag1*sin(theta1)*sin(phi1);
        auto pz = pmag1*cos(theta1);
        ROOT::Math::PxPyPzEVector p1(px, py, pz, sqrt(m1*m1 + pmag1*pmag1));

        return p1;
    };

    //DEFINE a lambda function that will take the two pion 4-momenta and use conservation laws to give a prediction on the proton's 4-momentum
    
    double m2 = 0.938; //proton mass

    auto calculate_4mom= [&m2](ROOT::Math::PxPyPzEVector p1 ,ROOT::Math::PxPyPzEVector p2)
    {
        ROOT::Math::PxPyPzEVector p_target(0,0,0,0.938);
        ROOT::Math::PxPyPzEVector p_beam(0,0,10,10);

        //using conservation of 4-momentum to get 4-momenta of the proton
        ROOT::Math::PxPyPzEVector p3(p_target.Px()+p_beam.Px()-p1.Px()-p2.Px(), p_target.Py()+p_beam.Py()-p1.Py()-p2.Py(), p_target.Pz()+p_beam.Pz()-p1.Pz()-p2.Pz(), p_target.E()+p_beam.E()-p1.E()-p2.E());

        return p3;
    };

    //define new columns in the dataframe containing the 4-momentum of the pions using the reconstructed variables
    auto pionsRec_data = data.Define("pipRec_4mom", pion_4mom, {"recon.pipP", "recon.pipTheta", "recon.pipPhi"}).Define("pimRec_4mom", pion_4mom, {"recon.pimP", "recon.pimTheta", "recon.pimPhi"});

    //define new columns in the dataframe containing the 4-momentum of the pions using their truth variables
    auto pionsTru_data = data.Define("pipTru_4mom", pion_4mom, {"truth.pipP", "truth.pipTheta", "truth.pipPhi"}).Define("pimTru_4mom", pion_4mom, {"truth.pimP","truth.pimTheta","truth.pimPhi"});

    //define lambda function to output the properties of the proton from it's 4-momentum
    auto P= [](ROOT::Math::PxPyPzEVector p) {return p.P();};
    auto theta= [](ROOT::Math::PxPyPzEVector p) {return p.Theta();};
    auto phi= [](ROOT::Math::PxPyPzEVector p) {return p.Phi();};
    auto mass= [](ROOT::Math::PxPyPzEVector p){return p.mass();};

    //define new dataframes which contain the 4-momentum of the proton using the 4-momenta of the pions
    auto proton_rec_data = pionsRec_data.Define("proton_4mom", calculate_4mom, {"pimRec_4mom","pipRec_4mom"}); 
    auto full_rec_data = proton_rec_data.Define("calcP",P,{"proton_4mom"}).Define("calcTheta", theta, {"proton_4mom"}).Define("calcPhi", phi, {"proton_4mom"}).Define("pMass", mass, {"proton_4mom"});
    auto proton_tru_data = pionsTru_data.Define("proton_4mom", calculate_4mom, {"pimTru_4mom","pipTru_4mom"});
    auto full_tru_data = proton_tru_data.Define("calcP", P, {"proton_4mom"}).Define("calcTheta", theta, {"proton_4mom"}).Define("calcPhi", phi, {"proton_4mom"}).Define("pMass", mass, {"proton_4mom"}); 


    //define lambda function to apply to column of dataframe so that we can rename the columns in output
    return_column = [](double value){return value;};

    auto reconstructed_data = full_rec_data.Define("detP", return_column, {"recon.pP"}).Define("detTheta", return_column, {"recon.pTheta"}).Define("detPhi", return_column, {"recon.pPhi"}).Define("truP", return_column, {"truth.pP"}).Define("truTheta", return_column, {"truth.pTheta"}).Define("truPhi", return_column, {"truth.pPhi"});

    //auto colNames = reconstructed_data.GetColumnNames();

    //for (auto &&colName : colNames) std::cout << colName << std::endl; 
    
    //OUTPUT PROTON VARIABLES TO TREE IN ROOT FILE
    //Output the proton variables that were calculated using the reconstructed pion variables
    auto snapshotOptions = RSnapshotOptions();
    snapshotOptions.fMode = "RECREATE";
    reconstructed_data.Snapshot("rec_tree","proton-data.root", {"truP", "truTheta", "truPhi","detP", "detTheta", "detPhi","calcP", "calcTheta", "calcPhi", "accepted_p"},snapshotOptions);

    //Output the proton variables that were calculated using the truth pion variables to a different tree
    snapshotOptions.fMode = "UPDATE";
    full_tru_data.Snapshot("tru_tree","proton-data.root", {"calcP", "calcTheta", "calcPhi", "accepted_p"},snapshotOptions);

    
    /*
    //GENERATING HISTOGRAMS

    //Histograms of the calculated proton variables
    auto Hispred_rec_P = full_rec_data.Histo1D( TH1DModel{"reconstructed_P", "", 100, 0, 10},{"truP"});
    auto Hispred_tru_P = full_tru_data.Histo1D( TH1DModel{"truth_P", "", 100, 0, 10},{"truP"});
    auto Hispred_rec_theta = full_rec_data.Histo1D( TH1DModel{"reconstructed_Theta", "", 100, 0, 2},{"truTheta"});
    auto Hispred_tru_theta = full_tru_data.Histo1D( TH1DModel{"truth_Theta", "", 100, 0, 2},{"truTheta"});
    auto Hispred_rec_phi = full_rec_data.Histo1D( TH1DModel{"reconstructed_Phi", "", 100, -4, 4},{"truPhi"});
    auto Hispred_tru_phi = full_tru_data.Histo1D( TH1DModel{"truth_Phi", "", 100, -4, 4},{"truPhi"});
    auto Hispred_rec_mass = full_rec_data.Histo1D( TH1DModel{"reconstructed_Mass", "", 100, 0, 2},{"pMass"});
    auto Hispred_tru_mass = full_tru_data.Histo1D( TH1DModel{"truth_Mass", "", 100, 0, 2},{"pMass"});

    //Histograms of the proton variables from the toy reaction file
    auto His_tru_P = data.Histo1D( TH1DModel{"tru_react_pP", "proton P", 100, 0, 10},{"truth.pP"});
    auto His_rec_P = data.Histo1D( TH1DModel{"rec_react_pP", "proton P", 100, 0, 10},{"recon.pP"});
    auto His_tru_theta = data.Histo1D( TH1DModel{"tru_react_pTheta", "proton #theta", 100, 0, 2},{"truth.pTheta"});
    auto His_rec_theta = data.Histo1D( TH1DModel{"rec_react_pTheta", "proton #theta", 100, 0, 2},{"recon.pTheta"});
    auto His_tru_phi = data.Histo1D( TH1DModel{"tru_react_pPhi", "proton #phi", 100, -6, 6},{"truth.pPhi"});
    auto His_rec_phi = data.Histo1D( TH1DModel{"rec_react_pPhi", "proton #phi", 100, -6, 6},{"recon.pPhi"});

    */

    //DEFINE FUNCTION FOR TAKING TWO HISTOGRAMS AND PLOTTING BOTH ON THE SAME CANVAS
    auto plot_comparison_histograms= [](ROOT::RDF::RResultPtr<TH1D> histo_1, ROOT::RDF::RResultPtr<TH1D> histo_2, char histo_1_label[], char histo_2_label[], char canvas_title[], char XaxisLabel[], char YaxisLabel[], float legend_position[4], char output_filename[])
    {
        //create new canvas and give it title
        TCanvas *c1 = new TCanvas();
        c1->SetTitle(canvas_title);

        //set histogram axis labels
        histo_1->GetXaxis()->SetTitle(XaxisLabel);
        histo_1->GetYaxis()->SetTitle(YaxisLabel);
        histo_1->GetXaxis()->SetTitleSize(0.05);
        histo_1->GetXaxis()->SetLabelSize(0.05);
        histo_1->GetYaxis()->SetLabelSize(0.05);
        
        histo_2->GetXaxis()->SetTitle(XaxisLabel);
        histo_2->GetYaxis()->SetTitle(YaxisLabel);
        histo_2->GetXaxis()->SetTitleSize(0.05);
        histo_2->GetXaxis()->SetLabelSize(0.05);
        histo_2->GetYaxis()->SetLabelSize(0.05);
        
        //Draw both histograms on the same canvas
        histo_1->Draw();
        histo_2->SetLineColor(2);
        histo_2->Draw("sames");

        //move stat pad of the second histogram so that they don't overlap 
        //gPad->Update();
        //auto *st = (TPaveStats*)histo_2->FindObject("stats");
        //st->SetY1NDC(0.5);
        //st->SetY2NDC(0.7);
        //gStyle->SetOptStat(0);

        //create legend for the plot 
        TLegend *leg = new TLegend(legend_position[0], legend_position[1], legend_position[2], legend_position[3]);

        leg->AddEntry(histo_1.GetPtr(), histo_1_label, "l");
        leg->AddEntry(histo_2.GetPtr(), histo_2_label, "l");
        leg->Draw();

        c1->Print(output_filename);

    };

    auto Hispred_rec_mass = full_rec_data.Histo1D( TH1DModel{"reconstructed_Mass", "proton mass", 100, 0, 2},{"pMass"});
    auto Hispred_tru_mass = full_tru_data.Histo1D( TH1DModel{"truth_Mass", "proton mass", 100, 0, 2},{"pMass"});

    //Plot histograms
    TCanvas *c1 = new TCanvas();
    c1->SetTitle("comparison of masses");

    Hispred_rec_mass->GetXaxis()->SetTitle("Mass [GeV]");
    Hispred_rec_mass->GetXaxis()->SetTitleSize(0.05);
    Hispred_rec_mass->GetXaxis()->SetLabelSize(0.05);
    Hispred_rec_mass->SetLineWidth(2);

    Hispred_tru_mass->GetXaxis()->SetTitle("Mass [GeV]");
    Hispred_tru_mass->GetXaxis()->SetTitleSize(0.05);
    Hispred_tru_mass->GetXaxis()->SetLabelSize(0.05);
    Hispred_tru_mass->SetLineWidth(2);

    Hispred_rec_mass->Draw();
    Hispred_tru_mass->SetLineColor(2);
    Hispred_tru_mass->Draw("same");

     TLegend *leg = new TLegend(0.75,0.45,0.95,0.65);

    leg->AddEntry(Hispred_rec_mass.GetPtr(), "reconstructed", "l");
    leg->AddEntry(Hispred_tru_mass.GetPtr(), "truth", "l");
    leg->Draw();

    c1->Print("mass_plots/proton-mass.png");











}
