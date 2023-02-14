{
    using namespace ROOT;
    using namespace ROOT::RDF;

    //get data from root file and input into a dataframe
    auto df = RDataFrame("tree", "/home/ross-mcharg/cloned/directory/ross_macparticles/examples/toy/toy_reaction.root");

    //filter the database for the cases where both the pions are detected
    auto data = df.Filter("accepted_p==1 && accepted_pim==1");

    //Define a lambda function that will reconstruct the components of the 4-momentum of the pions using the p,theta, phi variables
    
    double m1 = 0.139; //charged pion mass

    auto pion_4mom= [&m1](double pmag1, double theta1, double phi1)
    {
        auto px = pmag1*sin(theta1)*cos(phi1);
        auto py = pmag1*sin(theta1)*sin(phi1);
        auto pz = pmag1*cos(theta1);
        ROOT::Math::PxPyPzEVector p1(px, py, pz, sqrt(m1*m1 + pmag1*pmag1));

        return p1;
    };

    //lambda function tat will reconstruct the components of the 4-momenta of the proton using the p,theta,phi variables
    double m2 = 0.938; //proton mass

    auto proton_4mom= [&m2](double pmag2, double theta2, double phi2)
    {
        auto px = pmag2*sin(theta2)*cos(phi2);
        auto py = pmag2*sin(theta2)*sin(phi2);
        auto pz = pmag2*cos(theta2);
        ROOT::Math::PxPyPzEVector p2(px, py, pz, sqrt(m2*m2 + pmag2*pmag2));

        return p2;
    };

    //define new columns in the dataframe containing the 4-momentum of the proton and the pim calculated with reconstructed variables
    auto Rec_data = data.Define("pRec_4mom", proton_4mom, {"recon.pP", "recon.pTheta", "recon.pPhi"}).Define("pimRec_4mom", pion_4mom, {"recon.pimP", "recon.pimTheta", "recon.pimPhi"});

    //define new columns in the dataframe containing the 4-momentum of the proton ad the pim calculated with truth variables
    auto Tru_data = data.Define("pTru_4mom", proton_4mom, {"truth.pP", "truth.pTheta", "truth.pPhi"}).Define("pimTru_4mom", pion_4mom, {"truth.pimP","truth.pimTheta","truth.pimPhi"});

    //DEFINE a lambda function that will take the pion 4-momenta/proton 4-momenta and use conservation laws to give a prediction on the pips 4-momentum
    
    auto calculate_4mom= [&m1](ROOT::Math::PxPyPzEVector p1 ,ROOT::Math::PxPyPzEVector p2)
    {
        ROOT::Math::PxPyPzEVector p_target(0,0,0,0.938);
        ROOT::Math::PxPyPzEVector p_beam(0,0,10,10);

        //using conservation of 4-momentum to get 4-momenta of the proton
        ROOT::Math::PxPyPzEVector p3(p_target.Px()+p_beam.Px()-p1.Px()-p2.Px(), p_target.Py()+p_beam.Py()-p1.Py()-p2.Py(), p_target.Pz()+p_beam.Pz()-p1.Pz()-p2.Pz(), p_target.E()+p_beam.E()-p1.E()-p2.E());

        return p3;
    };

    //define lambda function to output the properties of the pip from it's 4-momentum
    auto P= [](ROOT::Math::PxPyPzEVector p) {return p.P();};
    auto theta= [](ROOT::Math::PxPyPzEVector p) {return p.Theta();};
    auto phi= [](ROOT::Math::PxPyPzEVector p) {return p.Phi();};
    auto mass= [](ROOT::Math::PxPyPzEVector p){return p.mass();};
    auto mass_squared = [](ROOT::Math::PxPyPzEVector p){return p.M2();};

    //DEFINE new dataframes with the functions above applied
    //auto pip_rec_data = Rec_data.Define("pip_4mom", calculate_4mom, {"pimRec_4mom","pRec_4mom"}); 

    auto full_rec_data = Rec_data.Define("pip_4mom", calculate_4mom, {"pimRec_4mom","pRec_4mom"}).Define("calcP",P,{"pip_4mom"}).Define("calcTheta", theta, {"pip_4mom"}).
                            Define("calcPhi", phi, {"pip_4mom"}).Define("pipMass", mass, {"pip_4mom"}).Define("pipM2", mass_squared, {"pip_4mom"});

    //auto pip_tru_data = Tru_data.Define("pip_4mom", calculate_4mom, {"pimTru_4mom","pTru_4mom"});

    auto full_tru_data = Tru_data.Define("pip_4mom", calculate_4mom, {"pimTru_4mom","pTru_4mom"}).Define("calcP", P, {"pip_4mom"}).Define("calcTheta", theta, {"pip_4mom"}).
                            Define("calcPhi", phi, {"pip_4mom"}).Define("pipMass", mass, {"pip_4mom"}).Define("pipM2", mass_squared, {"pip_4mom"}); 

    
    //OUTPUT PIP VARIABLES TO TREE IN ROOT FILE

    //Rename columns in dataframe before output to root file
    return_column = [](double value){return value;};

    auto reconstructed_data = full_rec_data.Define("detP", return_column, {"recon.pipP"}).Define("detTheta", return_column, {"recon.pipTheta"}).Define("detPhi", return_column, {"recon.pipPhi"}).Define("truP", return_column, {"truth.pipP"}).Define("truTheta", return_column, {"truth.pipTheta"}).Define("truPhi", return_column, {"truth.pipPhi"});

    //Output the pip variables that were calculated using the reconstructed pion variables
    auto snapshotOptions = RSnapshotOptions();
    snapshotOptions.fMode = "RECREATE";
    reconstructed_data.Snapshot("rec_tree","particle-data/pip-data.root", {"truP", "truTheta", "truPhi","detP", "detTheta", "detPhi","calcP", "calcTheta", "calcPhi", "accepted_pip"},snapshotOptions);

    //Output the proton variables that were calculated using the truth pion variables to a different tree
    snapshotOptions.fMode = "UPDATE";
    full_tru_data.Snapshot("tru_tree","particle-data/pip-data.root", {"calcP", "calcTheta", "calcPhi", "accepted_pip"},snapshotOptions);

    //PLOTTING 

    //create and plot histograms of the masses
    auto Hispred_rec_mass = full_rec_data.Histo1D( TH1DModel{"recMass", "Calculated #pi+ Mass", 100, 0, 2},{"pipMass"});
    auto Hispred_tru_mass = full_tru_data.Histo1D( TH1DModel{"truMass", "Calculated #pi+ Mass", 100, 0, 2},{"pipMass"});

    auto Hispred_rec_mass2 = full_rec_data.Histo1D( TH1DModel{"recMass2", "Calculated #pi+ M2", 100, -0.1, 0.1},{"pipM2"});
    auto Hispred_tru_mass2 = full_tru_data.Histo1D( TH1DModel{"truMass2", "Calculated #pi+ M2", 100, -0.1, 0.1},{"pipM2"});

    //Plot of pip mass 
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

    c1->Print("mass_plots/pip-mass.png");

    //Plot of pip M2
    TCanvas *c2 = new TCanvas();
    c2->SetTitle("Comparison of M2");

    Hispred_rec_mass2->GetXaxis()->SetTitle("M2");
    Hispred_rec_mass2->GetXaxis()->SetTitleSize(0.05);
    Hispred_rec_mass2->GetXaxis()->SetLabelSize(0.05);
    Hispred_rec_mass2->SetLineWidth(2);

    Hispred_tru_mass2->GetXaxis()->SetTitle("M2");
    Hispred_tru_mass2->GetXaxis()->SetTitleSize(0.05);
    Hispred_tru_mass2->GetXaxis()->SetLabelSize(0.05);
    Hispred_tru_mass2->SetLineWidth(2);

    Hispred_rec_mass2->Draw();
    Hispred_tru_mass2->SetLineColor(2);
    Hispred_tru_mass2->Draw("same");

     TLegend *leg2 = new TLegend(0.75,0.15,0.95,0.35);

    leg2->AddEntry(Hispred_rec_mass2.GetPtr(), "reconstructed", "l");
    leg2->AddEntry(Hispred_tru_mass2.GetPtr(), "truth", "l");
    leg2->Draw();

    c2->Print("mass_plots/pip-mass2.png");

    


}