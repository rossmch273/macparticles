{
    /*This code extracts the data for the DNN and BDT models and stores them in the one root file for the purpose of comparing different
        training datasets*/

        
    using namespace ROOT;
    using namespace ROOT::RDF;

    //open file containing the BDT values
    TFile *BDT = new TFile("fast_sim/reweight/pi+/training_acceptances.root", "ROOT");
    TTree *modelTree = (TTree*)BDT->Get("BDT");
    modelTree->AddFriend("kerasDNN", "fast_sim/acceptance/pi+/training_acceptances.root");
    
    //Read data in root file into data frame
    RDataFrame acceptanceModel(*modelTree);

    //write to training models file
    auto snapshotOptions = RSnapshotOptions();
    snapshotOptions.fMode = "UPDATE";
    acceptanceModel.Snapshot("training_detected", "training-models.root", acceptanceModel.GetColumnNames(), snapshotOptions);

}


