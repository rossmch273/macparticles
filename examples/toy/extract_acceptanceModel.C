#include <stdlib.h>
#include <stdio.h>
//extract_acceptanceModel("pi+", "detected")
void extract_acceptanceModel(string particle_name, string training_type)
{
    /*This code extracts the data for the DNN and BDT models and stores them in the one root file for the purpose of comparing different
        training datasets*/



    using namespace ROOT;
    using namespace ROOT::RDF;

    //string specifying directory location for the DNN and BDT as well as the filename containing the weights
    string DNNdir = "fast_sim/acceptance/";
    string BDTdir = "fast_sim/reweight/";
    string accFilename = "training_acceptances.root";

    //create full filepath using string concatenation and the particle name
    string strDNNpath = DNNdir + particle_name + "/" + accFilename;
    string strBDTpath = BDTdir + particle_name + "/" + accFilename;

    const char* BDTpath = strBDTpath.c_str();
    const char* DNNpath = strDNNpath.c_str();

    std::cout << BDTpath << std::endl << DNNpath << std::endl;

    TFile *BDT = new TFile(BDTpath, "ROOT");
    TTree *modelTree = (TTree*)BDT->Get("BDT");
    modelTree->AddFriend("kerasDNN", DNNpath);

    //Read data in root file into data frame
    RDataFrame acceptanceModel(*modelTree);

    //Write to training models file

    //Alter particle names to align with the other filenames
    string particle;

    if (particle_name=="pi+"){
        particle = "pip";
        
    }
    
    if (particle_name=="pi-"){
        particle = "pim";
        
    }
    if (particle_name=="proton"){
        particle = "proton";
        
    }
    
    std::cout << particle << std::endl;
    //generate the tree name and the file name from strings

    
    string strTreeName = "training_" + training_type ;
    string strSaveFile = particle + "-" + "training-models.root";
    
   
    const char* TreeName = strTreeName.c_str();
    const char* SaveFile = strSaveFile.c_str();

    std::cout << TreeName << std::endl << SaveFile << std::endl;

    //Write to training models file
    auto snapshotOptions = RSnapshotOptions();
    snapshotOptions.fMode = "UPDATE";
    snapshotOptions.fOverwriteIfExists = true;
    acceptanceModel.Snapshot(TreeName, SaveFile, acceptanceModel.GetColumnNames(), snapshotOptions);
   
}


