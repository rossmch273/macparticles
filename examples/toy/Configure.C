{


  auto info = TrainingInfo{"pi+"};


  info.treeName = "rec_tree";
  info.reconFile = "particleData/particle-data/pip-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables ={{"detP","detP","detP","#pi+ momentum",0,10},
		   {"detTheta","detTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
		   {"detPhi","detPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_pip";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";


  info.WriteTo("training.root");
  /*********************************************************************************************************/
  info = TrainingInfo{"pi-"};


  info.treeName = "rec_tree";
  info.reconFile = "particleData/particle-data/pim-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables = 	{{"detP","detP","detP","#pi+ momentum",0,10},
			 {"detTheta","detTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
			 {"detPhi","detPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_pim";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";

 

  info.WriteTo("training.root");

 /*********************************************************************************************************/
  info = TrainingInfo{"proton"};


  info.treeName = "rec_tree";
  info.reconFile = "particleData/particle-data/proton-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables = 	{{"detP","detP","detP","#pi+ momentum",0,10},
			 {"detTheta","detTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
			 {"detPhi","detPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_p";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";


  info.WriteTo("training.root");


}
