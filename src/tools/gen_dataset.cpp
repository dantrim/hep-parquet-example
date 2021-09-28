#include "event_generator.h"
#include "logging.h"
#include "parquet_writer.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Incorrect number of arguments" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <n-events> <config>" << std::endl;
    return 1;
  }
  unsigned n_events = std::stoi(argv[1]);
  std::cout << "Generating " << n_events << " events" << std::endl;

  // initialize parquet-writer
  std::ifstream ifs(argv[2]);
  namespace pw = parquetwriter;
  // pw::logging::set_debug();
  pw::Writer writer;
  writer.set_layout(ifs);
  writer.set_metadata(ifs);
  writer.set_dataset_name("hep_dataset");
  writer.set_output_directory("hep_dataset");
  writer.set_compression(pw::Compression::UNCOMPRESSED);
  writer.initialize();

  EventGenerator gen(100., 3, 10);

  for (size_t ievent = 0; ievent < n_events; ievent++) {

    if (ievent % 100 == 0 || (ievent + 1) == n_events) {
      std::cout << "====================================================="
                << std::endl;
      std::cout << " *** Processing event "
                << (ievent % 100 == 0 ? ievent : (ievent + 1)) << "/"
                << n_events << " ***" << std::endl;
    }

    auto event = gen.event();
    auto leptons = gen.leptons();
    auto jets = gen.jets();
    auto met = gen.met();

    //
    // event container
    //
    uint32_t event_idx = static_cast<uint32_t>(ievent);
    double w = event.w;
    double w2 = event.w2;
    std::vector<bool> trigger_mask = event.trigger_mask;
    pw::struct_t event_container_data{
        {"index", event_idx},
        {"w", w},
        {"w2", w2},
        {"trigger_mask", trigger_mask}
    };

    //
    // leptons container
    //
    uint32_t nleptons = leptons.size();
    pw::struct_t lepton_container_data{
        {"size", nleptons}
    };

    std::vector<pw::struct_t> leptons_data;
    for (size_t ilepton = 0; ilepton < leptons.size(); ilepton++) {
      Lepton lepton = leptons.at(ilepton);
      pw::struct_t lepton_data{
          {"pt", lepton.pt},
          {"eta", lepton.eta},
          {"phi", lepton.phi},
          {"flavor", lepton.flavor},
          {"trigger_match_mask", lepton.trigger_match_mask},
          {"isLoose", lepton.isLoose},
          {"isMedium", lepton.isMedium},
          {"isTight", lepton.isTight}
      };
      leptons_data.push_back(lepton_data);
    }

    //
    // jets container
    //
    uint32_t njets = jets.size();
    pw::struct_t jets_container_data{
        {"size", njets}
    };
    std::vector<pw::struct_t> jets_data;
    for (size_t i = 0; i < njets; i++) {
      auto jet = jets.at(i);
      pw::struct_t jet_data{
          {"pt", jet.pt},
          {"eta", jet.eta},
          {"phi", jet.phi},
          {"mass", jet.mass},
          {"truthHadronPt", jet.truthHadronPt},
          {"truthHadronId", jet.truthHadronId},
          {"nTrk", jet.nTrk},
          {"isBJet", jet.isBJet},
          {"bTagScore", jet.bTagScore}
      };
      jets_data.push_back(jet_data);
    }

    //
    // met container
    //
    pw::struct_t met_data{
        {"sumEt", met.sumEt},
        {"met", met.met},
        {"phi", met.phi},
        {"electronTerm", met.electronTerm},
        {"muonTerm", met.muonTerm},
        {"jetTerm", met.jetTerm},
        {"softTerm", met.softTerm}
    };

    //
    // fill columns
    //

    // event container
    writer.fill_struct("event_container", {event_container_data});

    // lepton container
    writer.fill_struct("lepton_container", {lepton_container_data});
    writer.fill_struct_list("lepton_container.leptons", {leptons_data});

    // jet container
    writer.fill_struct("jet_container", {jets_container_data});
    writer.fill_struct_list("jet_container.jets", {jets_data});

    // met container
    writer.fill_struct("met_container", {met_data});

    // finish this row
    writer.end_row();
  }
  writer.finish();

  return 0;
}
