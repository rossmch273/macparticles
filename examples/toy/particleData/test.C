{
    using namespace ROOT;
    using namespace ROOT::RDF;

    auto df = RDataFrame("tree", "/home/ross-mcharg/cloned/directory/ross_macparticles/examples/toy/toy_reaction.root");

    auto colNames = df.GetColumnNames();

    for( auto &&colName : colNames){
        std::cout << colName << std::endl;
    }
}