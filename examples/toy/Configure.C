{


  auto info = TrainingInfo{"pi+"};


  info.treeName = "rec_tree";
  info.reconFile = "pip-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables ={{"calcP","calcP","detP","#pi+ momentum",0,10},
		   {"calcTheta","calcTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
		   {"calcPhi","calcPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_pip";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";


  info.WriteTo("training.root");
  /*********************************************************************************************************/
  info = TrainingInfo{"pi-"};


  info.treeName = "rec_tree";
  info.reconFile = "pim-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables = 	{{"calcP","calcP","detP","#pi+ momentum",0,10},
			 {"calcTheta","calcTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
			 {"calcPhi","calcPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_pim";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";

 

  info.WriteTo("training.root");

 /*********************************************************************************************************/
  info = TrainingInfo{"proton"};


  info.treeName = "rec_tree";
  info.reconFile = "proton-data.root";
  
  //what fraction of events in each file do you want to use?
  info.recFrac = 1;

  //truth name, generated name, reconstructed name, title, range
  info.variables = 	{{"calcP","calcP","detP","#pi+ momentum",0,10},
			 {"calcTheta","calcTheta","detTheta","#pi+ #theta",0,TMath::Pi()},
			 {"calcPhi","calcPhi","detPhi","#pi+ #phi",-TMath::Pi(),TMath::Pi()}};

  //variable to flag if particle was reconstructed in this event
  info.acceptVar = "accepted_p";

  //in case need further fltering of reconstructed events (e.g. truth matching,..)
  info.recFilter = "";


  info.WriteTo("training.root");


}
