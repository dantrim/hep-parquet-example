#include <iostream>
#include "event_generator.h"
#include "parquet_writer.h"
#include <fstream>

int main(int argc, char* argv[]) {

    if(argc != 3) {
        std::cerr << "Incorrect number of arguments" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <n-events> <config>" << std::endl;
        return 1;
    }
    unsigned n_events = std::stoi(argv[1]);
    std::cout << "Generating " << n_events << " events" << std::endl;

    // initialize parquet-writer
    std::ifstream ifs(argv[2]);
    namespace pw = parquetwriter;
    pw::Writer writer;
    writer.set_layout(ifs);
    writer.set_metadata(ifs);
    writer.set_dataset_name("hep_dataset");
    writer.set_output_directory("hep_dataset");
    writer.set_compression(pw::Compression::UNCOMPRESSED);
    writer.initialize();

    EventGenerator gen(100., 3, 10);

    for(size_t i = 0; i < n_events; i++) {
            std::cout << " *** Processing event " << (i+1) << "/" << n_events << " ***" << std::endl;

        auto event = gen.event();
        auto leptons = gen.leptons();
        auto jets = gen.jets();
        auto met = gen.met();

        std::cout << " -> nlep = " << leptons.size() << ", njet = " << jets.size() << std::endl;

        //
        // event container
        //
        double w = event.w;
        double w2 = event.w2;
        std::vector<bool> trigger_mask = event.trigger_mask;
        pw::struct_t event_container_data{w};//, w2, trigger_mask};

        //
        // leptons container
        //
        //leptons.clear();
        uint32_t nleptons = leptons.size();
        //pw::struct_t leptons_container{nleptons};


        pw::struct_list1d leptons_object_data;
        //if(leptons.size()>0) {
            for(size_t i = 0; i < nleptons; i++) {
                Lepton lepton = leptons.at(i);

                //float pt = lepton.pt;
                //float eta = lepton.eta;
                //float phi = lepton.phi;
                //uint32_t flavor = lepton.flavor;
                //std::vector<bool> trigger_match_mask = lepton.trigger_match_mask;
                //bool isLoose = lepton.isLoose;
                //bool isMedium = lepton.isMedium;
                //bool isTight = lepton.isTight;
                pw::struct_t lepton_data{
                                        lepton.pt,
                                        lepton.eta,
                                        lepton.phi,
                                        lepton.flavor,
                                        lepton.trigger_match_mask,
                                        lepton.isLoose,
                                        lepton.isMedium,
                                        lepton.isTight};
                //pw::struct_t lepton_data{pt};
                //leptons_container_data.push_back(lepton_data);
                leptons_object_data.push_back(lepton_data);
            }
      //  } 
       // else {
       //         float pt = lepton.pt;
       //         float eta = lepton.eta;
       //         float phi = lepton.phi;
       //         uint32_t flavor = lepton.flavor;
       //         std::vector<bool> trigger_match_mask{false};
       //         bool isLoose = lepton.isLoose;
       //         bool isMedium = lepton.isMedium;
       //         bool isTight = lepton.isTight;
       //         //pw::struct_t lepton_data{pt,
       //         //                        eta,
       //         //                        phi,
       //         //                        flavor,
       //         //                        trigger_match_mask,
       //         //                        isLoose,
       //         //                        isMedium,
       //         //                        isTight};
       //         pw::struct_t lepton_data{pt};
       //         leptons_container_data.push_back(lepton_data);
       // }
       // else {
       //         float pt = 0.0;
       //         float eta = 0.0;
       //         float phi = 0.0;
       //         uint32_t flavor = 0;
       //         std::vector<bool> trigger_match_mask{false};
       //         bool isLoose = false;
       //         bool isMedium = false;
       //         bool isTight = false;
       //         pw::struct_t lepton_data{pt,
       //                                 eta,
       //                                 phi,
       //                                 flavor,
       //                                 trigger_match_mask,
       //                                 isLoose,
       //                                 isMedium,
       //                                 isTight};
       //         leptons_container_data.push_back(lepton_data);
       // }
        //std::cout << "FOO size = nlep = " << leptons.size()<< " : "  << leptons_container.size() << ", " << leptons_container_data.size() << std::endl;
        std::cout << "FILL" << std::endl;
        writer.fill("leptons", {leptons_object_data});
        //writer.fill("leptons.leptons", {leptons_container_data});
        writer.fill("event", {event_container_data});

//        //
//        // jets container
//        //
//        pw::struct_list1d jets_container_data;
//        uint32_t njets = jets.size();
//        for(size_t i = 0; i < njets; i++) {
//            auto jet = jets.at(i);
//            pw::struct_t jet_data{
//                jet.pt,
//                    jet.eta,
//                    jet.phi,
//                    jet.mass,
//                    jet.truthHadronPt,
//                    jet.truthHadronId,
//                    jet.nTrk,
//                    jet.isBJet,
//                    jet.bTagScore};
//            jets_container_data.push_back(jet_data);
//
//        }
//        writer.fill("jets", {njets});
//        writer.fill("jets.jets", {jets_container_data});
//
//        // 
//        // met container
//        //
//        pw::struct_t met_data{met.sumEt,
//                                met.met,
//                                met.phi,
//                                met.electronTerm,
//                                met.muonTerm,
//                                met.jetTerm,
//                                met.softTerm};
//        writer.fill("met", {met_data});
    }
    writer.finish();

    return 0;
}
